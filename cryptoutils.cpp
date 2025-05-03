#include "CryptoUtils.h"

CryptoUtils::CryptoUtils(QObject *parent)
{

}

CryptoUtils::~CryptoUtils()
{

}

bool CryptoUtils::createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath) {
    if(!QFile::exists(publicKeyPath) && !QFile::exists(privateKeyPath)){
        RSA *rsa = RSA_new();
        BIGNUM *bn = BN_new();
        BN_set_word(bn, RSA_F4);

        if (!RSA_generate_key_ex(rsa, 2048, bn, nullptr)) {
            qCritical() << "RSA key generation failed:" << ERR_error_string(ERR_get_error(), nullptr);
            RSA_free(rsa);
            BN_free(bn);
            return false;
        }

        FILE *pub = fopen(publicKeyPath.toLocal8Bit().constData(), "wb");
        if (!pub || !PEM_write_RSA_PUBKEY(pub, rsa)) {
            qCritical() << "Failed to write public key";
            RSA_free(rsa);
            BN_free(bn);
            return false;
        }
        fclose(pub);

        FILE *priv = fopen(privateKeyPath.toLocal8Bit().constData(), "wb");
        if (!priv || !PEM_write_RSAPrivateKey(priv, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
            qCritical() << "Failed to write private key";
            RSA_free(rsa);
            BN_free(bn);
            return false;
        }
        fclose(priv);

        RSA_free(rsa);
        BN_free(bn);
        return true;
    }
    else{
        return false;
    }
}

bool CryptoUtils::encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath) {
    if(QFile::exists(publicKeyPath) && QFile::exists(inputFile)){
        // 1. Generate random AES key and IV
        unsigned char aesKey[32]; // 256-bit key
        unsigned char iv[AES_BLOCK_SIZE];
        if (RAND_bytes(aesKey, sizeof(aesKey)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
            qCritical() << "Failed to generate random bytes";
            return false;
        }

        // 2. Load RSA public key
        FILE *pub = fopen(publicKeyPath.toLocal8Bit().constData(), "rb");
        if (!pub) {
            qCritical() << "Failed to open public key file";
            return false;
        }
        RSA *rsa = PEM_read_RSA_PUBKEY(pub, nullptr, nullptr, nullptr);
        fclose(pub);
        if (!rsa) {
            qCritical() << "Failed to read public key";
            return false;
        }

        // 3. Encrypt AES key with RSA
        int rsaSize = RSA_size(rsa);
        unsigned char *encryptedKey = new unsigned char[rsaSize];
        int result = RSA_public_encrypt(sizeof(aesKey), aesKey, encryptedKey, rsa, RSA_PKCS1_PADDING);
        RSA_free(rsa);
        if (result == -1) {
            qCritical() << "RSA encryption failed";
            delete[] encryptedKey;
            return false;
        }

        // 4. Prepare output file
        QFile in(inputFile);
        if (!in.open(QIODevice::ReadOnly)) {
            qCritical() << "Failed to open input file";
            delete[] encryptedKey;
            return false;
        }

        QFile out(encryptedFile);
        if (!out.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to open output file";
            in.close();
            delete[] encryptedKey;
            return false;
        }

        // Write encrypted key length, encrypted key and IV
        out.write(reinterpret_cast<char*>(&result), sizeof(int));
        out.write(reinterpret_cast<char*>(encryptedKey), result);
        out.write(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE);
        delete[] encryptedKey;

        // 5. Encrypt file content
        AES_KEY aes;
        AES_set_encrypt_key(aesKey, 256, &aes);

        const int BUFFER_SIZE = 16 * 1024; // 16KB buffer
        unsigned char inBuf[BUFFER_SIZE];
        unsigned char outBuf[BUFFER_SIZE + AES_BLOCK_SIZE]; // Extra space for padding
        int bytesRead;

        while ((bytesRead = in.read(reinterpret_cast<char*>(inBuf), BUFFER_SIZE)) > 0) {
            // Add PKCS#7 padding
            int padding = AES_BLOCK_SIZE - (bytesRead % AES_BLOCK_SIZE);
            memset(inBuf + bytesRead, padding, padding);
            bytesRead += padding;

            AES_cbc_encrypt(inBuf, outBuf, bytesRead, &aes, iv, AES_ENCRYPT);
            out.write(reinterpret_cast<char*>(outBuf), bytesRead);
        }

        in.close();
        out.close();
        return true;
    }
    else{
        return false;
    }
}

bool CryptoUtils::decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath) {
    if(QFile::exists(encryptedFile) && QFile::exists(privateKeyPath)){
        // 1. Open encrypted file
        QFile in(encryptedFile);
        if (!in.open(QIODevice::ReadOnly)) {
            qCritical() << "Failed to open encrypted file";
            return false;
        }

        // 2. Read encrypted AES key
        int encKeyLen;
        if (in.read(reinterpret_cast<char*>(&encKeyLen), sizeof(int)) != sizeof(int)) {
            qCritical() << "Failed to read key length";
            in.close();
            return false;
        }

        unsigned char *encryptedKey = new unsigned char[encKeyLen];
        if (in.read(reinterpret_cast<char*>(encryptedKey), encKeyLen) != encKeyLen) {
            qCritical() << "Failed to read encrypted key";
            delete[] encryptedKey;
            in.close();
            return false;
        }

        // 3. Read IV
        unsigned char iv[AES_BLOCK_SIZE];
        if (in.read(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE) != AES_BLOCK_SIZE) {
            qCritical() << "Failed to read IV";
            delete[] encryptedKey;
            in.close();
            return false;
        }

        // 4. Load RSA private key
        FILE *priv = fopen(privateKeyPath.toLocal8Bit().constData(), "rb");
        if (!priv) {
            qCritical() << "Failed to open private key file";
            delete[] encryptedKey;
            in.close();
            return false;
        }
        RSA *rsa = PEM_read_RSAPrivateKey(priv, nullptr, nullptr, nullptr);
        fclose(priv);
        if (!rsa) {
            qCritical() << "Failed to read private key";
            delete[] encryptedKey;
            in.close();
            return false;
        }

        // 5. Decrypt AES key
        unsigned char aesKey[32];
        int result = RSA_private_decrypt(encKeyLen, encryptedKey, aesKey, rsa, RSA_PKCS1_PADDING);
        RSA_free(rsa);
        delete[] encryptedKey;
        if (result != sizeof(aesKey)) {
            qCritical() << "RSA decryption failed";
            in.close();
            return false;
        }

        // 6. Prepare output file
        QFile out(decryptedFile);
        if (!out.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to open output file";
            in.close();
            return false;
        }

        // 7. Decrypt file content
        AES_KEY aes;
        AES_set_decrypt_key(aesKey, 256, &aes);

        const int BUFFER_SIZE = 16 * 1024 + AES_BLOCK_SIZE; // 16KB + padding
        unsigned char inBuf[BUFFER_SIZE];
        unsigned char outBuf[BUFFER_SIZE];
        int bytesRead;
        bool lastBlock = false;

        while (!lastBlock && (bytesRead = in.read(reinterpret_cast<char*>(inBuf), BUFFER_SIZE)) > 0) {
            if (in.atEnd()) lastBlock = true;

            AES_cbc_encrypt(inBuf, outBuf, bytesRead, &aes, iv, AES_DECRYPT);

            // Remove PKCS#7 padding from last block
            if (lastBlock) {
                int padding = outBuf[bytesRead - 1];
                if (padding > 0 && padding <= AES_BLOCK_SIZE) {
                    bytesRead -= padding;
                }
            }

            out.write(reinterpret_cast<char*>(outBuf), bytesRead);
        }

        in.close();
        out.close();
        return true;
    }
    else{
        return false;
    }
}

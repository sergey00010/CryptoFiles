#include "CryptoUtils.h"

CryptoUtils::CryptoUtils(QObject *parent) : QObject(parent)
{
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
}

CryptoUtils::~CryptoUtils()
{
    EVP_cleanup();
    ERR_free_strings();
}

bool CryptoUtils::createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath, int size) {
    RSA *rsa = RSA_new();
    BIGNUM *bn = BN_new();
    BN_set_word(bn, RSA_F4);

    if (!RSA_generate_key_ex(rsa, size, bn, nullptr)) {
        qCritical() << "RSA key generation failed:" << ERR_error_string(ERR_get_error(), nullptr);
        RSA_free(rsa);
        BN_free(bn);
        return false;
    }

    FILE *pub = fopen(publicKeyPath.toLocal8Bit().constData(), "wb");
    if (!pub) {
        qCritical() << "Failed to open public key file for writing:" << publicKeyPath;
        RSA_free(rsa);
        BN_free(bn);
        return false;
    }
    if (!PEM_write_RSA_PUBKEY(pub, rsa)) {
        qCritical() << "Failed to write public key:" << ERR_error_string(ERR_get_error(), nullptr);
        fclose(pub);
        RSA_free(rsa);
        BN_free(bn);
        return false;
    }
    fclose(pub);

    FILE *priv = fopen(privateKeyPath.toLocal8Bit().constData(), "wb");
    if (!priv) {
        qCritical() << "Failed to open private key file for writing:" << privateKeyPath;
        RSA_free(rsa);
        BN_free(bn);
        return false;
    }
    if (!PEM_write_RSAPrivateKey(priv, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
        qCritical() << "Failed to write private key:" << ERR_error_string(ERR_get_error(), nullptr);
        fclose(priv);
        RSA_free(rsa);
        BN_free(bn);
        return false;
    }
    fclose(priv);

    RSA_free(rsa);
    BN_free(bn);
    return true;
}

bool CryptoUtils::encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath) {
    if(!QFile::exists(publicKeyPath) || !QFile::exists(inputFile)){
        qCritical() << "Public key or input file does not exist.";
        return false;
    }

    // 1. Generate random AES key and IV
    unsigned char aesKey[32]; // 256-bit key
    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(aesKey, sizeof(aesKey)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
        qCritical() << "Failed to generate random bytes:" << ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }

    // 2. Load RSA public key
    FILE *pub = fopen(publicKeyPath.toLocal8Bit().constData(), "rb");
    if (!pub) {
        qCritical() << "Failed to open public key file:" << publicKeyPath;
        return false;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(pub, nullptr, nullptr, nullptr);
    fclose(pub);
    if (!rsa) {
        qCritical() << "Failed to read public key:" << ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }

    // 3. Encrypt AES key with RSA
    int rsaSize = RSA_size(rsa);
    unsigned char *encryptedKey = new unsigned char[rsaSize];
    int result = RSA_public_encrypt(sizeof(aesKey), aesKey, encryptedKey, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    if (result == -1) {
        qCritical() << "RSA encryption failed:" << ERR_error_string(ERR_get_error(), nullptr);
        delete[] encryptedKey;
        return false;
    }

    // 4. Prepare output file
    QFile in(inputFile);
    if (!in.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open input file:" << inputFile;
        delete[] encryptedKey;
        return false;
    }

    QFile out(encryptedFile);
    if (!out.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open output file:" << encryptedFile;
        in.close();
        delete[] encryptedKey;
        return false;
    }

    // Save original file name and extension
    QFileInfo fileInfo(inputFile);
    QString originalName = fileInfo.completeBaseName(); // Имя файла без расширения
    QString originalExtension = fileInfo.suffix();

    // Convert to UTF-8 bytes
    QByteArray nameBytes = originalName.toUtf8();
    QByteArray extensionBytes = originalExtension.toUtf8();

    // Write lengths and data
    qint32 nameLength = nameBytes.length();
    qint32 extensionLength = extensionBytes.length();

    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(qint32));
    out.write(reinterpret_cast<const char*>(&extensionLength), sizeof(qint32));
    out.write(nameBytes);
    out.write(extensionBytes);

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
    qint64 bytesRead;

    while ((bytesRead = in.read(reinterpret_cast<char*>(inBuf), BUFFER_SIZE)) > 0) {
        int bytesToEncrypt = bytesRead;
        // Apply PKCS#7 padding only to the last block
        if (in.atEnd()) {
            int padding = AES_BLOCK_SIZE - (bytesRead % AES_BLOCK_SIZE);
            if (padding == 0) { // If bytesRead is a multiple of AES_BLOCK_SIZE, add a full block of padding
                padding = AES_BLOCK_SIZE;
            }
            memset(inBuf + bytesRead, padding, padding);
            bytesToEncrypt += padding;
        }

        AES_cbc_encrypt(inBuf, outBuf, bytesToEncrypt, &aes, iv, AES_ENCRYPT);
        out.write(reinterpret_cast<char*>(outBuf), bytesToEncrypt);
    }

    in.close();
    out.close();
    return true;
}

bool CryptoUtils::decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath) {
    if(!QFile::exists(encryptedFile) || !QFile::exists(privateKeyPath)){
        qCritical() << "Encrypted file or private key does not exist.";
        return false;
    }

    // 1. Open encrypted file
    QFile in(encryptedFile);
    if (!in.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open encrypted file:" << encryptedFile;
        return false;
    }

    // Read file name and extension lengths
    qint32 nameLength, extensionLength;
    if (in.read(reinterpret_cast<char*>(&nameLength), sizeof(qint32)) != sizeof(qint32) ||
        in.read(reinterpret_cast<char*>(&extensionLength), sizeof(qint32)) != sizeof(qint32)) {
        qCritical() << "Failed to read name or extension length.";
        in.close();
        return false;
    }

    // Read file name
    QByteArray nameBytes;
    nameBytes.resize(nameLength);
    if (in.read(nameBytes.data(), nameLength) != nameLength) {
        qCritical() << "Failed to read original name.";
        in.close();
        return false;
    }
    QString originalName = QString::fromUtf8(nameBytes);

    // Read file extension
    QByteArray extensionBytes;
    extensionBytes.resize(extensionLength);
    if (in.read(extensionBytes.data(), extensionLength) != extensionLength) {
        qCritical() << "Failed to read original extension.";
        in.close();
        return false;
    }
    QString originalExtension = QString::fromUtf8(extensionBytes);

    // 2. Read encrypted AES key length
    int encKeyLen;
    if (in.read(reinterpret_cast<char*>(&encKeyLen), sizeof(int)) != sizeof(int)) {
        qCritical() << "Failed to read key length.";
        in.close();
        return false;
    }

    // 3. Read encrypted AES key
    unsigned char *encryptedKey = new unsigned char[encKeyLen];
    if (in.read(reinterpret_cast<char*>(encryptedKey), encKeyLen) != encKeyLen) {
        qCritical() << "Failed to read encrypted key.";
        delete[] encryptedKey;
        in.close();
        return false;
    }

    // 4. Read IV
    unsigned char iv[AES_BLOCK_SIZE];
    if (in.read(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE) != AES_BLOCK_SIZE) {
        qCritical() << "Failed to read IV.";
        delete[] encryptedKey;
        in.close();
        return false;
    }

    // 5. Load RSA private key
    FILE *priv = fopen(privateKeyPath.toLocal8Bit().constData(), "rb");
    if (!priv) {
        qCritical() << "Failed to open private key file:" << privateKeyPath;
        delete[] encryptedKey;
        in.close();
        return false;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(priv, nullptr, nullptr, nullptr);
    fclose(priv);
    if (!rsa) {
        qCritical() << "Failed to read private key:" << ERR_error_string(ERR_get_error(), nullptr);
        delete[] encryptedKey;
        in.close();
        return false;
    }

    // 6. Decrypt AES key
    unsigned char aesKey[32];
    int result = RSA_private_decrypt(encKeyLen, encryptedKey, aesKey, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    delete[] encryptedKey;
    if (result != sizeof(aesKey)) {
        qCritical() << "RSA decryption failed. Expected AES key size" << sizeof(aesKey) << ", got" << result << ":" << ERR_error_string(ERR_get_error(), nullptr);
        in.close();
        return false;
    }

    // 7. Prepare output file with original name and extension
    QString finalDecryptedFilePath;
    QFileInfo decryptedFileInfo(decryptedFile);

    if (!originalName.isEmpty() || !originalExtension.isEmpty()) {
        QString fileName = originalName;
        if (!originalExtension.isEmpty()) {
            fileName += "." + originalExtension;
        }
        finalDecryptedFilePath = decryptedFileInfo.absolutePath() + "/" + fileName;
    } else {
        finalDecryptedFilePath = decryptedFile;
    }

    QFile out(finalDecryptedFilePath);
    if (!out.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open output file:" << finalDecryptedFilePath;
        in.close();
        return false;
    }

    // 8. Decrypt file content
    AES_KEY aes;
    AES_set_decrypt_key(aesKey, 256, &aes);

    const int BUFFER_SIZE = 16 * 1024 + AES_BLOCK_SIZE; // 16KB + max padding
    unsigned char inBuf[BUFFER_SIZE];
    unsigned char outBuf[BUFFER_SIZE];
    qint64 bytesRead;
    bool lastBlock = false;

    unsigned char currentIv[AES_BLOCK_SIZE];
    memcpy(currentIv, iv, AES_BLOCK_SIZE);

    while ((bytesRead = in.read(reinterpret_cast<char*>(inBuf), BUFFER_SIZE)) > 0) {
        if (in.atEnd()) lastBlock = true;

        AES_cbc_encrypt(inBuf, outBuf, bytesRead, &aes, currentIv, AES_DECRYPT);

        qint64 bytesToWrite = bytesRead;
        if (lastBlock) {
            int padding = outBuf[bytesRead - 1];
            if (padding > 0 && padding <= AES_BLOCK_SIZE) {
                bool validPadding = true;
                for (int i = 0; i < padding; ++i) {
                    if (outBuf[bytesRead - 1 - i] != padding) {
                        validPadding = false;
                        break;
                    }
                }

                if (validPadding) {
                    bytesToWrite -= padding;
                } else {
                    qWarning() << "Invalid PKCS#7 padding detected during decryption. Writing full block.";
                }
            } else {
                qWarning() << "Padding value out of range or zero. Writing full block.";
            }
        }

        out.write(reinterpret_cast<char*>(outBuf), bytesToWrite);
    }

    in.close();
    out.close();
    return true;
}

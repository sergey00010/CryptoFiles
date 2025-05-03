#pragma once

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

class CryptoUtils {
public:
    // Generate RSA keys and save to files
    static bool createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath);

    // File encryption: AES encrypts data, RSA encrypts AES key
    static bool encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath);

    // Decrypt file: RSA decrypts AES key, AES decrypts data
    static bool decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath);
};

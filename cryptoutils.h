#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

class CryptoUtils : public QObject {
    Q_OBJECT
public:
    explicit CryptoUtils(QObject *parent = nullptr);
    ~CryptoUtils() override;
    // Generate RSA keys and save to files
    Q_INVOKABLE virtual bool createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath, int size);

    // File encryption: AES encrypts data, RSA encrypts AES key
    Q_INVOKABLE virtual bool encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath);

    // Decrypt file: RSA decrypts AES key, AES decrypts data
    Q_INVOKABLE virtual bool decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath);
};

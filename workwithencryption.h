#ifndef WORKWITHENCRYPTION_H
#define WORKWITHENCRYPTION_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>
#include <QThread>

#include "cryptoutils.h"

class WorkWithEncryption : public CryptoUtils
{
    Q_OBJECT
public:
    explicit WorkWithEncryption(QObject *parent = nullptr);
    Q_INVOKABLE  bool createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath, int size) override;
    Q_INVOKABLE  bool encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath, bool deleteOriginalFileCheck);
    Q_INVOKABLE  bool decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath, bool deleteEncryptFileCheck);

public slots:
    void onDialogResult(bool accepted);

signals:
    void showConfirmationDialog(const QString &message);
    bool keysCreationFinished(bool success);

private:
    QString m_publicKeyPath;
    QString m_privateKeyPath;
    int m_size;
};

#endif // WORKWITHENCRYPTION_H

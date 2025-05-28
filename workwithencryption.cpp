#include "workwithencryption.h"

WorkWithEncryption::WorkWithEncryption(QObject *parent) : CryptoUtils(parent){

}

bool WorkWithEncryption::createRsaKeys(const QString &publicKeyPath, const QString &privateKeyPath, int size)
{
    m_publicKeyPath = publicKeyPath;
    m_privateKeyPath = privateKeyPath;
    m_size = size;

    if(QFile::exists(publicKeyPath) || QFile::exists(privateKeyPath)){
        emit showConfirmationDialog("the keys already exist, rewrite them?");
        // The action will continue in onDialogResult.
        return true; // Return true, indicating that the confirmation request was sent.
            // The actual result of the operation (creation of keys) will be transmitted later.
    } else {
        // If the files do not exist, you can create keys immediately
        return CryptoUtils::createRsaKeys(publicKeyPath, privateKeyPath, size);
    }
}

bool WorkWithEncryption::encryptFile(const QString &inputFile, const QString &encryptedFile, const QString &publicKeyPath, bool deleteOriginalFileCheck)
{
    bool result = CryptoUtils::encryptFile(inputFile,encryptedFile,publicKeyPath);
    if(result){
        if(deleteOriginalFileCheck && QFile::exists(inputFile)){
            QFile::remove(inputFile);
        }
        return true;
    }else{
        return false;
    }
}

bool WorkWithEncryption::decryptFile(const QString &encryptedFile, const QString &decryptedFile, const QString &privateKeyPath, bool deleteEncryptFileCheck)
{
    bool result = CryptoUtils::decryptFile(encryptedFile,decryptedFile,privateKeyPath);
    if(result){
        if(deleteEncryptFileCheck && QFile::exists(encryptedFile)){
            QFile::remove(encryptedFile);
        }
        return true;
    }else{
        return false;
    }
}


void WorkWithEncryption::onDialogResult(bool accepted)
{
    if (accepted) {
        // The user agreed, now we create keys
        bool success = CryptoUtils::createRsaKeys(m_publicKeyPath, m_privateKeyPath, m_size);
        emit keysCreationFinished(success); // inform about the completion of the operation
    } else {
        emit keysCreationFinished(false); // inform that the operation has been cancelled.
    }
}


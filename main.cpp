#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "cryptoutils.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // if (!CryptoUtils::createRsaKeys("public.pem", "private.pem")) {
    //     qCritical() << "Failed to create RSA keys";
    //     return 1;
    // }

    // if (!CryptoUtils::encryptFile("input.txt", "encrypted.bin", "public.pem")) {
    //     qCritical() << "Failed to encrypt file";
    //     return 1;
    // }

    // if (!CryptoUtils::decryptFile("encrypted.bin", "decrypted.txt", "private.pem")) {
    //     qCritical() << "Failed to decrypt file";
    //     return 1;
    // }

    qmlRegisterType<CryptoUtils>("CryptoUtils", 1, 0, "CryptoUtils");

    QQmlApplicationEngine engine;

    CryptoUtils cryptoUtils;
    engine.rootContext()->setContextProperty("cryptoUtils", &cryptoUtils);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("ForcksCryptoFiles", "Main");

    return app.exec();
}

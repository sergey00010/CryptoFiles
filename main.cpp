#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "workwithencryption.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<WorkWithEncryption>("WorkWithEncryption", 1, 0, "WorkWithEncryption");

    QQmlApplicationEngine engine;

    WorkWithEncryption workWithEncription;
    engine.rootContext()->setContextProperty("workWithEncription", &workWithEncription);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("ForcksCryptoFiles", "Main");

    return app.exec();
}

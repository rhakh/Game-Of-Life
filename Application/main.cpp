#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "GameOfLifeModel.hpp"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    // set style
    QQuickStyle::setStyle("Material");

    // register model for qml
    qmlRegisterType<GameOfLifeModel>("GOL", 1, 0, "GameOfLifeModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

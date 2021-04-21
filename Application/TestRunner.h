#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <string>
#include <vector>
#include <QList>
#include <QString>
#include <QObject>
#include <QJSValue>
#include <QJSEngine>

class TestRunner: public QObject
{
    Q_OBJECT

    std::vector<std::string> mFiles;
    std::vector<std::string> mGenerations;
    std::string mMap;

public:
    TestRunner();

public slots:
    void run(const QJSValue &callback);
    void setup(QList<QString> files, QList<QString> generations, QString map);
};

#endif // TESTRUNNER_H

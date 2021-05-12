#ifndef ALGORITHMRUNNER_H
#define ALGORITHMRUNNER_H

#include <string>
#include <vector>
#include <atomic>
#include <QList>
#include <QString>
#include <QObject>
#include <QJSValue>
#include <QJSEngine>

class AlgorithmRunner: public QObject
{
    Q_OBJECT

    std::vector<std::string> mFiles;
    int mLastGeneration;
    int mStep;
    std::string mMap;
    std::atomic_bool mStopSent;


    // used to update chart
    QObject *mRootObject;

public:
    AlgorithmRunner();

public slots:
    void run(const QJSValue &callback);
    void setup(QList<QString> files, QString lastGeneration, QString step, QString map);
    void stop();
};

#endif // ALGORITHMRUNNER_H

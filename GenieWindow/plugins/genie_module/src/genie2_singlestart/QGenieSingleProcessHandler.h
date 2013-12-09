#ifndef QGENIESINGLEPROCESSHANDLER_H
#define QGENIESINGLEPROCESSHANDLER_H

#include <QObject>
#include <QSharedMemory>
class QGenieWatchDog;

class QGenieSingleProcessHandler:public QObject
{
    Q_OBJECT
public:
    QGenieSingleProcessHandler();
    ~QGenieSingleProcessHandler();
    bool tryCreate();
    void startWatch();
    QGenieWatchDog *mDog;
#ifdef Q_OS_WIN32
    QSharedMemory mSharedMemory;
#endif
signals:
    void signal_OtherProcessStarted();
};

#endif // QGENIESINGLEPROCESSHANDLER_H

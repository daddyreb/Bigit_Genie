#ifndef QGENIEWATCHDOG_H
#define QGENIEWATCHDOG_H

#include <QObject>
#include <QThread>
#include <QMutex>


class QLaunchSemaphore;
class QGenieLaunchInf;

class QGenieWatchDog : public QThread
{
    Q_OBJECT
public:
    QGenieWatchDog();
    ~QGenieWatchDog();
    void stopit();
protected:
    void run();
//    QMutex mMutex;
    bool mOk;
    QLaunchSemaphore *mSemaphore;
signals:
    void dogwakeup();


};

#endif // QGENIEWATCHDOG_H

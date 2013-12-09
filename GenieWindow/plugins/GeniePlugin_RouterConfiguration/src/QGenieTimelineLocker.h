#ifndef QGENIETIMELINELOCKER_H
#define QGENIETIMELINELOCKER_H
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class QPiObject;

class QGenieTimelineLocker:public QObject,public Singleton<QGenieTimelineLocker>
{
    Q_OBJECT
public:
    QGenieTimelineLocker();
    ~QGenieTimelineLocker();
    void startTimer(int duration,QObject *obj=NULL,const char *slot=NULL,int flag = 0);
    void setNotifySsidData(const QString &ssid,const QString &pwd
                           ,const QString &wpa,const QString &rz);
    void setNeedReconnect(const QString &new24ssid=QString(),const QString &new24pwd=QString()
            ,const QString &new5ssid=QString(),const QString &new5pwd=QString());
    void setNeedSingleFinish(bool bneed=true);
    void forceStop();
    void whenUpdateUi();
    bool isWorking()
    {
        return (mTimeLine.state()==QTimeLine::Running);
    }
    void setDelayFinishTime(int secs);

protected:
    QTimeLine mTimeLine;
    QWidget *mLockerUi;
    QLabel *mLabel;
    QString mMode;
    QObject *mEndObj;
    const char *mEndSLOT;
    QPiObject *mPi;

    QString mNotifySsid;
    QString mNotifyPwd;
    QString mNotifyWpa;
    QString mNotifyRZ;
    QString mNeedReconnectSsid;
    QString mNeedReconnectPwd;
    enum TIMELINE_FLAG
    {
        TF_NEED_NOTIFY_SSID=(1<<0)
        ,TF_NEED_RECONNECT=(1<<1)
        ,TF_NEET_SEND_SINGLEFINISH=(1<<2)
    };

    quint32 mFlags;
    int mDelayFinishTime;
//    QTimer mDelayFinishTimer;
    void real_Finish();
protected slots:
    void slot_ValueChanged(qreal v);
    void slot_Finished();

};

#endif // QGENIETIMELINELOCKER_H

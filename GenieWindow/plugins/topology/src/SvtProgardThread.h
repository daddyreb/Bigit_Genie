#ifndef SVTPROGARDTHREAD_H
#define SVTPROGARDTHREAD_H

#include <QThread>
#include <QtCore>
class SVT_Core;
class SVT_NodePool;

typedef QList<QMap<int,QString> > SVT_NotifyType;

class SVT_ProgardThread:public QThread
{
public:
    enum PROGUARD_FLAGS
    {
        PF_NEED_REGEN_SUBSCRIB_LIST=1<<1,
        PF_NEED_NOTIFY_SUBSCRIB_CHANGE=1<<2,
        PF_NEED_REGEN_POOL=1<<3,
        PF_INRESTARTING=1<<4,
        PF_NEWDEVICE_ADDED=1<<5,
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
        PF_REFRESH_ATTACHDEVICE=1<<6,
#endif
        PF_LOOP_SPEEDUP=1<<7,
        //add by caiyujun
        PF_NEED_REGEN_EXTENDER_LIST=1<<8
        //add end

    };

    SVT_ProgardThread(SVT_Core *core);
    ~SVT_ProgardThread();
    void run();
    void setFlag(quint32 flags);
    void restart();
    void stop();
    QMutex mProgardMutex;
    int mSubscribStartIdx;
    int mSubscribCount;
    int mSbuscribNetInterfaceIdx; 
    quint32 mProgardFlags;
	//add by caiyujun
	unsigned long mExtenderIp;
	//add end

protected:
    void checkInterface();
    void regenSubscribList(const int &start,const int &count);
    void _addNodeToNotifyData(SVT_NotifyType *data,SVT_NodeRef &noderef);
    void _checkDefaultGatewayChange(const QList<SVT_NetworkInterface>&);
#ifdef NOTIFY_ADDED_DEVICE
    void _notifyNewDevice();
#endif
    void _notifySubscribed();
    void _notifyDeviceNum();
    SVT_Core *mCore;
    QDateTime mStartTime;

//    SVT_NodePool *mSubScribedPool;
//    quint32 mLastNotifyCount;

    int         mCountCache;


};
#endif // SVTPROGARDRUNNABLE_H

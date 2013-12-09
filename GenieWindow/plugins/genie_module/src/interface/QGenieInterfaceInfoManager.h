#ifndef QGENIEINTERFACEINFOMANAGER_H
#define QGENIEINTERFACEINFOMANAGER_H
#include "Singleton.h"



class QGenieInterfaceInfoManager:
        public IInfoManager,public Singleton<QGenieInterfaceInfoManager>
{
public:
    QGenieInterfaceInfoManager();
    virtual QIcon  getIcon();
    /*return a string include version number and something else*/
    virtual QString getInfo(int what,int para=0);
    virtual const QRect &getGeometry(int what);
    virtual void *getSharedMemory(int para);
    virtual void notifyPluginMessage(int msg,QString para0="",QString para1="");
    inline void setInfoFromPluginFlag(bool b=true)
    {
        mCanGetInfoFromPlugin=b;
    };
    QString getRouteImageName(int what,int para);
    QString mGatewayCache;
protected:
    bool mCanGetInfoFromPlugin;
    QString getNetState();
    QString getInfoFromPlugin(int para);
    QString getMainGeometry();
    QString ssidIsThis();
    int constRouteWaitTime(int type);
    QString getIfWirelessOk();
    QString getOSInfo();
    QString formatMac(const QString &mac);
    QString mSavedMac;
    QString mSaveRegion;
    QString mSaveChannel;


    QMutex mInfoMutex_netstate;
    QMutex mInfoMutex_plugininfo;
    QMutex mInfoMutex_fromui;

};

#endif // QGENIEINTERFACEINFOMANAGER_H

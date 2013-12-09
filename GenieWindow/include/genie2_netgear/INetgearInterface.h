#ifndef INETGEARINTERFACE_H
#define INETGEARINTERFACE_H
#include <QtCore>
#include <QStringList>
class _SoapNode;
class QGenieNetgearInterface;
class INetgearInterface
{
public:
    enum IFACE_COMMAND
    {
        IC_CURRENTSETTING
        ,IC_LOGIN
        ,IC_GET_WIRELESS
        ,IC_SET_WIRELESS
        ,IC_GET_GACC
        ,IC_SET_GACC
        ,IC_GET_MACADDR
        ,IC_GET_GACCSSID_ALIVE
        ,IC_GET_CHANNEL
        ,IC_GET_TM_OPION
        ,IC_GET_TM_STATISTICS
        ,IC_SET_TM_OPTION
        ,IC_SET_TM_ENABLE
        ,IC_SEND_SINGLE_FINISH
        ,IC_CHECK_NEWFIRMWARE
        ,IC_CHECK_NEWFIRMWARE2
        ,IC_UPDATE_NEWFIRMWARE
        ,IC_REBOOT
        ,IC_BLOCKINTERFACE
        ,IC_IS_5GOK
        ,IC_GET_5GINFO
        ,IC_SET_5GINFO
        ,IC_GET_5G_GACC
        ,IC_SET_5G_GACC
        ,IC_ATTACHDEVICE
		
		////////////////////////////// kai.yan add////////////////////////////////////////////
		,IC_SET_QOSENABLESTATUS 
		,IC_GET_QOSENABLESTATUS
		,IC_GET_QOSRULES
		,IC_ADD_QOSRULE
		,IC_ADD_QOSRULEBYETHERNETPORT
		,IC_ADD_QOSRULEBYMAC
		,IC_DELETE_QOSRULE
		,IC_UPDATE_QOSPRIORITY
		////////////////////////////////////////////////////////////////////////////////

		 ,IC_RAWSOAP

        ,IC_SM_LOGIN
        ,IC_SM_WIRELESS
        ,IC_SM_SET_WIRELESS
        ,IC_SM_GACC
        ,IC_SM_SET_GACC
        ,IC_SM_GET_TM_OPTION
        ,IC_SM_GET_TM_STATISTICS
        ,IC_SM_SET_TM_OPTION
        ,IC_SM_SET_TM_ENABLE
        ,IC_SM_GET_FIRMWARE
        ,IC_SM_IS_5GOK
        ,IC_SM_GET_5GINFO
        ,IC_SM_SET_5GINFO
        ,IC_SM_GET_5GGACC
        ,IC_SM_SET_5GGACC
        ,IC_SM_ATTACHDEVICE
		,IC_SM_LOGOUT


        ,IC_SM_END
    };
    enum IFACE_SETTINGS
    {
        IS_SM_CPLOGIN
        ,IS_SM_REMOVE_SECURE
    };

    virtual int netgearCommand(IFACE_COMMAND cmd,const QStringList &cmd_paras,QObject *receiver,const char *slot)=0;
    virtual void setConfiguration(IFACE_SETTINGS cmd,const QStringList &seting_paras)=0;

};

class IThreadInterface:public QThread
{
    Q_OBJECT
public:
    IThreadInterface(QGenieNetgearInterface *inf);
    ~IThreadInterface();
//    virtual void startGetInfo(int cmd,const QStringList &paras,int session);
    virtual bool canProcess(int cmd)=0;
    QSemaphore mSemaphore;
signals:
    void signal_DataReturn(_SoapNode *node,QStringList lst);
protected:
    int mCmd;
    void run();
    virtual void getNetgearData(int cmd,const QStringList &reqlist,QStringList &retlist)=0;
//    _SoapNode *mNode;
    QStringList mReqParas;
//    QStringList mRetValues;
    QGenieNetgearInterface *mInterface;

    bool &mExitFlag;
//    friend class QGenieNetgearInterface_todelete;
};

#endif // INETGEARINTERFACE_H

#ifndef SVTCONTROLPROXY_H
#define SVTCONTROLPROXY_H
#include <QtCore>
class SVT_Core;
typedef QList<QMap<int,QString> > SVT_NotifyType;
Q_DECLARE_METATYPE(SVT_NotifyType)
Q_DECLARE_METATYPE(QVariant)

class _InnerFSMController;
class SVT_Tracer;
namespace BLOCKDEVICE_FSM
{
enum BLOCK_DEVICE_FSM_STATE
{
    FS_INIT
    ,FS_GETTING_ATTACHDEVICE
    ,FS_GETTING_ENABLESTAT
    ,FS_RUNNING
    ,FS_CLOSED
    ,FS_RUNNING_ADMIN
    ,FS_CLOSED_ADMIN
    ,FS_LOGING
    ,FS_ENABLING
    ,FS_DISABLING
    ,FS_ENABLE_DISABLE_MAC
    ,FS_END

};
enum BLOCK_DEVICE_CMDS
{
    BDC_ENABLE,
    BDC_DISABLE,
    BDC_ENABLINGDISABLING_MAC,
    BDC_BLOCK_DEVICE,
    BDC_LOGIN_TO_RUNNING_ADMIN,
    BDC_LOGIN_TO_CLOSED_ADMIN,
    BDC_LOGIN_TO_DISABLING,
    BDC_LOGIN_TO_ENABLING,
    BDC_LOGIN_TO_ENABLINGDISABLING_MAC,
    BDC_NONE,
    BDC_NUM
};


}
class SVT_ControlProxy:public QObject
{
    Q_OBJECT
public:
    enum RETURN_DATA_TYPE
    {
        RDT_NODE_COUNT,
        RDT_SUBSCRIBED_NODE_DETAIL
    };


    SVT_ControlProxy();
	~SVT_ControlProxy();
    void startControl();
    void endControl();
    int refresh();
	//modify by caiyujun
    //int subscrib(int adpIdx,int startIdx,int count);
	int subscrib(int adpIdx,int startIdx,int count,bool bExtendMap,unsigned long extenderIp);
	//modify end
//    void refreshNetgearInfo();
    void setExitFlag();
    void blockDeviceCmd(int cmd,QVariant para);
    QWidget *getDbgUi();
//    virtual void funcDataReturned(const SVT_NotifyType &data)=0;
//    virtual void countChanged(const int &count)=0;
signals:
    void signal_nodeDataReturned(SVT_NotifyType data);
    void signal_ifaceDataReturned(SVT_NotifyType data);
    void signal_countChanged(int count);
	//add by caiyujun
	void signal_otherDevCountChanged(QString deviceIp, int count);
	//add end
    void signal_gatewayChanged(QString gateway);
    void signal_newDeviceAdded(SVT_NotifyType data);
    void signal_NetgearInfo(QMap<int,QString> data);
    void signal_FSMStateChanged(int,int);
protected:
    void funcDataReturned( SVT_NotifyType &data);
    void funcIfaceReturned(SVT_NotifyType &data);
    void funcNewDeviceAdded(SVT_NotifyType &data);
    void countChanged( int count);
	//add by caiyujun
	void otherDeviceCountChanged(QString deviceIp, int count);
	//add end
    void funcGatewayChanged(const QString &gateway);
    void funcNetgearInfoReturned(const QMap<int,QString> data);

protected:
    SVT_Core *mCore;
    _InnerFSMController *mInnerFSMController;
    SVT_Tracer *mTracer;
    friend class SVT_ProgardThread;
    friend class SVT_NetgearInfoThread;
    friend class SVT_NetgearSpecificHelper;
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
class SVT_NetgearSpecificFSM;
class _InnerFSMController/*:public QObject*/
{
//    Q_OBJECT
    _InnerFSMController();
    QMutex mSetContentMutex;
    SVT_NetgearSpecificFSM *mFSM;
    void setFSM(SVT_NetgearSpecificFSM *fsm);
    void setCmd(int cmd,const QVariant &para);
//    void FSMCmd(int cmd,const QVariant &para);
signals:
//    void signal_FSMCmd(int cmd,QVariant para);
    friend class SVT_ControlProxy;
    friend class SVT_NetgearSpecificHelper;
};


#endif // SVTCONTROLPROXY_H

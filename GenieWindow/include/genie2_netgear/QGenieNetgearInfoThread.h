#ifndef QGENIENETGEARINFOTHREAD_H
#define QGENIENETGEARINFOTHREAD_H
#include <QThread>
#include "QGenieNetgearInterface.h"
#include "SvtNetworkTool.h"

class QGenieNetgearInfoThread:public IThreadInterface
{
public:
    QGenieNetgearInfoThread(QGenieNetgearInterface *inf);
    ~QGenieNetgearInfoThread();
    bool canProcess(int cmd);
//    void startGetInfo(int cmd,const QStringList &paras,int session);
protected:
    void getNetgearData(int cmd,const QStringList &reqlist,QStringList &retlist);
    void getCurrentSetting(QMap<int,QString> &retvalues);
    void doCmdCurrentSetting(const QStringList &reqlist,QStringList &retlist);
    void doCmdLogin(const QStringList &reqlist,QStringList &retlist);
    void doCmdWirelessInfo(const QStringList &reqlist,QStringList &retlist);
    void doCmdSetWireless(const QStringList &reqlist,QStringList &retlist);
    void doCmdGaccInfo(const QStringList &reqlist,QStringList &retlist);
    void doCmdSetGacc(const QStringList &reqlist,QStringList &retlist);
    void doCmdGetMacAddr(const QStringList &reqlist,QStringList &retlist);
    void doCmdGetGaccSsidAlive(const QStringList &reqlist,QStringList &retlist);
    void doCmdGetChannel(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMOption(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMStatistics(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMEnable(const QStringList &reqlist,QStringList &retlist);
    void doCmdSetTM(const QStringList &reqlist,QStringList &retlist);
    void doCmdReBoot(const QStringList &reqlist,QStringList &retlist);
    void doCmdSendSingleFinish(const QStringList &reqlist,QStringList &retlist);
    void doCmdCheckNewFirmware(const QStringList &reqlist,QStringList &retlist);//use soap
    void doCmdCheckNewFirmware2(const QStringList &reqlist,QStringList &retlist);//not use soap
    void doCmdUpdateNewFirmware(const QStringList &reqlist,QStringList &retlist);
    void doCmdBlockInterface(const QStringList &reqlist,QStringList &retlist);
    void doCmdIs5GOk(const QStringList &reqlist, QStringList &retlist);
    void doCmd5GInfo(const QStringList &reqlist,QStringList &retlist);
    void doCmdSet5GInfo(const QStringList &reqlist,QStringList &retlist);
    void doCmdAttachDevice(const QStringList &reqlist, QStringList &retlist);
    void doCmd5GGaccInfo(const QStringList &reqlist, QStringList &retlist);
    void doCmdSet5GGAccInfo(const QStringList &reqlist, QStringList &retlist);

    void doCmdRawSoap(const QStringList &reqlist,QStringList &retlist);
	//////////////////////////////////////kai.yan add////////////////////////////////////
	void doCmdGetQoSEnableStatus(const QStringList &reqlist,QStringList &retlist);
	void doCmdSetQoSEnableStatus(const QStringList &reqlist,QStringList &retlist);
	void doCmdGetQoSRules(const QStringList &reqlist, QStringList &retlist);
	void doCmdAddQoSRule(const QStringList &reqlist, QStringList &retlist);
	void doCmdAddQoSRuleByEhernetPort(const QStringList &reqlist, QStringList & retlist);
	void doCmdAddQoSRuleByMac(const QStringList &reqlist, QStringList &retlist);
	void doCmdDeleteQoSRule(const QStringList &reqlist, QStringList &retlist);
	void docmdUpdateQoSPrority(const QStringList &reqlist, QStringList &retlist);
	//////////////////////////////////////kai.yan end////////////////////////////////////
protected:


    SVT_Soap mSoap;
//    friend class QGenieNetgearInterface_todelete;
};

#endif // QGENIENETGEARINFOTHREAD_H

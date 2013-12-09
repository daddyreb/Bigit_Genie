#ifndef QGENIEDELAYPROCESSLOGIC_H
#define QGENIEDELAYPROCESSLOGIC_H

#include <QVariant>
#include "Singleton.h"
#define ALL_LOCAL_CMD (RCF_TRUNTO_CHANEL|RCF_TURNTO_GACC|RCF_TURNTO_FUPDATE|RCF_TURNTO_PWD)
#define ALL_REMOTE_CMD (RCF_TRUNTO_CHANEL_REMOTE|RCF_TURNTO_GACC_REMOTE|RCF_TURNTO_PWD_REMOTE)

class QGenieDelayProcessLogic:public Singleton<QGenieDelayProcessLogic>
{
public:


    QGenieDelayProcessLogic();
//    void setCmd(quint32 cmd);
    void parseOuterCommand(int cmd,const QVariant &paras);
//    void turnToReadyshare();
//    void trunToOrigin();
    void whenLoginButton();
    void whenLogout(bool remote);
//    void whenLoginEnd(bool ok, bool remote);
//    void whenGetWirelessInfoEnd(bool b);
//    void whenGetGaccLocalInfoEnd(bool b);
    void whenSettingProcessEnd(); 
    void wirelessExportReturn(const QVariant &paras);

//    DELAY_COMMAND mCmd;
    void tryProcessPendedCmd();


};

#endif // QGENIEDELAYPROCESSLOGIC_H

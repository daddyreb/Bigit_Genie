#include "QGenieRCSoapProxy.h"
#include "ISoapManager.h"
#include "mainwindow.h"
#include "IManagerHandler.h"
#include "ILogManager.h"
#include "IClickManager.h"
#include <QUuid>

struct waitNode
{
    QString model;
    QString method;
    QMap<QString,QString> paras;
    int wait;
};

static QList<waitNode> sSelfCmd;


QGenieRCSoapProxy::QGenieRCSoapProxy(QObject *parent) :
    QObject(parent)
{
    mWireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(mWireless /*&& mWirelessInfo && mWirelessSetting*/);

    mLogin=QGenieRCPageLogin::GetSingletonPtr();
    ASSERT(mLogin);

    ISoapManager *soapmanager= MainWindow::sHandler->getSoapManager();
    ASSERT(soapmanager);
    mSoapApi=soapmanager->getSoapAPI();
    ASSERT(mSoapApi);
    connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(slot_GetSoapResponse(QtSoapMessage&,QUuid&,QString&)));
    connect(mSoapApi,SIGNAL(signal_SoapError(QUuid&,QString&)),this,SLOT(slot_SoapError(QUuid&,QString&)));
//    connect(&mSelfCommitTimer,SIGNAL(timeout()),this,SLOT(slot_selfCommitTimeout()));

}

QGenieRCSoapProxy::~QGenieRCSoapProxy()
{
//    mSelfCommitTimer.stop();
    disconnect(mSoapApi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
}

void QGenieRCSoapProxy::command(int cmd, QMap<QString, QString> *paras)
{
 //   connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(GetSoapResponse(QtSoapMessage&,QUuid&,QString&)));

    const char *cFunctions[][2]=
    {
        {"ParentalControl","Authenticate"},                 //0 a
        {"WLANConfiguration","GetWPASecurityKeys"},         //1 b
        {"WLANConfiguration","GetInfo"},                    //2 c
        {"WLANConfiguration","SetWLANNoSecurity"},          //3 d
        {"WLANConfiguration","SetWLANWPAPSKByPassphrase"},  //4 e
        {"WLANConfiguration","GetGuestAccessEnabled"},      //5 f
        {"WLANConfiguration","GetGuestAccessNetworkInfo"},  //6 g
        {"WLANConfiguration","SetGuestAccessEnabled"},      //7 h
        {"WLANConfiguration","SetGuestAccessEnabled2"},      //8 i
        {"WLANConfiguration","SetGuestAccessNetwork"},      //9 j
        {"WLANConfiguration","GetGuestAccessEnabled"},      //10 k
        {"WLANConfiguration","GetInfo"},
    };

    const char *cCmdType[ESC_NUM+20]={ "a","b","c","d","e","f","g","h","i","j","k","l","m"};
    static int sTypeCounter=0;
    sTypeCounter++;
    mCurType[cmd]=cCmdType[cmd]+QString::number(sTypeCounter);
    QMap<QString,QString> tempmap;
    if(!paras)
    {
        paras=&tempmap;
    }


    QString modename=cFunctions[cmd][0];
    QString method=cFunctions[cmd][1];


    QString id =mCurType[cmd];
    QUuid uuid(UUID_OF_THIS);
    QString logstr;

#ifdef SUBMIT_FINISH_AFTER_REBOOT
        if(cmd == ESC_APPLY_CHANGES_NOSECURE || cmd == ESC_APPLY_CHANGES_WITHSECURE
                ||cmd ==ESC_GUESTACCESS_SETENABLE ||cmd == ESC_GUESTACCESS_SETINFO2
                ||cmd ==ESC_GUESTACCESS_SETINFO)
        {
            QGenieRCLogic::mPendingCmd |= SLP_SUBMIT_SOAP_FINISH;
        }
#endif


    mSoapApi->submitrequest(modename,method,*paras,uuid,id);
    logstr="mSoapApi->submitrequest ";

    qDebug()<<"mSoapApi->submitrequest"<<modename<<method<<(*paras)<<id;
    ILogManager *logmanager=MainWindow::sHandler->getLogManager();
    logstr+=QString("soap method %1-%2 ").arg(modename,method);
    QMapIterator<QString,QString> i(*paras);
    while(i.hasNext())
    {
        i.next();
        QString stemp="[%1,%2]";
        logstr += stemp.arg(i.key(),i.value());
    }
    logstr+=QString(" ")+id;
    logmanager->writeLog(logstr,QUuid(UUID_OF_THIS));
    if(cmd == ESC_GUESTACCESS_SETENABLE ||
       cmd == ESC_GUESTACCESS_SETINFO2 ||
       cmd == ESC_APPLY_CHANGES_NOSECURE ||
       cmd == ESC_APPLY_CHANGES_WITHSECURE)
    {
        STATISTIC();
    }

}
/*
void QGenieRCSoapProxy::commitUseSelfClass(QString &modulename, QString &methodname, QMap<QString, QString> &params)
{
    if(!mSelfSoap5000)
    {
        mSelfSoap5000=new NetgearSoapAPI();
        mSelfSoap5000->setHost("routerlogin.net", 5000);
    }
    if(!mSelfSoap80)
    {
        mSelfSoap80=new NetgearSoapAPI();
        mSelfSoap80->setHost("routerlogin.net");
    }
    QMap<QString, QString> tempparas;
    tempparas.insert("NewSessionID","E6A88AE69687E58D9A00");

    QString sDeviceConfig="DeviceConfig";
    QString sConfigStarted="ConfigurationStarted";
    QString sConfigEnded="ConfigurationFinished";

    waitNode node;
    node.model=sDeviceConfig;
    node.method=sConfigStarted;
    node.paras=tempparas;
    node.wait=6000;
    sSelfCmd.append(node);

    node.model=modulename;
    node.method=methodname;
    node.paras=params;
    node.wait=6000;
    sSelfCmd.append(node);

    tempparas.clear();
    tempparas.insert("NewStatus","ChangesApplied");
    node.model=sDeviceConfig;
    node.method=sConfigEnded;
    node.paras=tempparas;
    node.wait=0;
    sSelfCmd.append(node);
#ifdef SUBMIT_FINISH_AFTER_REBOOT
    QGenieRCLogic::mPendingCmd |= SLP_SUBMIT_SOAP_FINISH;
#endif
    slot_selfCommitTimeout();


}
*/
#ifdef SUBMIT_FINISH_AFTER_REBOOT
void QGenieRCSoapProxy::submitFinishCommandAfterReboot()
{
    mSoapApi->sendFinishCmd();
}
#endif

//void QGenieRCSoapProxy::slot_selfCommitTimeout()
//{

//    qDebug()<<"QGenieRCSoapProxy::slot_selfCommitTimeout()";
//    if(sSelfCmd.isEmpty())
//    {
//        mSelfCommitTimer.stop();
//    }
//    else
//    {
//        waitNode node=sSelfCmd.takeFirst();
//        qDebug()<<node.model<<node.method<<node.paras;
//        mSelfSoap80->submitrequest(node.model,node.method,node.paras);
//        mSelfSoap5000->submitrequest(node.model,node.method,node.paras);
//        mSelfCommitTimer.stop();
//        if(node.wait>0)
//        {
//            mSelfCommitTimer.start(node.wait);
//        }
//    }
//}

void QGenieRCSoapProxy::ignoreAllRequest()
{
    QGenieRCLogic logic;
    logic.writeLog("ignore all soap request");
    for(int i=0;i<ESC_NUM;i++)
    {
        mCurType[i]="";
    }
}

void QGenieRCSoapProxy::ignoreRequest(int cmd)
{
    ASSERT(cmd >=ESC_LOGIN_PWD && cmd <ESC_NUM);
    if(cmd >=ESC_LOGIN_PWD && cmd <ESC_NUM)
    {
        mCurType[cmd]="";
    }
}

ISoapClient *QGenieRCSoapProxy::getSoapClient(const QString &id,int *cmd)
{
    int itemp=-1;
    for(int i=0;i<ESC_NUM;i++)
    {
        if(mCurType[i]==id)
        {
            itemp=i;
            break;
        }
    }
    if(cmd)
    {
        *cmd=(0<=itemp&&itemp<ESC_NUM)?itemp:-1;
    }

    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    ISoapClient *cFindClientHelper[ESC_NUM]={
        mLogin,mWireless,mWireless,mWireless,mWireless,
        gacc,gacc,gacc,gacc,gacc,gacc,gacc
    };
    return itemp<ESC_NUM?cFindClientHelper[itemp]:NULL;

}

void QGenieRCSoapProxy::slot_GetSoapResponse(QtSoapMessage &message, QUuid &uuid, QString &id)
{
    if(QUuid(UUID_OF_THIS) !=uuid)
    {
        return;
    }
    QGenieRCLogic logic;
    logic.writeLog(QString("QGenieRCSoapProxy::GetSoapResponse ")+id);

    qDebug()<<"QGenieRCSoapProxy::GetSoapResponse"<<id;

    int cmd=-1;
    ISoapClient *soapclient=getSoapClient(id,&cmd);
    if(soapclient && 0<=cmd && cmd < ESC_NUM)
    {
        soapclient->parseSoapResponse(cmd,message);
    }
    else
    {
        qDebug()<<"ignore this response!!!!";
        logic.writeLog(QString("ignore request ")+id);
    }

}

void QGenieRCSoapProxy::slot_SoapError(QUuid &uuid,QString& id)
{
    if(QUuid(UUID_OF_THIS) !=uuid)
    {
        return;
    }
    QGenieRCLogic logic;
    logic.writeLog(QString("QGenieRCSoapProxy::slot_SoapError ")+id);

    int cmd=-1;
    ISoapClient *soapclient=getSoapClient(id,&cmd);
    if(soapclient && 0<=cmd && cmd < ESC_NUM)
    {
        soapclient->whenSoapError(cmd,"");
    }
    else
    {
        logic.writeLog(QString("can not find ISoapClientWhen Soap Error ")+id);
    }
}

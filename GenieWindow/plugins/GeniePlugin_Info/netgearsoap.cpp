#include <QtGui>
#include "netgearsoap.h"
//#include "GetInformation.h"
#include "GeniePlugin_Info.h"

//#include "IUiManager.h"


IManagerHandler *NetgearSoap::sHandler = NULL;

NetgearSoap::NetgearSoap()
{
    mUuid = "f9b01961-e3b2-474c-8521-42371d2cab61";

    //connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&)), this, SLOT(dealMACData(QtSoapMessage&,QUuid&)));

    setSoapFlag = true;
    //mSoapManager = NetgearSoap::sHandler->getSoapManager();
    //mSoapApi = mSoapManager->getSoapAPI();
    //connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)), this, SLOT(dealMACData(QtSoapMessage&,QUuid&,QString&)));

    intID = 1;

}

NetgearSoap::~NetgearSoap()
{

}

void NetgearSoap::getMacAddr(const QString &para)
{
    GeniePlugin_Info::writeLog("SOAP:Start get NETGEAR router MAC.");
//    if(NetgearSoap::sHandler==NULL)
//    {
//        qDebug()<<"test2";
//    }

    if (setSoapFlag){
        mSoapManager = NetgearSoap::sHandler->getSoapManager();
        mSoapApi = mSoapManager->getSoapAPI();
        connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),
                this, SLOT(dealMACData(QtSoapMessage&,QUuid&,QString&)));

        setSoapFlag = false;
    }

    //disconnect(mSoapApi,SIGNAL(getResponse(QtSoapMessage&, QUuid&)),0,0);
    QString ModeName = "LANConfigSecurity";
    QString MethodName = "GetInfo";
    QMap<QString , QString> params;
    //connect(mSoapApi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)), this, SLOT(dealMACData(QtSoapMessage&,QUuid&,QString&)));
    QString strInt;
    strInt.setNum(intID);
    getID = "getMac" + strInt;
    intID++;
    mSoapApi->submitrequest(ModeName, MethodName, params, mUuid, getID);

}

void NetgearSoap::dealMACData(QtSoapMessage &data, QUuid &uuid, QString &id)
{
    GeniePlugin_Info::writeLog("SOAP send signal:getResponse.");
    mac_addr.clear();

    if( uuid == mUuid && id == getID ){
        GeniePlugin_Info::writeLog("SOAP send signal:getResponse for function getMacAddr()."+getID);
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr ="NewLANSubnet,NewPassword,NewWANLAN_Subnet_Match,NewLANMACAddress";
        queryfieldlist = qureyfieldsstr.split(",");
        if ( data.isFault() ){
            qDebug("Error: %s", data.faultString().value().toString().toLatin1().constData());
            //QMessageBox::about(NULL,"Test","T");

            mac_addr = "NULL";
            //emit GetMACFinish(mac_addr);
        }else {
            const QtSoapType &res = data.returnValue();
            if ( data.returnResponseCode().toString().toInt() == 0){
                bool emptyflag;
                emptyflag = true;
                for (int i = 0; i < queryfieldlist.size(); ++i){
                    if ( queryfieldlist.at(i) =="NewLANMACAddress" ){
                        if ( emptyflag ){
                            mac_addr = res[queryfieldlist.at(i)].value().toString();

                            emptyflag = false;
                            //QMessageBox::about(NULL,"Test",mac_addr);

                            //emit GetMACFinish(mac_addr);
                        }
                    }
                }
                if ( emptyflag )
                    mac_addr = "NULL";
            }else {
                mac_addr = "NULL";
            }
        }

        QString wlog = "By SOAP, Get Netgear router MAC : " + mac_addr;
        GeniePlugin_Info::writeLog(wlog);
        emit GetMACFinish(mac_addr);

    }

}


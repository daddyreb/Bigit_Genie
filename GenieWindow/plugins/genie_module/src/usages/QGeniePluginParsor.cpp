#include "stable.h"



#define PASER_TOTAL_ERROR "TOTAL PARSE ERROR"
#define PARSE_UUID_ERROR "CAN NOT PARSE UUID"
#define PARSE_BIND_ERROR "CAN NOT PARSE BIND"
#define PARSE_HAVEWND_ERROR "CAN NOT PARSE HAVEWND"
#define PARSE_VERSION_ERROR "CAN NOT PARSE VERSION"
#define DUPLICATE_UID_ERROR "DUPLICATE UUID"
#define IN_BLACK_LIST_ERROR "ERROR- IN BLACKLIST"


enum INFO_FIELDS
{
    IFD_UUID=0,
    IFD_BIND,
    IFD_HAVEWND,
    IFD_VERSION,
    IFD_NUM
};


QGeniePluginParsor::QGeniePluginParsor(QObject *parent)
     :   QObject(parent)
    ,mPi(NULL)
{
}

 bool QGeniePluginParsor::parsePlugin(IPluginInf* pi, PlgMsg *msg)
 {
     ASSERT(pi && msg);
     mPi=pi;

     QStringList slst=pi->info().split(";");
     if(slst.size() < IFD_NUM)
     {
         mResult=PASER_TOTAL_ERROR;
         return false;
     }


     if(!parseUuid(slst.at(IFD_UUID),&(msg->uid))) return false;
     if(!parseBind(slst.at(IFD_BIND),&(msg->bind))) return false;
     if(!parseHaveWnd(slst.at(IFD_HAVEWND),&(msg->haveW))) return false;
     if(!parseVersion(slst.at(IFD_VERSION),&(msg->versionStr))) return false;

     return true;


 }
 bool QGeniePluginParsor::checkBlackList( QUuid uuid)
 {
     if(mBlackList.contains(uuid))
     {
         mResult=IN_BLACK_LIST_ERROR;
         return false;
     }
     return true;
 }

 bool QGeniePluginParsor::parseUuid(QString s, QUuid *uuid)
 {
     QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
     ASSERT(proxymanager);

     QUuid test=QUuid::createUuid();
     qDebug("uuid %s",test.toString().toLatin1().data());

     ASSERT(uuid);
     QStringList slst=s.split(":");
     if(slst.size() !=2 )
     {
         mResult=PARSE_UUID_ERROR;
         return false;
     }
     if(slst.at(0).trimmed().toUpper() != "UUID")
     {
         mResult=PARSE_UUID_ERROR;
         return false;
     }

     QUuid uidtemp=QUuid(slst.at(1).trimmed());
     if(uidtemp.isNull())
     {
         mResult=PARSE_UUID_ERROR;
         return false;
     }


     if(mUidList.contains(uidtemp))
     {
         mResult=DUPLICATE_UID_ERROR;
         return false;
     }

     if(!checkBlackList(uidtemp))
     {
         mResult=IN_BLACK_LIST_ERROR;
         return false;
     }
     mUidList.append(uidtemp);

//     if(uidtemp == QUuid("{6c7b332f-a107-40ab-84f3-2163ac13ad0a}"))
//     {
//         ASSERT(proxymanager->mPluginDiagnose == NULL);
//         ASSERT(mPi);
//         proxymanager->mPluginDiagnose=mPi;
//
//     }

     *uuid=uidtemp;
     return true;
 }

 bool QGeniePluginParsor::parseBind(QString s, bool *bind)
 {
     ASSERT(bind);
     QStringList slst=s.split(":");
     if(slst.size() !=2)
     {
         mResult=PARSE_BIND_ERROR;
         return false;
     }
     if(slst.at(0).trimmed().toUpper() != "BIND")
     {
         mResult=PARSE_BIND_ERROR;
         return false;
     }

     QString temp=slst.at(1).trimmed();
     *bind =(temp=="1");
     return true;

 }

 bool QGeniePluginParsor::parseHaveWnd(QString s, bool *haveWnd)
 {
     ASSERT(haveWnd);
     QStringList slst=s.split(":");
     if(slst.size() !=2)
     {
         mResult=PARSE_HAVEWND_ERROR;
         return false;
     }
     if(slst.at(0).trimmed().toUpper() != "HAVEWND")
     {
         mResult=PARSE_HAVEWND_ERROR;
         return false;
     }

     QString temp=slst.at(1).trimmed();
     *haveWnd= (temp=="1");
     return true;
 }

 bool QGeniePluginParsor::parseVersion(QString s, QString *version)
 {
     ASSERT(version);
     QStringList slst=s.split(":");
     if(slst.size() !=2)
     {
         mResult=PARSE_VERSION_ERROR;
         return false;
     }
     if(slst.at(0).trimmed().toUpper() != "VERSION")
     {
         mResult=PARSE_VERSION_ERROR;
         return false;
     }
     QString vs=slst.at(1).trimmed();
     *version=vs;
     int vi=vs.replace(".","").toInt();
     int vn=QString(VERSION_STRING).replace(".","").toInt();
     if(vi/100 != vn/100)
     {
         qDebug()<<"plugin version wrong";
         GENIE_LOG(QString("plugin version wrong %1").arg(vs));
         mResult=PARSE_VERSION_ERROR;
         return false;
     }

     return true;

 }

 void QGeniePluginParsor::clearResult()
 {
     mResult="";
     mPi=NULL;

 }

 void QGeniePluginParsor::clearUidCache()
 {
     mUidList.clear();
 }


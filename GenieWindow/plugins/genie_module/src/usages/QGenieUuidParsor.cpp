#include "QGeniePluginParsor.h"
#include "IPluginInf.h"
#include "QGenieStackManager.h"
#include <QString>
#include <QStringList>

struct PlgMsg{
        QUuid uid;
        bool bind;
        bool haveW;
        QString versionStr;
};

#define PASER_TOTAL_ERROR "TOTAL PARSE ERROR"
#define PARSE_UUID_ERROR "CAN NOT PARSE UUID"
#define PARSE_BIND_ERROR "CAN NOT PARSE BIND"
#define PARSE_HAVEWND_ERROR "CAN NOT PARSE HAVEWND"
#define PARSE_VERSION_ERROR "CAN NOT PARSE VERSION"


enum INFO_FIELDS
{
    IFD_UUID=0,
    IFD_BIND,
    IFD_HAVEWND,
    IFD_VERSION,
    IFD_NUM
};


QGeniePluginParsor::QGeniePluginParsor()
{
}

 bool QGeniePluginParsor::parsePlugin(IPluginInf* pi, PlgMsg *msg)
 {
     ASSERT(pi && msg);
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

 bool QGeniePluginParsor::parseUuid(QString s, QUuid *uuid)
 {
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

     *uuid=QUuid(slst.at(1).trimmed());
     return true;
 }

 bool QGeniePluginParsor::parseBind(QString s, bool *bind)
 {
     ASSERT(bind);
     QStringList slst=s.split(":");
     if(slst.size() !=2)
     {
         mResult=PARSE_BIND_ERROR
         return false;
     }
     if(slst.at(0).trimmed().toUpper() != "BIND")
     {
         mResult=PARSE_BIND_ERROR;
         return false;
     }

     QString temp=slst.at(1).trimmed();
     *bind (temp=="1");
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
     *version=slst.at(1).trimmed();
     return true;

 }

 void QGeniePluginParsor::cleanup()
 {
     mResult="";
 }


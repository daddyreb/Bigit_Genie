#include "netgearsoapschedule.h"

//todo 在几次失败的情况下要能够换端口 done
//todo 设置的时候要两个端口一起发消息 done

#define SOAP_HOST "routerlogin.net"

const quint16 cRetryPorts[]={80,5000};
const int cRetryTimes=sizeof(cRetryPorts)/sizeof(cRetryPorts[0]);


NetgearSoapSchedule::NetgearSoapSchedule(QObject *parent)
    :QObject(parent)
    ,mRetryStartIdx(0)
    ,mEnabled(true)
{
    for(int i=0;i<SOAP_NODE_READ_COUNT;i++)
    {
        mNodesForRead[i].state=SNS_ENDED;
        mNodesForRead[i].retrytime=0;
        mTimerNodeMap.insert(&mNodesForRead[i].timer,&mNodesForRead[i]);
        mSoapNodeMap.insert(&mNodesForRead[i].soaphttp,&mNodesForRead[i]);
        mNodesForRead[i].timer.setSingleShot(true);
        connect(&mNodesForRead[i].timer,SIGNAL(timeout()),this,SLOT(slot_nodeTimeout()));
        connect(&mNodesForRead[i].soaphttp,SIGNAL(responseReady()),this,SLOT(slot_getResponse()));
    }
    for(int i=0;i<SOAP_NODE_WRITE_COUNT;i++)
    {
        mNodesForWrite[i].state=SNS_ENDED;
        mNodesForWrite[i].retrytime=0;
        mTimerNodeMap.insert(&mNodesForWrite[i].timer,&mNodesForWrite[i]);
        mSoapNodeMap.insert(&mNodesForWrite[i].soaphttp,&mNodesForWrite[i]);
        mNodesForWrite[i].timer.setSingleShot(true);
        connect(&mNodesForWrite[i].timer,SIGNAL(timeout()),this,SLOT(slot_nodeTimeout()));
        connect(&mNodesForWrite[i].soaphttp,SIGNAL(responseReady()),this,SLOT(slot_getResponse()));
    }
    setHost(SOAP_HOST);
    mSendMode = GET_WITH_START;//GET_WITHOUT_START;
}

NetgearSoapSchedule::~NetgearSoapSchedule()
{
    stopAll();
}
void NetgearSoapSchedule::submitrequest(const QString &modulename, const QString &methodname,
                                   QMap<QString, QString> &params,QUuid &uuid,QString &id)
{
    if(!mEnabled){return;}
    iner_submitrequest(modulename,methodname,params,uuid,id,mRetryStartIdx,mRetryStartIdx);
}

void NetgearSoapSchedule::iner_submitrequest(const QString &modulename, const QString &methodname,
                                   QMap<QString, QString> &params,QUuid &uuid,QString &id,
                                             int times,int t_end)
{
    if(!mEnabled){return;}
    QRegExp rx("^Set");
    QRegExp rx2("^Enable");
    soap_node_option sno;
    sno=(methodname.contains(rx) || methodname.contains(rx2) || methodname.contains("UpdateNewFirmware") || methodname.contains("PressWPSPBC")
         ||methodname=="Reboot")
            ?SNO_WRITE:SNO_READ;

    soap_node *node=getEndedNode(sno);
    if(!node)
    {
        emit signal_SoapError(uuid,id);
        return;
    }
    node->uuid_cache=uuid;
    node->id_cache=id;
    node->paras_cache=params;
    node->method_name=methodname;
    node->module_name=modulename;


//    if(SNO_READ==node->option)
//    {
//        node->retrytime=times;
//        node->retrytime_end=t_end;
//      node->soaphttp.setHost(m_hostname,cRetryPorts[node->retrytime]);
//        doNextStep(node);
//    }
//    else{
        soap_node *othernode=getEndedNode(sno);
        if(!othernode)
        {
            node->state=SNS_ENDED;
            emit signal_SoapError(uuid,id);
            return;
        }
//        Q_ASSERT(othernode->option==SNO_WRITE );
        othernode->uuid_cache=uuid;
        othernode->id_cache=id;
        othernode->method_name=methodname;
        othernode->module_name=modulename;
        othernode->paras_cache=params;
        node->retrytime=0;
        othernode->retrytime=1;
        node->soaphttp.setHost(m_hostname,cRetryPorts[node->retrytime]);
        doNextStep(node);
        othernode->soaphttp.setHost(m_hostname,cRetryPorts[othernode->retrytime]);
        doNextStep(othernode);
//    }
}

void NetgearSoapSchedule::sendFinishCmd()
{
    if(!mEnabled){return;}
    soap_node *node=getEndedNode(SNO_WRITE_FINISH);
    soap_node *othernode=getEndedNode(SNO_WRITE_FINISH);
    if(node && othernode)
    {
        node->retrytime=0;
        othernode->retrytime=1;
        node->soaphttp.setHost(m_hostname,cRetryPorts[node->retrytime]);
        doNextStep(node);
        othernode->soaphttp.setHost(m_hostname,cRetryPorts[othernode->retrytime]);
        doNextStep(othernode);
    }
    else{
        if(node)
        {
            node->state=SNS_ENDED;
        }
        if(othernode)
        {
            othernode->state=SNS_ENDED;
        }
    }
}

void NetgearSoapSchedule::stopAll()
{
    mEnabled=false;
    for(int i=0;i<SOAP_NODE_READ_COUNT;i++)
    {
        mNodesForRead[i].timer.stop();
        disconnect(&mNodesForRead[i].timer,SIGNAL(timeout()),0,0);
        disconnect(&mNodesForRead[i].soaphttp,SIGNAL(responseReady()),0,0);
        mNodesForRead[i].soaphttp.abort();
    }
    for(int i=0;i<SOAP_NODE_WRITE_COUNT;i++)
    {
        mNodesForWrite[i].timer.stop();
        disconnect(&mNodesForWrite[i].timer,SIGNAL(timeout()),0,0);
        disconnect(&mNodesForWrite[i].soaphttp,SIGNAL(responseReady()),0,0);
        mNodesForWrite[i].soaphttp.abort();
    }
}

void NetgearSoapSchedule::real_submitrequest(soap_node *node)
{
    if(!mEnabled){return;}
    QString actionstr;

    QString replaceModName="DeviceConfig";
    QString replaceFuncName=(SNS_STARTSET==node->state)?"ConfigurationStarted":"ConfigurationFinished";
    QString modname=(SNS_CODE==node->state)?node->module_name:replaceModName;
    QString funcname=(SNS_CODE==node->state)?node->method_name:replaceFuncName;
    actionstr = QString("urn:NETGEAR-ROUTER:service:%1:1#%2").arg(modname).arg(funcname);
    node->soaphttp.setAction(actionstr);
    QString paramstr;
    QString headerstr;
    if(SNS_STARTSET==node->state)
    {
        paramstr="<NewSessionID>E6A88AE69687E58D9A00</NewSessionID>";
    }
    else if(SNS_CODE==node->state)
    {
        if(node->module_name.compare("ParentalControl"))
        {
            headerstr = QString("<SOAP-ENV:Header><SessionID>E6A88AE69687E58D9A00</SessionID></SOAP-ENV:Header>");
            QMapIterator<QString, QString> i(node->paras_cache);
            while (i.hasNext()) {
                i.next();
                QString tmp;
                tmp = QString("<%1>%2</%1>").arg(i.key()).arg(i.value());
                paramstr += tmp;
            }
        }
        else
        {
            QtSoapMessage request;
            QString uristr;
            request.addHeaderItem(new QtSoapSimpleType(QtSoapQName("SessionID", uristr), "E6A88AE69687E58D9A00"));
            request.setFlag(cRetryPorts[node->retrytime] == 80);

            request.setMethod(QtSoapQName(node->method_name, uristr));

            QMapIterator<QString, QString> i(node->paras_cache);
            while (i.hasNext()) {
                i.next();
                request.addMethodArgument(i.key(), "", i.value());
            }

            node->soaphttp.submitRequest(request, "/soap/server_sa/");
            return;
        }
    }
    else if(SNS_FINISHSET==node->state)
    {
      //  node->soaphttp.abort();
        headerstr = QString("<SOAP-ENV:Header><SessionID>E6A88AE69687E58D9A00</SessionID></SOAP-ENV:Header>");
        paramstr ="<NewStatus>ChangesApplied</NewStatus>";
    }
    else
    {
        Q_ASSERT(0);
    }

    QString request = QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                      "<SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" "
                        "xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" "
                        "xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" "
                        "xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">"
                      "%4"
                        "<SOAP-ENV:Body>"
                      "<M1:%2 xmlns:M1=\"urn:NETGEAR-ROUTER:service:%1:1\">"
                      "%3"
                      "</M1:%2>"
                      "</SOAP-ENV:Body></SOAP-ENV:Envelope>")
            .arg(modname).arg(funcname).arg(paramstr).arg(headerstr);


    node->soaphttp.submitRequest(request, "/soap/server_sa/");
}

void NetgearSoapSchedule::doNextStep(soap_node *node)
{
    if(!mEnabled){return;}
    Q_ASSERT(node);
    const int c_state_map[SNO_NUM][SNS_NUM]={
   // SNS_READY     STARTSET,    SNS_CODE     FINISHSET   SNS_TIMEOUT   SNS_ENDED
//      {SNS_CODE     ,-1         ,SNS_ENDED    ,-1         , SNS_ENDED   ,SNS_ENDED},
      {SNS_STARTSET ,SNS_CODE   ,SNS_ENDED,   -1  , SNS_ENDED          ,SNS_ENDED},
      {SNS_STARTSET ,SNS_CODE   ,SNS_FINISHSET,SNS_ENDED  , -1          ,SNS_ENDED},
      {SNS_FINISHSET,-1         ,-1           ,SNS_ENDED  , -1          ,SNS_ENDED}
    };
    const int c_timer_map[SNO_NUM][SNS_NUM]={
//      {-1           ,-1         ,10*1000      ,-1         , -1           ,-1      },
      {-1           ,2*1000     ,6*1000      ,-1          , -1           ,-1      } ,
      {-1           ,6*1000     ,6*1000      ,5*1000          , -1           ,-1      } ,
      {-1           ,-1         ,-1           ,5*1000          , -1           ,-1      }
    };
    int nextstate=c_state_map[node->option][node->state];
    node->state=(soap_node_state)nextstate;
    Q_ASSERT(-1!=nextstate);
    if( node->option == SNO_READ && node->state == SNS_STARTSET)
        {
            if(mSendMode == GET_WITHOUT_START || node->module_name.compare("ParentalControl") == 0)
                {
                    nextstate=c_state_map[node->option][node->state];
                    node->state=(soap_node_state)nextstate;
                    Q_ASSERT(-1!=nextstate);
                }
        }

    int nti=c_timer_map[node->option][node->state];

    Q_ASSERT(0<=node->retrytime && node->retrytime<cRetryTimes);
  //  if(SNO_READ==node->option)
    {
        node->soaphttp.setHost(m_hostname,cRetryPorts[node->retrytime]);
        qDebug()<<"sethost"<<m_hostname<<cRetryPorts[node->retrytime];
    }

    QtSoapMessage request;
    switch(node->state)
    {
    case SNS_CODE:
    case SNS_STARTSET:
    case SNS_FINISHSET:
        real_submitrequest(node);
        break;
    case SNS_ENDED:
        node->paras_cache.clear();
        node->soaphttp.abort();
        break;
    default:
        break;
    }
    node->timer.stop();
    if(nti >=0)
    {
        node->timer.setInterval(nti);
        node->timer.start();
    }
}

void NetgearSoapSchedule::setHost(const QString &host)
{
    if(!mEnabled){return;}
    m_hostname=host;
}

void NetgearSoapSchedule::tryChangePortOrSendError(soap_node *node)
{
    if(!mEnabled){return;}
    node->soaphttp.abort();
    node->timer.stop();
    node->state=SNS_TIMEOUT;

    node->retrytime=(node->retrytime+1)%cRetryTimes;
    if(node->retrytime !=node->retrytime_end)
    {
//      m_port=cRetryPorts[node->retrytime];
      iner_submitrequest(node->module_name,node->method_name,node->paras_cache,
                    node->uuid_cache,node->id_cache,node->retrytime,node->retrytime_end);
    }
    else
    {

        emit signal_SoapError(node->uuid_cache,node->id_cache);
    }
}

soap_node *NetgearSoapSchedule::getEndedNode(soap_node_option option)
{
    if(!mEnabled){return NULL;}
    soap_node *ret=NULL;

    const int n=(SNO_READ==option)?SOAP_NODE_READ_COUNT:SOAP_NODE_WRITE_COUNT;
    soap_node *loop=(SNO_READ==option)?(mNodesForRead):(mNodesForWrite);
    for(int i=0;i<n;i++)
    {
        if(SNS_ENDED== loop->state)
        {
            ret=loop;
            break;
        }
        loop++;
    }
    if(ret)
    {
        Q_ASSERT(SNS_ENDED== ret->state);
        Q_ASSERT(!ret->timer.isActive());
        ret->option=option;
        ret->state=SNS_READY;
    }
    return ret;
}

void NetgearSoapSchedule::slot_nodeTimeout()
{
    if(!mEnabled){return;}
    QTimer *timer=qobject_cast<QTimer *>(sender());
    Q_ASSERT(timer);
    soap_node *node=mTimerNodeMap.value(timer);
    Q_ASSERT(node);
    Q_ASSERT(node->state==SNS_CODE || node->state==SNS_STARTSET ||node->state==SNS_FINISHSET);
//    if(node->option==SNO_READ )
//    {
//        Q_ASSERT(node->state==SNS_CODE);
//        tryChangePortOrSendError(node);
//
//    }
    doNextStep(node);

}

void NetgearSoapSchedule::slot_getResponse()
{
    if(!mEnabled){return;}
    QtSoapHttpTransport *soaphttp=qobject_cast<QtSoapHttpTransport *>(sender());
    Q_ASSERT(soaphttp);
    soap_node *node=mSoapNodeMap.value(soaphttp);
    Q_ASSERT(node);

//    if(node->option==SNO_READ && node->state==SNS_CODE)
    if(node->state==SNS_CODE)
    {
        const QtSoapMessage &message = node->soaphttp.getResponse();
        if(message.isFault())
        {
//            tryChangePortOrSendError(node);
        }
        else
        {
            mRetryStartIdx=node->retrytime;
            QtSoapMessage m=message;
            emit getResponse(m,node->uuid_cache,node->id_cache);
        }
        doNextStep(node);
    }
}

void NetgearSoapSchedule::setRouteType(const QString &rts)
{
    QRegExp rx("^CG");
//    QRegExp rx2("^DG");
    QRegExp rx2("^D");
    if(rts.contains(rx)||rts.contains(rx2))
    {
        mSendMode =  GET_WITH_START;
    }
    else
    {
        mSendMode = GET_WITHOUT_START;
    }
}

#include "netgearsoapschedule.h"

QString SoapOptcode::m_modulenamedeviceconfig = "DeviceConfig";
QString SoapOptcode::m_methodnamestart = "ConfigurationStarted";
QString SoapOptcode::m_methodnamefinish = "ConfigurationFinished";
QMap<QString, QString> SoapOptcode::m_startparams;
QMap<QString, QString> SoapOptcode::m_finishparams;

#ifndef TESTSOAPAPI
#include "ILogManager.h"
#endif

SoapOptcode::SoapOptcode()
{
        if(SoapOptcode::m_startparams.isEmpty())
        {
            SoapOptcode::m_startparams.insert("NewSessionID","E6A88AE69687E58D9A00");
        }
        if(SoapOptcode::m_finishparams.isEmpty())
        {
            SoapOptcode::m_finishparams.insert("NewStatus","ChangesApplied");
        }
}

SoapOptcode::SoapOptcode(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id, OPTTYPE_E opttype)
    :m_modulename(modulename),m_methodname(methodname),m_params(params),m_uuid(uuid), m_id(id),m_opttype(opttype)
{
    m_status = STARTED;
    if(SoapOptcode::m_startparams.isEmpty())
    {
        SoapOptcode::m_startparams.insert("NewSessionID","99999999999");
    }
    if(SoapOptcode::m_finishparams.isEmpty())
    {
        SoapOptcode::m_finishparams.insert("NewStatus","ChangesApplied");
    }
}

void SoapOptcode::setSoapParams(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id, OPTTYPE_E opttype)
{
    m_modulename = modulename;
    m_methodname= methodname;
    m_params = params;
    m_uuid = uuid;
    m_id = id;
    m_opttype = opttype;
    if(m_opttype == READ)
    {
        m_status = SETCODE;
    }
    else  if(m_opttype == WRITE)
    {
        m_status = STARTED;
    }
}

bool SoapOptcode::nextstatus()
{
    if(m_status == END)
    {
        return false;
    }

    if(m_opttype == READ)
    {
        m_status = END;
        return false;
    }

    if(m_status == STARTED)
    {
        m_status = SETCODE;
        return true;
    }
    else  if(m_status == SETCODE)
    {
        m_status = FINISHED;
        return true;
    }
    else  if(m_status == FINISHED)
    {
        m_status = END;
        return false;
    }
	return false;
}

void SoapOptcode::getSoapParams(QString &modulename, QString &methodname, QMap<QString, QString> &params)
{
    if(m_opttype == WRITE)
    {
        if(m_status == STARTED)
        {
            modulename = m_modulenamedeviceconfig;
            methodname =  m_methodnamestart;
            params = m_startparams;
        }
        else if(m_status == FINISHED)
        {
            modulename = m_modulenamedeviceconfig;
            methodname =  m_methodnamefinish;
            params = m_finishparams;
        }
        else
        {
            modulename = m_modulename;
            methodname= m_methodname;
            params = m_params;
        }
    }
    else
    {
        modulename = m_modulename;
        methodname= m_methodname;
        params = m_params;
    }
}

SoapOptcodeQueue::SoapOptcodeQueue()
{
}

SoapOptcodeQueue::~SoapOptcodeQueue()
{
    int i = 0;
    SoapOptcode * tmp;
    for(i=0; i < optcodelist.size(); i++)
    {
        tmp = optcodelist.value(i);
        if(tmp)
        {
            delete tmp;
        }
    }
    for(i=0; i < freelist.size(); i++)
    {
        tmp = freelist.value(i);
        if(tmp)
        {
            delete tmp;
        }
    }
}

QList<SoapOptcode *> SoapOptcodeQueue::freelist;

void SoapOptcodeQueue::insert(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id)//, OPTTYPE_E opttype)
{
    SoapOptcode * tmp;
    OPTTYPE_E opttype;
//    if(methodname.contains("Get"))
//    {
//        opttype = READ;
//    }
//    else
//    {
//        opttype = WRITE;
//    }
//    opttype = WRITE;
    QRegExp rx("^Set");
    QRegExp rx2("^Enable");
    if(methodname.contains(rx) || methodname.contains(rx2) || methodname.contains("UpdateNewFirmware") || methodname.contains("PressWPSPBC"))
    {
        opttype = WRITE;
    }
    else
    {
        opttype = READ;
    }

    if(freelist.isEmpty())
    {
       tmp =  new SoapOptcode();
    }
    else
    {
        tmp = freelist.front();
        freelist.pop_front();
    }
    tmp->setSoapParams(modulename, methodname, params, uuid, id, opttype);
    optcodelist.append(tmp);
}

bool SoapOptcodeQueue::nextOptcode()
{
    SoapOptcode * currentoptcode;
    if(optcodelist.isEmpty())
    {
        return false;
    }
    else
    {
        currentoptcode = optcodelist.front();
        if(currentoptcode)
        {
            if(!currentoptcode->nextstatus())
            {
                freelist.append(currentoptcode);
                optcodelist.pop_front();
            }
        }
        else
        {
            return false;
        }
    }
	return false;
}

SoapOptcode * SoapOptcodeQueue::getSoapOptcode()
{
    if(optcodelist.isEmpty())
        return NULL;
    else
        return optcodelist.front();
}

NetgearSoapSchedule::NetgearSoapSchedule()
#ifndef TESTSOAPAPI
    :mLogmanager(NULL)
#endif
{
    m_eventloop = NULL;
    m_timer = NULL;
//    m_timer2 =  new QTimer;
    m_netgearsoap_80 = NULL;
    m_netgearsoap_5000 = NULL;
    m_netgearsoap = NULL;
    flag = 0;
    sendflag = false;
    sendstatus=NOTSEND;
}

#ifndef TESTSOAPAPI
void NetgearSoapSchedule::setLogManager(ILogManager *p)
{
    mLogmanager=p;
}
#endif

NetgearSoapSchedule::~NetgearSoapSchedule()
{
    if(m_eventloop)
    {
        m_eventloop->exit(0);
        delete m_eventloop;
        m_eventloop = NULL;
    }
    if(m_timer)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = NULL;
    }
//    if(m_timer2)
//    {
//        m_timer2->stop();
//        delete m_timer2 ;
//        m_timer2 = NULL;
//    }
    if(m_netgearsoap_5000)
    {
        disconnect(m_netgearsoap_5000, 0, 0, 0);
        m_netgearsoap_5000->abort();
        delete m_netgearsoap_5000;
        m_netgearsoap_5000 = NULL;
    }
    if(m_netgearsoap_80)
    {
        disconnect(m_netgearsoap_80, 0, 0, 0);
        m_netgearsoap_80->abort();
        delete m_netgearsoap_80;
        m_netgearsoap_80 = NULL;
    }
    if(m_netgearsoap)
    {
        m_netgearsoap = NULL;
    }
}

void NetgearSoapSchedule::run()
{
    m_netgearsoap_5000 = new NetgearSoapAPI;
    m_netgearsoap_5000->setHost("routerlogin.net", 5000);
    m_netgearsoap_80 = new NetgearSoapAPI;
    m_netgearsoap_80->setHost("routerlogin.net");
    xorptr = (int)m_netgearsoap_80 ^ (int)m_netgearsoap_5000;
    m_netgearsoap = m_netgearsoap_5000;
    //m_eventloop = new QEventLoop;
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
//    connect(this, SIGNAL(submitrequestsignal(QString&,QString&,QMap<QString,QString>&,QUuid&,QString&)), \
//           this, SLOT(submitrequest(QString&,QString&,QMap<QString,QString>&,QUuid&,QString&)) );
//    connect(this, SIGNAL(submitrequesttosoapsignal(QString&,QString&,QMap<QString,QString>&)), \
//             m_netgearsoap, SLOT(submitrequest(QString&,QString&,QMap<QString,QString>&)));
    connect(m_netgearsoap, SIGNAL(getResponse(QtSoapMessage&)), this, SLOT(getResponseslot(QtSoapMessage&)));

    m_timer->start(GETINTERVAL);
}

void NetgearSoapSchedule::changeport()
{
    disconnect(m_netgearsoap, 0, this, 0);
//    disconnect(this, 0, m_netgearsoap, 0);
    m_netgearsoap = (NetgearSoapAPI *)((int)m_netgearsoap ^ xorptr);
//    connect(this, SIGNAL(submitrequestsignal(QString&,QString&,QMap<QString,QString>&,QUuid&,QString&)), \
//           this, SLOT(submitrequest(QString&,QString&,QMap<QString,QString>&,QUuid&,QString&)) );
//    connect(this, SIGNAL(submitrequesttosoapsignal(QString&,QString&,QMap<QString,QString>&)), \
//             m_netgearsoap, SLOT(submitrequest(QString&,QString&,QMap<QString,QString>&)));
    connect(m_netgearsoap, SIGNAL(getResponse(QtSoapMessage&)), this, SLOT(getResponseslot(QtSoapMessage&)));
}

void NetgearSoapSchedule::timeout()
{
    send();
//    m_netgearsoap->abort();
}

void NetgearSoapSchedule::getResponseslot(QtSoapMessage &message)
{
    static int optcount = OPTCOOUNT;
    static QUuid myuuid;
    int i = 0;
    if (message.isFault() && flag < POLLCOUNT) {

        if(sendstatus == STARTSEND || ((sendstatus == TIMEOUT || sendstatus == SENDERROR) && flag < POLLCOUNT) )
        {
            sendstatus = SENDERROR;
            flag++;
        }

//        QString resultstr = QString("Error: %1").arg(message.faultString().value().toString().toLatin1().constData());
        qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
    }
    else
    {
        QString modulename, methodname;
        QMap<QString, QString>params;
        QUuid uuid;
        QString id;
        SoapOptcode * tmp = soapoptcodequeue.getSoapOptcode();
        sendstatus = SUCCESSSEND;
        if(tmp != NULL)
        {
             tmp->getSoapParams(modulename, methodname, params);

            // emit submitrequesttosoapsignal(modulename, methodname, params);
             if((tmp->getStatus() == SETCODE) || (flag != 0 && tmp->getStatus() != FINISHED))
             {
                 tmp->getUuid(uuid);
                 tmp->getId(id);
                 emit getResponse(message, uuid,id);
             }
             flag = 0;
        }
    }
//    send();
}

void NetgearSoapSchedule::submitrequest(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id)
{
    {
//        QWriteLocker lock(&m_lock);
#ifndef TESTSOAPAPI
        if(mLogmanager)
        {

            QString s = modulename;
            s += methodname;
            s += uuid.toString();
            s += id;
            mLogmanager->writeLog(s,uuid);

        }
#endif

        soapoptcodequeue.insert(modulename, methodname, params, uuid, id);
//        send();
    }
}

void NetgearSoapSchedule::send()
{
    QString modulename;
    QString methodname;
    QMap<QString, QString> params;
    int interval =  GETINTERVAL ;

    if(m_netgearsoap->http.httpDriver()->hasPendingRequests())
    {
        if(sendstatus == STARTSEND || ((sendstatus == TIMEOUT || sendstatus == SENDERROR) && flag < POLLCOUNT) )
        {
            sendstatus = TIMEOUT;
            flag++;
        }
        m_netgearsoap->abort();
//        qDebug() << "hasPendingRequests true.";
//        return;
    }
    else
    {
//        qDebug() << "hasPendingRequests false.";
    }

    {
       // QWriteLocker lock(&m_lock);
        SoapOptcode * tmp = soapoptcodequeue.getSoapOptcode();
        if(sendstatus == SUCCESSSEND \
           || ((sendstatus == TIMEOUT || sendstatus == SENDERROR) && flag == POLLCOUNT) \
           || (tmp != NULL && tmp->gettype() == WRITE && tmp->getStatus() != STARTED) )
        {
            soapoptcodequeue.nextOptcode();
            sendstatus = NOTSEND;
            flag = 0;
        }
        else if(sendstatus != NOTSEND)
        {
            changeport();
        }

        tmp = soapoptcodequeue.getSoapOptcode();
        if(tmp != NULL)
        {
            if(sendstatus == NOTSEND)
            {
                sendstatus = STARTSEND;
            }
            sendflag = true;
             tmp->getSoapParams(modulename, methodname, params);
             if(tmp->gettype() == WRITE && tmp->getStatus() == SETCODE)
             {
                 interval = SETINTERVAL;
             }
             else
             {
                 interval = GETINTERVAL;
             }

             m_timer->start(interval);
             m_netgearsoap->submitrequest(modulename, methodname, params);
            // emit submitrequesttosoapsignal(modulename, methodname, params);
        }
        else
        {
            interval = IDLEINTERVAL;
            m_timer->start(interval);
        }
     }
}

void NetgearSoapSchedule::quitevent()
{
    if(m_eventloop)
        m_eventloop->exit(0);
}

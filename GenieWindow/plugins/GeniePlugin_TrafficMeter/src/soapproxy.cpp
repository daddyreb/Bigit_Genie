#include "soapproxy.h"

IManagerHandler *SoapProxy::sHandler = NULL;
SoapProxy::SoapProxy()
{
    uuid = PLUGIN_UUID;
    soapapi = NULL;
    timer = new QTimer();

    idstrmap.insert("EnableTrafficMeter",0);
    idstrmap.insert("GetTrafficMeterStatistics",0);
    idstrmap.insert("SetTrafficMeterOptions",0);

    if(SoapProxy::sHandler == NULL)
        return;

    soapmanager = SoapProxy::sHandler->getSoapManager();
    if(soapmanager == NULL)
        return;

    soapapi = soapmanager->getSoapAPI();

    id = 0;

    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)), \
            this, SLOT(GetResponseGetEnable(QtSoapMessage&,QUuid&,QString&)));
//    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),
//            this, SLOT(GetResponseSetOptions(QtSoapMessage&,QUuid&,QString&)));
//    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),
//            this, SLOT(GetResponseSet(QtSoapMessage&,QUuid&,QString&)));
    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)), \
            this, SLOT(GetResponseStatistics(QtSoapMessage&,QUuid&,QString&)));
    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)), \
            this, SLOT(GetResponseGetOptions(QtSoapMessage&,QUuid&,QString&)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

    connect(this, SIGNAL(response(bool,int)), \
            this, SLOT(stoptimer()));
    connect(this, SIGNAL(response(bool,int,gettrafficmeter_t&)), \
            this, SLOT(stoptimer()));
    connect(this, SIGNAL(response(bool,int,settrafficmeter_t&)), \
            this, SLOT(stoptimer()));

    connect(this, SIGNAL(response(bool,int)), \
            this, SLOT(updateid()));
    connect(this, SIGNAL(response(bool,int,gettrafficmeter_t&)), \
            this, SLOT(updateid()));
    connect(this, SIGNAL(response(bool,int,settrafficmeter_t&)), \
            this, SLOT(updateid()));
}

SoapProxy::~SoapProxy()
{
    if(timer)
    {
        timer->stop();
        delete timer;
    }
}

void SoapProxy::timeout()
{
    gettrafficmeter_t data;
    settrafficmeter_t data1;
    updateid();
    emit response(true, L_TRAFFICMETER_BOX_3_MESSAGE);
    emit response(true, L_TRAFFICMETER_BOX_3_MESSAGE, data);
  //  emit response(true, L_TRAFFICMETER_BOX_3_MESSAGE, data1);
}

void SoapProxy::updateid()
{
    id++;
}

QString SoapProxy::getid(QString &idstr)
{
  return QString("%1%2").arg(idstr).arg(id);
}

void SoapProxy::settrafficmeterenable(settrafficmeter_t &data)
{
    int id;
    this->data = data;

    QString ModeName = "DeviceConfig";
    QString MethodName = "EnableTrafficMeter";
    updateid();
    QMap<QString , QString> params;
    params.insert("NewTrafficMeterEnable", data.NewTrafficMeterEnable);
//    TrafficMeterEnable = data.NewTrafficMeterEnable;
    QString idstr = getid(MethodName);
    starttimerset();
    soapapi->submitrequest(ModeName,MethodName,params,uuid,idstr);

    qDebug() \
            << "receive settrafficmeter_t" \
            << data.NewTrafficMeterEnable \
            << data.NewControlOption \
            << data.NewMonthlyLimit \
            << data.RestartHour \
            << data.RestartMinute \
            << data.RestartDay;

    qDebug() << "NewTrafficMeterEnable";
}

void SoapProxy::settrafficmeter(settrafficmeter_t &data)
{
    this->data = data;
    sendsettrafficmeter();
//    int id;
//    this->data = data;
//
//    QString ModeName = "DeviceConfig";
//    QString MethodName = "EnableTrafficMeter";
//    updateid();
//    QMap<QString , QString> params;
//    params.insert("NewTrafficMeterEnable", data.NewTrafficMeterEnable);
//    QString idstr = getid(MethodName);
//    soapapi->submitrequest(ModeName,MethodName,params,uuid,idstr);
//    starttimerset();
//
//    qDebug() \
//            << "receive settrafficmeter_t" \
//            << data.NewTrafficMeterEnable \
//            << data.NewControlOption \
//            << data.NewMonthlyLimit \
//            << data.RestartHour \
//            << data.RestartMinute \
//            << data.RestartDay;
//
//    qDebug() << "settrafficmeter";
}

void SoapProxy::gettrafficmeteroptions()
{
    QString modulenamestr ="DeviceConfig";
    QString methodnamestr ="GetTrafficMeterOptions";
    updateid();
    QMap<QString , QString> params;
    QString idstr = getid(methodnamestr);
//    starttimerget();
    soapapi->submitrequest(modulenamestr,methodnamestr,params,uuid,idstr);

    qDebug() << "gettrafficmeteroptions";
}

void SoapProxy::sendsettrafficmeter()
{
    QString modulenamestr ="DeviceConfig";
    QString methodnamestr ="SetTrafficMeterOptions";
    updateid();
    QMap<QString , QString> params;
    params.insert("NewControlOption", data.NewControlOption);
    params.insert("NewMonthlyLimit", data.NewMonthlyLimit);
    params.insert("RestartHour", data.RestartHour);
    params.insert("RestartMinute", data.RestartMinute);
    params.insert("RestartDay", data.RestartDay);
    QString idstr = getid(methodnamestr);
    soapapi->submitrequest(modulenamestr,methodnamestr,params,uuid,idstr);
    starttimerset();

    qDebug() << "sendsettrafficmeter";
}

void SoapProxy::get()
{
    gettrafficmeterenable();
    qDebug() << "SoapProxy::get";
}

void SoapProxy::gettrafficmeterenable()
{
    qDebug() << "SoapProxy::gettrafficmeterenable";
    QString ModeName ="DeviceConfig";
    QString MethodName ="GetTrafficMeterEnabled";
    TrafficMeterEnable = "";
    updateid();
    QMap<QString , QString> params;
    QString idstr = getid(MethodName);
    soapapi->submitrequest(ModeName,MethodName,params,uuid,idstr);
    starttimerget();
}

void SoapProxy::gettrafficmeter()
{
    qDebug() << "gettrafficmeter";
    QString ModeName ="DeviceConfig";
    QString MethodName ="GetTrafficMeterStatistics";
    updateid();
    QMap<QString , QString> params;
    QString idstr = getid(MethodName);
//    starttimerget();
    soapapi->submitrequest(ModeName,MethodName,params,uuid,idstr);
}

void SoapProxy::GetResponseGetOptions(QtSoapMessage & message, QUuid & uuid, QString &id)
{
    QString methodnamestr = "GetTrafficMeterOptions";
    if(uuid =="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}" && id == getid(methodnamestr))
    {
        settrafficmeter_t data;
        QString NewControlOption,NewMonthlyLimit,RestartHour,RestartMinute,RestartDay;
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        if (message.isFault())
        {
            emit response(true, L_TRAFFICMETER_BOX_1_MESSAGE, data);
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
               QString tmp="0";
               NewControlOption = res["NewControlOption"].value().toString();
               NewMonthlyLimit = res["NewMonthlyLimit"].value().toString();
               RestartHour = res["RestartHour"].value().toString();
               RestartMinute = res["RestartMinute"].value().toString();
               RestartDay = res["RestartDay"].value().toString();
               data.set(tmp,NewControlOption,NewMonthlyLimit,RestartHour,RestartMinute,RestartDay);
               qDebug() << "SoapProxy::GetResponseGetOptions" << tmp << NewControlOption << NewMonthlyLimit << RestartHour << RestartMinute << RestartDay;
               emit response(false, 0, data);
            }
            else
            {
                emit response(true, L_TRAFFICMETER_BOX_2_MESSAGE, data);
            }
        }
        gettrafficmeter();
    }
}

void SoapProxy::GetResponseGetEnable(QtSoapMessage & message, QUuid & uuid, QString &id)
{
    QString methodnamestr = "GetTrafficMeterEnabled";
    if(uuid =="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}" && id == getid(methodnamestr))
    {
        gettrafficmeter_t data;
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        if (message.isFault())
        {
            emit response(true, L_TRAFFICMETER_BOX_1_MESSAGE, data);
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
               TrafficMeterEnable = res["NewTrafficMeterEnable"].value().toString();
               qDebug() << "SoapProxy::GetResponseGetEnable" << TrafficMeterEnable;
               if(TrafficMeterEnable == "1")
               {
                   gettrafficmeteroptions();
               }
               else
               {
                   data.settrafficmeterenable(TrafficMeterEnable);
                   emit response(false, 0, data);
               }
            }
            else
            {
                TrafficMeterEnable = "2";
//                gettrafficmeteroptions();
                emit response(true, L_TRAFFICMETER_BOX_2_MESSAGE, data);
            }
        }
    }
}

void SoapProxy::GetResponseSetOptions(QtSoapMessage & message, QUuid & uuid, QString &id)
{
    QString methodnamestr ="SetTrafficMeterOptions";
    if(uuid =="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}" && id == getid(methodnamestr))
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        if (message.isFault())
        {
            emit response(true, L_TRAFFICMETER_BOX_1_MESSAGE);
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            if( message.returnResponseCode().toString().toInt() == 0 || message.returnResponseCode().toString().toInt() == 1)
            {
                emit response(false,0);
            }
            else
            {
                emit response(true, L_TRAFFICMETER_BOX_2_MESSAGE);
            }
        }
    }
}

void SoapProxy::GetResponseSet(QtSoapMessage & message, QUuid & uuid, QString &id)
{
    QString methodnamestr ="EnableTrafficMeter";
    if(uuid =="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}" && id == getid(methodnamestr))
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr ="NewTrafficMeterEnable";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            emit response(true, L_TRAFFICMETER_BOX_1_MESSAGE);
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            if( message.returnResponseCode().toString().toInt() == 0 || message.returnResponseCode().toString().toInt() == 1)
            {
                get();
                emit response(false,0);
            }
            else
            {
                emit response(true, L_TRAFFICMETER_BOX_2_MESSAGE);
            }
        }
    }
}

void SoapProxy::GetResponseStatistics(QtSoapMessage & message, QUuid & uuid, QString &id)
{
    QString methodnamestr ="GetTrafficMeterStatistics";
    if(uuid =="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}" && id == getid(methodnamestr))
    {
        gettrafficmeter_t data;
        data.settrafficmeterenable(TrafficMeterEnable);
        qDebug() << "GetResponseStatistics start";
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr ="NewTodayConnectionTime,NewTodayUpload,NewTodayDownload,NewYesterdayConnectionTime,NewYesterdayUpload,NewYesterdayDownload,NewWeekConnectionTime,NewWeekUpload,NewWeekDownload,NewMonthConnectionTime,NewMonthUpload,NewMonthDownload,NewLastMonthConnectionTime,NewLastMonthUpload,NewLastMonthDownload";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            emit response(true, L_TRAFFICMETER_BOX_1_MESSAGE, data);
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            QString todyconnection,yesterdayconnection,weekconnection,monthconnection,lastmonthconnection;
            QString todayupload,todaydownload,yesterdayupload,yesterdaydownload;
            QString weekupload,weekdownload,monthupload,monthdownload;
            QString lastupload,lastdownload;
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    //Today
                    if(queryfieldlist.at(i) =="NewTodayConnectionTime")
                    {
                        todyconnection = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewTodayUpload")
                    {
                         todayupload = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewTodayDownload")
                    {
                        todaydownload = res[queryfieldlist.at(i)].value().toString();
                    }

                    //Yesterday
                    if(queryfieldlist.at(i) =="NewYesterdayConnectionTime")
                    {
                        yesterdayconnection = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewYesterdayUpload")
                    {
                        yesterdayupload = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewYesterdayDownload")
                    {
                        yesterdaydownload = res[queryfieldlist.at(i)].value().toString();
                    }

                    //This week
                    if(queryfieldlist.at(i) =="NewWeekConnectionTime")
                    {
                        weekconnection = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewWeekUpload")
                    {
                        weekupload = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewWeekDownload")
                    {
                         weekdownload = res[queryfieldlist.at(i)].value().toString();
                    }

                    //This month
                    if(queryfieldlist.at(i) =="NewMonthConnectionTime")
                    {
                        monthconnection = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewMonthUpload")
                    {
                        monthupload = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewMonthDownload")
                    {
                        monthdownload = res[queryfieldlist.at(i)].value().toString();
                    }

                    //Last month
                    if(queryfieldlist.at(i) =="NewLastMonthConnectionTime")
                    {
                        lastmonthconnection = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewLastMonthUpload")
                    {
                        lastupload = res[queryfieldlist.at(i)].value().toString();
                    }
                    if(queryfieldlist.at(i) =="NewLastMonthDownload")
                    {
                        lastdownload = res[queryfieldlist.at(i)].value().toString();
                    }
                }
                data.set(todyconnection, todayupload, todaydownload, \
                         yesterdayconnection, yesterdayupload, yesterdaydownload, \
                         weekconnection, weekupload, weekdownload,\
                         monthconnection, monthupload, monthdownload, \
                         lastmonthconnection, lastupload, lastdownload);

                qDebug() << "send data:"<< todyconnection << todayupload << todaydownload \
                        << yesterdayconnection << yesterdayupload << yesterdaydownload \
                        << weekconnection << weekupload << weekdownload \
                        << monthconnection << monthupload << monthdownload \
                        << lastmonthconnection << lastupload << lastdownload;

                qDebug() << "GetResponseStatistics end";
                emit response(false, 0, data);
            }
            else
            {
                emit response(true, L_TRAFFICMETER_BOX_2_MESSAGE, data);
            }
        }
    }
}

void SoapProxy::starttimerget()
{
    if(timer)
        timer->start(GET_INTERVAL);
}

void SoapProxy::starttimerset()
{
    if(timer)
        timer->start(SET_INTERVAL);
}
void SoapProxy::stoptimer()
{
    if(timer)
        timer->stop();
}

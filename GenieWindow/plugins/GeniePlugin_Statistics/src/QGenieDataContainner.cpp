#include "QGenieDataContainner.h"
#include "QGenieStaticInterface.h"
#include "GeniePlugin_Statistics.h"
#include "ui_QGenieStaticInterface.h"
#include <QNetworkInterface>
#include "INetgearInterface.h"
#ifdef Q_OS_WIN32
//#include "DiagnoseInterface.h"
//static QPluginLoader *sDiagnoseInterfaceLoader=NULL;
//static DiagnoseInterface *sDiagnoseInterface=NULL;
#endif
static int sLocalInfoFlag=0;




#define TIMER_INTERVAL 60000 //1 minites
//#define GET_INFO_TICKERS 20
#define COMMIT_FIRSTTIME_TICKERS (2)
#define COMMIT_FIRSTTIME_TIMES 3
#define COMMIT_CLICK_TICKERS (24*60)
#define COMMIT_LOCAL_INFO_TICKERS (1*60)
#define COMMIT_SEARCH_TICKERS (2*60)
#define COMMIT_RETRY_INTERVAL (1*60)
#define RECORD_ROUTETYPE_TICKERS (1*60)
#define COMMIT_ROUTETYPE_TICKERS (24*60+1)
#define MAX_ROUTETYPE_COUNT 20
#define COMMIT_RETRY_TIMES 3
#define SEARCH_FILE_SIZE_LIMIT (1024*1024)
#define ALIVE_MESSAGE_INTERVAL (60)


#define MAX_SEARCH_PAIR_COUNT 10



#ifdef USE_DLL_GETINFO
#define DLL_NetSCAN_FILE_NAME "./netscan.dll"
#endif


///
#ifdef Q_OS_WIN32
typedef BOOL (_stdcall *PFN)(LPCTSTR);
static PFN IsThisPhysicalNic = NULL;
#endif

static bool isMacFormat(const QString &s)
{
    QRegExp m("^[0-9a-fA-F]([0-9a-fA-F][:-][0-9a-fA-F]){5}[0-9a-fA-F]$");
    return m.exactMatch(s);
}

static bool isIpAddress(const QString &s)
{
    QRegExp m("^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$");
    return m.exactMatch(s);
}

QString changeCode(const QString &s)
{
    QString ret=s;
    return ret.replace("&","%26").replace("=","%3D").replace(" ","%20");
}

QGenieDataContainner::QGenieDataContainner(QObject *parent)
    :QObject(parent)
    #if USE_DLL_GETINFO
    ,m_getadapterinfo(NULL)
    #endif
    ,mTickerCounter(0)
    ,mRequestInstallInfoId(-1)
    ,mRequestSearchId(-1)
    ,mSearchMapCount(-1)
    ,mFirstCommitTimes(COMMIT_FIRSTTIME_TIMES)
    #ifdef Q_OS_WIN32
    ,MyLib(NULL)
    #endif
    ,mHttpRequestManager(new QGenieHttpRequestManager())
    //    ,sHandler(NULL)
{
    //    mSoftwareUuid=QUuid::createUuid();

    mLastSendTime=QDateTime::currentDateTime();
    mTimer.start(TIMER_INTERVAL);
    initHash();
    serialize(true);

    //    connect(&mHttp,SIGNAL(requestFinished(int,bool)),
    //                     this,SLOT(slot_RequestFinished(int,bool)));
    connect(mHttpRequestManager,SIGNAL(signal_Done(int,bool,QString))
            ,this, SLOT(slot_Done(int,bool,QString)) );
    connect(&mTimer,SIGNAL(timeout()),
            this,SLOT(slot_Timeout()));
#if USE_DLL_GETINFO
    m_myLib = new QLibrary;
    Q_ASSERT(m_myLib);

    m_myLib->setFileName(DLL_NetSCAN_FILE_NAME);
    m_getadapterinfo = (GetAdapterInfo) m_myLib->resolve("get_adapter_info");
#endif
    QMap<QString,int> searchmap;
    QFile f(FILE_OF_SEARCHMAP);
    if(f.open(QIODevice::ReadOnly))
    {
        QDataStream in(&f);
        if(!in.atEnd())
        {
            in >> searchmap;
        }
        mSearchMapCount=searchmap.count();
        f.close();
    }


}

QGenieDataContainner::~QGenieDataContainner()
{
//    mHttp.httpAbort(true);
    GENIE2_SAFEDELETE(mHttpRequestManager);
    mTimer.stop();
#if USE_DLL_GETINFO
    if(m_myLib)
    {
        delete m_myLib;
    }
#endif
    serialize(false);
#ifdef Q_OS_WIN32
    if(MyLib)
    {
        ///        sDiagnoseInterfaceLoader->unload();
        ///        delete sDiagnoseInterfaceLoader;
        ///        sDiagnoseInterfaceLoader=NULL;
        MyLib->unload();
        delete MyLib;
        MyLib = NULL;
    }
#endif
    QMutableMapIterator<QDate,QMap<QString,int> *> i(mClickCountMap);
    while (i.hasNext()) {
        i.next();
        QMap<QString,int> *tempmap=i.value();
        if(tempmap)
        {
            delete tempmap;
        }
    }
    mClickCountMap.clear();

    QMutableMapIterator<QString,tRouteInfo *> ii(mRoutTypeMap);
    while (ii.hasNext()) {
        ii.next();
        tRouteInfo *p=ii.value();
        if(p)
        {
            delete p;
        }
    }
    mRoutTypeMap.clear();

}

void QGenieDataContainner::addClick(const QString &para,ADD_TYPE tp,int c)
{
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    Q_ASSERT(ui);
    if(!mShortHashDetail.keys().contains(para))
    {
        ui->ui->lab_unknown_click->setText(
                    ui->ui->lab_unknown_click->text()+" "+para );
        if(ui->isVisible())
        {
            ui->updateUi();
        }
        return;
    }
    QDate curdate=QDate::currentDate();
    QMap<QString,int> *curmap;
    if(!mClickCountMap.contains(curdate))
    {
        curmap=new QMap<QString,int>;
        Q_ASSERT(curmap);
        mClickCountMap.insert(curdate,curmap);
    }
    else
    {
        curmap=mClickCountMap.value(curdate);
        Q_ASSERT(curmap);
    }

    //-----------------------------------
    int curcount=curmap->value(para,0);
    if(tp==AT_ADD)
    {
        curcount+=c;
    }
    else if(tp==AT_REGMAX)
    {
        curcount=qMax(curcount,c);
    }
    curmap->insert(para,curcount);


    if(ui->isVisible())
    {
        ui->updateUi();
    }
}

void QGenieDataContainner::addSearchStr(const QString &str)
{
    qDebug()<<"QGenieDataContainner::addSearchStr";
    QMap<QString,int> searchmap;
    if(!QDir::home().exists(NETGEARGENIE_DIR))
    {
        QDir::home().mkdir(NETGEARGENIE_DIR);
    }
    QFile f(FILE_OF_SEARCHMAP);
    if(f.size()>SEARCH_FILE_SIZE_LIMIT)
    {
        return;
    }
    if(f.open(QIODevice::ReadOnly))
    {
        QDataStream in(&f);
        if(!in.atEnd())
        {
            in >> searchmap;
        }
    }
    f.close();

    int itemp=searchmap.value(str,0);
    searchmap.insert(str,itemp+1);
    mSearchMapCount=searchmap.count();
    if(f.open(QIODevice::WriteOnly))
    {
        QDataStream out(&f);
        out <<searchmap;
    }
    f.close();
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    Q_ASSERT(ui);
    ui->updateUi();
}

void QGenieDataContainner::initHash()
{
    mShortHashDetail.clear();
    struct _t{
        char * key;
        char *value;
    } cParas[]=
    {
    {"H1","Retrieve wireless password"},
    {"H2","menu option"},
    {"H3","dashboard icon"},
    {"H4","promotion banners"},
    {"H5","Search"},
    {"H6","Show Network Connectivity Change of tray unckeck"},
    {"H7","Show Network Connectivity Change of tray ckeck"},
    {"H8","Enable Autostart of tray uncheck"},
    {"H9","Enable Autostart of tray check"},
    {"H10","Fix it of dashboard"},
    {"H11","Advertissing links"},
    {"H12","Add 1 every one hour"},
    {"I1","Speed Test"},
    {"I2","click http://www.speedtest.net"},
    {"I3","click process view"},
    {"I4"," checks or unchecks \"Show Details\" "},
    {"I5","\"Fix it\" again"},
    {"I6","\"Incorrect wireless password\" is prompted"},
    {"I7","router reboot is prompted"},
    {"I8","\"Fix it\" succeeds"},
    {"I9","\"Fix it\" fails"},
    {"W1"," \"Connect WiFi\" tab "},
    {"W2","How many neighbor networks "},
    {"W3","How many secured wireless network"},
    {"W4","How many are unsecured wireless network"},
    {"W5"," \"WiFi Channel\" tab"},
    {"W6"," \"Change Channel\""},
    {"W7","\"Guest Access\" tab in \"WiFi Connection\""},
    {"W8","wireless wep"},
    {"R1","non-default password"},
    {"R2","un-checks \"Remember password\""},
    {"R3"," clicks \"Apply\""},
    {"R4"," exports successfully"},
    {"R5"," \"Wireless Settings\" tab"},
    {"R6","\"Guest Access\" tab"},
    {"R7","from \"Disable\" to \"Enable Guest Access\""},
    {"R8","\"Enable Guest Access\" to \"Disable\""},
    {"R9","Guest Access Timeout"},
    {"R10","\"Traffic Meter\" tab"},
    {"R11"," enables \"Traffic Meter\""},
    {"R12"," disables \"Traffic Meter\" "},
    {"R13"," \"Router Update\" tab"},
    {"R14"," \"Next\" to check update"},
    {"R15","ReadyShare->BasicMode->NetworkName"},
    {"R16","ReadyShare->CloundMode->ClickHere"},
    {"R17","ReadyShare->CloundMode->readyshare.netgear.com/"},
    {"R18"," \"ReadyShare\" tab Register"},
    {"R19"," \"ReadyShare\" tab UnRegister"},
    {"R20","Switch to ReadyShare Tab"},
    {"M1","Map Refresh"},
    {"M2","check \"Notify me ...\" "},
    {"M3","uncheck \"Notify me ...\" "},
    {"M4","\"Settings\" of router node"},
    //add by caiyujun
    {"M5","Send File"},
    {"M6","Send Folder"},
    //add end
    {"NQ1","FAQ Question1"},
    {"NQ2","FAQ Question1"},
    {"NQ3","FAQ Question1"},
    {"NQ4","FAQ Question1"},
    {"NQ5","FAQ Question1"},
    {"NQ6","FAQ Question1"},
    {"NQ7","FAQ Question1"},
    {"NQ8","FAQ Question1"},
    {"N2"," clicks \"Ping\""},
    {"N3"," clicks \"Trace Route\""},
    {"N4"," clicks \"DNS Lookup\""},
    {"N5"," \"Save\" in \"Computer Profile\" "},
    {"NS1","\"Downloads\" of \"Support\""},
    {"NS2","\"Forums\" of \"Support\""},
    {"NS3","\"Register Your Product\" of \"Support\""},
    {"NS4","\"Premium Support\" of \"Support\""},
    {"F1","Submit"},
    {"P1","Enable ParentControl"},
    {"P2","Disable ParentControl"},
    {"P3","BasicSetting"},
    {"P4","CustomSetting"},
    {"PL1","LPC LEVEL CUSTOM"},
    {"PL2","LPC LEVEL HIGH"},
    {"PL3","LPC LEVEL MODERATE"},
    {"PL4","LPC LEVEL LOW"},
    {"PL5","LPC LEVEL MINIMAL"},
    {"PL6","LPC LEVEL NONE"},


}   ;

    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        mShortHashDetail.insert(cParas[i].key,cParas[i].value);
    }

}

void QGenieDataContainner::cleanClickCount()
{
    if(mClickCountMap.count()>MAX_DATE_COUNT)
    {
        int c=mClickCountMap.count()-MAX_DATE_COUNT;
        QDate curdate=QDate::currentDate();
        QMutableMapIterator<QDate,QMap<QString,int> *> i(mClickCountMap);
        while (i.hasNext()) {
            i.next();
            if (i.key() >curdate || i.key()<=curdate.addDays(-MAX_DATE_COUNT))
            {
                if(i.value()!=NULL)
                {
                    delete i.value();
                }
                i.remove();
            }
            if(--c <0 )
            {
                break;
            }
        }
    }
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    if(ui)
    {
        ui->updateUi();
    }
}

void QGenieDataContainner::serializeClickCount(bool load)
{
    QFile f(NETGEARGENIE_DIR+"function_statistics.dat");
    if(load && f.open(QIODevice::ReadOnly))
    {
        QMutableMapIterator<QDate,QMap<QString,int> *> i(mClickCountMap);
        while (i.hasNext()) {
            i.next();
            QMap<QString,int> *tempmap=i.value();
            if(tempmap)
            {
                delete tempmap;
            }
        }
        mClickCountMap.clear();
        QDataStream in(&f);
        QDate date;
        QMap<QString,int> *map;
        //        if(!in.atEnd())
        //        {
        //            in >> mSoftwareUuid;
        //        }
        while(!in.atEnd())
        {
            map=new QMap<QString,int>;
            in >> date>> *map;
            mClickCountMap.insert(date,map);
        }
    }
    else if(!load && f.open(QIODevice::WriteOnly))
    {
        QDataStream out(&f);
        // out <<mSoftwareUuid;
        QMapIterator<QDate, QMap<QString,int> *> i(mClickCountMap);
        while (i.hasNext()) {
            i.next();
            out << i.key() <<  *(i.value());
        }
    }
    f.close();
}

void QGenieDataContainner::serializeRouteType(bool load)
{
    QFile f(NETGEARGENIE_DIR+"routetype_statistics.dat");
    QString routemac,localmac,model,fversion;
    if(load && f.open(QIODevice::ReadOnly))
    {

        QMutableMapIterator<QString,tRouteInfo *> i(mRoutTypeMap);
        while (i.hasNext()) {
            i.next();
            tRouteInfo *p=i.value();
            if(p)
            {
                delete p;
            }
        }
        mRoutTypeMap.clear();
        QDataStream in(&f);
        //        if(!in.atEnd())
        //        {
        //            in >> mSoftwareUuid;
        //        }
        while(!in.atEnd())
        {
            tRouteInfo *p=new tRouteInfo;
            in >> p->routemac >> p->localmac >> p->modeln >> p->fversion;

            //            in.readRawData((char *)p,sizeof(tRouteInfo));
            if(isMacFormat(p->routemac))
            {
                mRoutTypeMap.insert(p->routemac,p);
            }
            else
            {
                delete p;
            }
        }
    }
    else if(!load && f.open(QIODevice::WriteOnly))
    {
        QDataStream out(&f);
        QList<tRouteInfo *> tlist=mRoutTypeMap.values();
        foreach (tRouteInfo *p, tlist) {
            //			out.writeRawData((const char *)p,sizeof(tRouteInfo));
            out << p->routemac << p->localmac << p->modeln << p->fversion;
            ;
        }
    }
    f.close();


}

void QGenieDataContainner::serialize(bool load)
{
    if(!QDir::home().exists(NETGEARGENIE_DIR))
    {
        QDir::home().mkdir(NETGEARGENIE_DIR);
    }
    serializeClickCount(load);
    serializeRouteType(load);

}

//void QGenieDataContainner::slot_RequestFinished( int id, bool error )
//{
//    qDebug()<<"QGenieDataContainner::requestFinished";
//    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
//    Q_ASSERT(ui);
//
//    bool canDeleteLocalData=false;
//    QByteArray temparray;
//    temparray = mHttp.readAll();
//    QString strInfo = temparray.constData();
//
//    strInfo.trimmed();
//    canDeleteLocalData=(!error && !(strInfo.length()>0 && strInfo.indexOf("errorcode")>-1));
//
//    QString logstr;
//
//    if(mRequestIdMapDate.contains(id))
//    {
//        QDate date=mRequestIdMapDate.value(id);
//        mRequestIdMapDate.remove(id);
//        QMap<QString,int> *map=mClickCountMap.value(date);
//        if(canDeleteLocalData)
//        {
//            mClickCountMap.remove(date);
//            if(map)
//            {
//                delete map;
//            }
//        }
//
//        logstr=QString("%1's data commited %2")
//                .arg(date.toString("yyyy-MM-dd"),canDeleteLocalData?"success":"error");
//
//    }
//
//    if(mRequestIdMapRoutetype.contains(id))
//    {
//        QString routemac=mRequestIdMapRoutetype.value(id);
//        mRequestIdMapRoutetype.remove(id);
//        tRouteInfo *p= mRoutTypeMap.value(routemac);
//        if(canDeleteLocalData)
//        {
//            mRoutTypeMap.remove(routemac);
//            if(p)
//            {
//                delete p;
//            }
//        }
//        logstr=QString("route type %1 commited %2")
//                .arg(routemac,canDeleteLocalData?"success":"error");
//
//    }
//    if(id == mRequestInstallInfoId)
//    {
//        if(canDeleteLocalData)
//        {
//            SET_SETTING_VALUE(INSTALL_INFO_COMMITED,true);
//        }
//        logstr=QString("Install Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
//    }
//    if(id ==mRequestSearchId && mSearchMapCount >0)
//    {
//        if(canDeleteLocalData)
//        {
//            QMap<QString,int> searchmap;
//            QFile f(FILE_OF_SEARCHMAP);
//            if(f.open(QIODevice::ReadOnly))
//            {
//                QDataStream in(&f);
//                if(!in.atEnd())
//                {
//                    in >> searchmap;
//                }
//            }
//            f.close();
//
//            QString tempkey;
//            int tempcount;
//            QMapIterator<QString,int> i(mRequestSearchMap);
//            while(i.hasNext())
//            {
//                i.next();
//                tempkey=i.key();
//                tempcount=i.value();
//                Q_ASSERT(tempcount >0);
//                tempcount=searchmap.value(tempkey,0)-tempcount;
//                if(tempcount >0)
//                {
//                    searchmap.insert(tempkey,tempcount);
//                }
//                else
//                {
//                    searchmap.remove(tempkey);
//                }
//
//            }
//            if(f.open(QIODevice::WriteOnly))
//            {
//                QDataStream out(&f);
//                out <<searchmap;
//            }
//            f.close();
//        }// if !error
//        logstr=QString("Search Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
//        mRequestSearchMap.clear();
//    }
//    if(mRequestRouterInfoId==id)
//    {
//        logstr=QString("RouterInfo Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
//        mRequestSearchMap.clear();
//    }
//
//    if(ui && ui->isVisible() && !logstr.isEmpty())
//    {
//        ui->ui->retstr_lst->addItem(logstr);
//    }
//    if(ui && ui->isVisible() && !canDeleteLocalData && !strInfo.isEmpty())
//    {
//        QString stemp=QString("error msg is: %1").arg(strInfo);
//        ui->ui->retstr_lst->addItem(stemp);
//    }
//}
//
//
//
void QGenieDataContainner::slot_Timeout()
{

    sLocalInfoFlag=0;

    //    bool isNetgear=(infm->getInfo(INFO_CMD_NETGEAR)=="y");
    bool isNetgear = true;

    mTickerCounter++;
    //    if(0 == mTickerCounter%GET_INFO_TICKERS)
    //    {
    //        getLocalInfo();
    //    }
    if(0==mTickerCounter%ALIVE_MESSAGE_INTERVAL)
    {
        addClick("H12");
    }
    bool bFirstCommit=(0==mTickerCounter%COMMIT_FIRSTTIME_TICKERS)
            &&(--mFirstCommitTimes > 0);


    bool commitclick=bFirstCommit;
    if(!commitclick)
    {
        for(int i=0;i<COMMIT_RETRY_TIMES;i++)
        {
            if(0==mTickerCounter%(COMMIT_CLICK_TICKERS+i*COMMIT_RETRY_INTERVAL))
            {
                commitclick=true;
                break;
            }
        }
    }


    if(commitclick /*&& isNetgear*/)
    {
        cleanClickCount();
        getLocalInfo();
        commitClickCount();

    }
    if(0==mTickerCounter%COMMIT_SEARCH_TICKERS || bFirstCommit)
    {
        commitSearchData();
    }

    if(0== mTickerCounter%COMMIT_LOCAL_INFO_TICKERS || bFirstCommit)
    {
        getLocalInfo();
        if(false==GENIE2_GET_SETTING(INSTALL_INFO_COMMITED).toBool())
        {
            commitInstallInfo();
        }

    }

    if(0== mTickerCounter%COMMIT_ROUTETYPE_TICKERS || bFirstCommit)
    {
        commitRouterInfo();
    }

    if(0 == mTickerCounter%RECORD_ROUTETYPE_TICKERS)
    {
        if(isNetgear || RECORD_NO_NETGEARROUTER)
        {
            recordRouteInfo();
        }
    }
    sLocalInfoFlag=0;
}

//void QGenieDataContainner::slot_ReadyRead(const QHttpResponseHeader & resp)
//{
//    QByteArray mHttpInfo;
//    mHttpInfo = mHttp.readAll();
//    QString strInfo = mHttpInfo.constData();

//    strInfo.trimmed();
//    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
//    if(ui && ui->isVisible())
//    {
//        ui->ui->retstr_lst->addItem(strInfo);
//    }
//}

void QGenieDataContainner::slot_Done(int id, bool error,QString str)
{
    qDebug()<<"QGenieDataContainner::slot_Done";
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    Q_ASSERT(ui);

    bool canDeleteLocalData=!error;
    QString strInfo = str;

    strInfo.trimmed();

    QString logstr;

    if(mRequestIdMapDate.contains(id))
    {
        QDate date=mRequestIdMapDate.value(id);
        mRequestIdMapDate.remove(id);
        QMap<QString,int> *map=mClickCountMap.value(date);
        if(canDeleteLocalData)
        {
            mClickCountMap.remove(date);
            if(map)
            {
                delete map;
            }
        }

        logstr=QString("%1's data commited %2")
                .arg(date.toString("yyyy-MM-dd"),canDeleteLocalData?"success":"error");

    }

    if(mRequestIdMapRoutetype.contains(id))
    {
        QString routemac=mRequestIdMapRoutetype.value(id);
        mRequestIdMapRoutetype.remove(id);
        tRouteInfo *p= mRoutTypeMap.value(routemac);
        if(canDeleteLocalData)
        {
            mRoutTypeMap.remove(routemac);
            if(p)
            {
                delete p;
            }
        }
        logstr=QString("route type %1 commited %2")
                .arg(routemac,canDeleteLocalData?"success":"error");

    }
    if(id == mRequestInstallInfoId)
    {
        if(canDeleteLocalData)
        {
            GENIE2_SET_SETTING(INSTALL_INFO_COMMITED,true);
        }
        logstr=QString("Install Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
    }
    if(id ==mRequestSearchId /*&& mSearchMapCount >0*/)
    {
        if(canDeleteLocalData)
        {
            QMap<QString,int> searchmap;
            QFile f(FILE_OF_SEARCHMAP);
            if(f.open(QIODevice::ReadOnly))
            {
                QDataStream in(&f);
                if(!in.atEnd())
                {
                    in >> searchmap;
                }
            }
            f.close();

            QString tempkey;
            int tempcount;
            QMapIterator<QString,int> i(mRequestSearchMap);
            while(i.hasNext())
            {
                i.next();
                tempkey=i.key();
                tempcount=i.value();
                Q_ASSERT(tempcount >0);
                tempcount=searchmap.value(tempkey,0)-tempcount;
                if(tempcount >0)
                {
                    searchmap.insert(tempkey,tempcount);
                }
                else
                {
                    searchmap.remove(tempkey);
                }

            }
            if(f.open(QIODevice::WriteOnly))
            {
                QDataStream out(&f);
                out <<searchmap;
            }
            f.close();
        }// if !error
        logstr=QString("Search Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
        mRequestSearchMap.clear();
    }
//    if(mRequestRouterInfoId==id)
//    {
//        logstr=QString("RouterInfo Info Commit ret %1").arg(canDeleteLocalData?"success":"error");
//        mRequestSearchMap.clear();
//    }

    if(ui && ui->isVisible() && !logstr.isEmpty())
    {
        ui->ui->retstr_lst->addItem(logstr);
    }
    if(ui && ui->isVisible() && !canDeleteLocalData && !strInfo.isEmpty())
    {
        QString stemp=QString("error msg is: %1").arg(strInfo);
        ui->ui->retstr_lst->addItem(stemp);
    }

//    mRequestIdMapDate.clear();
//    mRequestSearchMap.clear();
    //mRequestIdMapRoutetype.clear();

    if(ui->isVisible())
    {
        ui->updateUi();
    }

}

void QGenieDataContainner::getLocalInfo()
{
    if(sLocalInfoFlag)
    {
        return;
    }
    sLocalInfoFlag=1;

    //    mLocalInfo.insert("modeln",infm->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE));
    //    mLocalInfo.insert("fversion",infm->getInfo(INFO_CMD_FIRMWARE_IMMEDIATE_SHORT));

    //    QString routermac=infom->getInfo(INFO_CMD_ROUTELOGINDOTNET_MAC);
    QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM ->netgearCommand(INetgearInterface::IC_GET_MACADDR,QStringList(),this,SLOT(slot_macAddr(int,QStringList)));
    //    mLocalInfo.insert("modeln","WNDR3700");
    //    mLocalInfo.insert("fversion","V1.0.7.98");

    //    QString routermac="c0:3f:0e:8c:4f:ed";




}

void QGenieDataContainner::genDbgData()
{

}

int QGenieDataContainner::sendRequest(const QString &path)
{
//    QString rhost=GENIE2_GET_SETTING(PLUGIN_STATICS_IP).toString();
//    QString disp=GENIE2_GET_SETTING(PLUGIN_STATICS_DISP).toString();
    // strDisp="";
//    int iport=GENIE2_GET_SETTING(PLUGIN_STATICS_PORT).toInt();


    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    Q_ASSERT(ui);
    if(ui && ui->isVisible())
    {
        ui->ui->retstr_lst->addItem(QString("Send %1").arg(path));
    }

    GENIE2_LOG(QString("send data [%1]").arg(path));

    Q_ASSERT(mHttpRequestManager);
    return mHttpRequestManager->addRequest(path);
//    return mHttp.getCurId();
}

void QGenieDataContainner::commitClickCount()
{
    QDate curdate=QDate::currentDate();
    QString strPath;


    QList<QDate> testdates=mRequestIdMapDate.values();

    QString routermac=mLocalInfo.value("routermac");
    QString localmac=mLocalInfo.value("localmac");
    if(!isMacFormat(localmac))
    {
        return;
    }

    QMapIterator<QDate, QMap<QString,int> *> i(mClickCountMap);
    while (i.hasNext()) {
        i.next();
        QDate date=i.key();
        if(date < curdate && !testdates.contains(date))
        {

            strPath="/f?"  ;
            strPath+= QString("PM=%2")
                    .arg(localmac);
            strPath+=QString("&D=%1").arg(date.toString("yyyy-MM-dd"));
            QMap<QString,int> *map=i.value();
            Q_ASSERT(map);

            QMapIterator<QString,int> ii(*map);
            while(ii.hasNext())
            {
                ii.next();
                strPath+=QString("&%1=%2").
                        arg(ii.key(),QString::number(ii.value()));
            }

            QString sOsInfo = getOSInfo();
            sOsInfo = changeCode(sOsInfo);

            QVariant t=GENIE2_TELL_OTHERPLUGIN(FRAME_UUID,I2CMD_VERSIONNUMBER,0);
            QString sGenieInfo = t.toString();
            sGenieInfo = changeCode(sGenieInfo);

            strPath+=QString("&OS=%1&GV=%2")
                    .arg(sOsInfo,sGenieInfo);

            int sendid =sendRequest(strPath);
            mRequestIdMapDate.insert(sendid, date);
            // http->setHost(url.host(), url.port(80));

        }
    }

}


void QGenieDataContainner::commitInstallInfo()
{
    QString strUrl;

//    QString rhost=GENIE2_GET_SETTING(PLUGIN_STATICS_IP).toString();
//    QString disp=GENIE2_GET_SETTING(PLUGIN_STATICS_DISP).toString();
    // strDisp="";

    //    IInfoManager *infom=sHandler->getInfoManager();

    QString localmac=mLocalInfo.value("localmac");
    if(!isMacFormat(localmac))
    {
        return;
    }
    QString inst_date=GENIE2_GET_SETTING(INSTALL_INFO_DATE).toString();
    QString inst_time=GENIE2_GET_SETTING(INSTALL_INFO_TIME).toString();
    strUrl="/I?"  ;
    strUrl+=QString("PM=%1&D=%2&T=%3").arg(localmac,inst_date,inst_time);
    mRequestInstallInfoId=sendRequest(strUrl);

}

void QGenieDataContainner::recordRouteInfo()
{
    getLocalInfo();
    QString routermac=mLocalInfo.value("routermac");
    QString localmac=mLocalInfo.value("localmac");
    QString modeln=mLocalInfo.value("modeln");
    QString fversion=mLocalInfo.value("fversion");
    if(!mRoutTypeMap.contains(routermac) && mRoutTypeMap.count()<MAX_ROUTETYPE_COUNT
       && isMacFormat(routermac))
    {
        tRouteInfo *p=new tRouteInfo;
        p->routemac=routermac;
        p->localmac=localmac;
        p->modeln=modeln;
        p->fversion=fversion;
        mRoutTypeMap.insert(p->routemac,p);
    }
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    if(ui && ui->isVisible())
    {
        ui->updateUi();
    }
}

void QGenieDataContainner::commitRouterInfo()
{
    QString strUrl;

//    QString rhost=GENIE2_GET_SETTING(PLUGIN_STATICS_IP).toString();
//    QString disp=GENIE2_GET_SETTING(PLUGIN_STATICS_DISP).toString();
    // strDisp="";



//    QString inst_date=GENIE2_GET_SETTING(INSTALL_INFO_DATE).toString();
//    QString inst_time=GENIE2_GET_SETTING(INSTALL_INFO_TIME).toString();
    foreach (tRouteInfo *p, mRoutTypeMap) {
        strUrl="/R?"  ;
        strUrl+=QString("RM=%1&PM=%2").arg(p->routemac,p->localmac);
        strUrl+=QString("&SN=00");
        strUrl+=QString("&MN=%1&FV=%2").arg(p->modeln,p->fversion);
        int sendid=sendRequest(strUrl);
        mRequestIdMapRoutetype.insert(sendid,p->routemac);
    }


}

void QGenieDataContainner::commitSearchData()
{
    QMap<QString,int> searchmap;
    QFile f(FILE_OF_SEARCHMAP);
    if(f.open(QIODevice::ReadOnly))
    {
        QDataStream in(&f);
        if(!in.atEnd())
        {
            in >> searchmap;
        }
    }
    f.close();
    if(!searchmap.isEmpty() && mRequestSearchMap.isEmpty())
    {
//        QString rhost=GENIE2_GET_SETTING(PLUGIN_STATICS_IP).toString();
//        QString disp=GENIE2_GET_SETTING(PLUGIN_STATICS_DISP).toString();
        // strDisp="";
//        int iport=GENIE2_GET_SETTING(PLUGIN_STATICS_PORT).toInt();
        QString strUrl="/S?"  ;

        QMapIterator<QString,int> i(searchmap);
        int maxcount=MAX_SEARCH_PAIR_COUNT;
        while(i.hasNext())
        {
            i.next();
            QString key=i.key();
            mRequestSearchMap.insert(key,i.value());
            strUrl+=QString("SK=%1&C=%2&").
                    arg(changeCode(key),
                        QString::number(i.value()));

            if(--maxcount  < 0)
            {
                break;
            }
        }
        strUrl=strUrl.replace(QRegExp("&$"),"");
        QUrl url=QUrl(strUrl);
        // http->setHost(url.host(), url.port(80));
        mRequestSearchId=sendRequest(strUrl);

    }
}

////////////////////////////////////////////////////////////
//        NAME: getOSInfo()
// DESCRIPTION: Get local system infomation.
//   ARGUMENTS: None.
//     RETURNS: QString
//      AUTHOR: Chensi
//        DATA: 2012-2-21
////////////////////////////////////////////////////////////
QString QGenieDataContainner::getOSInfo()
{
#ifdef Q_OS_WIN32
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionEx(&info);
    QString ret;
    ret.sprintf("%d.%d Build %d %s",
                info.dwMajorVersion,info.dwMinorVersion,(int)(info.dwBuildNumber & 0xFFFF),info.szCSDVersion);
    return ret;
#elif defined(PLATFORM_MACOSX)
    return "macos";
#else
    return "unknow";
#endif
}

void QGenieDataContainner::slot_curSetting(int sesstion, QStringList strData)
{
    bool btemp;
    QString mCurrentSetting;
    int c = strData.count();
    bool soapOk = (c > 0) && (strData[0].toInt(&btemp) == 0) && btemp;

    if(!soapOk)
    {
        return;
    }

    mLocalInfo.insert("modeln", strData[1]);
    mLocalInfo.insert("fversion",strData[2]);

#if USE_DLL_GETINFO
    struct AdapterInfo *adapterinfo = new  struct AdapterInfo;
    memset(adapterinfo, 0, sizeof(struct AdapterInfo));

    if (adapterinfo != NULL && m_getadapterinfo != NULL){
        try {
            m_getadapterinfo(adapterinfo);
        }catch(std::exception &e){
            goto get_info_end;
        }
        QString routerip,localip,localmac;
        //index = adapterinfo->m_index;
        routerip = QString("%1").arg(adapterinfo->m_gateway);
        localip = QString("%1").arg(adapterinfo->m_local_ip);
        localmac=QString("%1").arg(adapterinfo->m_local_mac);



        mLocalInfo.insert("localip", localip);
        mLocalInfo.insert("routerip", routerip);

        if(isMacFormat(localmac))
        {
            mLocalInfo.insert("localmac",localmac);
        }

    }
get_info_end:
    delete adapterinfo;
#else

#ifdef Q_OS_WIN32
    ///    if(!sDiagnoseInterfaceLoader)
    if(!MyLib)
    {
        ///        QString stemp=qApp->applicationDirPath()+"/DiagnoseDll.dll";
        ///        sDiagnoseInterfaceLoader=new QPluginLoader(stemp);
        MyLib = new QLibrary("NetcardApi");
        IsThisPhysicalNic = (PFN)MyLib->resolve("IsThisPhysicalNic");
        Q_ASSERT(IsThisPhysicalNic);
    }
    ///    if(sDiagnoseInterfaceLoader && !sDiagnoseInterface)
    ///    {
    ///        sDiagnoseInterface=qobject_cast<DiagnoseInterface *>(sDiagnoseInterfaceLoader->instance());
    ///    }
#endif

    QString localmac;
    QList<QNetworkInterface> ifaces=QNetworkInterface::allInterfaces();
    bool finded=false;
    foreach(QNetworkInterface iface,ifaces)
    {
        if(finded)
        {
            break;
        }
#ifdef Q_OS_WIN32
        ///        if(!sDiagnoseInterface ||
        ///             !sDiagnoseInterface->IsThisPhysicalNic(iface.name()))


        if(!MyLib ||
           !IsThisPhysicalNic(iface.name().toStdWString().c_str()) )
        {
            continue;
        }

#endif
        unsigned int flags=iface.flags();

        if(!(flags & QNetworkInterface::IsLoopBack))
        {
            localmac=iface.hardwareAddress();
            if(isMacFormat(localmac))
            {
                QList<QNetworkAddressEntry> entrys=iface.addressEntries();
                foreach(QNetworkAddressEntry entry,entrys)
                {
                    if(isIpAddress(entry.ip().toString()))
                    {
                        finded=true;
                        break;
                    }
                }

                // break;
            }
        }
    }
    if(isMacFormat(localmac))
    {
        mLocalInfo.insert("localmac",localmac);
    }
#endif
    QGenieStaticInterface *ui=QGenieStaticInterface::GetSingletonPtr();
    if(ui && ui->isVisible())
    {
        ui->updateUi();
    }
}

void QGenieDataContainner::slot_macAddr(int session, QStringList strData)
{
    bool btemp;
    QString mCurrentSetting;
    int c = strData.count();
    bool soapOk = (c > 0) && (strData[0].toInt(&btemp) == 0) && btemp;
    if(!soapOk)
    {
        return;
    }

    QString strTmp = strData[1];
	QString routermac;
	int ntmp = (strTmp.count() / 2) - 1;
	for(int i = 0; i < ntmp; i++)
	{
		routermac += strTmp.left(2);
		strTmp.remove(0, 2);
		routermac += ":";
	}
	routermac += strTmp.right(2);
    if(isMacFormat(routermac))
    {
        mLocalInfo.insert("routermac",routermac);
    }
    QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM ->netgearCommand(INetgearInterface::IC_CURRENTSETTING,QStringList(),this,SLOT(slot_curSetting(int,QStringList)));

}

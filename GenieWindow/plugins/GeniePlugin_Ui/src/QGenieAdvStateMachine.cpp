#include "QGenieAdvStateMachine.h"
#include "genie2_interface.h"
#include <QDesktopServices>
#include <QLocale>
#include <QDataStream>
//#ifdef Q_OS_WIN32
//#define AD_NETGEAR_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
//#else
//#define AD_NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
//#endif
//#define AD_NETGEAR_DIR (QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/NETGEARGenie/")

#define AD_LOCAL_DIR (NETGEAR_DIR+"pic/")

#define DO_AND_CHECKEXIT(x) ((x) && (!mExitFlag))


//#define DUMMY_WHEN_STARTUP (3*60*1000)
//#define DUMMY_WHEN_GET_TOTAL (5*60*1000)
//#define DUMMY_WHEN_GET_ROUTEDISP (10*60*1000)
//#define DUMMY_WHEN_GET_ROUTEPIC (10*60*1000)

#define DUMMY_WHEN_STARTUP (0.5*60*1000)
#define DUMMY_WHEN_GET_TOTAL (0.5*60*1000)
#define DUMMY_WHEN_GET_ROUTEDISP (0.5*60*1000)
#define DUMMY_WHEN_GET_ROUTEPIC (0.5*60*1000)

_InerSignalEmiter::_InerSignalEmiter()
{

}

void _InerSignalEmiter::routeChange(const QString &route)
{
    emit signal_Router(route);
}

void _InerSignalEmiter::lanChange(const QString &lan)
{
    emit signal_Lan(lan);
}

////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////////////////////////////////////////
QGenieAdvStateMachine::QGenieAdvStateMachine()
    :mUtil(NULL)
{
    QDir::home().mkdir(NETGEAR_DIR);
    QDir(NETGEAR_DIR).mkdir(AD_LOCAL_DIR);
//    start();
}
QGenieAdvStateMachine::~QGenieAdvStateMachine()
{
    mMutex.lock();;
    if(mEventLoop)
    {
        mEventLoop->exit();
    }
    if(mUtil)
    {
        mUtil->mExitFlag=true;
    }
    mMutex.unlock();
    wait();
}

void QGenieAdvStateMachine::setRouter(const QString &route)
{
    QRegExp regx2("v(\\d+)",Qt::CaseInsensitive);
    QString stemp=route;
    stemp=stemp.replace(regx2,"");
    mSignalEmiter.routeChange(stemp);
}

void QGenieAdvStateMachine::setLan(const QString &lan)
{
    mSignalEmiter.lanChange(lan);
}


void QGenieAdvStateMachine::run()
{
    mMutex.lock();
    mEventLoop=new QEventLoop;
    mUtil=new _InerAdvUtil(this);
    connect(&mSignalEmiter,SIGNAL(signal_Router(QString)),mUtil,SLOT(slot_RouteChange(QString)));
    connect(&mSignalEmiter,SIGNAL(signal_Lan(QString)),mUtil,SLOT(slot_LanChange(QString)));
    connect(mUtil,SIGNAL(changeADV(QString,QString)),this,SIGNAL(changeADV(QString,QString)));
    mMutex.unlock();
    mEventLoop->exec();
    mMutex.lock();
    disconnect(&mSignalEmiter,SIGNAL(signal_Router(QString)),NULL,NULL);
    disconnect(&mSignalEmiter,SIGNAL(signal_Lan(QString)),NULL,NULL);
    disconnect(mUtil,SIGNAL(changeADV(QString,QString)),NULL,NULL);
    delete mUtil;
    delete mEventLoop;
    mMutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

_InerAdvUtil::_InerAdvUtil(QGenieAdvStateMachine *p)
    :QObject(NULL)
    ,mState(AS_INIT)
    ,mExitFlag(false)
{
    Q_ASSERT(QThread::currentThread()!=qApp->thread());
    mGenieAdvStateMachine=p;
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));

    mTimer.start(20000);

    mDummyTimer.setSingleShot(true);
    mDummyTimer.start(DUMMY_WHEN_STARTUP);
    mLastChangeTime=QDateTime::currentDateTime().addDays(-1);
}

_InerAdvUtil::~_InerAdvUtil()
{
    mTimer.stop();
    disconnect(&mTimer,SIGNAL(timeout()),NULL,NULL);
}

SVT_IpAddress _InerAdvUtil::getDNS(const QString &addr)
{
    QHostInfo hinfo = QHostInfo::fromName(addr);
    quint32 ip=0;
    if ( hinfo.error() == QHostInfo::NoError && !hinfo.addresses().isEmpty())
    {
        ip=hinfo.addresses()[0].toIPv4Address();
    }
    SVT_IpAddress ret=SVT_IpAddress(ip);
    qDebug()<<ret.toString();
    return ret;
}

QByteArray &_InerAdvUtil::getHttpResult(const QString &address)
{
    mArrayBuffer.clear();
    QUrl url(address);
    SVT_IpAddress ip=getDNS(url.host());
    SVT_TcpSocket tcp_sock;
    char revc_buff[1024];
    QString request= "GET %1 HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: %2\r\n\r\n";
    request=request.arg(url.path(),url.host());
    const QByteArray reqarray=request.toLatin1();

    if(!ip.isValid())
    {
        return mArrayBuffer;
    }

    if(
            DO_AND_CHECKEXIT(tcp_sock.create())
            &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,url.port(80)))
            &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
            &&DO_AND_CHECKEXIT(reqarray.size()==tcp_sock.sendData(reqarray.data(),reqarray.size()) )
            )
    {
        for(int i=0;i<100 &&!mExitFlag;i++)
        {
            if(tcp_sock.waitForReadReady(1000))
            {
				int readedtemp=tcp_sock.recvData(revc_buff,sizeof(revc_buff));
               // qDebug()<<"readedtemp"<<readedtemp;
                if(readedtemp <=0)
                {
                    break;
                }
                mArrayBuffer.append(revc_buff, readedtemp);
            }
        }
    }
    tcp_sock.close();

	//get Http data
	QString spliter = "\r\n\r\n";
	int itemp = mArrayBuffer.indexOf(spliter);

	if(itemp < 0)
		mArrayBuffer = "";
	else
		mArrayBuffer.remove(0, itemp + spliter.length());

    return mArrayBuffer;
}

bool parseTotalDiscription(const QString &str, QMap<QString,QString> &ret)
{
    ret.clear();
    QStringList allList=str.split("\n");
    foreach(QString line,allList)
    {
        line=line.trimmed();
        QStringList paras=line.split(QRegExp("(#+)"));
        if(paras.size() <3)
        {
            continue;
        }
        QString regx=paras[1];
        QString url=paras[2];
        ret.insert(regx,url);
        qDebug()<<"mRoutMapUrl.insert "<<regx<<url;
    }
    if(ret.size()==0)
    {
        return false;
    }
    return true;
}


bool _InerAdvUtil::getTotalDiscription()
{
	//get total file
    QString strDespAddr = GENIE2_GET_SETTING(AD_DESP_ADDR).toString();
    strDespAddr = QString("http://%1%2").arg(GENIE2_GET_SETTING(AD_ADDR).toString(),strDespAddr);

    QByteArray &array = getHttpResult(strDespAddr);
    QString strInfo = array.constData();

    QMap<QString,QString> maptemp;
    bool bret = false;
    bret = parseTotalDiscription(strInfo, maptemp);
    QString localfname = AD_LOCAL_DIR+"total.txt";
    if(bret)
    {
        qDebug()<<"parse ok";
        QFile f(localfname);
        if(f.open(QIODevice::WriteOnly))
        {
            f.write(strInfo.toLatin1().constData());
            f.close();
        }
    }
    else
    {
        QFile f(localfname);
        if(f.open(QIODevice::ReadOnly))
        {
            array = f.readAll();
            strInfo = array.constData();
            bret = parseTotalDiscription(strInfo, maptemp);
            f.close();
        }
    }
    if(bret)
    {
        Q_ASSERT(!maptemp.isEmpty());
        mRouteMapUrl=maptemp;
    }

	if (! bret)
	{
		return false;
	}

	//Get area support file
	QString strAreaAddr = GENIE2_GET_SETTING(AD_AREADESP_ADDR).toString();
	strAreaAddr = QString("http://%1%2").arg(GENIE2_GET_SETTING(AD_ADDR).toString(),strAreaAddr);
	QByteArray & data = getHttpResult(strAreaAddr);

	int n = data.length();
	QMap<QString, QStringList> mapArea;
	QString areaFileName = AD_LOCAL_DIR + "area.txt";
	if(! data.isEmpty())
	{
		QDataStream stream_buff(data);
		stream_buff >> mapArea;

		if (! mapArea.isEmpty())
		{
			QFile f(areaFileName);
			if(f.open(QIODevice::ReadWrite))
			{
				f.write(data, data.length());	
				f.close();
			}
		}

		mLanTypeCountryMap = mapArea;
		bret = true;
	}
	else
	{
		bret = false;
	}
 
    return bret;
}

QString getFileNameFromUrl(const QString &para)
{
    QUrl urlTemp(para);
    QFileInfo fileInfoTemp(urlTemp.path());
    return fileInfoTemp.fileName();
}

bool parseRouteDiscription(const QString &strInfo,QMap<QString,QMap<int,QString> > &ret)
{
    ret.clear();
    QStringList allList=strInfo.split("\n");

    foreach(QString line,allList)
    {
        line=line.trimmed();
        QStringList paras=line.split(QRegExp("(#+)"));
        if(paras.size() <5)
        {
            continue;
        }
        QMap<int,QString> temppara;
        QString filepath=paras[1];
        QString link=paras[2];
        QString md5=paras[3];
        QString interval=paras[4];
        QString filename =getFileNameFromUrl(filepath);

        temppara.insert(_InerAdvUtil::CI_URL,filepath);
        temppara.insert(_InerAdvUtil::CI_LINK,link);
        temppara.insert(_InerAdvUtil::CI_MD5,md5);
        temppara.insert(_InerAdvUtil::CI_INTERVAL,interval);
        ret.insert(filename,temppara);

    }
    return !ret.isEmpty();
}

bool _InerAdvUtil::CheckSupportArea(const QString& lanType, QString& area)
{
	QMap<QString, QStringList>::iterator it_temp = mLanTypeCountryMap.begin();
	for(; it_temp != mLanTypeCountryMap.end(); ++it_temp)
	{
		qDebug()<<"L:"<<it_temp.key();
		QStringList list = it_temp.value();
		foreach(QString str, list)
			qDebug()<<str;
	}


	QMap<QString, QStringList>::const_iterator it = mLanTypeCountryMap.find(lanType);
	if (it != mLanTypeCountryMap.end())
	{
		QStringList list = it.value();
		if (-1 != list.indexOf(area))
		{
			return true;
		}
	}

	return false;
}

bool _InerAdvUtil::getRouteDiscription()
{
    if(mLanType =="")
    {
        return false;
    }
    const QString wrongurl=":::::";
    QString route_para=mRoute;
    QString str_url=mRouteMapUrl.value(mRoute,wrongurl);
    bool bret=false;

    if(str_url == wrongurl)
    {
        route_para="default";
        str_url=mRouteMapUrl.value("default",wrongurl);
    }


    if(str_url != wrongurl )
    {
        mLocalDirName=AD_LOCAL_DIR+route_para;
        QDir::home().mkdir(mLocalDirName);

        str_url = str_url.replace("{address}",GENIE2_GET_SETTING(AD_ADDR).toString());

		QLocale locale = QLocale::system();
		QString area = locale.countryToString(locale.country());
		qDebug()<<"Country: "<<area;

		QString replace_str = mLanType;
		if (CheckSupportArea(mLanType, area))
			replace_str = mLanType + "_" + area.toLower();

        str_url=str_url.replace("{la}",replace_str);
		qDebug()<<"area file url: "<<str_url;
        qDebug()<<"getRouteDiscription()"<<str_url;
        QByteArray &array=getHttpResult(str_url);


        QString stemp=array.constData();
        QMap<QString,QMap<int,QString > > maptemp;
        bret=parseRouteDiscription(stemp,maptemp);
        QString localfile=mLocalDirName+"/"+getFileNameFromUrl(str_url);
        if(bret)
        {
            QFile f(localfile);
            if(f.open(QIODevice::WriteOnly))
            {
                f.write(array);
                f.close();
            }
        }
        else
        {
            QFile f(localfile);
            if(f.open(QIODevice::ReadOnly))
            {
                array=f.readAll();
                stemp=array.constData();
                bret=parseRouteDiscription(stemp,maptemp);
                f.close();
            }
        }
        if(bret)
        {
            mFileMapConf=maptemp;
        }
    }

    return bret;
}

bool _InerAdvUtil::getAllPictures()
{
    bool bret=false;
    QMap<QString,QMap<int,QString> >::iterator i=mFileMapConf.begin();
    while (i != mFileMapConf.end() ) {      
//        qDebug()<<i.key()<<i.value().value(CI_URL);


        QCryptographicHash md5Judge(QCryptographicHash::Md5);
        QString fname=mLocalDirName+"/"+i.key();
//        qDebug()<<fname;
        QFile f(fname);
        bool thisfileok=false;
        if(f.open(QIODevice::ReadOnly))
        {
            QByteArray farray=f.readAll();
            f.close();
            md5Judge.reset();
            md5Judge.addData(farray);
            thisfileok=(i.value().value(CI_MD5) == md5Judge.result().toHex());
        }
        if(!thisfileok)
        {
            QString str_url=i.value().value(CI_URL);
            str_url=str_url.replace("{address}",GENIE2_GET_SETTING(AD_ADDR).toString());
            qDebug()<<str_url;
            QByteArray &array=getHttpResult(str_url);
			int n = array.length();
            md5Judge.reset();
            md5Judge.addData(array);
            qDebug()<<i.value().value(CI_MD5)<<md5Judge.result().toHex();
            thisfileok=(i.value().value(CI_MD5) == md5Judge.result().toHex());
            if(thisfileok)
            {
                QFile f(fname);
                if(f.open(QIODevice::WriteOnly))
                {
                    f.write(array);
                    f.close();
                }
            }

        }

        bret=bret || thisfileok;
        i++;
    }
    return bret;
}

void _InerAdvUtil::showingLoopUpdate()
{
    if(mShowingStartTime.addDays(1)<QDateTime::currentDateTime())
    {
        changeState(AS_INIT);
        return;
    }

    int interval=0;
    int last_index=0;
    Q_ASSERT(!mFileMapConf.isEmpty());
    if(mFileMapConf.contains(mCurrentShowingFname))
    {
        int itemp;
        bool btemp;
        QString stemp;
        stemp=mFileMapConf.value(mCurrentShowingFname).value(CI_INTERVAL,"0");

        itemp=stemp.toInt(&btemp);
        if(btemp)
        {
            interval=itemp;
        }

        last_index=mFileMapConf.keys().indexOf(mCurrentShowingFname);
    }
//    qDebug()<<mLastChangeTime.msecsTo(QDateTime::currentDateTime())<<interval;
    if(mLastChangeTime.addMSecs(interval)<QDateTime::currentDateTime())
    {
        mLastChangeTime=QDateTime::currentDateTime();
        int cur_index=(last_index+1)%(mFileMapConf.count());
        QString curFname=mFileMapConf.keys()[cur_index];
        if(curFname != mCurrentShowingFname)
        {
            mCurrentShowingFname=curFname;
            emit changeADV(mLocalDirName+"/"+curFname,mFileMapConf.value(curFname).value(CI_LINK));
        }
    }
}

void _InerAdvUtil::changeState(ADV_STATE state)
{
    mState=state;
    mDummyTimer.stop();
    if(state==AS_ROUTE_DISP_OK)
    {
        Q_ASSERT(!mLocalDirName.isEmpty());
    }
    if(state==AS_SHOWING)
    {
        Q_ASSERT(!mFileMapConf.isEmpty());
        mCurrentShowingFname="";
        mShowingStartTime=QDateTime::currentDateTime();
    }
}

void _InerAdvUtil::slot_Timeout()
{
    Q_ASSERT(QThread::currentThread()!=qApp->thread());
//    qDebug()<<"_InerAdvUtil::slot_Timeout()"<<mState;
    if(mDummyTimer.isActive())
    {
        return;
    }
    switch(mState)
    {
    case AS_INIT:
        if(getTotalDiscription())
        {
            changeState(AS_TOTAL_DISP_OK);
        }
        else
        {
            mDummyTimer.start(DUMMY_WHEN_GET_TOTAL);
        }
        break;
    case AS_TOTAL_DISP_OK:
        if(getRouteDiscription())
        {
            changeState(AS_ROUTE_DISP_OK);
        }
        else
        {
            mDummyTimer.start(DUMMY_WHEN_GET_ROUTEDISP);
        }
        break;
    case AS_ROUTE_DISP_OK:
        if(getAllPictures())
        {
            changeState(AS_SHOWING);
        }
        else
        {
            mDummyTimer.start(DUMMY_WHEN_GET_ROUTEPIC);
        }
        break;
    case AS_SHOWING:
        showingLoopUpdate();
        break;
    default:
        Q_ASSERT(0);
    }
}

void _InerAdvUtil::slot_RouteChange(QString route)
{
    qDebug()<<"slot_RouteChange(QString route)";
    Q_ASSERT(QThread::currentThread()!=qApp->thread());
    if(mRoute !=route)
    {
        mRoute=route;
        if(mState!=AS_INIT)
        {
            Q_ASSERT(!mRouteMapUrl.isEmpty());
            changeState(AS_TOTAL_DISP_OK);
        }
    }
}

void _InerAdvUtil::slot_LanChange(QString lan)
{
    qDebug()<<"_InerAdvUtil::signal_Lan(QString lan)";
    Q_ASSERT(QThread::currentThread()!=qApp->thread());
    if(mLanType !=lan)
    {
        mLanType=lan;
        if(mState!=AS_INIT)
        {
            Q_ASSERT(!mRouteMapUrl.isEmpty());
            changeState(AS_TOTAL_DISP_OK);
        }
    }
}



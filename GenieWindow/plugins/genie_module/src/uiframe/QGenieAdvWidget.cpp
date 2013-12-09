#include "stable.h"

const int cDefaltInerval=60000;

#ifdef PLATFORM_WIN32
#define AD_NETGEAR_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
#else
#define AD_NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
#endif
#define AD_LOCAL_DIR (AD_NETGEAR_DIR+"pic/")
//#define AD_LOCAL_DIR GENIE_PATH+QString("../pic/")

#define LOCAL_DISP_FILE "disp.txt"
#define LOCAL_TOTAL_DISP "total.txt"

#define QSS_MOD "background-image: url(%1);"

QGenieAdvWidget::QGenieAdvWidget(QWidget *parent)
    : QLabel(parent)
    ,mState(AS_INIT)
    ,mRouteCache("")
    ,mGifMovie(NULL)
{

    qtmAdvChange=new QTimer();
    http = new QHttp(this);
    httpPic=new QHttp(this);
    totalhttp=new QHttp(this);
    mPicCount=0;
    iPicCoutLoop=0;
    qtmAdvChange->stop();
    strCurrentUrl="";
    strCurrentFileName="";
    connect(http, SIGNAL(done(bool)), this, SLOT(slot_InitialMap(bool)));
    connect(totalhttp, SIGNAL(done(bool)), this, SLOT(slot_TotalDiscriptionDownloaded(bool)));
    connect(httpPic, SIGNAL(done(bool)), this, SLOT(slot_DownloadPicFinished(bool)));
    //void	timeout ()
    connect(qtmAdvChange, SIGNAL(timeout ()), this, SLOT(slot_AdvTimeout ()));


    strCurrentFileName=":/fg_ad_frame.png";
    strCurrentUrl="www.netgear.com/avseries";

    QString stylesheet=QSS_MOD;
    stylesheet=stylesheet.arg(strCurrentFileName);

    qDebug() << "stylesheet" << stylesheet;
    this->setStyleSheet(stylesheet);
    //this->setStyleSheet("background-color:red");
//    QPixmap pixmap;
//    pixmap.load(strCurrentFileName);
//    this->setPixmap(pixmap);
    //this->showFullScreen();


    this->setFocusPolicy(Qt::NoFocus);
}

QGenieAdvWidget::~QGenieAdvWidget()
{
        if(http) http->abort();
        if(httpPic) httpPic->abort();
        if(totalhttp) totalhttp->abort();
	while(!mFileList.isEmpty())
	{
            QFile *tempf=mFileList.takeFirst();
            tempf->close();
            delete tempf;
	}
	qtmAdvChange->stop();
        SAFE_DELETE(http);
        SAFE_DELETE(httpPic);
        SAFE_DELETE(totalhttp);
        SAFE_DELETE(qtmAdvChange);
        SAFE_DELETE(mGifMovie);
}

void QGenieAdvWidget::startup(QString route)
{
#ifndef GET_ADV_DISP_WHENSTARTUP
    if(mState==AS_INIT || mState==AS_GETING_TOTAL_DSP || mState == AS_TOTAL_ERROR)
    {
        mRouteCache=(route=="")?"default":route;
    }
    else
    {
        startGetAdvForRoute(route);
        return;
    }
    if(mState==AS_GETING_TOTAL_DSP)
    {
        return;
    }
#endif
//    if(!deleteDirectory(AD_LOCAL_DIR))
//    {
//        qDebug() << "can not delete adv addr";
//        return;
//    }

    QDir::home().mkdir(AD_NETGEAR_DIR);
    QDir(AD_NETGEAR_DIR).mkdir(AD_LOCAL_DIR);
    if(!QDir::home().exists(AD_LOCAL_DIR))
    {
        qDebug()<<"can not mkdir "+AD_LOCAL_DIR;
        return;
    }

    http->abort();
    httpPic->abort();
    totalhttp->abort();

    qtmAdvChange->stop();
    mPicCount=0;

    QString strDespAddr=GET_SETTING_STR(AD_DESP_ADDR);
//    strDespAddr=strDespAddr.replace("{address}",GET_SETTING_STR(AD_ADDR));
    strDespAddr=QString("http://%1%2").arg(GET_SETTING_STR(AD_ADDR),strDespAddr);
    SetTotalDiscriptionUrl(strDespAddr,GET_SETTING_INT(AD_PORT));
    qDebug()<<"get adv disp file "<<strDespAddr;
}



void QGenieAdvWidget::mouseReleaseEvent ( QMouseEvent * event )
{
    qDebug()<<"QGenieAdvWidget::mouseReleaseEvent";
    if (event->button () ==Qt::LeftButton )
    {
        if(this->geometry().contains(event->pos()))
        {
            QUrl qurl(strCurrentUrl);
            GENIE_LOG(QString("open url %1").arg(strCurrentUrl));
            QDesktopServices::openUrl(qurl);
        }
    }
}

bool QGenieAdvWidget::deleteDirectory(QString DirName,int stack)
{
   if(""==DirName)
       return false;

   QDir filedir(DirName);
   if(!filedir.exists())
   {
       return true;
   }



   QString tempstr;
   const QFileInfoList  files = filedir.entryInfoList();
   if ( !files.isEmpty() )
   {
       QString tempstr;
       for(int i=0;i <files.size();i++)
       {
           QFileInfo fi=files.at(i);
           tempstr=fi.fileName();
           if ( tempstr == "." || tempstr == ".." ) ; // nothing
           else if ( fi.isDir() )
           {
               QString stempdir = DirName+tempstr;
                 deleteDirectory(stempdir,stack+1 );
           }
           else if ( fi.isFile() )
           {
              // QFile tempfile(tempstr);
               filedir.remove( tempstr);
           }
       }
   }    //end if



   if(  GENIE_DIR.exists(DirName)
#if 1
       && stack !=0
#endif
       )
   {
       if(!GENIE_DIR.rmdir(DirName)){
           return FALSE;
       }
   }
   return true;
}

void QGenieAdvWidget::getAdvWhenRouteChanged(QString route)
{
    qDebug()<<"QGenieAdvWidget::getAdvWhenRouteChanged()"<<route;
    if(route=="")
    {
        route="default";
    }
//    QRegExp regx("(\\d+)");
//    int itemp=regx.indexIn(route);
//    QStringList stlist=regx.capturedTexts();
//    if(stlist.size()!=0)
//    {
//        route=stlist.at(0);
//    }
//    else
//    {
//        route="default";
//    }

    qtmAdvChange->stop();


    if(mState==AS_INIT || mState==AS_GETING_TOTAL_DSP)
    {
        mRouteCache=route;
    }
    else
    {
        startGetAdvForRoute(route);
    }
}

void QGenieAdvWidget::startGetAdvForRoute(QString route)
{
    const QString wrongurl=":::::";
    QString url=mRoutMapUrl.value(route,wrongurl);

    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);
    if(url == wrongurl)
    {
        route="default";
        url=mRoutMapUrl.value("default",wrongurl);
    }
    if(url != wrongurl )
    {
        if(mState !=AS_INIT && mState != AS_GETING_TOTAL_DSP && mState !=AS_TOTAL_ERROR)
        {
            http->abort();
            httpPic->abort();
            totalhttp->abort();
            url=url.replace("{address}",GET_SETTING_STR(AD_ADDR));
            url=url.replace("{la}",lanmanager->getTypeStr().toLower());
           // mCurDispName=QFileInfo(url).fileName();
            strCurrentAdvDir=AD_LOCAL_DIR+route+"/";
            SetSettingTxtUrl(url,GET_SETTING_INT(AD_PORT));
        }

    }
}


void QGenieAdvWidget::SetSettingTxtUrl(QString strUrl,int port)
{
    totalhttp->abort();
    http->abort();
    httpPic->abort();
//    if(!deleteDirectory(strCurrentAdvDir))
//    {
//        qDebug() << "can not delete "<<strCurrentAdvDir<<"addr";
//        return;
//    }

    if(!GENIE_DIR.exists(strCurrentAdvDir)&&!GENIE_DIR.mkdir(strCurrentAdvDir))
    {
        qDebug()<<"can not mkdir "+strCurrentAdvDir;
        return;
    }

    qDebug()<<"QGenieAdvWidget::SetSettingTxtUrl"<<strUrl<<port;
    url=QUrl(strUrl);
    http->setHost(url.host(), port);
    http->get(strUrl);
}

void QGenieAdvWidget::SetTotalDiscriptionUrl(QString strUrl,int port)
{
    url=QUrl(strUrl);
    totalhttp->setHost(url.host(), port);
    qDebug()<<"set host "<<url.host()<<port;
    totalhttp->get(strUrl);
    mState=AS_GETING_TOTAL_DSP;
}


void QGenieAdvWidget::slot_AdvTimeout ()
{

    ASSERT(iPicCoutLoop < qmapLinkUrl.count());
    strCurrentFileName=qmapLinkUrl.keys().at(iPicCoutLoop);
    strCurrentUrl=qmapLinkUrl.value(strCurrentFileName);

 //   qDebug() << "QGenieAdvWidget::slot_AdvTimeout pixmap " << strCurrentFileName;
//    QPixmap pixmap;
//    pixmap.load(strCurrentFileName);
//    this->setPixmap(pixmap);
    //this->setMask(pixmap.mask());

    if(md5_verify(strCurrentFileName))
    {
        if(strCurrentFileName.toLower().endsWith(".gif") || strCurrentFileName.toLower().endsWith(".swf") )
        {
            this->setStyleSheet("");
            SAFE_DELETE(mGifMovie);
            mGifMovie=new QMovie(strCurrentAdvDir+strCurrentFileName);
            this->setMovie(mGifMovie);
            mGifMovie->start();
            qDebug()<<"set Movie to adv pan"<<strCurrentFileName;
        }
        else
        {
            if(mGifMovie)
            {
                mGifMovie->stop();
                SAFE_DELETE(mGifMovie);
            }

            QString stylesheet=QSS_MOD;
            stylesheet=stylesheet.arg(strCurrentAdvDir+strCurrentFileName);

            qDebug() << "stylesheet" << stylesheet;
            this->setStyleSheet(stylesheet);
        }
    }
    else
    {
        this->setStyleSheet("");
        qDebug()<<"adv md5 error!!!"<<strCurrentFileName;
    }


    iPicCoutLoop=iPicCoutLoop+1;
    iPicCoutLoop=iPicCoutLoop%mPicCount;
    int interval=qmapIntval.value(strCurrentFileName,cDefaltInerval);

    qtmAdvChange->stop();
    qtmAdvChange->start(interval);


}

void QGenieAdvWidget::slot_InitialMap(bool  error)
{
    // QMap<QString,QString> qmapPicUrl;

    bool bHaveNewPic=false;
    QByteArray tempArray;
    QString strInfo;
    if (!error)
    {

        tempArray = http->readAll();
         strInfo= tempArray.constData();
       // http->close();
        strInfo.trimmed();

        bool btemp=parseRouteDiscription(strInfo,qmapPicUrl,qmapLinkUrl,
                                         qmapFileMd5,qmapIntval);
        if(btemp)
        {
            bHaveNewPic=true;
        }
        else
        {

        }//else
    }//if (!error)
#ifdef USE_LOCAL_ADV_DESP_WHEN_ERROR
	if(!bHaveNewPic)
	{
                if(!GENIE_DIR.exists(strCurrentAdvDir)&&!GENIE_DIR.mkdir(strCurrentAdvDir))
		{
			GENIE_LOG("can not create local dir ,so can not use local disp file2");
			return;
		}
		QString fname=strCurrentAdvDir+LOCAL_DISP_FILE;
		QFile tempf(fname);
		if(tempf.open(QIODevice::ReadOnly))
		{
			tempArray=tempf.readAll();
			strInfo=tempArray.constData();
			strInfo.trimmed();
			bool btemp=parseRouteDiscription(strInfo,qmapPicUrl,qmapLinkUrl,qmapFileMd5,
				qmapIntval);
			if(btemp)
			{
				bHaveNewPic=true;
			}
		}
	}
	else
	{
		QString fname=strCurrentAdvDir+LOCAL_DISP_FILE;
		QFile tempf(fname);
		if(tempf.open(QIODevice::WriteOnly))
		{
			tempf.write(tempArray);
		}
		tempf.close();

	}

#endif

    if(bHaveNewPic)
    {

        qDebug()<<"have new pic to download";
        QDir advdir(qApp->applicationDirPath());
        if (!advdir.cd(strCurrentAdvDir))
        {
            qDebug()<<"error when enter strCurrentAdvDir"<<strCurrentAdvDir;
            return;
        }
        QStringList toDelete=advdir.entryList(QDir::Files);
        QStringList filelist=qmapPicUrl.keys();
//        foreach(QString curadvurl,urllist)
//        {
//            QUrl url(curadvurl);
//            curadvurl=curadvurl.replace("{address}",GET_SETTING_STR(AD_ADDR));
//            QFileInfo fileInfo(url.path());
//            QString fileName = fileInfo.fileName();
//            curadvlist.append(fileName);
//        }
        foreach(QString sfname,toDelete)
        {
            if(!filelist.contains(sfname)
#ifdef USE_LOCAL_ADV_DESP_WHEN_ERROR
                &&!(sfname== LOCAL_DISP_FILE)
#endif
                )
            {
                qDebug()<<"remove file"<<sfname;
                advdir.remove(sfname);
            }
        }

        bool bhttplaunched=false;
        for (int i=0;i<qmapPicUrl.count();i++)
        {
            QString strKey=qmapPicUrl.values().at(i);
            strKey=strKey.replace("{address}",GET_SETTING_STR(AD_ADDR));

            bool btemp=donwloadFile( strKey);
            qDebug()<<"try start get file "<<strKey<<btemp;
            bhttplaunched |= btemp;

        }
        if(!bhttplaunched)
        {
            qDebug()<<"start pic loop because no http command needed!";
            slot_DownloadPicFinished(false);
        }
        else
        {
            qDebug()<<"need reload image";
        }



    }//if(bHaveNewPic)
}



bool QGenieAdvWidget::donwloadFile(QString strFileName)
{
       //QMap<QString,QString> qmapFileUrl;
       //httpPic
       QUrl url(strFileName);
       QFileInfo fileInfo(url.path());
       QString fileName = fileInfo.fileName();
       if (fileName.isEmpty())
       {
           return false;
       }
       QString fullPath=strCurrentAdvDir +fileName;
       if (QFile::exists(fullPath))
       {
           if(!md5_verify(fileName))
           {
                QFile::remove(fullPath);
           }
           else
           {
               return false;
           }
       }

       QFile* file = new QFile(fullPath);
       if (!file->open(QIODevice::WriteOnly))
       {
           delete file;
           file = 0;
           return false;
       }

       QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
       httpPic->setHost(url.host(), mode, GET_SETTING_INT(AD_PORT));

       if (!url.userName().isEmpty())
           httpPic->setUser(url.userName(), url.password());

       httpRequestAborted = false;
       QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
       if (path.isEmpty())
           path = "/";
       httpGetId = httpPic->get(path, file);
       mFileList.append(file);
/*
       progressDialog->setWindowTitle(tr("HTTP"));
       progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
       downloadButton->setEnabled(false);
*/
       return true;
}


void QGenieAdvWidget::slot_DownloadPicFinished(bool  error)
{
    // QMap<QString,QString> qmapPicUrl;
    qDebug() << "QGenieAdvWidget::downloadPicFinished" << error;
    while(!mFileList.isEmpty())
    {
        QFile *tempf=mFileList.takeFirst();
        tempf->close();
        delete tempf;
    }

    if (!error)
    {
        if ( mPicCount>0)
        {
            qtmAdvChange->start(5000);
        }
    }
}

bool QGenieAdvWidget::parseTotalDiscription(const QString &str)
{
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
        mRoutMapUrl.insert(regx,url);
        qDebug()<<"mRoutMapUrl.insert "<<regx<<url;
    }
    if(mRoutMapUrl.size()==0)
    {
        qDebug()<<"parse adv discription error";
        return false;
    }
    return true;
}


bool QGenieAdvWidget::parseRouteDiscription(const QString &para,
                           QMap<QString,QString> &picurlmap,
                           QMap<QString,QString> &linkurlmap,
                           QMap<QString,QString> &md5map,
                           QMap<QString,int> &intervalmap)
{
    linkurlmap.clear();
    md5map.clear();
    intervalmap.clear();
    mPicCount=0;

    QStringList allList=para.split("\n");
  //  QRegExp regx("(#+)(.*)(#+)(.*)(#+)(.*)(#+)(.*)(#+)");
    foreach(QString line,allList)
    {
        line=line.trimmed();
        QStringList paras=line.split(QRegExp("(#+)"));
        if(paras.size() <5)
        {
            continue;
        }
        QString filepath=paras[1];
        QString url=paras[2];
        QString md5=paras[3];
        QString interval=paras[4];



        QUrl urlTemp(filepath);
        QFileInfo fileInfoTemp(urlTemp.path());
        QString filename = fileInfoTemp.fileName();
        linkurlmap.insert(filename,url);
        md5map.insert(filename,md5);
        picurlmap.insert(filename,filepath);

        bool btemp;
        int itemp=interval.toInt(&btemp);
        if(!btemp)
        {
            itemp=cDefaltInerval;
        }
        intervalmap.insert(filename,itemp);
    }
    mPicCount=linkurlmap.count();
    if(linkurlmap.isEmpty())
    {
        return false;
    }

    return true;
}


void QGenieAdvWidget::slot_TotalDiscriptionDownloaded(bool  error)
{

    qDebug() << "QGenieAdvWidget::slot_TotalDiscriptionDownloaded" << error;
    qDebug() << totalhttp->errorString();
    GENIE_LOG("QGenieAdvWidget::slot_TotalDiscriptionDownloaded");
    QString strInfo;
    if(!error)
    {
        mState=AS_TOTAL_DSP_OK;
        mRoutMapUrl.clear();
        QByteArray tempArray;
        tempArray = totalhttp->readAll();
        strInfo = tempArray.constData();
        strInfo.trimmed();
        bool btemp=parseTotalDiscription(strInfo);
        if(!btemp)
        {
            mState=AS_TOTAL_ERROR;
        }

        else if(mRouteCache!= "")
        {
            qDebug()<<"use cache to download adv for route";
            startGetAdvForRoute(mRouteCache);
        }
    }
    else
    {
        mState=AS_TOTAL_ERROR;
    }
#ifdef USE_LOCAL_ADV_DESP_WHEN_ERROR
    if(!GENIE_DIR.exists(AD_LOCAL_DIR)&&!GENIE_DIR.mkdir(AD_LOCAL_DIR))
    {
        GENIE_LOG("can not create local dir ,so can not use local disp file");
        return;
    }

    QString fname=AD_LOCAL_DIR+LOCAL_TOTAL_DISP;
    QFile tempf(fname);
    qDebug()<<"current local total desp file is"<<fname;
    if(mState == AS_TOTAL_ERROR)
    {
        //load file and parse
        if(tempf.open(QIODevice::ReadOnly))
        {
            strInfo=tempf.readAll().constData();
            if(parseTotalDiscription(strInfo))
            {
                mState=AS_TOTAL_DSP_OK;
                if(mRouteCache!= "")
                {
                    qDebug()<<"use cache to download adv for route2";
                    startGetAdvForRoute(mRouteCache);
                }
            }
        }

    }
    else
    {
        //save file

        if(tempf.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&tempf);
            stream<<strInfo;
        }

    }
    tempf.close();
#endif

}
bool QGenieAdvWidget::md5_verify(QString strFileName)
{
    QString md5;
    QFile file(strCurrentAdvDir+strFileName);
    if (file.open(QFile::ReadOnly))
    {
        QCryptographicHash md5Judge(QCryptographicHash::Md5);
        md5Judge.addData(file.readAll());
        md5 = md5Judge.result().toHex();

        file.close();

        QString strMd5Result=qmapFileMd5.value(strFileName);
        strMd5Result=strMd5Result.trimmed();
        md5=md5.trimmed();
        if (strMd5Result==md5)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }
    return false;
}

void QGenieAdvWidget::mousePressEvent ( QMouseEvent * event )
{
    QLabel::mousePressEvent(event);
    event->accept();
}

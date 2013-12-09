#include "stable.h"
#define LOG_FILENAME "genie"
#define LOG_DATE_FORMAT "dd-hh:mm:ss:zzz"
#define LOG_CACHE_MAX_SIZE 100

#define FILE_FLUSH_PERWRITE 30

#ifdef PLATFORM_WIN32
#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
#else
#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
#endif
#define LOG_FULL_PATH (NETGEAR_DIR+"log")

#define LOG_SIZE_LIMILIZE (1024*1024)
#define LOG_MAX_NUM 5
#define FILENAME_DATE_FORMAT "yyyyMMdd-hhmmss"

#define FILENAME_STRATEGY 2


QGenieInterfaceLogManager::QGenieInterfaceLogManager()
    :mUseCache(true)
    ,mMutex(QMutex::Recursive)
{
    qDebug()<<"QGenieInterfaceLogManager::QGenieInterfaceLogManager()";
    mCheckThreadId=(unsigned int)QThread::currentThreadId();
    ILaunchManager *launchinf=QGenieLaunchInf::sLaunchManager;
    ASSERT(launchinf);
    mAvalable=!(QApplication::arguments().contains("-nolog"));

    connect(this,SIGNAL(signal_WriteLog(QString)),this,SLOT(slot_RealWriteLog(QString)));
    mMutex.lock();
    whichFile();

//    if(mAvalable)
//    {
//        mLogfile.setFileName(LOG_FILENAME);
//        if(!mLogfile.open(QFile::Append))
//        {
//            QMessageBox::critical(0,"can not open log file",LOG_FILENAME);
//        }
//        mLogfile.write("\n\n\n");
//    }
    slot_RealWriteLog("\n\n\n");
    mMutex.unlock();


}
QGenieInterfaceLogManager::~QGenieInterfaceLogManager()
{
     closeFile();
}

QString QGenieInterfaceLogManager::getFileName(QUuid uuid,bool *ok)
{
    QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(pluginmanager);
    QString stemp_pluginfile="unknown plugin";
    plug_inf *p=pluginmanager->getPluginInf(uuid);
    bool btemp=false;
    if(p)
    {
        stemp_pluginfile=p->filename;
        btemp=true;
    }
    else
    {
        if(uuid==QUuid(FRAME_UUID))
        {
            stemp_pluginfile="mainframe";
            btemp=true;
        }
    }
    if(ok)
    {
        *ok=btemp;
    }
    return stemp_pluginfile;
}



void QGenieInterfaceLogManager::writeLog(QString s,QUuid uuid)
{
  //  if((unsigned int)QThread::currentThreadId()!=mCheckThreadId)
  //  {
   //     return;
  //  }

    mMutex.lock();
    if(!mAvalable)
    {
        mMutex.unlock();
        return;
    }
    bool ok;
    QString stemp_pluginfile=getFileName(uuid,&ok);

    QString stemp_time=QDateTime::currentDateTime().toString(LOG_DATE_FORMAT);

    QString stid=QString::number((unsigned int)QThread::currentThreadId());

    QString stemp_out=QString("[%1:%2:%3] %4\n").arg(stid,stemp_pluginfile,stemp_time,s);
    if(!mUseCache)
    {
        emit signal_WriteLog(stemp_out);
        //slot_RealWriteLog(stemp_out);
    }
    else
    {
        _cache cache;
        cache.str=QString("[%1:{uuid}:%2] %3\n").arg(stid,stemp_time,s);
        cache.uuid=uuid;
        mCache.append(cache);
    }

    mMutex.unlock();
}

void QGenieInterfaceLogManager::slot_RealWriteLog(const QString &s)
{
    ASSERT(mCheckThreadId==(unsigned int)QThread::currentThreadId());
    if(mLogfile.isWritable())
    {
        QTextStream out(&mLogfile);
        out<<s;
    }
    //qDebug()<<"logfile size"<<mLogfile.size();
    if(mLogfile.size()>LOG_SIZE_LIMILIZE)
    {
        whichFile();
    }

}

void QGenieInterfaceLogManager::enableCache()
{
    mMutex.lock();
    mUseCache=true;
    mMutex.unlock();
}

void QGenieInterfaceLogManager::disableCache()
{
    mMutex.lock();
    if(!mAvalable)
    {
        mMutex.unlock();
        return;
    }

    mUseCache=false;
    while(!mCache.isEmpty())
    {
        _cache cache=mCache.takeFirst();
        QString fname=getFileName(cache.uuid);
        QString strtemp=cache.str.replace("{uuid}",fname);
        emit signal_WriteLog(strtemp);
//        slot_RealWriteLog(strtemp);
    }

    mMutex.unlock();
}
void QGenieInterfaceLogManager::closeFile()
{
    mMutex.lock();
    mAvalable=false;

    mLogfile.close();
    mMutex.unlock();
}

void QGenieInterfaceLogManager::whichFile()
{
#ifdef PLATFORM_MACOSX
    QDir tempdir=QDir::home();
#else
    QDir tempdir=QDir::home();
#endif

    if(!tempdir.exists(NETGEAR_DIR))
    {
        tempdir.mkdir(NETGEAR_DIR);
    }
    if(!tempdir.exists(LOG_FULL_PATH))
    {
        tempdir.mkdir(LOG_FULL_PATH);
    }

    if (!tempdir.cd(LOG_FULL_PATH))
    {
        qDebug("can not find the plugin path");
        mAvalable=false;
        return;
    }
#if FILENAME_STRATEGY ==1
    QString stemp=QString("%1_(\\d+).log").arg(LOG_FILENAME);
#elif FILENAME_STRATEGY ==2
    QString stemp=QString("%1_(\\d{8})-(\\d{6}).log").arg(LOG_FILENAME);
#endif
    QRegExp logregex=QRegExp(stemp,Qt::CaseInsensitive);
    QMap<qint64,QString> tempmap;

    foreach (QString fileName, tempdir.entryList(QDir::Files))
    {
        if(logregex.exactMatch(fileName))
        {
#if FILENAME_STRATEGY ==1
            QString n=logregex.cap(1);
            bool btemp;
            int itemp=n.toInt(&btemp);
            if(btemp)
            {
                tempmap.insert(itemp,fileName);
            }
#elif FILENAME_STRATEGY == 2
            QString n1=logregex.cap(1);
            QString n2=logregex.cap(2);
            bool btemp1,btemp2;
            qint64 itemp=(qint64)(n1.toLongLong(&btemp1))*1000000+n2.toInt(&btemp2);
            if(btemp1 || btemp2)
            {
                tempmap.insert(itemp,fileName);
            }
#endif
            qDebug()<<"logfile name match"<<fileName;


        }
    }

    //check size of last file
#if FILENAME_STRATEGY ==1
    QString validfilename=QString("%1_00.log").arg(LOG_FILENAME);
#elif FILENAME_STRATEGY ==2
    QString validfilename=QString("%1_%2.log")
                          .arg(LOG_FILENAME,QDateTime::currentDateTime().toString(FILENAME_DATE_FORMAT));
#endif
    if(!tempmap.isEmpty())
    {
        QList<QString> templist= tempmap.values();
        for(int i=0;i<templist.size()-LOG_MAX_NUM;i++)
        {
            tempdir.remove(templist.at(i));
        }


        QString checkfilename=templist.last();
        validfilename=checkfilename;
        QString tempfilename=QString("%1/%2").arg(LOG_FULL_PATH,checkfilename);
        if(QFile(tempfilename).size() > LOG_SIZE_LIMILIZE)
        {
            if(templist.size()>=LOG_MAX_NUM)
            {
                tempdir.remove(templist.at(templist.size()-LOG_MAX_NUM));
            }
            qDebug()<<"file too large "<<checkfilename;

            int itemp=tempmap.keys().last()+1;
#if FILENAME_STRATEGY==1
            int itemp=tempmap.keys().last()+1;
            validfilename=QString("%1_%2.log").arg(LOG_FILENAME,QString::number(itemp));
#elif FILENAME_STRATEGY ==2
            validfilename=QString("%1_%2.log")
                                  .arg(LOG_FILENAME,QDateTime::currentDateTime().toString(FILENAME_DATE_FORMAT));

#endif
        }
    }

    mLogfile.close();;
    mLogfile.setFileName(QString("%1/%2").arg(LOG_FULL_PATH,validfilename));
    if(!mLogfile.open(QFile::Append))
    {
        QMessageBox::critical(0,"can not open log file",LOG_FILENAME);
        mAvalable=false;
    }

}


#include "QGenieInterface2LogManager.h"
#include "genie2_interface.h"
#include "QGenieInterface2Helper.h"
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#define LOG_FILENAME "genie2"
#define LOG_DATE_FORMAT "dd-hh:mm:ss:zzz"
//#define LOG_CACHE_MAX_SIZE 100

#define FILE_FLUSH_PERWRITE 30

//#ifdef Q_OS_WIN32
//#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
//#else
//#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
//#endif
//#define NETGEAR_DIR (QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/NETGEARGenie/")
#define LOG_FULL_PATH (NETGEAR_DIR+"log")

#define LOG_SIZE_LIMILIZE (1024*1024)
#define LOG_MAX_NUM 5
#define FILENAME_DATE_FORMAT "yyyyMMdd-hhmmss"

#define FILENAME_STRATEGY 2


QGenieInterface2LogManager::QGenieInterface2LogManager()
{
    mAvalable=!(QApplication::arguments().contains("-nolog"));

    whichFile();

    slot_RealWriteLog("\n\n\n");
    writeLog("Genie1.2 Started",QGenieIface2ShareObject::sUuidIdx);


}
QGenieInterface2LogManager::~QGenieInterface2LogManager()
{
     closeFile();
}




void QGenieInterface2LogManager::writeLog(const QString &s,int uuid)
{
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    if(!mAvalable)
    {
        return;
    }
    QGenieInterface2Helper*i2hlp=QGenieInterface2Helper::GetSingletonPtr();
    QString stemp_pluginfile=i2hlp?i2hlp->getPluginFileName(uuid):"unknown plugin";

    QString stemp_time=QDateTime::currentDateTime().toString(LOG_DATE_FORMAT);
    QString stemp_out=QString("[%1 %2] %3\n").arg(stemp_pluginfile,stemp_time,s);
    slot_RealWriteLog(stemp_out);

}

void QGenieInterface2LogManager::slot_RealWriteLog(const QString &s)
{
    if(mLogfile.isWritable())
    {
        QTextStream out(&mLogfile);
        out<<s;
    }
    if(mLogfile.size()>LOG_SIZE_LIMILIZE)
    {
        whichFile();
    }

}

void QGenieInterface2LogManager::closeFile()
{
    mAvalable=false;

    mLogfile.close();
}

void QGenieInterface2LogManager::whichFile()
{

  //  QString sstemp=LOG_FULL_PATH;
    if(!QDir::home().exists(LOG_FULL_PATH))
    {
        bool btemp=QDir::home().mkpath(LOG_FULL_PATH);
        Q_ASSERT(btemp);
    }

    QDir tempdir=QDir::home();

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

//            int itemp=tempmap.keys().last()+1;
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


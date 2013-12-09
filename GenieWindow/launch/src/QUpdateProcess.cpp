#include "QUpdateProcess.h"
#include "QLaunchUpdateDialog.h"
#include "ui_QLaunchUpdateDialog.h"
#include "QLauncher.h"
#include <QDir>
#include "defines.h"
#include "setting_def.h"
#include <QSettings>
#include <QCryptographicHash>
#include "QLaunchFileDiscription.h"
#include "QLaunchManager.h"
#include "LanguageMainFrame.h"
#include <QtCore>
#include "QGenieSettingHelper.h"
#include <QDesktopServices>

#define TIME_TEXT_UPDATE_INTERVAL 800


QUpdateProcess::QUpdateProcess(QLaunchUpdateDialog *dialog)
    :mUpdateReson(UR_UNKNOWN)
    ,mState(PS_INIT)
{
 //   ASSERT(dialog);
    mUpdateDialog=dialog;
#ifdef SHOW_REMAIN_TIME
    mUpdateStamp=QTime::currentTime();
    mCurCacheIdx=0;
    mSizeStamp=0;
#endif
    connect(&mHttp,SIGNAL(requestFinished(int,bool)),this,SLOT(slot_CommandFinished(int,bool)));
    connect(&mHttp,SIGNAL(dataReadProgress(int,int)),this,SLOT(slot_DataReadProgress(int,int)));
    connect(&mHttp,SIGNAL(done(bool)),this,SLOT(slot_Done(bool)));
    connect(&mHttp,SIGNAL(requestStarted(int)),this,SLOT(slot_CommandStarted(int)));
}

QUpdateProcess::~QUpdateProcess()
{
    mHttp.abort();
    QLaunchManager *lmanager=QLaunchManager::GetSingletonPtr();
    ASSERT(lmanager);
    if(mUpdateReson == UR_TIMER_TRIGGER)
    {
        ASSERT(lmanager->mProcess == this);
        lmanager->mProcess=NULL;
    }
    mReceiveFile.close();
}

void QUpdateProcess::startup(UPDATE_RESON reson)
{
    mUpdateReson=reson;

    if(mUpdateDialog)
    {
        mUpdateDialog->changeUiState(DS_GETDISP_DB);
    }

//    mCmdIdAndFile.clear();
    if(mUpdateReson==UR_USER_CHOISE)
    {
        startGetDisp();
    }
    else if(mUpdateReson==UR_WHEN_NOGENIE)
    {
        ASSERT(0);
    }
    else if(mUpdateReson == UR_TIMER_TRIGGER)
    {
        startGetDisp();
    }
    else
    {
        ASSERT(0);
    }
}

void QUpdateProcess::startGetDisp()
{

    mReceiveFile.close();
    QDir::home().mkdir(NETGEAR_DIR);
    if(!deleteDirectory(HTTP_LOCAL_DIR))
    {
        qDebug() << "can not delete old file";
        mState=PS_FINISH_ERROR_DIROP;
        if(mUpdateDialog)
        {
            mUpdateDialog->changeUiState(DS_FINISH);
        }
        return ;
    }

    QDir::home().mkdir(HTTP_LOCAL_DIR);
    if(!QDir::home().exists(HTTP_LOCAL_DIR))
    {
        mState=PS_FINISH_ERROR_DIROP;
        qDebug()<<"can not mkdir "<<HTTP_LOCAL_DIR;
        if(mUpdateDialog)
        {
            mUpdateDialog->changeUiState(DS_FINISH);
        }
        return;
    }

    mState=PS_GETDISP;
    QString localfile=QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME;

    mReceiveFile.setFileName(localfile);
    mReceiveFile.open(QIODevice::WriteOnly);
    mHttp.setHost(GET_SETTING_STR(UPDATE_IP),QHttp::ConnectionModeHttp,GET_SETTING_INT(UPDATE_PORT));



    QString tempdespfilename=GET_SETTING_STR(UPDATE_DESP_FILE);
#ifdef MULTILAN_RELEASENOTE
    QLaunchManager *lanchmanager=QLaunchManager::GetSingletonPtr();
    ASSERT(lanchmanager);
    tempdespfilename=tempdespfilename.replace("{la}",lanchmanager->mGenieLanCache.toLower());

#endif

    int itemp= mHttp.get(tempdespfilename,&mReceiveFile);
    qDebug()<<"GetDispFile"<<tempdespfilename;
//     mCmdIdAndFile.insert(itemp,GET_SETTING_STR(UPDATE_DESP_FILE));
    //addListMessage(QString("Get Remote Disp File http://"HTTP_HOST HTTP_DISP" to ")+localfile);
}

#if 0
void QUpdateProcess::startGetDB()
{
    qDebug()<<"QUpdateProcess::startGetDB()";
    mReceiveFile.close();

    QFile file(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);
    file.open(QFile::ReadOnly);

    QString desp_verify=file.readLine().replace("\n","");
    desp_verify=desp_verify.trimmed();
    if(desp_verify !=DESP_VERIFY_STR)
    {

        qDebug()<<"desp file verify error";
        whenError();
        return;
    }
    QString remotedb = QString("http://")+GET_SETTING_STR(UPDATE_IP)+QLatin1String(file.readLine());

    mHttpPath=remotedb;
    mHttpPath.remove(QRegExp("/"HTTP_REMOTE_FILE_NAME));

    mState=PS_GETDB;
    mReceiveFile.setFileName(QString(HTTP_LOCAL_DIR)+"/"+DB_IN_LOCAL);
    mReceiveFile.open(QIODevice::WriteOnly);
    int itemp=mHttp.get(remotedb,&mReceiveFile);
//    mCmdIdAndFile.insert(itemp,remotedb);
}
#endif

void QUpdateProcess::startGetAllFiles()
{
    mReceiveFile.close();
    QLaunchFileDiscription fdisp(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);

    QString host=fdisp.getHost();
    QString httppath=fdisp.getPath();
    int port=fdisp.getPort();

    QString localfile=QString(HTTP_LOCAL_DIR)+"/"+HTTP_UPDATE_FILE_NAME;


//    QString httpfilename=httppath+ "/"+tempfilename;
    mReceiveFile.setFileName(localfile);
    if(localfile=="" || !mReceiveFile.open(QIODevice::WriteOnly))
    {
        qDebug()<<mReceiveFile.fileName();
        mState=PS_FINISH_ERROR_GETFILES;
        if(mUpdateDialog)
        {
            mUpdateDialog->changeUiState(DS_FINISH);
        }
        return;
    }
    mState=PS_GETFILES;
    if(mUpdateDialog)
    {
        mUpdateDialog->changeUiState(DS_GETFILE);
    }
    mHttp.setHost(host,QHttp::ConnectionModeHttp,port);
    int itemp=mHttp.get(httppath,&mReceiveFile);
#ifdef SHOW_REMAIN_TIME
    mCommandStamp=itemp;
#endif
   // addListMessage(QString("start get file: "+tempfilename));
    qDebug()<<"update getfile :"<<host<<httppath<<port;
//    mCmdIdAndFile.insert(itemp,httpfilename);
}

void QUpdateProcess::userCancel()
{
    mHttp.clearPendingRequests();
    mHttp.abort();
    mReceiveFile.close();
    mState=PS_FINISH_ERROR_USERCLICK;
    if(mUpdateDialog)
    {
        mUpdateDialog->changeUiState(DS_FINISH);
    }
}

bool QUpdateProcess::deleteDirectory(QString DirName,int stack)
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
               QString stempdir = DirName+("/")+tempstr;
                 deleteDirectory(stempdir,stack+1 );
           }
           else if ( fi.isFile() )
           {
              // QFile tempfile(tempstr);
               filedir.remove( tempstr);
           }
       }
   }    //end if



   if(  filedir.exists(DirName)
#if 1
       && stack !=0
#endif
       )
   {
       if(!filedir.rmdir(DirName)){
           return FALSE;
       }
   }
   return true;
}

bool QUpdateProcess::md5Match(QString filename,QString md5str)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<< "can not open file "<<filename<<"QLaunchUpdateDialog::md5Match";
        return false;
    }
    QString tempstr;
    QCryptographicHash *hash=new QCryptographicHash(QCryptographicHash::Md5);
    hash->addData(file.readAll());
    tempstr=hash->result().toHex();
    delete hash;
    return tempstr==md5str;


}

void QUpdateProcess::runExeFile(QString fpath)
{
#ifdef _WIN32
        QFileInfo fi(fpath);
        const QString cmd=fi.absoluteFilePath();
        STARTUPINFOA si/*={sizeof(si)}*/;
        PROCESS_INFORMATION pi;
        memset(&si,'\0',sizeof(si));
        memset(&pi,'\0',sizeof(pi));
        si.cb = sizeof (si);
        char cmdbuffer[256];
        sprintf(cmdbuffer,"%s",cmd.toLocal8Bit().data());
        char* wc=(char *)cmdbuffer;

//            bool ret = CreateProcessA ( NULL,wc, NULL, NULL, FALSE, /*CREATE_SUSPENDED*/0, NULL ,NULL, &si, &pi);
        bool ret = CreateProcessA ( NULL,wc, NULL, NULL, FALSE, /*CREATE_SUSPENDED*/0, NULL ,NULL, &si, &pi);

#else
        //QString strUrl=GET_SETTING_STR(TITLE_URL);
      //  QDesktopServices::openUrl(QDir::homePath());

          static const QString PRG = "open %1 ";
          system( PRG.arg( fpath ).toLocal8Bit() );
          QTimer::singleShot(5000,this,SLOT(slot_startUpdateDMG()));

//          sleep(3000);

#endif
}
#ifdef PLATFORM_MACOSX
void    QUpdateProcess::slot_startUpdateDMG()
{
    static const QString mdir="open /Volumes/NETGEARGenie/NETGEAR_Genie_Installer.mpkg";
    system(mdir.toLocal8Bit());
}
#endif




void    QUpdateProcess::slot_CommandStarted ( int id)
{
#ifdef SHOW_REMAIN_TIME
    if(id == mCommandStamp)
    {
        mUpdateStamp=QTime::currentTime();
        mTextChangeTime=QTime::currentTime().addSecs(-60);
        mCurCacheIdx=0;
        mSizeStamp=0;
        for(int i=0;i<cCacheSize;i++)
        {
            mTimeIntervalCache[i]=mSizeCache[i]=-1;
        }
        mUpdateDialog->ui->text_remain->setText("");
    }
#endif
    if(mUpdateDialog)
    {
        QString stemp="";
        if(mState == PS_GETDISP || mState ==PS_GETDB)
        {
            stemp=mUpdateDialog->getText(L_MAIN_UPDATE_CHECK_DESP);
        }
        else if(mState ==PS_GETFILES)
        {
            stemp=mUpdateDialog->getText(L_MAIN_UPDATE_DOWN_FILE);
        }
        else
        {
            ASSERT(0);
        }
        qDebug()<<"QLaunchUpdateDialog::slot_CommandStarted"<<id;
      //  QString s=QString("start getting file ")+mCmdIdAndFile.value(id,"");
        mUpdateDialog->ui->text_curfile->setText(stemp);
        mUpdateDialog->ui->progressBar->setValue(0);
    }

}

void    QUpdateProcess::slot_CommandFinished ( int id, bool error )
{

}

void    QUpdateProcess::slot_DataReadProgress ( int done, int total )
{
    qDebug()<<"QUpdateProcess::slot_DataReadProgress";
    if(mUpdateDialog)
    {
#ifdef SHOW_REMAIN_TIME
        mTimeIntervalCache[mCurCacheIdx]=mUpdateStamp.msecsTo(QTime::currentTime());
    mSizeCache[mCurCacheIdx]=done-mSizeStamp;

    mCurCacheIdx=(mCurCacheIdx+1)%cCacheSize;
    mUpdateStamp=QTime::currentTime();
    mSizeStamp=done;

    if(mTextChangeTime.msecsTo(QTime::currentTime()) >= TIME_TEXT_UPDATE_INTERVAL)
    {
        mTextChangeTime=QTime::currentTime();
        int s=0;int t=0;
        for(int i=0;i<cCacheSize;i++)
        {
            if(mSizeCache[i]>=0)
            {
                s+=mSizeCache[i];
                t+=mTimeIntervalCache[i];
            }
        }
        int result=9999999;
        if(s !=0)
        {
            result=(total-done)/s*t/1000;
        }
        QTime mTime = QTime(result/3600,(result%3600)/60 ,result%60 , 0);

        QString strTime = mTime.toString("hh:mm:ss");
        mUpdateDialog->ui->text_remain->setText(strTime);
    }







#endif
        int percent=(total>0)?(done*100/total):0;
        if(mState==PS_GETDISP)
        {
            percent/=2;
        }
        else if(mState==PS_GETDB)
        {
            percent=50+percent/2;
        }
        qDebug()<<"update process percent"<<percent;
        mUpdateDialog->ui->progressBar->setValue(percent);
    }
}

void QUpdateProcess::whenError()
{
    qDebug()<<"QUpdateProcess::whenError()"<<mState;
    PROCESS_STATE st=PS_UNKNOWN;
    if(mState==PS_GETDISP)
    {
        st=PS_FINISH_ERROR_GETDISP;
    }
    else if(mState==PS_GETDB)
    {
        st==PS_FINISH_ERROR_GETDB;
    }
    else if(mState==PS_GETFILES)
    {
        st=PS_FINISH_ERROR_GETFILES;
    }
    mState=st;
    if(mUpdateDialog)
    {
        mUpdateDialog->changeUiState(DS_FINISH);
    }
}


bool QUpdateProcess::analyzeDBFile(QString &releasenote)
{
    mReceiveFile.close();
    QLaunchFileDiscription disp(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);

    QLauncher *launcher=QLauncher::GetSingletonPtr();
    ASSERT(launcher);
    ILaunchInf *li=launcher->getLaunchInf();
    ASSERT(li);
    bool btemp;
    int vcur=li->getInfo(GET_INTO_VERSION).replace(".","").toInt(&btemp);
    ASSERT(btemp);
    int vnew=disp.getVersionNum().replace(".","").toInt(&btemp);
    ASSERT(btemp);
    bool ret=(vcur < vnew);

    releasenote=disp.getReleaseNote();
    qDebug()<<releasenote;
    return ret;

}

void    QUpdateProcess::slot_Done ( bool error )
{
    qDebug()<<"QUpdateProcess::slot_Done";
    if(error)
    {
        whenError();
        return;
    }
    if(PS_GETDISP == mState)
    {
#if 0
        if(mUpdateDialog)
        {
            mUpdateDialog->changeUiState(DS_GETDISP_DB);
        }
        startGetDB();
    }
    else if(PS_GETDB==mState)
    {
#endif
        mReceiveFile.close();
        QFile file(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);
        file.open(QFile::ReadOnly);

        QByteArray barraytemp=file.readLine();
        QString desp_verify=QString::fromUtf8(barraytemp).replace("\n","");
        desp_verify=desp_verify.trimmed();
        if(desp_verify !=DESP_VERIFY_STR)
        //if(!desp_verify.contains(DESP_VERIFY_STR))
        {

            qDebug()<<"desp file verify error";
            whenError();
            file.close();
            return;
        }
        file.close();


        QString releasenote;
        bool haveupdate=analyzeDBFile(releasenote);
        if(mUpdateDialog)
        {
            if(haveupdate)
            {
                mUpdateDialog->changeUiState(DS_SHOW_RELEASENOTE);
                mUpdateDialog->ui->label_note->setText(releasenote);
              ///  ui->but_left->setText("Download");
                QLauncher *launcher=QLauncher::GetSingletonPtr();
                if(launcher && launcher->mLi)
                {
                    launcher->mLi->command(CMD_SHOW_UPDATE_SOFTWARE);
                }

            }
            else
            {
                mState=PS_FINISH_ERROR_ALREADYNEWEST;
                mUpdateDialog->changeUiState(DS_FINISH);
            }
        }
        if(mUpdateReson ==UR_TIMER_TRIGGER)
        {
            //set ui here
            qDebug()<<"set update button "<<haveupdate;

            QLauncher *launcher=QLauncher::GetSingletonPtr();
            ASSERT(launcher && launcher->mLi);
            if(haveupdate)
            {
                launcher->mLi->command(CMD_SHOW_UPDATE_SOFTWARE);
            }
            else
            {
                launcher->mLi->command(CMD_HIDE_UPDATE_SOFTWARE);
            }
            this->deleteLater();
        }


    }
    else if(PS_GETFILES== mState)
    {
        mReceiveFile.close();
       QLaunchFileDiscription fdisp(QString(HTTP_LOCAL_DIR)+"/"+HTTP_DISP_FILE_NAME);
        QString tempfilename=fdisp.getPath();

        if(!md5Match(QString(HTTP_LOCAL_DIR)+"/"+HTTP_UPDATE_FILE_NAME,fdisp.getMd5()))
        {
            qDebug()<<"md5 not ok";
            mState=PS_FINISH_ERROR_MD5;
        }
        else
        {
            mState=PS_FINISH_OK;
/*            QLauncher *launcher=QLauncher::GetSingletonPtr();
            if(launcher && launcher->mLi)
            {
                launcher->mLi->command(CMD_SHOW_UPDATE_SOFTWARE);
            }*/
        }
        if(mUpdateDialog)
        {
            mUpdateDialog->changeUiState(DS_FINISH);
        }

    }


}

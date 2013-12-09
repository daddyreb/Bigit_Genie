#include "QGenieUpdateObject.h"
#include <QUiLoader>
#include "genie2_interface.h"
#include "SvtNetworkTool.h"
#include "language_Firmwareupdate.h"
#include "QSetTextHelper.h"

#define DESP_FNAME "supdate_disp.txt"
#ifdef Q_OS_MAC
#define INSTALLER_FNAME "NETGEARGenieInstaller.dmg"
#else
#define INSTALLER_FNAME "NETGEARGenie-install.exe"
#endif
#define TURN_TO_PAGE(_p) do{\
mStack->setCurrentWidget(mStack->findChild<QWidget *>(_p));\
        setLeftButLan();\
    }while(0)

//#ifdef PLATFORM_WIN32
//#define FINISH_FONT "  font-family:'Arial'; font-size:9pt; font-weight:400; font-style:normal;"
//#else
//#define FINISH_FONT "  font-family:'Arial'; font-size:11pt; font-weight:400; font-style:normal;"
//#endif
//#define FINISH_TEXT_MODE "<p style=\" "FINISH_FONT"\">%1 %2</p>"\
//    "<p style=\" "FINISH_FONT"\"><a href=\"http://www.netgear.com/genie\"><span style=\" text-decoration: underline; color:#0000ff;\">http://www.netgear.com/genie</span></a></p>"


static void runExeFile(const QString &fpath)
{
#ifdef Q_OS_WIN32
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
    CreateProcessA ( NULL,wc, NULL, NULL, FALSE, /*CREATE_SUSPENDED*/0, NULL ,NULL, &si, &pi);

#else
    //QString strUrl=GET_SETTING_STR(TITLE_URL);
    //  QDesktopServices::openUrl(QDir::homePath());

    static const QString PRG = "open \"%1\" ";
   // QMessageBox::warning(NULL,"",PRG.arg( fpath ).toLocal8Bit());
    system( PRG.arg( fpath)/*.replace(" ","\\ ")*/ .toLocal8Bit() );
//    QTimer::singleShot(5000,this,SLOT(slot_startUpdateDMG()));

    //          sleep(3000);

#endif
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

QGenieUpdateObject::QGenieUpdateObject(QObject *parent)
    :QObject(parent)
    ,mDialog(NULL)
    ,mStack(NULL)
    ,but_left(NULL)
    ,but_right(NULL)
    ,mProgressBar(NULL)
    ,mHttp(NULL)
    ,mSettings(NULL)
    ,lab_remain_r(NULL)
{
    mPool=new SVT_HttpDownloadPool();
    mLocalDir=GENIE2_HANDLER_CMD(HCMD_CHECK_DIR,"update_temp").toString()+"/";
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_StartProcess()));
    if(QApplication::arguments().contains("-dbg"))
    {
        mTimer.start(12*1000);
    }
    else
    {
        mTimer.start(5*60*1000);
    }
    mRefreshTimes=GENIE2_GET_SETTING(SUPDATE_RETRYTIME).toInt();
}

QGenieUpdateObject::~QGenieUpdateObject()
{
    GENIE2_SAFEDELETE(mSettings);
    GENIE2_SAFEDELETE(mPool);
}

void QGenieUpdateObject::openDialog()
{
    if(mDialog)
    {
        translateUi();
    }
    if(!mDialog || !mDialog->isVisible())
    {
        slot_StartProcess();
        Q_ASSERT(mDialog);
        GENIE2_UI_COMMAND(UICMD_SHOW_MAINFRAME,0);
        GENIE2_SET_FRAMELESSSTYLE(mDialog);
        mDialog->exec();
        mPool->setExitFlag(mHttp);
    }

}

void QGenieUpdateObject::doPageFunc()
{
    if(!mStack || !mDialog)
    {
        return;
    }
    QString obj_name=mStack->currentWidget()->objectName();
    if(obj_name=="p1_getdisp")
    {
        QString local_dir= mLocalDir+DESP_FNAME;
        QString fname = GENIE2_GET_SETTING(UPDATE_DESP_FILE).toString();
        QString tempLan = GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_LANGUAGE).toString();
        fname.replace("{la}",tempLan);
        QString ip=GENIE2_GET_SETTING(UPDATE_IP).toString();
        quint16 port = GENIE2_GET_SETTING(UPDATE_PORT).toInt();
        downLoadFile(QString("http://")+ip+fname,port,local_dir);
        but_left->setEnabled(false);
        GENIE2_LOG(QString("genie update get disp %1 %2 %3")
                   .arg(fname,local_dir).arg(port));
    }
    else if(obj_name=="p2_releasenote")
    {
        Q_ASSERT(mSettings);
        QString local_dir=mLocalDir+INSTALLER_FNAME;
        QString fname=mSettings->value("update/filename","####").toString();
        if(fname=="####")
        {
            GENIE2_LOG("genie update filename not ok!");
            return;
        }
        QString ip=GENIE2_GET_SETTING(UPDATE_IP).toString();
        quint16 port = GENIE2_GET_SETTING(UPDATE_PORT).toInt();
        downLoadFile(QString("http://")+ip+fname,port,local_dir);
        TURN_TO_PAGE("p3_progress");
        but_left->setEnabled(false);
        but_right->setFocus();
    }
    else if(obj_name=="p3_progress")
    {
        Q_ASSERT(0);
        //retry button clicked
        Q_ASSERT(mSettings);
        QString local_dir=mLocalDir+INSTALLER_FNAME;
        QString fname=mSettings->value("update/filename","####").toString();
        QString ip=GENIE2_GET_SETTING(UPDATE_IP).toString();
        quint16 port = GENIE2_GET_SETTING(UPDATE_PORT).toInt();
        downLoadFile(QString("http://")+ip+fname,port,local_dir,QFile(local_dir).size());
        but_left->setEnabled(false);
    }
    else if(obj_name=="p4_finish")
    {
        QString local_dir=mLocalDir+INSTALLER_FNAME;
        runExeFile(local_dir);
        mDialog->accept();
    }
}

void QGenieUpdateObject::goToFinishPage(int lan0,int lan1)
{
    bool bOk=(L_UPGRADE_UPDATE_PAGE_FINISH==lan0);
    but_left->setEnabled(bOk);
    if(bOk)
    {
        but_left->setFocus();
    }
    else
    {
        but_right->setFocus();
    }

    QLabel *lab_finish_text=mStack->findChild<QLabel *>("lab_finish_text");
	lab_finish_text->setOpenExternalLinks(true);
    QLabel *lab_finish_ico=mStack->findChild<QLabel *>("lab_finish_ico");
    Q_ASSERT(lab_finish_ico && lab_finish_text);

    if(lan1 >=0)
    {

        QString str_lan=  "<p >%1 </p><p>%2</p>"
                          "<p><a href=\"http://www.netgear.com/genie\"><span style=\" text-decoration: underline; color:#0000ff;\">http://www.netgear.com/genie</span></a></p>";
        str_lan=str_lan.arg(GENIE2_GET_TEXT(lan0),GENIE2_GET_TEXT(lan1));
        lab_finish_text->setText(str_lan);


    }
    else
    {
        lab_finish_text->setText(GENIE2_GET_TEXT(lan0));
    }


    QString qss="background-image: url(%1);background-repeat:none;"
                "background-position: center;";
    lab_finish_ico->setStyleSheet(qss.arg(
                (L_UPGRADE_UPDATE_PAGE_FINISH==lan0 ||L_UPGRADE_UPDATE_ALREADY_NEWEST==lan0)?GENIE2_RES("images/label_update_right.png")
                    :GENIE2_RES("images/label_update_wrong.png")
                                          ) //arg
                                  );

    TURN_TO_PAGE("p4_finish");
}

void QGenieUpdateObject::downLoadFile(const QString &url, quint16 port, const QString &local_file, quint64 startpos)
{
    mPool->setExitFlag(mHttp);
    mHttp=mPool->newHttp();
    connect(mHttp,SIGNAL(done(bool,QString)),this,SLOT(slot_HttpDone(bool,QString)));
    connect(mHttp,SIGNAL(progress(quint64,quint64,quint32))
            ,this,SLOT(slot_HttpProcess(quint64,quint64,quint32)));
//    connect(mHttp,SIGNAL(destroyed(QObject*)),this,SLOT(slot_HttpDestroyed(QObject*)));
    mHttp->startGetFile(url,port,local_file,startpos);
}

void QGenieUpdateObject::translateUi()
{
    struct _{const char *obj;int lan;} cParas[]={
        {"but_right",L_UPGRADE_UPDATE_CANCEL}
        ,{"lab_start",L_UPGRADE_UPDATE_CHECK_DESP}
        ,{"label_title",L_UPGRADE_UPDATE_TITLE}
        ,{"lab_remain_l",L_UPGRADE_DOWNLOAD_LEAVETIME}
        ,{"lab_getting",L_UPGRADE_UPDATE_DOWN_FILE}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        QSetTextHelper(mDialog->findChild<QWidget *>(cParas[i].obj))
                .setText(GENIE2_GET_TEXT(cParas[i].lan));
    }
    setLeftButLan();
}

void QGenieUpdateObject::setLeftButLan()
{
    if(!mStack || !but_left)
    {
        return;
    }
    const int cPara[]={
        L_UPGRADE_UPDATE_DOWNLOAD
        ,L_UPGRADE_UPDATE_DOWNLOAD
        ,L_UPGRADE_UPDATE_DOWNLOAD
        ,L_UPGRADE_UPDATE_FINISH
    };
    int idx=mStack->currentIndex();
    if(idx >=0 &&idx <sizeof(cPara)/sizeof(cPara[0]))
    {
       but_left->setText(GENIE2_GET_TEXT(cPara[idx]));
    }
}


void QGenieUpdateObject::slot_HttpDone(bool error, const QString &serror)
{
    GENIE2_LOG(QString("slot_HttpDone  ")+serror);
    QString obj_name=(mDialog && mStack)?mStack->currentWidget()->objectName():"";
    if(obj_name=="p1_getdisp")
    {
        if(error)
        {
            goToFinishPage(L_UPGRADE_UPDATE_NETWORK_ERROR);
        }
        else
        {

            GENIE2_SAFEDELETE(mSettings);
            QString local_dir= mLocalDir+DESP_FNAME;
            Q_ASSERT(QFile::exists(local_dir));
            mSettings=new QSettings(local_dir,QSettings::IniFormat);

            qDebug()<<mSettings->allKeys();

            QPlainTextEdit *edt=mStack->findChild<QPlainTextEdit *>("edt_releasenote");
            Q_ASSERT(edt);
            edt->setPlainText(mSettings->value("update/releasenote","release note").toString());
            but_left->setEnabled(true);
            QString newversion=mSettings->value("update/version","").toString();
            QString oldversion=GENIE2_TELL_OTHERPLUGIN(FRAME_UUID,I2CMD_VERSIONNUMBER,0).toString().replace(".","");

            if(newversion.toULongLong()>oldversion.toULongLong())
            {

                TURN_TO_PAGE("p2_releasenote");
                but_left->setFocus();
                GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"soft_on");
            }
            else
            {
                GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"soft_off");
                goToFinishPage(L_UPGRADE_UPDATE_ALREADY_NEWEST);
            }
            mTimer.stop();
        }
    }
    else if(obj_name=="p3_progress")
    {
        if(error)
        {
            goToFinishPage(L_UPGRADE_UPDATE_PAGE_WRONG,L_UPGRADE_UPDATE_LINK);
        }
        else
        {
            goToFinishPage(L_UPGRADE_UPDATE_PAGE_FINISH);
        }
    }
}

void QGenieUpdateObject::slot_HttpProcess(quint64 done, quint64 total, quint32 speed)
{
    if(mProgressBar && mProgressBar->isVisible())
    {
        mProgressBar->setValue(total?done*100/total:0);
    }

    if(lab_remain_r)
    {
        int remain=!speed?0xfffffff:((total-done)/speed);
        QTime mTime = QTime(remain/3600,(remain%3600)/60 ,remain%60 , 0);

        QString strTime = mTime.toString("hh:mm:ss");
        lab_remain_r->setText(strTime);
    }
}

void QGenieUpdateObject::slot_ButClicked()
{
    doPageFunc();
}

void QGenieUpdateObject::slot_StartProcess()
{
    if(--mRefreshTimes <=0)
    {
        mTimer.stop();
    }
    if(!mDialog)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/GenieUpdate.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        mDialog=qobject_cast<QDialog *>(w);

        Q_ASSERT(mDialog);
        mStack=w->findChild<QStackedWidget *>("stack");
        but_left=w->findChild<QPushButton *>("but_left");
        but_right=w->findChild<QPushButton *>("but_right");
        mProgressBar=w->findChild<QProgressBar *>("progressBar");
        lab_remain_r=w->findChild<QLabel *>("lab_remain_r");
        Q_ASSERT(mStack && but_left && but_right && mProgressBar
                 &&lab_remain_r);
        mDialog->hide();
//        mDialog->setFixedSize(mDialog->size());
        connect(but_left,SIGNAL(clicked()),this,SLOT(slot_ButClicked()));
        translateUi();
    }

    //防止timmer过后的检测机制，在dialog弹起时起作用
    if(mDialog->isVisible())
    {
        return;
    }
    ///////////////////////////////////

    mStack->setCurrentIndex(0);
    doPageFunc();
    mProgressBar->setValue(0);
    lab_remain_r->setText("");
}

//void QGenieUpdateObject::slot_HttpDestroyed(QObject *obj)
//{
//    if(obj==mHttp)
//    {
//        mHttp=NULL;
//    }
//}

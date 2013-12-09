#include "stable.h"
#ifdef PLATFORM_MACOSX
#include <unistd.h>
#endif

#define TRAY_MESSAGE_TITLE WINDOW_TITLE
#ifdef SELF_TRAY_BOX
#define MESSAGE_SHOWING_TIME 3000
#define LIST_MAX_SIZE 4
#endif

#ifdef TRAY_MESSAGEBUG
const int cMBCheckInterval=(60*1);
const int cMBIntervalthreshold=(60*2);
const int cMBNoMessageForThisLong=30;
#endif

QGenieTrayManager::QGenieTrayManager()
    :mMainWnd(NULL)
    ,mCurToolTip(-1)
    ,mCurMessage(-1)
	,mTrayIco(NULL)
{
    qDebug()<<"QGenieTrayManager::QGenieTrayManager()";

}
QGenieTrayManager::~QGenieTrayManager()
{
    SAFE_DELETE(mTrayIco);

}

void QGenieTrayManager::startService(QWidget *parent, QString iconFile)
{

    mMainWnd = parent;

    int strategy=GET_SETTING_INT(TRAY_STRATEGY);
    GENIE_LOG(QString("tray strategy is %1").arg(strategy));

    if(strategy==1)
    {
        mTrayIco=new QGenieTrayIco_In(this);
    }
    else if(strategy !=0 )//(strategy==2)
    {
        mTrayIco=new QGenieTrayIco_Out(this);
    }

    if(mTrayIco)
    {
       mTrayIco->setIcon(iconFile);
    }

    QMap<int,bool> paramap;
#ifdef PLATFORM_MACOSX
    paramap.insert(IGenieTrayIco::AUTO_START_CHECKED,GET_SETTING_BOOL(MACOS_AUTOSTART));
#else
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);
	paramap.insert(IGenieTrayIco::AUTO_START_CHECKED,settings.contains("NETGEARGenie"));
#endif
    paramap.insert(IGenieTrayIco::SHOW_NETSTATE_CHECKED,GET_SETTING_BOOL(SHOW_NETWORK_CHANGE));

    mTrayIco->setActionState(paramap);


}


//void QGenieTrayManager::setToolTipText(QString textStr)
//{
//    GENIE_LOG(QString("set tooltip %1").arg(textStr));
//    mCurToolTip=-1;
//    if(!textStr.isEmpty())
//    {
//        mTrayIco->setToolTip(textStr);
//    }
//}

void QGenieTrayManager::setToolTipText(int  idx)
{
    GENIE_LOG(QString("set tooltip %1").arg(idx));
    mCurToolTip=idx;
    if(idx >=0)
    {
        QGenieInterfaceLanguageManager *interfacelan=QGenieInterfaceLanguageManager::GetSingletonPtr();
        ASSERT(interfacelan);
        if(mTrayIco)
        {
           mTrayIco->setToolTip(interfacelan->getText(idx,FRAME_UUID));
        }
    }
}

void QGenieTrayManager::showMsg(QString textTitle,QString textStr,QSystemTrayIcon::MessageIcon ico)
{
    setMessage(textStr);
}

void QGenieTrayManager::setIcon(QString sIconPath)
{
    GENIE_LOG(QString("set tray icon %1").arg(sIconPath));
    if(mTrayIco)
    {
    mTrayIco->setIcon(sIconPath);
    }

}


void QGenieTrayManager::addPluginAction(QAction *act)
{
//    mTrayMenu->insertAction(mPluginMenuHere,act);
    if(mTrayIco)
    {
       mTrayIco->addPluginAction(act);
    }
}

QAction* QGenieTrayManager::addPluginMenu(QMenu *menu)
{
    if(!mTrayIco)
	{
		return NULL;
	}
    return mTrayIco->addPluginMenu(menu);
}

void QGenieTrayManager::setMessage(const QString &strText)
{

    GENIE_LOG(QString("set tray message %1").arg(strText));
    if(mTrayIco && GET_SETTING_BOOL(SHOW_NETWORK_CHANGE))
    {
       mTrayIco->setMessage(strText);
    }
}


void QGenieTrayManager::exit()
{
    GENIE_LOG("QGenieTrayManager::exit()");
    
    QGenieInterfaceClickManager *clkm=QGenieInterfaceClickManager::GetSingletonPtr();
    ASSERT(clkm);
    clkm->notifySaveData();



  //  SAFE_DELETE(mDialogExit);
    QGenieInterfaceInfoManager *infomanager=QGenieInterfaceInfoManager::GetSingletonPtr();
    ASSERT(infomanager);
    infomanager->setInfoFromPluginFlag(false);

    //QGenieInterfaceLogManager *logmanager=QGenieInterfaceLogManager::GetSingletonPtr();
   // logmanager->closeFile();

    QGenieSettings *isetting=QGenieSettings::GetSingletonPtr();
    ASSERT(isetting);
    isetting->writeToDisk();

    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    ASSERT(main);
    main->mDog.stopit();

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);
    stackmanager->mInProcess=false;

    mMainWnd->close();
  //  qApp->exit(0);
   // mMainWnd->hide();
    QGenieInterfaceTrayManager *interfacetray=QGenieInterfaceTrayManager::GetSingletonPtr();
    ASSERT(interfacetray);
    interfacetray->notifyTrayMessage(QSystemTrayIcon::Trigger,MFR_HIDE);

    QGenieInterfaceSoapManager *interfacesoap=QGenieInterfaceSoapManager::GetSingletonPtr();
    if(interfacesoap && interfacesoap->schedule)
    {
        interfacesoap->schedule->stopAll();
    }

   // QTimer::singleShot(0, qApp, SLOT(quit()));
#ifdef PLATFORM_MACOSX
    _exit(0);
#else
    qApp->quit();
#endif
}



void QGenieTrayManager::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    MAIN_FRAME_RESPONSE res=MFR_UNKNOWN;
    if(reason == QSystemTrayIcon::DoubleClick
       || reason == QSystemTrayIcon::Trigger)
    {

#ifdef USE_MAIN_PROXY
        QGenieFrameMainProxy * mainframeproxy=QGenieFrameMainProxy::GetSingletonPtr();
        ASSERT(mainframeproxy);
        mainframeproxy->showNormal();
        mMainWnd->activateWindow();
#else
#ifndef SIMPLE_FRAME
        mMainWnd->showNormal();
        mMainWnd->activateWindow();
#endif
#endif

    }
    QGenieInterfaceTrayManager *interfacetray=QGenieInterfaceTrayManager::GetSingletonPtr();
    ASSERT(interfacetray);
    interfacetray->notifyTrayMessage(reason,MFR_NORMALIZE);
}

void QGenieTrayManager::changeText()
{
    DEFINE_LANGUAGE_MANAGER
    QStringList texts;
    texts << GET_TEXT(L_MAIN_RESTORE)<<GET_TEXT(L_MAIN_UPDATE)
             <<GET_TEXT(L_MAIN_EXIT)<<GET_TEXT(mCurToolTip)
             <<GET_TEXT(L_MAIN_EXIT)<<GET_TEXT(L_MAIN_QUESTION_EXIT)
             <<GET_TEXT(L_MAIN_OK)<<GET_TEXT(L_MAIN_CANCEL)
             <<GET_TEXT(L_MAIN_TRAY_SETTING)<<GET_TEXT(L_MAIN_TRAY_AUTOSTART)
             <<GET_TEXT(L_MAIN_TRAY_NETCONNECTION_CHANGE);
    if(mTrayIco)
    {
        mTrayIco->retranslateUi(texts);
    }

}

void QGenieTrayManager::unloadPi(IPluginInf *pi)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    plug_inf *p=proxymanager->getPluginInf(pi);

    while(!p->actions.isEmpty())
    {
        QAction *action=p->actions.takeFirst();

//        mTrayIco->mTrayMenu->removeAction(action);
		ASSERT(0);

    }
}
void QGenieTrayManager::setLayoutDirection(Qt::LayoutDirection drt)
{
    if(mTrayIco)
    {
        mTrayIco->setLayoutDirection(drt);
    }
}

void QGenieTrayManager::startUpdate()
{
    qDebug() << "QGenieFrameTitle::slot_startUpdate()";
    QGenieLaunchInf *launchinf=QGenieLaunchInf::GetSingletonPtr();
    ASSERT(launchinf);
    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);


    ILaunchManager *manager=QGenieLaunchInf::sLaunchManager;
    ASSERT(manager);



    bool mustretart=manager->startUpdate(UR_USER_CHOISE,lanmanager->getTypeStr()
 #ifdef SHEET_STYLE_FORMAC
                                         ,mMainWnd
  #endif
                                         );

}

void QGenieTrayManager::startMovie()
{
#ifdef USE_TRAY_MOVIE
    if(mTrayIco)
    {
        mTrayIco->startMovie();
    }
#endif
}

void QGenieTrayManager::stopMovie()
{
#ifdef USE_TRAY_MOVIE
    if(mTrayIco)
    {
        mTrayIco->stopMovie();
    }
#endif
}

void QGenieTrayManager::slot_SetAutoStart(bool b)
{
    GENIE_LOG("QGenieTrayManager::setAutoStart");
#ifdef PLATFORM_WIN32


     QSettings setting_lm("HKEY_LOCAL_MACHINE\\Software\\"APP_NAME,QSettings::NativeFormat);
     if(!setting_lm.contains("Install_Dir"))
     {
         QString stemp=qApp->applicationDirPath().replace("/","\\")+"\\NETGEARGenie.exe";
         setting_lm.setValue("Install_Dir",stemp);
     }


    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);
    if(b)
    {
		//QString apppath=qApp->applicationDirPath();
		QString appname=qApp->applicationFilePath();
        QString cmd=QString("\"%1\" -mini -redirect").arg(appname.replace("/","\\"));

        settings.setValue("NETGEARGenie",cmd);
    }
    else{
        settings.remove("NETGEARGenie");
    }
#else
    SET_SETTING_VALUE(MACOS_AUTOSTART,b);

#endif
    ADD_CLICK(b?"H9":"H8");
}

void QGenieTrayManager::slot_SetShowNetworkChange(bool b)
{
    GENIE_LOG("QGenieTrayManager::setShowNetworkChange");
    SET_SETTING_VALUE(SHOW_NETWORK_CHANGE,b);
    ADD_CLICK(b?"H7":"H6");
}


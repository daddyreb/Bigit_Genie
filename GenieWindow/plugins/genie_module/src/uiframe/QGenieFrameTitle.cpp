#include "stable.h"

enum UPDATE_STATE
{
    SOFTWARE_UPDATE=(1<<0),
    FIRMWARE_UPDATE=(1<<1)
};

#define CHECK_UPDATE_INTERVAL_SEC 150
#define SOFT_FIRMWARE_CHANGE_INTERVAL 30 //sec
#define CHECK_UPDATE_LOOP (6*60*60*1000)

QGenieFrameTitle::QGenieFrameTitle(QWidget *parent)
    :QFrame(parent)
    ,mUpdateState(0)
    ,mSoftFirst(false)
{
    mUi.setupUi(this);
    mUi.frame_update->setVisible(false);

    connect(mUi.but_close,SIGNAL(clicked()),this,SLOT(slot_ButCloseClicked()));
    connect(mUi.but_minimize,SIGNAL(clicked()),this,SLOT(slot_ButMinimizeclicked()));
    connect(mUi.but_maximize,SIGNAL(clicked()),this,SLOT(slot_ButMaximizeClicked()));
    connect(mUi.but_normalize,SIGNAL(clicked()),this,SLOT(slot_ButNormalizeClicked()));
#ifdef SHOW_ABOUT_BUG
    connect(&testtimer,SIGNAL(timeout()),this,SLOT(slot_testTimeout()));
#endif

//    mUi.label_version->setText(VERSION_STRING);

    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);

    connect(mUi.cob_sl_lan,SIGNAL(currentIndexChanged(int)),lanmanager,SLOT(slot_setLanguage(int)));

    mUi.but_getpwd->setVisible(false);
    mUi.cob_sl_lan->installEventFilter(this);
    mUi.but_title->installEventFilter(this);

    connect(&mTimerCheckUpdate,SIGNAL(timeout()),this,SLOT(slot_checkUpdateTimeout()));
    connect(&mTimerSoftwareOrFirmware,SIGNAL(timeout()),this,SLOT(slot_SoftwareOrFirmwareTimeout()));
    mTimerCheckUpdate.setSingleShot(true);
    mTimerCheckUpdate.setInterval(CHECK_UPDATE_INTERVAL_SEC*1000);
    mTimerCheckUpdate.start();

//    mUi.text_route->hide();
    mUi.text_firmware->hide();


    mTimerSoftwareOrFirmware.setInterval(SOFT_FIRMWARE_CHANGE_INTERVAL*1000);
    mTimerSoftwareOrFirmware.start();
    mAbout=new QGenieFrameAbout(this);

}
#ifdef SHOW_ABOUT_BUG
void QGenieFrameTitle::slot_testTimeout()
{
    GENIE_LOG("QGenieFrameTitle::slot_testTimeout()");
//    if(mAbout)
//    {
//        mAbout->show();
//    }
	QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
	ASSERT(main);
        //main->hide();
	main->show();

        QString log=QString("window opacity when about menu start :main-%1 about-%2")
                .arg(main->windowOpacity()).arg(mAbout->windowOpacity());
        GENIE_LOG(log);

//    main->showNormal();
//    main->activateWindow();
  //  QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
  //  ASSERT(traymanager);
  //  traymanager->trayIconClicked(QSystemTrayIcon::Trigger);
}
#endif

QGenieFrameTitle::~QGenieFrameTitle()
{
    mTimerCheckUpdate.stop();
    mTimerSoftwareOrFirmware.stop();
}

bool	QGenieFrameTitle::eventFilter ( QObject * watched, QEvent * event )
{
    if(watched == mUi.cob_sl_lan && event->type()==QEvent::Wheel)
    {
        return true;
    }
#if 0
    if(watched ==mUi.but_title)
    {
        qDebug()<<event->type();
    }
    if(watched == mUi.but_title && event->type()==QEvent::MouseButtonRelease)
    {
        //QMessageBox::critical(NULL,"msg","title",QMessageBox::Ok);
        QMouseEvent *mevent=qobject_cast<QMouseEvent *>(event);
        if(mevent)
        {
            on_but_title_clicked();
        }


        return true;
    }
#endif
    return QFrame::eventFilter(watched,event);
}


void QGenieFrameTitle::slot_ButMaximizeClicked()
{
    qDebug("QGenieFrameTitle::slot_ButMaximizeClicked");
    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
    ASSERT(mainframe);
    mainframe->showMaximized();
    hideUselessButton(true);
}

void QGenieFrameTitle::slot_ButMinimizeclicked()
{
    qDebug("QGenieFrameTitle::slot_ButMinimizeclicked");
//    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
//    ASSERT(mainframe);
//    mainframe->showMinimized();
#ifdef USE_MAIN_PROXY
    QGenieFrameMainProxy * mainframeproxy=QGenieFrameMainProxy::GetSingletonPtr();
    ASSERT(mainframeproxy);
    mainframeproxy->showMinimized();
#else
        QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
        ASSERT(mainframe);
        mainframe->showMinimized();
#endif

}

void QGenieFrameTitle::slot_ButCloseClicked()
{
    qDebug("QGenieFrameTitle::slot_ButCloseClicked();");
//    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
//    ASSERT(mainframe);
//    mainframe->showMinimized();
//    mainframe->hide();
#ifdef USE_MAIN_PROXY
    QGenieFrameMainProxy * mainframeproxy=QGenieFrameMainProxy::GetSingletonPtr();
    ASSERT(mainframeproxy);
    mainframeproxy->showMinimized();
    mainframeproxy->hide();
#else
    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
    ASSERT(mainframe);
    mainframe->showMinimized();
 #ifndef PLATFORM_MACOSX
    mainframe->hide();
#endif
#endif

}

void QGenieFrameTitle::slot_ButNormalizeClicked()
{
    qDebug("QGenieFrameTitle::slot_ButMinimizeclicked");
    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
    ASSERT(mainframe);
    mainframe->showNormal();
    hideUselessButton(false);
}

void QGenieFrameTitle::slot_checkUpdateTimeout()
{
    qDebug()<<"QGenieFrameTitle::slot_checkUpdateTimeout()";
    ASSERT(QGenieLaunchInf::sLaunchManager);
    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);

    QGenieLaunchInf::sLaunchManager->startUpdate(UR_TIMER_TRIGGER,lanmanager->getTypeStr());

    mTimerCheckUpdate.stop();
    mTimerCheckUpdate.setSingleShot(true);
    mTimerCheckUpdate.setInterval(CHECK_UPDATE_LOOP);
    mTimerCheckUpdate.start();
}

void QGenieFrameTitle::slot_SoftwareOrFirmwareTimeout()
{
    if((mUpdateState & (FIRMWARE_UPDATE |SOFTWARE_UPDATE))==(FIRMWARE_UPDATE |SOFTWARE_UPDATE))
    {
        mSoftFirst=!mSoftFirst;
        updateUpdateIcon();
    }
}

void QGenieFrameTitle::on_but_about_clicked()
{
    qDebug() << "QGenieFrameTitle::on_but_about_clicked()";
        //QTimer::singleShot(0,mAbout,SLOT(show()));

   // QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    //ASSERT(main);
    //main->hide();
#ifdef SHOW_ABOUT_BUG    
    testtimer.stop();
    testtimer.setSingleShot(true);
    testtimer.setInterval(0);
    testtimer.start();
    GENIE_LOG("in flag SHOW_ABOUT_BUG,timer start");
#endif
    mAbout->exec();
}

void QGenieFrameTitle::on_but_update_clicked()
{
    /* software update
    QGenieLaunchInf *launchinf=QGenieLaunchInf::GetSingletonPtr();
    ASSERT(launchinf);

    ILaunchManager *manager=QGenieLaunchInf::sLaunchManager;
    ASSERT(manager);
    manager->startUpdate();
    */
    ASSERT(mUpdateState != 0);
    if(mUpdateState ==0 )
    {
        return;
    }
    bool isSoft=softOrFirmware();
    if(isSoft)
    {
        ASSERT(QGenieLaunchInf::sLaunchManager);
        QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
        ASSERT(lanmanager);
        QGenieLaunchInf::sLaunchManager->startUpdate(UR_USER_CHOISE,lanmanager->getTypeStr());
    }
    else
    {
        QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(pluginmanager);
        QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
        ASSERT(stackmanager);

        plug_inf *p=pluginmanager->getPluginInf(PLUGIN_UUID_NETCONFIG);
        if(p && p->page && p->pi)
        {
            stackmanager->changeStackViewIdx(p->page);
            p->pi->command(COMMAND_NOTIFY_FUPDATE,"","");
#ifdef ENABLE_MULTI_LEFT_BUTTON
            if(p->leftpanbut[0])
            {
                p->leftpanbut[0]->setChecked(true);
            }
#else

            if(p->leftpanbut)
            {
                p->leftpanbut->setChecked(true);
            }
#endif
        }
        else
        {
            qDebug()<<"can not find firmwareupdate plugin when QGenieFrameTitle::on_but_update_clicked()";
        }
    }



}
void QGenieFrameTitle::on_but_getpwd_clicked()
{
    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    if(uimanager)
    {
        uimanager->turnToThisPage(PLUGIN_UUID_NETCONFIG,COMMAND_GETPWD);
    }
    ADD_CLICK("H1");

}

void QGenieFrameTitle::on_but_title_clicked()
{
    qDebug()<<"QGenieFrameTitle::on_but_title_clicked()";

    QString strUrl=GET_SETTING_STR(TITLE_URL);
	QDesktopServices::openUrl(strUrl);


}

void QGenieFrameTitle::hideUselessButton(bool maximize)
{
#ifdef NO_MAXIMIZE_BUTTON
    mUi.but_maximize->setVisible(false);
    mUi.but_normalize->setVisible(false);
#else
    if(maximize)
    {
        mUi.but_maximize->setVisible(false);
        mUi.but_normalize->setVisible(true);
    }
    else
    {
        mUi.but_maximize->setVisible(true);
        mUi.but_normalize->setVisible(false);
    }
#endif
}

void	QGenieFrameTitle::showEvent ( QShowEvent * event )
{

    QGenieFrameMain *mainframe=QGenieFrameMain::GetSingletonPtr();
    ASSERT(mainframe);
    QFrame::showEvent(event);
    hideUselessButton(mainframe->isFullScreen());
}

void QGenieFrameTitle::changeEvent(QEvent* event)
{
    DEFINE_LANGUAGE_MANAGER
    if (event->type()==QEvent::LanguageChange)
    {
        SET_TEXT(mUi.but_about,L_MAIN_TITLE_ABOUT);

        SET_TEXT(mUi.but_getpwd,L_MAIN_TITLE_GETPWD);
        SET_TEXT(mUi.text_lanselect,L_MAIN_LAN_SELECT);
        setWifiName(mWifiType);

        updateUpdateIcon();
    }
    QFrame::changeEvent(event);
}

bool QGenieFrameTitle::softOrFirmware()
{
    if((mUpdateState &(FIRMWARE_UPDATE |SOFTWARE_UPDATE)) ==(FIRMWARE_UPDATE |SOFTWARE_UPDATE))
    {
        return  mSoftFirst;
    }
    else
    {
        if((mUpdateState & FIRMWARE_UPDATE) !=0)
        {
            return false;
        }
        else if((mUpdateState & SOFTWARE_UPDATE) !=0)
        {
            return true;
        }
    }
    return false;
}

void QGenieFrameTitle::updateUpdateIcon()
{
    if(mUpdateState !=0)
    {
        bool isSoft=softOrFirmware();
        QGenieInterfaceLanguageManager *inlanmanager=QGenieInterfaceLanguageManager::GetSingletonPtr();
        ASSERT(inlanmanager);
        int lanidx=(isSoft)?L_MAIN_SOFTWARE_UPD_AVALABLE:L_MAIN_FIRMWARE_UPD_AVALABLE;

        QString text=inlanmanager->getText(lanidx,QUuid(FRAME_UUID));
        mUi.but_update->setText(text);

    }
   mUi.frame_update->setVisible(mUpdateState !=0);
// mUi.but_update->setVisible(true);
}

void QGenieFrameTitle::setSoftWareUpdateState(bool show)
{
    if(show)
    {
        mUpdateState |= SOFTWARE_UPDATE;
        mTimerCheckUpdate.stop();
    }
    else
    {
        mUpdateState &=~SOFTWARE_UPDATE;
        mTimerCheckUpdate.stop();
        mTimerCheckUpdate.setSingleShot(true);
        mTimerCheckUpdate.setInterval(CHECK_UPDATE_LOOP);
        mTimerCheckUpdate.start();
    }
    updateUpdateIcon();
}

void QGenieFrameTitle::setFirmwareUpdateState(bool show)
{
    if(show)
    {
        mUpdateState |= FIRMWARE_UPDATE;
    }
    else
    {
        mUpdateState &=~FIRMWARE_UPDATE;
    }
    updateUpdateIcon();
}

void QGenieFrameTitle::setWifiName(const QString &wname)
{
    mWifiType=wname;
    QString tipname=wname;
    QString textname=wname;
    if(!wname.isEmpty())
    {
        DEFINE_LANGUAGE_MANAGER
                tipname=QString(TITLE_WIFI_TOOLTIP_MODE).arg(GET_TEXT(L_MAIN_WIRELES_NETWORK),wname);
#ifdef TEST_UI
        QString f("./wifi_mode.txt");
        QString text_mode;
        if(QFile::exists(f))
        {
            QFile file(f);
            file.open(QFile::ReadOnly);
            text_mode = QLatin1String(file.readAll());
            file.close();
            textname=QString(text_mode).arg(GET_TEXT(L_MAIN_WIRELES_NETWORK),wname);

        }
        else
#endif
        {
            textname=QString(TITLE_WIFI_TEXT_MODE).arg(GET_TEXT(L_MAIN_WIRELES_NETWORK),wname);
        }
    }
    mUi.text_wifi->setText(textname);
    //mUi.text_wifi->setToolTip(tipname);
    mUi.text_wifi->setVisible(!wname.isEmpty());
}






//void QGenieFrameTitle::setRouteText(QString s)
//{
//    dddd
//    mUi.text_route->setText(s);
//    QRegExp regx(" +");
//    QRegExp regx2("v(\\d+)",Qt::CaseInsensitive);
//    if(s=="")
//    {
//        mRouteType="";
//    }
//    else
//    {
//        mRouteType=s.split(regx).last();
//        mRouteType=mRouteType.replace(regx2,"");
//    }
//    GENIE_LOG("route type "+mRouteType);

//    QGenieAdvWidget *advw=QGenieAdvWidget::GetSingletonPtr();
//    ASSERT(advw);

//    QString routetype=mRouteType;
//    if(routetype =="")
//    {
//        routetype="default";
//    }
//#ifndef GET_ADV_DISP_WHENSTARTUP
//    advw->startup(routetype);
//#else
//    advw->getAdvWhenRouteChanged(routetype);
//#endif
//    bool vis=(mRouteType != "");
//    mUi.but_getpwd->setVisible(vis);

//    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
//    ASSERT(uimanager);
//    uimanager->notifyRouteType(mRouteType);




//}

void QGenieFrameTitle::setFirmwareText(QString s)
{
    mUi.text_firmware->setText(s);
    QRegExp regx(" +");
    if(s=="")
    {
        mRouteFirmware="";
    }
    else
    {
        mRouteFirmware=s.split(regx).last();
    }
}





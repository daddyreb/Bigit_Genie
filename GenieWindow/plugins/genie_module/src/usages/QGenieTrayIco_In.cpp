#include "stable.h"

#define MESSAGE_SHOWING_TIME 3000

namespace Ui
{
  class QGenieSelfTrayMessage;
}

QGenieTrayIco_In::QGenieTrayIco_In(QGenieTrayManager *traymanager)
    :mTrayIcon(new QSystemTrayIcon())
    ,mTrayManager(traymanager)
    ,mTrayMenu(NULL)
    ,mQuitAction(NULL)
    ,mPluginMenuHere(NULL)
    ,mUpdateAction(NULL)
    ,mRestoreAction(NULL)
    ,mSettingSubMenu(NULL)
    ,mShowNetworkChangeAction(NULL)
    ,mAutoStartAction(NULL)
#ifdef SELF_TRAY_BOX
    ,mSelfTrayMessage(NULL)
#endif
    ,mMoviewManager(NULL)
    ,mDialogExit(NULL)
{
    mTrayIcon->show();
    startup(traymanager);
}

QGenieTrayIco_In::~QGenieTrayIco_In()
{
    mMessageTimer.stop();
    SAFE_DELETE(mTrayIcon);
   // SAFE_DELETE( mTrayManager);
    SAFE_DELETE( mTrayMenu);
  //  SAFE_DELETE( mQuitAction);
   // SAFE_DELETE( mPluginMenuHere);
   // SAFE_DELETE( mUpdateAction);
    //SAFE_DELETE( mRestoreAction);
#ifdef SELF_TRAY_BOX
    SAFE_DELETE( mSelfTrayMessage);
#endif
    SAFE_DELETE( mMoviewManager);
}

void QGenieTrayIco_In::startMovie()
{
    if(mMoviewManager)
    {
        mMoviewManager->startMovie();
    }
}

void QGenieTrayIco_In::stopMovie()
{
    if(mMoviewManager)
    {
        mMoviewManager->stopMovie();
    }

}

void QGenieTrayIco_In::setToolTip(const QString &tooltip)
{
    mTrayIcon->setToolTip(tooltip);
}

void QGenieTrayIco_In::setMessage(const QString &msg)
{
#ifndef SELF_TRAY_BOX
    qDebug()<<"QGenieTrayIco_In::setMessage"<<msg;
    mTrayIcon->showMessage("NETGEARGenie",msg);
#else
    if(!mSelfTrayMessage)
    {
        mSelfTrayMessage=new QGenieSelfTrayMessage();
    }
    mSelfTrayMessage->ui->label_txt->setText(msg);
    QPoint p=mTrayIcon->geometry().center();
    mSelfTrayMessage->setPosition(p.x(),p.y());
    mSelfTrayMessage->show();

    mMessageTimer.setSingleShot(true);
    mMessageTimer.stop();
    mMessageTimer.start(MESSAGE_SHOWING_TIME);
#endif
}

void QGenieTrayIco_In::setIcon(const QString &ico)
{
    if(!mMoviewManager->isOn())
    {
       mTrayIcon->setIcon(QIcon(ico));
    }
    mIcoCache4Movie=ico;

}

void QGenieTrayIco_In::retranslateUi(const QStringList &texts)
{
    if(texts.count()<11)
    {
        GENIE_LOG("error QGenieTrayIco_In::retranslateUi");
        return;
    }
    mRestoreAction->setText(texts[0]);
    mUpdateAction->setText(texts[1]);
    mQuitAction->setText(texts[2]);
    mTrayIcon->setToolTip(texts[3]);
    mSettingSubMenu->setTitle(texts[8]);
    mAutoStartAction->setText(texts[9]);
    mShowNetworkChangeAction->setText(texts[10]);

}

void QGenieTrayIco_In::exit()
{
    ASSERT(mTrayManager);
    mTrayMenu->setDisabled(true);

    if(mDialogExit)
    {
        SAFE_DELETE(mDialogExit);
    }
    mDialogExit=new QGenieDialogExit(NULL);
    mDialogExit->retranslateUi();
    mDialogExit->exec();
    mTrayMenu->setDisabled(false);

    if(ER_OK ==mDialogExit->mResult)
    {
		mTrayIcon->hide();
        mTrayManager->exit();
    }
}

void  QGenieTrayIco_In::addPluginAction(QAction *act)
{
    if(mTrayMenu)
    {
        mTrayMenu->insertAction(mPluginMenuHere,act);
    }
}

QAction* QGenieTrayIco_In::addPluginMenu(QMenu *menu)
{
    ASSERT(mTrayMenu);
    return mTrayMenu->insertMenu(mPluginMenuHere,menu);
}

void QGenieTrayIco_In::setActionState(const QMap<int ,bool> &para)
{
    qDebug()<<para;
   ASSERT(para.contains(AUTO_START_CHECKED));
   mAutoStartAction->setChecked(para.value(AUTO_START_CHECKED));
   ASSERT(para.contains(SHOW_NETSTATE_CHECKED));
   mShowNetworkChangeAction->setChecked(para.value(SHOW_NETSTATE_CHECKED));
}

void QGenieTrayIco_In::startup(QGenieTrayManager *traymanager)
{
    mTrayManager=traymanager;
    QWidget *parent=traymanager->mMainWnd;
    ASSERT(parent);
    ASSERT(mTrayIcon);
    connect(mTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason))
            ,this,SLOT(slot_TrayIconClicked(QSystemTrayIcon::ActivationReason)));

    mTrayIcon->show();
    mTrayMenu=new QMenu();
    mQuitAction = new QAction(QIcon(":/tray_exit.png"),QString::fromUtf8("Exit"),mTrayMenu);
    mRestoreAction = new QAction(QIcon(":/tray_show.png"),QString::fromUtf8("ShowWindow"),mTrayMenu);
    mPluginMenuHere = new QAction(mTrayMenu);
    mPluginMenuHere->setSeparator(true);

    mUpdateAction = new QAction(QIcon(":/tray_update.png"),"update Genie",mTrayMenu);

    connect(mQuitAction,SIGNAL(triggered()),this,SLOT(slot_ExitApp()));
    connect(mRestoreAction,SIGNAL(triggered()),this,SLOT(slot_ShowNormal()));
    connect(mUpdateAction,SIGNAL(triggered()),this,SLOT(slot_startUpdate()));

    connect(&mMessageTimer,SIGNAL(timeout()),this,SLOT(slot_MessageTimeout()));



    connect(mTrayMenu,SIGNAL(aboutToShow()),this,SLOT(slot_AboutToShow()));
    SET_STYLE_PROPERTY(TRAY_MENU,mTrayMenu);
    mTrayMenu->setObjectName("traymenu");
    mTrayMenu->addAction(mRestoreAction);
    mTrayMenu->addAction(mUpdateAction);

    mTrayMenu->addSeparator();
    mSettingSubMenu=mTrayMenu->addMenu("Settings");
    mShowNetworkChangeAction=mSettingSubMenu->addAction("Show Network Connectivity Changes");
    mShowNetworkChangeAction->setCheckable(true);

    mAutoStartAction=mSettingSubMenu->addAction("Enable Autostart");
    mAutoStartAction->setCheckable(true);

    ASSERT(mTrayManager);

    connect(mShowNetworkChangeAction,SIGNAL(triggered(bool)),mTrayManager,SLOT(slot_SetShowNetworkChange(bool)));
    connect(mAutoStartAction,SIGNAL(triggered(bool)),mTrayManager,SLOT(slot_SetAutoStart(bool)));

#ifndef PLATFORM_MACOSX
    mTrayMenu->addSeparator();
#endif
    mTrayMenu->addAction(mPluginMenuHere);
    mTrayMenu->addAction(mQuitAction);

    mTrayIcon->setContextMenu(mTrayMenu);
    mMoviewManager=new QGenieTrayMovieManager(this);

}

 void QGenieTrayIco_In::slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason)
 {
     qDebug()<<"QGenieTrayIco_In::slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason)";
     if(mTrayManager)
     {
         mTrayManager->trayIconClicked(reason);
     }

 }

 void QGenieTrayIco_In::slot_ExitApp()
 {
    exit();
 }

 void QGenieTrayIco_In::slot_startUpdate()
 {
	 ASSERT(mTrayMenu);
    if(mTrayManager)
    {
		mTrayMenu->setDisabled(true);
        mTrayManager->startUpdate();
		mTrayMenu->setDisabled(false);
    }
 }

 void QGenieTrayIco_In::slot_AboutToShow()
 {
    mTrayMenu->setWindowOpacity(1.0);
 }

 void QGenieTrayIco_In::slot_ShowNormal()
 {
    if(mTrayManager && mTrayManager->mMainWnd)
    {
        mTrayManager->mMainWnd->showNormal();
    }
 }

 void QGenieTrayIco_In::setLayoutDirection(Qt::LayoutDirection drt)
 {

 }

 void QGenieTrayIco_In::slot_MessageTimeout()
 {
#ifdef SELF_TRAY_BOX
     if(mSelfTrayMessage)
     {
         mSelfTrayMessage->hide();
     }
#endif
 }




#include "stable.h"

QGenieInterfaceTrayManager::QGenieInterfaceTrayManager(QObject *parent)
    :QObject(parent)
    ,mCurState(-1)
{
        qDebug()<<"QGenieInterfaceTrayManager::QGenieInterfaceTrayManage";
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_updateIcon()));
    mTimer.start(30000);
}
QGenieInterfaceTrayManager::~QGenieInterfaceTrayManager()
{
    mTimer.stop();
}

void QGenieInterfaceTrayManager::setMessage(QString textTitle,QString textStr,QSystemTrayIcon::MessageIcon ico)
{
    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(manager);
    manager->showMsg(textTitle,textStr,ico);
}

void QGenieInterfaceTrayManager::setAction(QAction *act,QUuid uuid)
{
    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(manager);
    manager->addPluginAction(act);

    QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(pluginmanager);

    plug_inf *p=pluginmanager->getPluginInf(uuid);
    ASSERT(p);
    if(p)
    {
        p->actions.append(act);
    }

}

void QGenieInterfaceTrayManager::startMovie(MOVIE_IDX idx,int msec)
{ 
    QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(traymanager);
    traymanager->startMovie();

}

void QGenieInterfaceTrayManager::stopMovie()
{
    QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(traymanager);
    traymanager->stopMovie();
}

void QGenieInterfaceTrayManager::hideWindow()
{
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    if(main)
    {
        main->showMinimized();
#ifndef PLATFORM_MACOSX
        main->hide();
#endif

    }
}

void QGenieInterfaceTrayManager::showWindow()
{
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    if(main)
    {
        main->showNormal();
        main->activateWindow();
    }
}

void QGenieInterfaceTrayManager::exitApp()
{
    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(manager);
    manager->exit();
}

void QGenieInterfaceTrayManager::addMenu(QMenu *menu, QUuid uuid)
{
    qDebug("QGenieInterfaceTrayManager::addMenu(QMenu *menu)");
    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(manager);
    QAction *act=manager->addPluginMenu(menu);

    QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(pluginmanager);


    plug_inf *p=pluginmanager->getPluginInf(uuid);
    ASSERT(p);
    if(p)
    {
        p->actions.append(act);
    }



}


void QGenieInterfaceTrayManager::setMessageFlag(IPluginInf *pi,bool b)
{
    if(b)
    {
        mPluginInfs.insert(pi);
    }
    else
    {
        mPluginInfs.remove(pi);
    }
}

void QGenieInterfaceTrayManager::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{
    QList<IPluginInf *> templist=mPluginInfs.toList();
    for(int i=0;i<templist.size();i++)
    {
        IPluginInf *pi=templist[i];
        pi->notifyTrayMessage(reason,res);
    }
}

void QGenieInterfaceTrayManager::unloadPi(IPluginInf *pi)
{
    mPluginInfs.remove(pi);

}

void QGenieInterfaceTrayManager::setTrayStateIcon(E_TRAY_STATUS state,QString selficon)
{


    const int cMessage[]={
        L_MAIN_TRAY_MESSAGE_CONNECT,
        L_MAIN_TRAY_MESSAGE_DISCONNECT,
        L_MAIN_TRAY_TOO_FLOW
    };

//    const int cMap[EPS_NUM]={NS_OK,NS_OK,NS_ERROR,NS_OVERFLOW,NS_OK,NS_OK};
//    int state=cMap[state];

    bool showmessage=false;
    if(mCurState!=state )
    {

        if(!(mCurState==NS_OVERFLOW && state==NS_OK)&& (mCurState !=-1))
        {
            showmessage=true;
        }

        GENIE_LOG(QString("netstate change from %1 to%2").arg(QString::number(mCurState),QString::number(state)));
        mCurState=state;
    }

    QGenieInterfaceUiManager *ituimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(ituimanager);
    ituimanager->setCurNetState((NET_STATE)mCurState);

    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(manager);

    if(showmessage && manager)
    {
       if( cMessage[state]>=0)
        {
            DEFINE_LANGUAGE_MANAGER
            manager->setMessage(GET_TEXT(cMessage[state]));
        }
    }

    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uimanager);
    uimanager->notifyNetState(state);
    slot_updateIcon();

}

void QGenieInterfaceTrayManager::slot_updateIcon()
{
    const char *cIcons[]={
        ":/ico_tray_good.ico",
        ":/ico_tray_error.ico",
        ":/ico_tray_good.ico" //overflow use good icon
    };

    const int cToolTips[]=
    {
        L_MAIN_TRAY_TOOLTIP_CONNECT,
        L_MAIN_TRAY_TOOLTIP_DISCONNECT,
        L_MAIN_TRAY_TOOLTIP_CONNECT
    };
    if(!(mCurState>=0 && mCurState<sizeof(cIcons)/sizeof(cIcons[0])))
    {
        return;
    }

    QString ico=cIcons[mCurState];
    QGenieTrayManager *manager=QGenieTrayManager::GetSingletonPtr();
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();


    if(manager && main)
    {
        manager->setIcon(ico);

        manager->setToolTipText(cToolTips[(int)mCurState]);
        main->setWindowIcon(QIcon(ico));
    }
    //QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    //ASSERT(pluginmanager);
    //plug_inf *p=pluginmanager->getPluginInf(PLUGIN_UUID_INTERNET);
    //if(p && p->butbig.size()>0 && mCurState >=0 && mCurState <3)
    //{
    //    int tempmap[]={BBS_GOOD,BBS_ERROR,BBS_WAITING};
    //    p->butbig[0]->setState(
    //            (BIG_BUTTON_STATUS)tempmap[mCurState]
    //            );
    //}
}





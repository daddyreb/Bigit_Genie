#include "stable.h"
QGenieInterfaceUiManager::QGenieInterfaceUiManager()
    :mCurState(NS_OK)
{

}

void QGenieInterfaceUiManager::setWaitState(IPluginInf *pi,QString resion)
{
#if 0
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    QGeniePagePluginProxy *proxypage= proxymanager->getPagePluginProxy(pi);
    ASSERT(proxypage);
    proxypage->setState(PLST_WAITING);
    proxypage->update();
#endif
}


void QGenieInterfaceUiManager::restorNormalState(IPluginInf *pi,QString resion)
{
#if 0
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    QGeniePagePluginProxy *proxypage= proxymanager->getPagePluginProxy(pi);
    ASSERT(proxypage);

    int state=pi->getStatus();
    ASSERT(state >=0 && state <PLST_NUM);

    proxypage->setState((PLUGIN_STATE)state);
    proxypage->update();
#endif

}

void QGenieInterfaceUiManager::changeStackIdx(int idx)
{

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);
#ifdef CAN_LOCK_STACK
    if(!stackmanager->canAutoChangePage())
    {
        qDebug()<<"can not change page know ,check macro CAN_LOCK_STACK!!!";
        return ;
    }
#endif

    stackmanager->changeStackViewIdx(NULL);

    stackmanager->mHomeBut->setChecked(true);

    QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
    ASSERT(leftpan);
    leftpan->changeStackIdx(0);
}


void QGenieInterfaceUiManager::setRouteType(const QString &rts)
{
//    mUi.text_route->setText(rts);
    QRegExp regx(" +");
//    QRegExp regx2("v(\\d+)",Qt::CaseInsensitive);
    if(rts=="")
    {
        mRouteType="";
    }
    else
    {
        mRouteType=rts.split(regx).last();
//        mRouteType=mRouteType.replace(regx2,"");
    }
    GENIE_LOG("route type "+mRouteType);

    QGenieAdvWidget *advw=QGenieAdvWidget::GetSingletonPtr();
    ASSERT(advw);

    QString routetype=mRouteType;
    if(routetype =="")
    {
        routetype="default";
    }
#ifndef GET_ADV_DISP_WHENSTARTUP
    advw->startup(routetype);
#else
    advw->getAdvWhenRouteChanged(routetype);
#endif
    bool vis=(mRouteType != "");
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    if(title)
    {
        title->mUi.but_getpwd->setVisible(vis);
    }



//    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
//    ASSERT(uimanager);
    notifyRouteType(mRouteType);
}

int QGenieInterfaceUiManager::command(QString cmd,QString para)
{
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    if(!title)
    {
        return 0;
    }
    if(cmd == UI_CMD_SET_ROUTE )
    {
        setRouteType(para);
        return 0;
    }
    else if(cmd == UI_CMD_SET_WIFI)
    {
        setWifiName(para);
        return 0;
    }
    else if(cmd == UI_CMD_SET_FRIMWARE)
    {
        title->setFirmwareText(para);
        return 0;
    }
    else if(cmd ==UI_CMD_SHOW_UPDATE_BUTTON)
    {
        title->setFirmwareUpdateState(true);
        return 0;
    }
    else if(cmd == UI_CMD_HIDE_UPDATE_BUTTON)
    {
        title->setFirmwareUpdateState(false);
        return 0;
    }
    else if(cmd ==UI_CMD_CHANGE_CHANNEL)
    {
        QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uimanager);
        uimanager->turnToThisPage(PLUGIN_UUID_NETCONFIG,COMMAND_CHANGECHANNEL);
        return 0;
    }
    else if(cmd == UI_CMD_TRURNTO_GACC )
    {
        QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uimanager);
        uimanager->turnToThisPage(PLUGIN_UUID_NETCONFIG,COMMAND_TURNTOGACC);
        return 0;
    }
    else if(cmd ==UI_CMD_DIALOG)
    {
        QGenieDialogExit exitdlg;
        exitdlg.retranslateUi();
        exitdlg.ui->label_title->setText("");
        QStringList lst=para.split(QChar('\0'));
        if(lst.length()>0)
        {
            exitdlg.ui->label_title->setText(lst[0]);
        }
        if(lst.length()>1)
        {
            exitdlg.ui->label->setText(lst[1]);
        }



        exitdlg.exec();
        return (exitdlg.mResult==ER_OK)?0:1;
    }
    else if(cmd == UI_CMD_SHOWNORMAL)
    {
        QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
        ASSERT(main);
        main->showNormal();
        return 0;
    }
    return -1;
}

//void QGenieInterfaceUiManager::setBigButtonState(IPluginInf *pi,ENUM_PLUGIN_STATUS state,int idx)
//{
//    //  GENIE_LOG(QString("setBigButtonState %1").arg(state));
//    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
//    ASSERT(proxymanager);
//    plug_inf *p=proxymanager->getPluginInf(pi);
//    if(p && p->butbig.size() >idx && 0<=idx)
//    {
//        QGenieButtonBig *bgbut=p->butbig.at(idx);

//        if(proxymanager->mLoadOk)
//        {
//            bgbut->setState((BIG_BUTTON_STATUS)state);
//        }
//        else
//        {
//            qDebug()<<"cache bbt state"<<idx<<":"<<state;
//            p->stateCache.insert(idx,(int)state);
//        }
//    }
//}

void QGenieInterfaceUiManager::setLeftButtonChecked(IPluginInf *pi,int idx)
{

    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(pi);
    if(p )
    {
#ifdef ENABLE_MULTI_LEFT_BUTTON
        if(p->leftpanbut.size() >idx && 0<=idx)
        {
            QGenieButtonStackViewController *leftbut=p->leftpanbut[idx];
            leftbut->checkThis();
        }
#else
        p->leftpanbut->checkThis();
#endif


    }

}

void QGenieInterfaceUiManager::setBigButtonSubState(IPluginInf *pi,ENUM_BIGBT_SUB_STATE state,int idx)
{
    GENIE_LOG(QString("setBigButtonSubState %1").arg(state));
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(pi);
    if(p )
    {
        if(p->butbig.size() >idx && 0<=idx)
        {
            QGenieButtonBig *bgbut=p->butbig.at(idx);

            if(proxymanager->mLoadOk )
            {
                ASSERT(bgbut);
               bgbut->setSubState(state);
            }
            else
            {
                p->substateCache.insert(idx,(int)state);
                qDebug()<<"cache bbt sub state"<<idx<<":"<<state;
            }
        }
    }
}

QString  QGenieInterfaceUiManager::getClipBoardString()
{
    return QApplication::clipboard()->text();
}

void QGenieInterfaceUiManager::setClipBoardString(const QString s)
{
    return QApplication::clipboard()->setText(s);
}

//QClipboard *QGenieInterfaceUiManager::getClipBoard()
//{
//    return QApplication::clipboard();
//}

void QGenieInterfaceUiManager::setBigButtonIcon(const QString &ico,QUuid uuid,int idx)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(uuid);
    if(p && idx<p->butbig.size())
    {
        p->butbig.at(idx)->setBackGroundIco(ico);
    }

}

void QGenieInterfaceUiManager::setBigButtonUserStateText(const QString &stylesheet,const QUuid &uuid,int idx)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(uuid);
    if(p && idx<p->butbig.size())
    {
        p->butbig.at(idx)->setUserStyle(stylesheet);
    }
    else if(p && !proxymanager->mLoadOk)
    {
        p->styleCache.insert(idx,stylesheet);
    }
    else
    {
        qDebug()<<"can not set the bigbutton style";
    }
}

void  QGenieInterfaceUiManager::setBigButtonUserStateBKGround(const QString &stylesheet,const QUuid &uuid,int idx)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(uuid);
    if(p && idx<p->butbig.size())
    {
        p->butbig.at(idx)->setBKG(stylesheet);
    }
    else if(p && !proxymanager->mLoadOk)
    {
        p->bkgCache.insert(idx,stylesheet);
    }
    else
    {
        qDebug()<<"can not set the bigbutton bkg style";
    }
}



void QGenieInterfaceUiManager::setNetStateFlag(QUuid uuid,bool setit)
{
    if(setit)
    {
        mNetStateUuids.insert(uuid);
        QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(proxymanager);
        plug_inf *p=proxymanager->getPluginInf(uuid);
        if(p && p->pi)
        {
            p->pi->command(COMMAND_NOTIFYNETSTATE,mCurState);
            qDebug()<<"notify net state change!! after QGenieInterfaceUiManager::setNetStateFlag";
        }
    }
    else
    {
        mNetStateUuids.remove(uuid);
    }
}

void QGenieInterfaceUiManager::setRouteTypeFlag(QUuid uuid,bool setit)
{
    if(setit)
    {
        mRouteTypeUuids.insert(uuid);
    }
    else
    {
        mRouteTypeUuids.remove(uuid);
    }
}

void QGenieInterfaceUiManager::setGatewayFlag(QUuid uuid,bool setit)
{
    if(setit)
    {
        mGatewayUuids.insert(uuid);
    }
    else
    {
        mGatewayUuids.remove(uuid);
    }
}

void QGenieInterfaceUiManager::setBackGroundImg(QUuid uuid,QString img,QString para)
{
    qDebug()<<"QGenieInterfaceUiManager::setBackGroundImg"<<uuid<<img<<para;
    if(img=="")
    {
        img=":/bg_plugin_page.png";
    }
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(uuid);
    if(p && p->page)
    {
        QString stylesheet="QWidget#QGeniePagePluginProxy"
                           "{"
                            "border-image: url(%1);"
                            "}";

        p->page->setStyleSheet(stylesheet.arg(img));
        qDebug()<<"setBackGroundImg done";
    }
}

#ifdef _WIN32
bool QGenieInterfaceUiManager::checkAdmin()
{
    bool ret=QGenieLaunchInf::sLaunchManager->elevateThis();
    return ret;
}
#endif




void QGenieInterfaceUiManager::notifyNetState(int st)
{
    QList<QUuid> uuids=mNetStateUuids.toList();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    foreach(QUuid uuid,uuids)
    {
        plug_inf *p=proxymanager->getPluginInf(uuid);
        if(p && p->pi)
        {
            p->pi->command(COMMAND_NOTIFYNETSTATE,st);
            qDebug()<<"notify net state change!!";
        }
    }
}

void QGenieInterfaceUiManager::notifyRouteType(QString type)
{
    QList<QUuid> uuids=mRouteTypeUuids.toList();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    foreach(QUuid uuid,uuids)
    {
        plug_inf *p=proxymanager->getPluginInf(uuid);
        if(p && p->pi)
        {
            p->pi->command(COMMAND_NOTIFYROUTETYPE,type);
            qDebug()<<"notifyRouteType!!";
        }
    }
}

void QGenieInterfaceUiManager::notifyGateway(QString str)
{
    QList<QUuid> uuids=mGatewayUuids.toList();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    QGenieInterfaceInfoManager *infomanager=QGenieInterfaceInfoManager::GetSingletonPtr();
    ASSERT(infomanager);
    infomanager->mGatewayCache=str;

    foreach(QUuid uuid,uuids)
    {
        plug_inf *p=proxymanager->getPluginInf(uuid);
        if(p && p->pi)
        {
            p->pi->command(COMMAND_NOTIFYGATEWAY,str);
            qDebug()<<"notifyRouteType!!";
        }
    }
}

void QGenieInterfaceUiManager::turnToThisPage(QUuid uuid,int msg)
{
    GENIE_LOG(QString("turn to this page: %1").arg(uuid.toString()));
    qDebug()<<"QGenieFrameTitle::on_but_getpwd_clicked()";
    QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(pluginmanager);

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);

    plug_inf *p=pluginmanager->getPluginInf(uuid);
    if(p && p->pi)
    {
        p->pi->command(msg,"");
        if(p->page)
        {
            stackmanager->changeStackViewIdx(p->page);
        }
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
        qDebug()<<"can not turn to this page";
    }
}

void QGenieInterfaceUiManager::setWifiName(const QString &wname)
{
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    title->setWifiName(wname);
    if(title)
    {
        title->setWifiName(wname);
    }
}

#include "QRCUiManager.h"
#include <QtCore>
#include "mainwindow.h"

QRCUiManager::QRCUiManager()
    :mBase(NULL)
    ,mInprocess(true)
{
}

void QRCUiManager::setWaitState(IPluginInf *pi,QString resion)
{
    if(mBase)
    {
        mBase->setWaitState(pi,resion);
    }
}

void QRCUiManager::restorNormalState(IPluginInf *pi,QString resion)
{
    if(mBase)
    {
        mBase->restorNormalState(pi,resion);
    }
}

void QRCUiManager::changeStackIdx(int idx)
{
    if(mBase)
    {
        mBase->changeStackIdx(idx);
    }
}

int QRCUiManager::command(QString cmd,QString para)
{
    if(!mInprocess) return -1;
    MainWindow *main=MainWindow::GetSingletonPtr();
	QGenieRCLogic logic;
    ASSERT(main);
    if(cmd =="logout")
    {
        logic.logout(para=="y");
        return 0;
    }
    else if(cmd == "towsetting")
    {
        QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
        ASSERT(wireless);
        wireless->endSettingMode();
        return 0;
    }
    else if(cmd == "apply")
    {
        main->whenApply(MainWindow::AT_TRAFFICMETER,para);
        return 0;
    }
    else if(cmd == "startrefresh")
    {
        main->setDisabled(true);

        qDebug()<<"routeconfig startrefresh";
        return 0;
    }
    else if(cmd == "endrefresh")
    {
        main->setDisabled(false);
		QGenieRCLogic logic;
		logic.processPendingCmd();
        qDebug()<<"routeconfif endrefresh";
        return 0;
    }
    if(mBase)
    {
        return mBase->command(cmd,para);
    }

    return 0;
}

// QClipboard *getClipBoard()
QString QRCUiManager::getClipBoardString()
{
    if(mBase)
    {
        return mBase->getClipBoardString();
    }
    return "";
}

void QRCUiManager::setClipBoardString(const QString s)
{
    if(mBase)
    {
        mBase->setClipBoardString(s);
    }
}

void QRCUiManager::setBigButtonIcon(const QString &ico,QUuid uuid,int idx)
{
	if (mBase)
	{
		mBase->setBigButtonIcon(ico,uuid,idx);
	}

}

void QRCUiManager::setBigButtonUserStateUi(QString stylesheet,QUuid uuid,int idx)
{
    if(mBase)
    {
        mBase->setBigButtonUserStateUi(stylesheet,uuid,idx);
    }
}

/*
  idx means the idx of bigbutton ,if the plugin have more than one bigbutton
*/
void QRCUiManager::setBigButtonState(IPluginInf *pi,ENUM_PLUGIN_STATUS state,int idx)
{
    //if(mBase)
    //{
    //    mBase->setBigButtonState(pi,state,idx);
    //}
}

void QRCUiManager::setBigButtonSubState(IPluginInf *pi,ENUM_BIGBT_SUB_STATE state,int idx)
{

}

void QRCUiManager::setNetStateFlag(QUuid uuid,bool setit)
{
    if(mBase)
    {
        mBase->setNetStateFlag(uuid,setit);
    }
}

void QRCUiManager::setRouteTypeFlag(QUuid uuid,bool setit)
{
    if(mBase)
    {
        mBase->setRouteTypeFlag(uuid,setit);
    }
}

void QRCUiManager::setGatewayFlag(QUuid uuid, bool setit)
{
    if(mBase)
    {
        mBase->setGatewayFlag(uuid,setit);
    }
}

void QRCUiManager::setBackGroundImg(QUuid uuid,QString img,QString para)
{
    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    wireless->mTrafficMetterImg=img;
    wireless->setBackgroundImg();
//    if(mBase)
//    {
//        mBase->setBackGroundImg(uuid,img,para);
//    }
}

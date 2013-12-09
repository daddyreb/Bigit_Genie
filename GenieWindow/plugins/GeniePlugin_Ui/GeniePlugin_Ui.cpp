
#include <QtGui>
#include <QUiLoader>
#include "GeniePlugin_Ui.h"
#include "version.h"
#include "genie2_interface.h"

#include "QGenieUiObject.h"
#include "QGenieTopoNodePaintStrategy.h"
#include "QGenieAdvStateMachine.h"
#ifdef Q_OS_MACX
#include "AppMenuHandler.h"
#endif
#include <QApplication>



static quint32 sUiFlags=0;

GeniePlugin_Ui::GeniePlugin_Ui()
    :mUiObj(new QGenieUiObject)
{

}

GeniePlugin_Ui::~GeniePlugin_Ui()
{
    delete mUiObj;
}

QVariant GeniePlugin_Ui::command(int cmd,const QVariant &paras)
{
//    HANDLER_CMD(0,QString());
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_CREATE_MAINFRAME)
    {
        mUiObj->createMainFrame();

    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_CONNECTION_STATE);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTERCONFIG_BUSY);
    }
    else if(cmd==I2CMD_NOTIFY_MESSAGE)
    {
        QVariantList varParas = paras.toList();
        //int nCount = varParas.count();
        int c=varParas.count();
        if(c>1)
        {
            int notify_message=varParas[0].toInt();
            if(notify_message==SCCMD_ROUTER_MODEL)
            {
                QString model=varParas[1].toString();
                if(!model.isEmpty())
                {
                    QTimer::singleShot(qrand()%3000,mUiObj,SLOT(slot_SaveSplash()));
                }


                GENIE2_LOG(QString("router model: \"")+model+"\"");

                QList<QGenieRouteWidget*> &lst=QGenieIface2ShareObject::sSingleInstance->mRouterWidgets;
                int c=lst.count();
                QImageNameGengerator gener(model);
                for(int i=0;i<c;i++)
                {
                    lst[i]->reloadImage();
                }

                if(!model.isEmpty())
                {
                    sUiFlags|=UIF_HAVE_MODEL;
                }
                else
                {
                    sUiFlags &=~UIF_HAVE_MODEL;
                }
                mUiObj->mGetPwd->setVisible((sUiFlags&(UIF_HAVE_MODEL|UIF_RC_BUSY))==UIF_HAVE_MODEL);
            }
            else if(notify_message==SCCMD_CONNECTION_STATE)
            {
                int netstate=varParas[1].toInt();
                mUiObj->setNetState(netstate);
            }
            else if(notify_message==SCCMD_ROUTERCONFIG_BUSY)
            {
                bool busy=varParas[1].toBool();
                if(busy)
                {
                    sUiFlags |=UIF_RC_BUSY;
                }
                else
                {
                    sUiFlags &=~UIF_RC_BUSY;
                }
                mUiObj->mGetPwd->setVisible((sUiFlags&(UIF_HAVE_MODEL|UIF_RC_BUSY))==UIF_HAVE_MODEL);
            }
        }
    }
	 else if(cmd==I2CMD_TIMETO_START)
    {
#ifdef Q_OS_MACX
    AppMenuHandler::installAppMenu();
    //AppMenuHandler::s_retranslateUi();
#endif
    }
//    else if(cmd==I2CMD_LANGUAGE_CHANGE)
//    {
//        mUiObj->changeLanguage();
//    }
    else if(cmd == I2CMD_REGISTER_UI)
    {
        mUiObj->registerUi(paras);
    }
    else if(cmd ==I2CMD_UI_COMMAND)
    {
        return mUiObj->parseUiCmd(paras);
    }
#ifdef USE_DBG_STACK
    else if(cmd ==I2CMD_REGISTER_DBG_UI)
    {
        mUiObj->registerDBGUi(paras);
    }
#endif
    else if(cmd==I2CMD_CREATEUI_END)
    {
        mUiObj->deployUi();
        if(!QApplication::arguments().contains("-mini"))
        {
            mUiObj->showMainFrame();
        }
    }
    else if(cmd==I2CMD_ABOUTTO_QUIT)
    {
#ifdef Q_OS_MACX
        AppMenuHandler::uninstallAppMenu();
#endif
    }
    return QVariant();
}





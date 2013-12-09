#include "GeniePlugin_MapV2.h"
#include "language_Map.h"
#include "version.h"

#include "QGenieTopoNodePaintStrategy.h"

#include <QtPlugin>
#include <QtGlobal>
#include <QUuid>
#include <QSettings>
#include <QApplication>
#include <QtDebug>

#include "networkmapview.h"
#include "netscanningcontroller.h"
#include "newdevicenotifymanager.h"
#include "blockdevicemanager.h"
#include "netcardspanel.h"
#include "genie2_netgear.h"
#include "QGenieMapDbgWObject.h"


#define PLUGINMAP_PRIORITY 1200


#define DEFAULT_CONFIGURATION_FILE  "config.ini"
#define CONFIGURATION_DEFAULTINTERFACE_KEY "plugin/Topology_DefaultInterface"
#define CONFIGURATION_NEWDEVICENOTIFY_KEY "plugin/Topology_Newdevicenotify"
#define CONFIGURATION_RECEIVEDIR_KEY "plugin/recvfolder"
#define CONFIGURATION_SENDDIR_KEY "plugin/sendsolder"

//#ifdef REBOUNCE_FOR_RAISE
//bool GeniePlugin_MapV2::sInFadeoutState=true;
//#endif
quint32 sMapFlags=ALL_SCAN_METHOD;

GeniePlugin_MapV2::GeniePlugin_MapV2(QObject *parent) :
    QObject(parent),
    //#ifdef DELAY_CURRENTSETTING_CMD
    //        m_routerTypeTimer(0),
    //#endif
    m_nmv(0),
    m_ndnm(0),
    m_nsc(0),
    m_deviceCount(0),
    m_bdm(0)

{
    mLastUpdateTime=QDateTime::currentDateTime().addDays(-1);
#ifdef POWERLINE_REVISION
    mPowerlineHandler=new PowerlineHandler;
#endif
    if(GENIE2_GET_SETTING(MAP_USE_RDP).toBool())
    {
        sMapFlags |= MF_USE_RDP_SCAN;
    }
    if(GENIE2_GET_SETTING(MAP_USE_PUTTY).toBool())
    {
        sMapFlags |= MF_USE_PUTTY_SCAN;
    }
    if(QApplication::arguments().contains("-dbg"))
    {
        sMapFlags |= MF_IN_DBG_MOD;
    }
}

GeniePlugin_MapV2::~GeniePlugin_MapV2()
{

    //    mNetgearInfoTimer.stop();
    //#ifdef DELAY_CURRENTSETTING_CMD
    //    if(m_routerTypeTimer)
    //        m_routerTypeTimer->stop();

    //    delete m_routerTypeTimer;
    //#endif
}

QVariant GeniePlugin_MapV2::command(int cmd, const QVariant &paras)
{
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_CONNECTION_STATE);

        QWidget *w=widget();
        GENIE2_REGISTER_UI(w,"1201:1600:Map:cmd1");
    }
    else if(cmd==I2CMD_TIMETO_START)
    {
        GENIE2_UI_COMMAND(UICMD_BIGBUTTON_BUTLINE_CLICKHERE,1206);
        if(sMapFlags &MF_IN_DBG_MOD)
        {
            QGenieMapDbgWObject *dbgO=new QGenieMapDbgWObject(this);
            GENIE2_REGISTER_DBG_UI(dbgO->getW());
        }
    }
    else if(cmd ==I2CMD_UI_ACTION )
    {
#ifdef USE_NODE_ANIMATION
#ifdef REBOUNCE_FOR_RAISE
        //        sInFadeoutState=false;
        sMapFlags |= MF_ONSTACK_TOP;
        m_nmv->reBounce();
#endif
#endif

#ifdef ENABLE_POWERLINE
        m_nmv->notifyRaise();
#endif

    }
    else if( cmd==I2CMD_UI_LOWER)
    {
#ifdef USE_NODE_ANIMATION
#ifdef REBOUNCE_FOR_RAISE
        //        sInFadeoutState=true;
        sMapFlags &= ~MF_ONSTACK_TOP;
        m_nmv->fadeoutAll();
#endif
#endif

#ifdef ENABLE_POWERLINE
        m_nmv->notifyLower();
#endif

    }
    else if(cmd==I2CMD_ABOUTTO_QUIT)
    {
        qDebug() << "exit";
        m_nsc->setExitFlag();
        m_nmv->setExit();
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
        QVariantList varParas = paras.toList();

        if(varParas.count() > 1){
            int notify_message = varParas[0].toInt();
            if(notify_message == SCCMD_CONNECTION_STATE)
            {
                int state= varParas[1].toInt();
                if(m_nmv)
                {
                    m_nmv->onInternetStateChanged(state != 1);
                }
            }
        }
        // GENIE2_STATISTICS("i");
    }
    //todo ÍøÂç×´Ì¬¸Ä±äÊ±
    //    if(cmd == COMMAND_NOTIFYNETSTATE)
    //    {
    //        bool internetConnected = (para != 1);
    //        if(m_nmv)
    //        {
    //            m_nmv->onInternetStateChanged(internetConnected);
    //        }
    //    }
    return QVariant();
}

//QString GeniePlugin_MapV2::text()
//{
//    return QString("Map:") + QString::number(L_MAP_LEFTBT);
//}

QWidget* GeniePlugin_MapV2::widget()
{
    //    retranslateUi();

    m_nmv = new NetworkMapView();
    m_ndnm = new NewDeviceNotifyManager(m_nmv);
    m_nsc = new NetScanningController(m_nmv);
    m_bdm = new BlockDeviceManager(m_nmv,m_nsc);

    connect(m_nsc,SIGNAL(newData(QList<Device*>)),
            m_nmv,SLOT(onNewData(QList<Device*>)));
    connect(m_nmv,SIGNAL(node2Device(int,Device*)),
            m_nsc,SLOT(onNode2Device(int,Device*)));
    //add by caiyujun
    connect(m_nmv,SIGNAL(ip2Device(QString,Device*)),
            m_nsc,SLOT(onIp2Device(QString,Device*)));
    connect(m_nmv,SIGNAL(bonjourTypeChanged(Device)),
            m_nsc,SLOT(onBonjourTypeChanged(Device)));
    //add end

    connect(m_nmv,SIGNAL(deviceDataChanged(Device)),
            m_nsc,SLOT(onDeviceDataChanged(Device)));
    connect(m_nsc,SIGNAL(infInformation(SVT_NotifyType)),
            m_nmv,SLOT(onInfInformation(SVT_NotifyType)));
    connect(m_nsc,SIGNAL(gatewayChanged(QString)),
            this,SLOT(onGatewayChanged(QString)));

    connect(m_nsc,SIGNAL(deviceCountChanged(int)),
            m_nmv,SLOT(onDeviceCountChanged(int)));
	//add by caiyujun
	connect(m_nsc,SIGNAL(otherDeviceCountChanged(QString,int)),
			m_nmv,SLOT(onOtherDeviceCountChanged(QString,int)));
	//add end
    connect(m_nmv,SIGNAL(refreshRequest()),
            m_nsc,SLOT(onRefresh()));
	//modify by caiyujun
    //connect(m_nmv,SIGNAL(subscribeDataRequest(int,int,int)),
    //        m_nsc,SLOT(onSubscribeData(int,int,int)));
	connect(m_nmv,SIGNAL(subscribeDataRequest(int,int,int,bool,unsigned long)),
			m_nsc,SLOT(onSubscribeData(int,int,int,bool,unsigned long)));
	//modify end

    //-------------------------------------------------
    connect(m_nsc,SIGNAL(deviceCountChanged(int)),
            this,SLOT(onDeviceCountChanged(int)));
    connect(m_nsc,SIGNAL(routerTypeChanged()),
            this,SLOT(onRouterTypeChanged()));
    //    connect(m_nsc,SIGNAL(currentSettingData(QMap<int,QString>)),
    //            this,SLOT(onNetgearInfo(QMap<int,QString>)));
    connect(m_nmv,SIGNAL(languageChanged()),
            this,SLOT(retranslateUi()));
    connect(m_nmv,SIGNAL(queryDefaultInterface(QString*)),
            this,SLOT(loadDefaultInterfaceFromConfiguration(QString*)));
    connect(m_nmv,SIGNAL(requestStoreDefaultInterface(QString)),
            this,SLOT(storeDefaultInterfaceToConfiguration(QString)));
	//add by caiyujun
	connect(m_nmv,SIGNAL(queryDefaultRecvFolder(QString*)),
            this,SLOT(loadRecvFolderFromConfiguration(QString*)));
    connect(m_nmv,SIGNAL(requestStoreDefaultRecvFolder(QString)),
            this,SLOT(storeDefaultRecvFolderToConfiguration(QString)));

	connect(m_nmv,SIGNAL(queryDefaultSendFolder(QString*)),
            this,SLOT(loadSendFolderFromConfiguration(QString*)));
    connect(m_nmv,SIGNAL(requestStoreDefaultSendFolder(QString)),
            this,SLOT(storeDefaultSendFolderToConfiguration(QString)));
	//add end
    connect(m_nmv,SIGNAL(translateText(int,QString*)),
            this,SLOT(translateText(int,QString*)));
    connect(m_nmv,SIGNAL(requestRouterImagePath(QString,int,QString*)),
            this,SLOT(onRequestRouterImagePath(QString,int,QString*)));

    connect(m_ndnm,SIGNAL(translateText(int,QString*)),
            this,SLOT(translateText(int,QString*)));



    connect(m_nmv,SIGNAL(queryNewDeviceNotifyOption(int*)),
            this,SLOT(loadNewDeviceNotifyOption(int*)));
    connect(m_nmv,SIGNAL(newDeviceNotifyOptionChanged(int)),
            this,SLOT(storeNewDeviceNotifyOption(int)));

    connect(m_nmv,SIGNAL(refreshRequest()),
            this,SLOT(onRefreshRequest()));
    connect(m_nmv,SIGNAL(deviceSetupMenuItemTriggered()),
            this,SLOT(onDeviceSetupMenuItemTriggered()));
    //add by caiyujun
    connect(m_nmv,SIGNAL(sendFileTriggered()),
            this,SLOT(onSendFileTriggered()));
    connect(m_nmv,SIGNAL(sendFolderTriggered()),
            this,SLOT(onSendFolderTriggered()));
    //add end
    //    connect(&mNetgearInfoTimer,SIGNAL(timeout()),
    //            this,SLOT(onNeedRefreshNetgearInfo()));
    //    mNetgearInfoTimer.start(5*60*1000);

#ifdef ENABLE_POWERLINE
    connect(m_nmv, SIGNAL(queryPLCDevicePassword(QString,QString*)),
            SLOT(loadPLCDevicePassword(QString,QString*)));
    connect(m_nmv, SIGNAL(storePLCDevicePassword(QString,QString)),
            SLOT(storePLCDevicePassword(QString,QString)));
#endif

#ifdef POWERLINE_REVISION
    PowerlineHandler *plhdl = PowerlineHandler::GetSingletonPtr();
    Q_ASSERT(plhdl);
    connect(plhdl,SIGNAL(powerLineStateChanged(int)),
            m_nsc,SLOT(onPowerLineStateChanged(int)));
#endif

    m_nsc->startScan();
#ifdef POWERLINE
    PowerlineHandler *plhdl=PowerlineHandler::GetSingletonPtr();
    Q_ASSERT(plhdl);
    Q_ASSERT(m_managerHandler);
    plhdl->setHandler(m_managerHandler);
#endif

    return m_nmv;
}


//void GeniePlugin_MapV2::command(int cmd,QString para0,QString para1)
//{
////    Q_UNUSED(cmd)
//    Q_UNUSED(para0)
//    Q_UNUSED(para1)
//    if(cmd == COMMAND_ABOUTTO_EXIT)
//    {
//        m_nsc->setExitFlag();
//    }
//    if(cmd ==COMMAND_DEBUG)
//    {
//        BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
//        if(bdm)
//        {
//            bdm->mDbgLabel->show();
//        }
//    }
//}

//void GeniePlugin_MapV2::command(int cmd, int para)
//{
//    if(cmd == COMMAND_NOTIFYNETSTATE)
//    {
//        bool internetConnected = (para != 1);
//        if(m_nmv)
//        {
//            m_nmv->onInternetStateChanged(internetConnected);
//        }
//    }
//
//}

//void* GeniePlugin_MapV2::getPoint(int para)
//{
//    Q_UNUSED(para)
//
//    return 0;
//}

//----------------------------------------------------------------------------
void GeniePlugin_MapV2::onDeviceCountChanged(int count)
{
    m_deviceCount = (count <= 0 ? 0 : count - 1);
    retranslateUi();
}

void GeniePlugin_MapV2::retranslateUi()
{
    if(m_deviceCount < 1)
    {
        GENIE2_UI_COMMAND(UICMD_BIGBUTTON_BUTLINE_CLICKHERE,
                          GENIE2_GET_TEXT(L_MAP_SCANNING));
    }
    else
    {
        GENIE2_UI_COMMAND(UICMD_BIGBUTTON_BUTLINE_CLICKHERE,
                          GENIE2_GET_TEXT(L_MAP_DEVICENUMBER)
                          +" "+QString::number(m_deviceCount));
        //text = text.arg(m_langManager->getText(,MAPPLUGIN_UUID) + " " + );
    }

    //    if(!m_uiManager || !m_langManager)
    //    {
    //        return;
    //    }

    //    QString text = BBT_MODE_CLICKHERE;

    //    if(m_deviceCount < 1)
    //    {
    //        text = text.arg(m_langManager->getText(L_MAP_SCANNING,MAPPLUGIN_UUID));
    //    }
    //    else
    //    {
    //        text = text.arg(m_langManager->getText(L_MAP_DEVICENUMBER,MAPPLUGIN_UUID) + " " + QString::number(m_deviceCount));
    //    }

    //    m_uiManager->setBigButtonUserStateText(text,MAPPLUGIN_UUID);
}

void GeniePlugin_MapV2::translateText(int idx ,QString *text)
{
    Q_ASSERT(text);
    text->clear();
    *text = GENIE2_GET_TEXT(idx);

}

//void GeniePlugin_MapV2::dumpLog(const QString &log)
//{
//    if(m_logManager)
//    {
//        m_logManager->writeLog(log,MAPPLUGIN_UUID);
//    }

//    qDebug() << log;

//}

void GeniePlugin_MapV2::onRequestRouterImagePath(const QString &model,int imageRole,QString *imagePath)
{
    Q_ASSERT(imagePath);
    Q_UNUSED(imageRole);
    imagePath->clear();
    qDebug()<<"onRequestRouterImagePath:"<<model;
    QImageNameGengerator routerImageSource(model);
    *imagePath =routerImageSource.getImageName(0);
}

void GeniePlugin_MapV2::loadDefaultInterfaceFromConfiguration(QString *infIdx)
{
    Q_ASSERT (infIdx);

    QVariant value;
    loadConfigurationItem(CONFIGURATION_DEFAULTINTERFACE_KEY,value);
    *infIdx = value.toString();
}

void GeniePlugin_MapV2::storeDefaultInterfaceToConfiguration(const QString &infIdx)
{
    storeConfigurationItem(CONFIGURATION_DEFAULTINTERFACE_KEY,infIdx);
}

//add by caiyujun
void GeniePlugin_MapV2::loadRecvFolderFromConfiguration(QString *folder)
{
    Q_ASSERT (folder);

    QVariant value;
    loadConfigurationItem(CONFIGURATION_RECEIVEDIR_KEY,value);
    *folder = value.toString();
}

void GeniePlugin_MapV2::storeDefaultRecvFolderToConfiguration(const QString &folder)
{
    storeConfigurationItem(CONFIGURATION_RECEIVEDIR_KEY,folder);
}

void GeniePlugin_MapV2::loadSendFolderFromConfiguration(QString *folder)
{
    Q_ASSERT (folder);

    QVariant value;
    loadConfigurationItem(CONFIGURATION_SENDDIR_KEY,value);
    *folder = value.toString();
}

void GeniePlugin_MapV2::storeDefaultSendFolderToConfiguration(const QString &folder)
{
    storeConfigurationItem(CONFIGURATION_SENDDIR_KEY,folder);
}

//add end

void GeniePlugin_MapV2::loadNewDeviceNotifyOption(int *option)
{
    Q_ASSERT (option);
    QVariant value;
    loadConfigurationItem(CONFIGURATION_NEWDEVICENOTIFY_KEY,value);
    *option = (value.canConvert(QVariant::Int) ? value.toInt() : 0);

    if(*option)
    {
        connect(m_nsc,SIGNAL(newDeviceNotify(SVT_NotifyType)),
                m_ndnm,SLOT(onNewDeviceNotify(SVT_NotifyType)),Qt::UniqueConnection);
    }
    else
    {
        disconnect(m_nsc,SIGNAL(newDeviceNotify(SVT_NotifyType)),
                   m_ndnm,SLOT(onNewDeviceNotify(SVT_NotifyType)));
    }
}

void GeniePlugin_MapV2::storeNewDeviceNotifyOption(int option)
{
    storeConfigurationItem(CONFIGURATION_NEWDEVICENOTIFY_KEY,option);

    if(option)
    {
        connect(m_nsc,SIGNAL(newDeviceNotify(SVT_NotifyType)),
                m_ndnm,SLOT(onNewDeviceNotify(SVT_NotifyType)),Qt::UniqueConnection);
    }
    else
    {
        disconnect(m_nsc,SIGNAL(newDeviceNotify(SVT_NotifyType)),
                   m_ndnm,SLOT(onNewDeviceNotify(SVT_NotifyType)));
    }

    Statistics(option ? "M2" : "M3");
}

#ifdef ENABLE_POWERLINE
#define PLC_DEVICE_PWD_KEY_PRE "PLC_Device_Pwd"

void GeniePlugin_MapV2::loadPLCDevicePassword(const QString &devMac, QString *pwd)
{
    if(0 == pwd){
        return;
    }

    QVariant value;
    const QString key = QString(PLC_DEVICE_PWD_KEY_PRE) + "/" + devMac;
    loadConfigurationItem(key, value);
    *pwd = value.toString();
}

void GeniePlugin_MapV2::storePLCDevicePassword(const QString &devMac, const QString &pwd)
{
    const QString key = QString(PLC_DEVICE_PWD_KEY_PRE) + "/" + devMac;
    storeConfigurationItem(key, pwd);
}

#endif

void GeniePlugin_MapV2::loadConfigurationItem(const QString &key,QVariant &value)
{
    value=GENIE2_GET_SETTING(key);
    //    if(m_settings)
    //    {
    //        value =  m_settings->value(key);
    //    }
    //    else
    //    {
    //        QString filePath = QApplication::applicationDirPath() + "/" + DEFAULT_CONFIGURATION_FILE;
    //        QSettings settings(filePath,QSettings::IniFormat);
    //        value = settings.value(key);
    //    }
}

void GeniePlugin_MapV2::storeConfigurationItem(const QString &key,const QVariant &value)
{
    GENIE2_SET_SETTING(key,value);
    //    if(m_settings)
    //    {
    //        m_settings->setValue(key,value);
    //    }
    //    else
    //    {
    //        QString filePath = QApplication::applicationDirPath() + "/" + DEFAULT_CONFIGURATION_FILE;
    //        QSettings settings(filePath,QSettings::IniFormat);
    //        settings.setValue(key,value);
    //    }
}

//void GeniePlugin_MapV2::setUiState(const QMap<int, QString> &data)
//{

//}

void GeniePlugin_MapV2::onRefreshRequest()
{
    Statistics("M1");
}

void GeniePlugin_MapV2::onRouterTypeChanged()
{
    //#ifdef DELAY_CURRENTSETTING_CMD
    //const int  ROUTERTYPE_REFRESH_DELAY =6000;

    //    if(0 == m_routerTypeTimer)
    //    {
    //        m_routerTypeTimer = new QTimer(this);
    //        m_routerTypeTimer->setSingleShot(true);
    //        m_routerTypeTimer->setInterval(ROUTERTYPE_REFRESH_DELAY);
    //        connect(m_routerTypeTimer,SIGNAL(timeout()),
    //                this,SLOT(tryRefreshRouterType()));
    //    }
    //    m_routerTypeTimer->stop();
    //    m_routerTypeTimer->start();
    //#else
    //    tryRefreshRouterType();
    //#endif

}

//void GeniePlugin_MapV2::tryRefreshRouterType()
//{
//    QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM->netgearCommand(
//            INetgearInterface::IC_CURRENTSETTING,QStringList(),NULL,NULL);
//}

void GeniePlugin_MapV2::onDeviceSetupMenuItemTriggered()
{
    Statistics("M4");
}

//add by caiyujun
void GeniePlugin_MapV2::onSendFileTriggered()
{
    Statistics("M5");
}

void GeniePlugin_MapV2::onSendFolderTriggered()
{
    Statistics("M6");
}
//add end

void GeniePlugin_MapV2::Statistics(const QString &funCode)
{
    GENIE2_STATISTICS(funCode);
}

//void GeniePlugin_MapV2::onNetgearInfo(const QMap<int,QString> &data)
//{
//    if(!m_uiManager)
//    {
//        return;
//    }

//    if(data.value(NIE_CURRENTSETTING_MESSAGETYPE)=="currentsetting")
//    {
//        if(data.contains(NIE_CURRENTSETTING_MODEL)
//                &&data.contains(NIE_CURRENTSETTING_FIRMWARE)
//                &&data.contains(NIE_CURRENTSETTING_REGION)
//                &&data.contains(NIE_CURRENTSETTING_REGIONTAG))
//        {
//            m_uiManager->command(UI_CMD_SET_FRIMWARE,data[NIE_CURRENTSETTING_FIRMWARE]);

//            m_uiManager->command(UI_CMD_SET_ROUTE,data[NIE_CURRENTSETTING_MODEL]);
//            mRouter=data[NIE_CURRENTSETTING_MODEL];
//            mFirmware=data[NIE_CURRENTSETTING_FIRMWARE];
//            mCurrentSetting=QString("Firmware=%1 RegionTag=%2 Region=%3 Model=%4")
//                    .arg(data[NIE_CURRENTSETTING_FIRMWARE],data[NIE_CURRENTSETTING_REGIONTAG]
//                         ,data[NIE_CURRENTSETTING_REGION],data[NIE_CURRENTSETTING_MODEL]);

//        }
//        else
//        {
//            m_uiManager->command(UI_CMD_SET_FRIMWARE,"");
//            m_uiManager->command(UI_CMD_SET_ROUTE,"");
//            mRouter=mFirmware=mCurrentSetting="";
//        }

//        dumpLog("Mapv2 onNetgearInfo mCurrentSetting = " + mCurrentSetting);

//         mRsLevel=data.value(NIE_CURRENTSETTING_RSLEVEL);
//         mRouterLoginNetIp=data.value(NIE_CURRENTSETTING_ROUTELOGINDOTNETIP);

//         static QString sMac="";
//         QString mac=data.value(NIE_CURRENTSETTING_ROUTELOGINDOTNETMAC);
//         if(mac != sMac)
//         {
//             sMac=mac;
//             m_infoManager->notifyPluginMessage(INFO_NOTIFY_ROUTELOGINDOTNET_MAC_CHANGE,sMac);
//         }

//        static QString sIp="";
//        QString ip=data.value(NIE_CURRENTSETTING_ROUTELOGINDOTNETIP);
//        if(ip !=sIp)
//        {
//            sIp=ip;
//            m_infoManager->notifyPluginMessage(INFO_NOTIFY_ROUTELOGINNET_IP_CHANGE,sIp);
//        }

//    }
//    else if(data.value(NIE_CURRENTSETTING_MESSAGETYPE)=="update")
//    {
//        if(
//                data.value(NIE_UPDATE1_NEEDUPDATE)=="y"
//                &&data.contains(NIE_UPDATE1_ADDR)
//                &&data.contains(NIE_UPDATE1_MD5)
//                &&data.contains(NIE_UPDATE1_SIZE)
//                &&data.contains(NIE_UPDATE1_VERSION)
//                )
//        {
//            m_uiManager->command(UI_CMD_SHOW_UPDATE_BUTTON,"");
//            mFUPUrl=data[NIE_UPDATE1_ADDR];
//            mFUPMd5=data[NIE_UPDATE1_MD5];
//            mFUPSize=data[NIE_UPDATE1_SIZE];
//            mFUPVersion=data[NIE_UPDATE1_VERSION];
//        }
//        else
//        {
////            m_uiManager->command(UI_CMD_HIDE_UPDATE_BUTTON,"");
////            mFUPUrl=mFUPMd5=mFUPSize="";
//        }
//    }
//    else
//    {
//        Q_ASSERT(0);
//    }

//}

//void GeniePlugin_MapV2::onNeedRefreshNetgearInfo()
//{
////	qDebug()<<mLastUpdateTime.daysTo(QDateTime::currentDateTime());
//    if(mLastUpdateTime.daysTo(QDateTime::currentDateTime())>=1)
//    {
//        mLastUpdateTime=QDateTime::currentDateTime();
//        m_nsc->refreshNetgearInfo();
//    }
//}

void GeniePlugin_MapV2::onGatewayChanged(QString data)
{
#if 0
    if(!m_infoManager || !m_uiManager)
    {
        return;
    }
    QString ip,mac;
    QStringList templst=data.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(templst.count()>=2)
    {
        ip=templst[0];
        mac=templst[1];
    }
    static QString sIp="";
    static QString sMac="";
    if(ip !=sIp)
    {
        sIp=ip;
        m_infoManager->notifyPluginMessage(INFO_NOTIFY_GATEWAY_CHANGE,sIp);
    }
    if(mac != sMac)
    {
        sMac=mac;
        m_infoManager->notifyPluginMessage(INFO_NOTIFY_MAC_CHANGE,sMac);
    }
#endif

}





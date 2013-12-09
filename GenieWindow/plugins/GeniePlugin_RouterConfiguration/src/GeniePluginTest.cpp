#include "GeniePluginTest.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include <QAction>
#include "ITrayManager.h"
#include "mainwindow.h"
#include "inc/genie_interface/date.h"
#include "inc/multilanguage_def/LanguageRouterConfiguration.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "inc/genie_interface/bbt_paras.h"
#include "inc/genie_interface/IClickManager.h"
#include "inc/genie_interface/cmd_defs.h"
#include "version.h"
#include "ui_QGenieRCPageGuessAccess.h"
#include "ILogManager.h"
#include "ui_QGenieRCPageWireless.h"
#include "QGenieRCLogic.h"
#include "common.h"

#ifdef USE_TEST_UI
#include "WidgetTestUi.h"
#endif


#define NETERR_STRATEGY 0
//0.LOGOUT 1.ENDSETTING

#define PLUGIN_INFO "UUID:{6c1c9b2f-5f5e-45f8-8dad-1633ff446377};\
BIND:1;\
            HAVEWND:1;\
                VERSION:%1"

                    IPluginInf *GeniePlugin_RouterConfiguration::sSinglePtr=NULL;

    QUuid uuid ="{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}";

    GeniePlugin_RouterConfiguration::GeniePlugin_RouterConfiguration()
        :mDialog(NULL)
        ,mW(NULL)
#ifdef ENABLE_READYSHARE
        ,m_bgo_readyshare(false)
        ,m_inner_readyshare_plugin(NULL)
#endif
    {
        Q_ASSERT(GeniePlugin_RouterConfiguration::sSinglePtr ==0);
        sSinglePtr=this;
        for(int i=0;i<SP_NUM;i++)
        {
            mPlugins[i]=NULL;
        }

#ifdef ENABLE_READYSHARE
        m_inner_readyshare_plugin = new QGenieReadyShare();
#endif
    }

    QString GeniePlugin_RouterConfiguration::text()
    {
#ifdef ENABLE_READYSHARE
        //return QString("Router Configuration:1500") + "#" + m_inner_readyshare_plugin->text();
        return QString("Router Configuration:1500") + "#" + QString("Ready Share:") + QString::number(L_READY_SHARE_BIGBT);
#else
        return QString("Router Configuration:1500");
#endif
    }

    QWidget* GeniePlugin_RouterConfiguration::widget(QWidget *parent)
    {
        mW=new MainWindow(parent);
        mW->wireless->loadSubPlugins();
#ifdef SHEET_STYLE_FORMAC
        IInfoManager *infom=MainWindow::sHandler->getInfoManager();
        ASSERT(infom);
        QWidget *sheetParent=(QWidget *)infom->getSharedMemory(INFO_CMD_SHEET_PARENT);
        mDialog=new QGenieMsgDialog(sheetParent);
#else
        mDialog=new QGenieMsgDialog();
#endif
#ifdef USE_TEST_UI
        WidgetTestUi *testui=new WidgetTestUi();
        testui->show();
#endif

	QGenieRCLogic logic;
	logic.limitTextLength();
        return mW;
    }

    QIcon * GeniePlugin_RouterConfiguration::icon()
    {
        return NULL;
    }

    int GeniePlugin_RouterConfiguration::changelanguage(int idx)
    {
        return 0;
    }

    QString  GeniePlugin_RouterConfiguration::info()
    {
        return QString(PLUGIN_INFO).arg(VERSION_STRING);
    }

    QString GeniePlugin_RouterConfiguration::getInfo(int para)
    {
        if(PLUGIN_INF_DEPENDENCE == para)
        {
            return QString("InnerPlugin_RouterConfiguration.dll!001;""");
        }
        if(PLUGIN_INF_LANGUAGE_FILE == para)
        {
            return "LanguageRouterConfiguration.txt";
        }
        if(PLUGIN_INF_CREATE_DATE==para)
        {
            return VERSTION_STR_DATE;
        }
        if(PLUGIN_INF_INNER_LANGUAGE == para)
        {
//#ifdef ENABLE_READYSHARE
//            QString readyshare_langfile = m_inner_readyshare_plugin->getInfo(PLUGIN_INF_LANGUAGE_FILE);

//            QString readyshare_lang_entry = getReadyShareUuid() + ":" + readyshare_langfile;

//#endif
            return QString(
                    "{35b9be0c-b073-4cf8-bc12-d11923dc3d49}:LanguageTrafficMeter.txt"
                    ";{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}:LanguageFirmwareUpdate.txt"
                    ";{0a1e6c13-c838-47e0-9217-6ef881af5641}:LanguageGuestAccess.txt"
                    ";{f8018c9f-5431-497e-ac5d-511dd0ae7402}:LanguageWirelessExport.txt"
                    )
//#ifdef ENABLE_READYSHARE
//                    + ";" + readyshare_lang_entry
//#endif
                    ;

        }
//        if("gacctimer"==para)
//        {
//            QGenieRCLogic logic;
//            return logic.getDisableTimerStatus();
//        }

        return "";
    }

    int GeniePlugin_RouterConfiguration::priority()
    {
        return 1125;
    }

    void GeniePlugin_RouterConfiguration::setHandler(IManagerHandler *hdl)
    {
        MainWindow::sHandler =hdl;
        mManagerHandler.setBase(hdl);
        //MainWindow::sHandler=mManagerHandler;
    }

    void GeniePlugin_RouterConfiguration::startup(bool mini)
    {

    }

    GeniePlugin_RouterConfiguration::~GeniePlugin_RouterConfiguration()
    {
        unloadAllSubPlugins();
        qDebug("GeniePlugin_RouterConfiguration::~GeniePlugin_RouterConfiguration()");
#ifndef SHEET_STYLE_FORMAC
        SAFE_DELETE(mDialog);
#endif

#ifdef ENABLE_READYSHARE
        delete m_inner_readyshare_plugin;
        m_inner_readyshare_plugin   = 0;
        mPlugins[SP_READYSHARE]     = 0;
#endif
    }

    void GeniePlugin_RouterConfiguration::notifyRaise()
    {
#ifdef ENABLE_READYSHARE
        if(m_bgo_readyshare)
        {
            //this->command(COMMAND_TURNTOREADYSHARE);
            return;
        }
#endif

        QGenieRCPageLogin *login=QGenieRCPageLogin::GetSingletonPtr();
        ASSERT(login);
	QGenieRCLogic logic;
        if(!logic.isLogin() && login->mUi.edt_pwd->text()!="")
        {
            login->tryStartLogin();
        }
    }

    void GeniePlugin_RouterConfiguration::notifyLower()
    {
        qDebug("GeniePlugin_RouterConfiguration::notifyLower()");
    }

    //int GeniePlugin_RouterConfiguration::getStatus()
    //{

    //    return 0;
    //}
    QStringList GeniePlugin_RouterConfiguration::bigButtonParas()
    {
        QStringList ret;
        QString testuiPara="Router Configuration:1125";
        testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_RTCF_BIGBT));
      //  testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=2");
        ret.append(testuiPara);
#ifdef ENABLE_READYSHARE
        //    QString readySharePara = "NETGEAR ReadyShare:1126";
        //    readySharePara += QString(":"BBT_PARA_LAN_IDX"=%1").arg("L_READY_SHARE_BIGBT");
        //    readySharePara += QString(":"BBT_PARA_STATE_UPDATE_POLICY"=2");
        //ret.append(m_inner_readyshare_plugin->bigButtonParas());
        QString readySharePara = "ReadySHARE:1526";
        readySharePara += QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_READY_SHARE_BIGBT));
        //readySharePara += QString(":"BBT_PARA_USER_TEXT_RIGHT"=%1").arg(QString::number(L_READY_SHARE_NOT_ENABLE));
        //readySharePara += QString(":"BBT_PARA_GOOD_TEXT_RIGHT"=%1").arg(QString::number(L_READY_SHARE_ENABLE));
     //   readySharePara += QString(":"BBT_PARA_STATE_UPDATE_POLICY"=2");

        ret.append(readySharePara);
#endif
        return ret;
    }

    void GeniePlugin_RouterConfiguration::notifyBigButton(QString s)
    {
#ifdef ENABLE_READYSHARE
        QStringList text_list = m_inner_readyshare_plugin->text().split(":");
        QStringList bigbutpara_list = m_inner_readyshare_plugin->bigButtonParas();
        QString text_match;
        QString bigbutpara_match;

        if(text_list.size() >= 1)
        {
            text_match = text_list[0];
        }

        if(bigbutpara_list.size() >= 1)
        {
            bigbutpara_list = bigbutpara_list[0].split(":");
            if(bigbutpara_list.size() >= 1)
            {
                bigbutpara_match = bigbutpara_list[0];
            }
        }

        m_bgo_readyshare = (!s.compare(text_match) || !s.compare(bigbutpara_match));

        if(m_bgo_readyshare)
        {
            this->command(COMMAND_TURNTOREADYSHARE);
        }
        else
        {
            QGenieRCLogic logic;
            if(logic.isLogin())
            {
                logic.turnto_wirelesssettings();
            }
        }
        //#include <QMessageBox>
        //    if(m_bgo_readyshare)
        //    {
        //        QMessageBox::information(NULL,"ready share","go to ready share now");
        //    }

        //    QMessageBox::information(NULL,"ready share",s);
#endif

        //s = "h";
        //IClickManager* clickmanage = MainWindow::sHandler->getClickManager();
        //clickmanage->click(s);
    }

    void GeniePlugin_RouterConfiguration::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
    {
        qDebug(":notifyTrayMessage(QSystemTrayIcon::ActivationReason %d)",reason);
    }

 /*   int GeniePlugin_RouterConfiguration::getStatus(QString notifyPara)
    {
        return EPS_USER2;
    }*/

    void GeniePlugin_RouterConfiguration::command(int cmd, int para)
    {
        QGenieRCLogic logic;
        logic.parseCmd(cmd,para);

    }


    void *GeniePlugin_RouterConfiguration::getPoint(int para)
    {
        return NULL;
    }

    void GeniePlugin_RouterConfiguration::command(int cmd, QString para0, QString para1)
    {
        QGenieRCLogic logic;
        logic.parseCmd(cmd,para0,para1);

    }

    QWidget* GeniePlugin_RouterConfiguration::loadSubPluginWidget(SUBPLUGINS type)
    {
        QWidget *w=NULL;
        //    return w;
#ifdef PLATFORM_WIN32
        const char *cFilename[]={
            "InnerPlugin_WirelessExport.dll",
            "InnerPlugin_TrafficMeter.dll",
            "InnerPlugin_FirmwareUpdate.dll"
        };
#elif defined PLATFORM_MACOSX
        const char *cFilename[]={
            "../PlugIns/libInnerPlugin_WirelessExport.dylib",
            "../PlugIns/libInnerPlugin_TrafficMeter.dylib",
            "../PlugIns/libInnerPlugin_FirmwareUpdate.dylib"
        };
#elif defined PLATFORM_LINUX
        const char *cFilename[]={
            "../PlugIns/libInnerPlugin_WirelessExport.so",
            "../PlugIns/libInnerPlugin_TrafficMeter.so",
            "../PlugIns/libInnerPlugin_FirmwareUpdate.so"
        };
#endif

#ifdef ENABLE_READYSHARE
        QObject *obj = NULL;
        if(SP_READYSHARE == type)
        {
            obj = m_inner_readyshare_plugin;
        }
        else
        {
            mLoaders[type].setFileName(GENIE_PATH+cFilename[type]);
            obj = mLoaders[type].instance();
        }
#else
        mLoaders[type].setFileName(GENIE_PATH+cFilename[type]);
        QObject *obj = mLoaders[type].instance();
#endif

        mPlugins[type] = qobject_cast<IPluginInf*>(obj);

        if(mPlugins[type])
        {
            ASSERT(mW && mW->wireless);
            QRCManagerHandler *handlerforsubplugin=QRCManagerHandler::GetSingletonPtr();
            ASSERT(handlerforsubplugin);
            mPlugins[type]->setHandler(handlerforsubplugin);
            w = mPlugins[type]->widget(mW->wireless);


        }
        return w;
    }

    void GeniePlugin_RouterConfiguration::unloadAllSubPlugins()
    {
        for(int i=0;i<SP_NUM;i++)
        {
            mPlugins[i]=0;
#ifdef ENABLE_READYSHARE
            if(SP_READYSHARE == i)
            {
                delete mPlugins[i];
                mPlugins[i] = 0;
                m_inner_readyshare_plugin = 0;
            }
            else
            {
                mLoaders[i].unload();
            }
#else
            mLoaders[i].unload();
#endif
        }
    }

#ifdef ENABLE_READYSHARE
    QString GeniePlugin_RouterConfiguration::getReadyShareUuid()
    {
        QString readyshare_uuid = m_inner_readyshare_plugin->info();
        QStringList info_list = readyshare_uuid.split(":");
        Q_ASSERT(info_list.size() >= 2);
        info_list = info_list[1].split(";");
        Q_ASSERT(info_list.size() >= 1);
        readyshare_uuid = info_list[0];

        return readyshare_uuid;
    }

#endif

    Q_EXPORT_PLUGIN2(GeniePlugin_RouterConfiguration, GeniePlugin_RouterConfiguration)

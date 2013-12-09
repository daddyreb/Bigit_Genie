#include "GeniePlugin_NetTraffic.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include <QAction>
#include "ITrayManager.h"
#include "QGenie_qtNetTraffic.h"
#include "bbt_paras.h"

#include "ILanguageManager.h"
#include "LanguageNetTraffic.h"
#include <QEvent>
#include <QSettings>
#include "setting_def.h"
#include <QThread>
#include "QGenieAboutThread.h"
#include "ILogManager.h"
#include "IUiManager.h"
#include "version.h"


//网络流量显示	Network Traffic	0	2000	{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}	0	邓治国

//,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")
#define PLUGIN_INFO "UUID:{bfc53cc5-97a4-4060-aff9-30a4b52b7e99};\
BIND:0;\
            HAVEWND:0;\
                VERSION:%1"


    IPluginInf *GeniePlugin_NetTraffic::sSinglePtr=NULL;

    GeniePlugin_NetTraffic::GeniePlugin_NetTraffic()
    {
        w=0;
        Q_ASSERT(GeniePlugin_NetTraffic::sSinglePtr ==0);
        sSinglePtr=this;
    }

    QString GeniePlugin_NetTraffic::text()
    {
        //return QString("GeniePlugin_NetTraffic");
        return QString("GeniePlugin_NetTraffic:")+QString::number(L_NETTRAFFIC_BIGBT);
    }

    QString GeniePlugin_NetTraffic::getInfo(QString para)
    {

        if(PLUGIN_INF_DEPENDENCE == para)
        {
            return "ExTable.dll!001";
        }
        if(PLUGIN_INF_LANGUAGE_FILE == para)
        {
           // return "LanguageNetTraffic.txt";
        }
        if(PLUGIN_INF_CREATE_DATE==para)
        {
            return VERSTION_STR_DATE;
        }
        return "";
    }

    QWidget* GeniePlugin_NetTraffic::widget(QWidget *parent)
    {
       return NULL;
    }


    QStringList GeniePlugin_NetTraffic::bigButtonParas()
    {
        QStringList ret;
        QString testuiPara="NetTraffic:2000";
        testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_NETTRAFFIC_LEFTBT));
        testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");
        ret.append(testuiPara);

        return ret;
    }


    void GeniePlugin_NetTraffic::notifyBigButton(QString s)
    {
        //qDebug()<< "GeniePlugin_TestUi::notifyBigButton" << s;
    }


    QIcon * GeniePlugin_NetTraffic::icon()
    {
        return NULL;
    }

    int GeniePlugin_NetTraffic::changelanguage(int idx)
    {
            return 0;
    }

    QString  GeniePlugin_NetTraffic::info()
    {
        return QString(PLUGIN_INFO).arg(VERSION_STRING);
    }

    int GeniePlugin_NetTraffic::priority()
    {
        return 2000;
    }


    void GeniePlugin_NetTraffic::setHandler(IManagerHandler *hdl)
    {
        QGenie_qtNetTraffic::sHandler=hdl;
    }

    void GeniePlugin_NetTraffic::startup(bool mini)
    {
        w=new QGenie_qtNetTraffic();
        w->Genie_Log("startup()");
        //w->show();

       // QSettings setting(ORGANIZATION_NAME,APPNAME);
        //bool b=setting.value(SETTING_TRAFFIC_START_MINIMIZE,false).toBool();
        //setting.value()
        if ( mini)
        {
            w->hide();
        }
        else
        {
            w->hide();
        }

        w->startGetDataThread();

        //ITrayManager *traymanager=w->sHandler->getTrayManager();
       // traymanager->addMenu(w->qmenuTroy,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
        //traymanager->setMessageFlag(this,true);

    }

    GeniePlugin_NetTraffic::~GeniePlugin_NetTraffic()
    {
        //qDebug("GeniePluginTest::~GeniePluginTest()");
        if (w)
        {
            w->Genie_Log("~GeniePlugin_NetTraffic()");
            w->Timerdraw->deleteLater();
            w->thd->selfstop();
            w->thd->wait();
            w->thd->deleteLater();
            w->close();
            delete w;
        }
    }

    void GeniePlugin_NetTraffic::notifyRaise()
    {
        qDebug("GeniePluginTest::notifyRaise()");

    }

    void GeniePlugin_NetTraffic::notifyLower()
    {
        qDebug("GeniePluginTest::notifyLower()");

    }

    int GeniePlugin_NetTraffic::getStatus(QString notifyPara)
    {
        //just for test
        return 0;
       // return 0;
    }


    void GeniePlugin_NetTraffic::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
    {
        //qDebug(":notifyTrayMessage(QSystemTrayIcon::ActivationReason %d)",reason);
        /*    MFR_MINIMIZE,
              MFR_MAXIMIZE,
              MFR_NORMALIZE,
              MFR_HIDE,
              MFR_UNKNOWN
        */
        /*
        if (w)
        {
            w->Genie_Log("notifyTrayMessage()");
        }
        if (res==MFR_MAXIMIZE || res==MFR_NORMALIZE)
        {
            w->iconActivated(reason,true);
        }
        if (res==MFR_MINIMIZE || res==MFR_HIDE)
        {
            w->iconActivated(reason,false);
        }
        */
    }


    void GeniePlugin_NetTraffic::command(QString cmd,QString para0,QString para1)
    {

    }

    void GeniePlugin_NetTraffic::command(QString cmd, int para)
    {
        if(cmd ==COMMAND_NOTIFYNETSTATE)
         {
             //char * cText[]={"net ok","net error","over flow"};
            if (w)
            {
                if (para==NS_OK)
                {
                    w->bNetOk=true;
                    //if (w->m_cTrafficClass)
                    w->m_cTrafficClass[0].GetInterfaces();
                    w->m_cTrafficClass[1].GetInterfaces();
                    w->m_cTrafficClass[2].GetInterfaces();
                    //w->m_cTrafficClass[3].GetInterfaces();
                   // w->m_cTrafficClass[4].GetInterfaces();

                    w->m_cTrafficClass1[0].GetInterfaces();
                    w->m_cTrafficClass1[1].GetInterfaces();
                    w->m_cTrafficClass1[2].GetInterfaces();
                   // w->m_cTrafficClass1[3].GetInterfaces();
                   // w->m_cTrafficClass1[4].GetInterfaces();

                    w->m_cTrafficClass1[0].GetInterfaces();
                    w->m_cTrafficClass1[1].GetInterfaces();
                    w->m_cTrafficClass1[2].GetInterfaces();
                    //w->m_cTrafficClass1[3].GetInterfaces();
                   // w->m_cTrafficClass1[4].GetInterfaces();

                    w->infChoose[0]=1;
                    w->infChoose[1]=1;
                    w->infChoose[2]=1;
                   // w->infChoose[3]=1;
                   // w->infChoose[4]=1;


                }
                if (para==NS_ERROR)
                {
                    w->bNetOk=false;
                    w->m_cTrafficClass[0].GetInterfaces();
                    w->m_cTrafficClass[1].GetInterfaces();
                    w->m_cTrafficClass[2].GetInterfaces();
                   // w->m_cTrafficClass[3].GetInterfaces();
                   // w->m_cTrafficClass[4].GetInterfaces();

                    w->m_cTrafficClass1[0].GetInterfaces();
                    w->m_cTrafficClass1[1].GetInterfaces();
                    w->m_cTrafficClass1[2].GetInterfaces();
                    //w->m_cTrafficClass1[3].GetInterfaces();
                    //w->m_cTrafficClass1[4].GetInterfaces();

                    w->m_cTrafficClass1[0].GetInterfaces();
                    w->m_cTrafficClass1[1].GetInterfaces();
                    w->m_cTrafficClass1[2].GetInterfaces();
                    //w->m_cTrafficClass1[3].GetInterfaces();
                    //w->m_cTrafficClass1[4].GetInterfaces();

                    w->infChoose[0]=1;
                    w->infChoose[1]=1;
                    w->infChoose[2]=1;
                  //  w->infChoose[3]=1;
                   // w->infChoose[4]=1;
                }
            }
        }
    }


    void *GeniePlugin_NetTraffic::getPoint(QString para)
    {
        //
//      #define PLUGIN_INF_SM_TRAFFIC_STATS "trstates"
//      #define PLUGIN_INF_SM_TRAFFIC_INCOMING "traffinco"
//      #define PLUGIN_INF_SM_TRAFFIC_OUTGOING "traffoutgo"
        if (para==PLUGIN_INF_SM_TRAFFIC_STATS)
        {
            if (w)
            {
                return (void *)(w->TrafficStats);
            }
        }
        if (para==PLUGIN_INF_SM_TRAFFIC_INCOMING)
        {
            if (w)
            {
                return (void *)(w->TrafficStatsIncoming);
            }
        }
        if (para==PLUGIN_INF_SM_TRAFFIC_OUTGOING)
        {
            if (w)
            {
                return (void *)(w->TrafficStatsOutgoing);
            }
        }
        return NULL;
    }

   // Q_EXPORT_PLUGIN2(GeniePluginTest, GeniePluginTest)

    Q_EXPORT_PLUGIN2(GeniePlugin_NetTraffic, GeniePlugin_NetTraffic)

//Q_EXPORT_PLUGIN2(GeniePlugin_NetTraffic, GeniePlugin_NetTraffic)
//Q_EXPORT_PLUGIN2(GeniePluginTest, GeniePluginTest)

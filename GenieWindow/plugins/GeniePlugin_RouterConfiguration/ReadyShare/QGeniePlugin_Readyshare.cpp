#include "QGeniePlugin_Readyshare.h"
#include <QtPlugin>

#include <QDir>
#include <QFile>

#include "bbt_paras.h"
//#include "LanguageReadyShare.h"
#include "LanguageRouterConfiguration.h"

#include "pcpform.h"
#include "QGeniePlugin_Not_Surport_ReadyShare.h"
#include "QGeniePlugin_Basic_ReadyShare.h"
#include "QGeniePlugin_Basic_Remote_ReadyShare.h"
#include "QGeniePlugin_Basic_Remote_ReadyShare.h"
#include "QGeniePlugin_Basic_Remote_Printing_ReadyShare.h"
#include "QGeniePlugin_Basic_Choosed_ReadyShare.h"
#include "QGeniePlugin_Remote_Choosed_ReadyShare.h"
#include "QGeniePlugin_Printing_Choosed_ReadyShare.h"


#include "IClickManager.h"
#include "version.h"



#define PLUGIN_INFO "UUID:{6c1c9b2f-5f5e-45f8-8dad-1633ff446377};\
BIND:1;\
          HAVEWND:1;\
              VERSION:%1"

QUuid g_readyShare_Uuid("{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}");

    QGenieReadyShare::QGenieReadyShare()
    {

    }

    QStringList QGenieReadyShare::bigButtonParas()
    {

        QStringList ret;
        QString testuiPara="ReadySHARE:1526";
//        //testuiPara+=":"BBT_PARA_WAITING_TEXT_LEFT"=testdd:"BBT_PARA_FREQUENCY"=10000";
//        testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_READY_SHARE_BIGBT));
//        testuiPara+=QString(":"BBT_PARA_USER_TEXT_RIGHT"=%1").arg(QString::number(L_READY_SHARE_NOT_ENABLE));
//        testuiPara+=QString(":"BBT_PARA_GOOD_TEXT_RIGHT"=%1").arg(QString::number(L_READY_SHARE_ENABLE));
//        testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");
        ret.append(testuiPara);

        return ret;

    }


    void QGenieReadyShare::notifyBigButton(QString s)
    {
        //qDebug()<< "GeniePlugin_TestUi::notifyBigButton" << s;

        IClickManager *clickmanager=PCPForm::sHandler->getClickManager();
        if (clickmanager)
        {
            clickmanager->click("f");
        }
    }


    QString QGenieReadyShare::text()
    {

        return QString("Ready Share:")+QString::number(L_READY_SHARE_BIGBT);
    }

    QWidget* QGenieReadyShare::widget(QWidget *parent)
    {
        return new PCPForm(parent);
    }

    QIcon * QGenieReadyShare::icon()
    {
        return NULL;
    }

    QString QGenieReadyShare::getInfo(int para)
    {

        if(PLUGIN_INF_LANGUAGE_FILE == para)
        {
            return "LanguageReadyShare.txt";
        }

        if(PLUGIN_INF_CREATE_DATE==para)
        {
            return VERSTION_STR_DATE;
        }

        return "";
    }


    int QGenieReadyShare::changelanguage(int idx)
    {
        return 0;
    }

    QString  QGenieReadyShare::info()
    {
        return QString(PLUGIN_INFO).arg(VERSION_STRING);
    }

    int QGenieReadyShare::priority()
    {
        return 1526;
    }


    void QGenieReadyShare::setHandler(IManagerHandler *hdl)
    {
        PCPForm::sHandler=hdl;
        QGeniePlugin_Not_Surport_ReadyShare::sHandler=hdl;
        QGeniePlugin_Basic_ReadyShare::sHandler=hdl;
        QGeniePlugin_Basic_Remote_ReadyShare::sHandler=hdl;
        QGeniePlugin_Basic_Printing_ReadyShare::sHandler=hdl;
        QGeniePlugin_Basic_Remote_Printing_ReadyShare::sHandler=hdl;
        QGeniePlugin_Basic_Choosed_ReadyShare::sHandler=hdl;
        QGeniePlugin_Remote_Choosed_ReadyShare::sHandler=hdl;
        QGeniePlugin_Printing_Choosed_ReadyShare::sHandler=hdl;
        QGeniePlugin_Remote_Unregister_ReadyShare::sHandler=hdl;
    }

    void QGenieReadyShare::startup(bool mini)
    {


    }

    QGenieReadyShare::~QGenieReadyShare()
    {
        //qDebug("GeniePluginTest::~GeniePluginTest()");
    }

    void QGenieReadyShare::notifyRaise()
    {

    }

    void QGenieReadyShare::notifyLower()
    {

    }

//    int QGenieReadyShare::getStatus(QString notifyPara)

//    {

//        return 0;
//    }

    void QGenieReadyShare::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
    {

    }


    void QGenieReadyShare::command(int cmd,QString para0,QString para1)
    {

    }

    void QGenieReadyShare::command(int cmd, int para)
    {

    }

    void *QGenieReadyShare::getPoint(int para)
    {
        return NULL;
    }



//Q_EXPORT_PLUGIN2(GeniePlugin_ReadyShare,QGenieReadyShare)

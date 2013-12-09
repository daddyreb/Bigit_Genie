#include "qgenieparentalcontrol.h"
#include <QtPlugin>

#include <QDir>
#include <QFile>

#include "bbt_paras.h"
#include "LanguageParentalControl.h"
#include "pcpform.h"
#include "IClickManager.h"
#include "version.h"

//#ifdef Q_WS_WIN
//#include <windows.h>
//#include <winbase.h>
//#include <winuser.h>
//#endif
//,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")
#define PLUGIN_INFO "UUID:{ce944b2a-342f-49ca-b66b-b08766ad6599};\
BIND:1;\
          HAVEWND:1;\
              VERSION:%1"


    QGenieParentalControl::QGenieParentalControl()
    {

    }


    QStringList QGenieParentalControl::bigButtonParas()
    {
        //QStringList ret;
        //ret.append("Parental Control:1300");
        //return ret;


        //        L_PARENTAL_CONTROL_ENABLE,
        //L_PARENTAL_CONTROL_NOT_ENABLE
        QStringList ret;
        QString testuiPara="Parental Controls:1300";
        //testuiPara+=":"BBT_PARA_WAITING_TEXT_LEFT"=testdd:"BBT_PARA_FREQUENCY"=10000";
        testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_PARENTAL_CONTROL_BIGBT));
        testuiPara+=QString(":"BBT_PARA_USER_TEXT_RIGHT"=%1").arg(QString::number(L_PARENTAL_CONTROL_NOT_ENABLE));
        testuiPara+=QString(":"BBT_PARA_GOOD_TEXT_RIGHT"=%1").arg(QString::number(L_PARENTAL_CONTROL_ENABLE));
        testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");
        ret.append(testuiPara);

        return ret;

    }


    void QGenieParentalControl::notifyBigButton(QString s)
    {
        //qDebug()<< "GeniePlugin_TestUi::notifyBigButton" << s;

        IClickManager *clickmanager=PCPForm::sHandler->getClickManager();
        if (clickmanager)
        {
            clickmanager->click("f");
        }
    }


    QString QGenieParentalControl::text()
    {
        //return QString("Parental Control");
        return QString("Parental Controls:")+QString::number(L_PARENTAL_CONTROL_BIGBT);
    }

    QWidget* QGenieParentalControl::widget(QWidget *parent)
    {
        return new PCPForm(parent);
    }

    QIcon * QGenieParentalControl::icon()
    {
        return NULL;
    }

    QString QGenieParentalControl::getInfo(QString para)
    {

		if(PLUGIN_INF_LANGUAGE_FILE == para.toInt()) //// zhouklansman 20130509 modify
        {
            return "LanguageParentalControl.txt";
        }

		if(PLUGIN_INF_CREATE_DATE==para.toInt()) // zhouklansman 20130509 modify
        {
            return VERSTION_STR_DATE;
        }

        return "";
    }


    int QGenieParentalControl::changelanguage(int idx)
    {
        return 0;
    }

    QString  QGenieParentalControl::info()
    {
        return QString(PLUGIN_INFO).arg(VERSION_STRING);
    }

    int QGenieParentalControl::priority()
    {
        return 1300;
    }


    void QGenieParentalControl::setHandler(IManagerHandler *hdl)
    {
        PCPForm::sHandler=hdl;
    }

    void QGenieParentalControl::startup(bool mini)
    {
        //
    }

    QGenieParentalControl::~QGenieParentalControl()
    {
        //qDebug("GeniePluginTest::~GeniePluginTest()");
    }

    void QGenieParentalControl::notifyRaise()
    {

    }

    void QGenieParentalControl::notifyLower()
    {

    }

    int QGenieParentalControl::getStatus(QString notifyPara)
  //int QGenieParentalControl::getStatus()
    {

        QString filePath;

        QString filePath32;
        QString filePath64;

        filePath.clear();
//
//        #ifdef Q_WS_WIN
//            int len =0;
//        wchar_t szDstFile[MAX_PATH]={0};
//        GetSystemDirectory(szDstFile,MAX_PATH);
//            for(int i = 0; i<MAX_PATH; i++)
//            {
//                    if(szDstFile[i] == 0)
//                    {
//                            len = i;
//                            break;
//                    }
//            }
//            filePath = QString::fromWCharArray(szDstFile,len);
//            filePath.trimmed();
//
//        #endif

        filePath = QDir::rootPath();
        filePath.trimmed();
        if (filePath.length()>0)
        {
            filePath = filePath.left(filePath.indexOf("/",0));
            filePath32 =filePath + "/Program Files/NETGEAR Live Parental Controls Management Utility";
            filePath64 =filePath + "/Program Files (x86)/NETGEAR Live Parental Controls Management Utility";

        }
        else
        {
            return 4;
        }

        QDir dir(filePath32);
        QDir dir64(filePath64);

        if(dir.exists() || dir64.exists() )
        {
            return 0;
        }
        else
        {
             return 4;
        }
        return 4;
    }

    void QGenieParentalControl::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
    {

    }


    void QGenieParentalControl::command(QString cmd,QString para0,QString para1)
    {

    }

    void QGenieParentalControl::command(QString cmd, int para)
    {

    }

    void *QGenieParentalControl::getPoint(QString para)
    {
        return NULL;
    }



Q_EXPORT_PLUGIN2(GeniePlugin_ParentalControl,QGenieParentalControl)

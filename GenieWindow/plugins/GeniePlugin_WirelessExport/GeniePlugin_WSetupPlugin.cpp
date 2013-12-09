#include "GeniePlugin_WSetupPlugin.h"
#include "QGenieWirelessSetupWizard.h"
#include "version.h"
#include "genie2_interface.h"

#include <QtPlugin>



QString GeniePlugin_WSetupPlugin::m_s_ssid;
QString GeniePlugin_WSetupPlugin::m_s_pwd;
QString GeniePlugin_WSetupPlugin::m_s_auth;
QString GeniePlugin_WSetupPlugin::m_s_cipher;

GeniePlugin_WSetupPlugin::GeniePlugin_WSetupPlugin():
        m_mainExportWizard(0)
        ,m_guestExportWizard(0)
{

}

/*void GeniePlugin_WSetupPlugin::command(int cmd,QString para0,QString para1)
{
    GeniePlugin_WSetupPlugin::output_log("wlan export command");
    if(INTO_CMD_ROUTECONFIG_EXPORT_PASSWORD == cmd)
    {
        m_s_ssid = para0;
        m_s_pwd = para1;
        GeniePlugin_WSetupPlugin::output_log("command==setpwd ssid:"+
                                             m_s_ssid+" pwd:"+m_s_pwd);
    }
    else if(INFO_CMD_ROUTECONFIG_EXPORT_AUTHOR == cmd)
    {
        m_s_auth    = para0;
        m_s_cipher  = para1;
        GeniePlugin_WSetupPlugin::output_log("command==setauth auth:"+
                                             m_s_auth+" cipher:"+m_s_cipher);
    }
}*/

QVariant GeniePlugin_WSetupPlugin::command(int cmd,const QVariant &paras)
{
    if(cmd == I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd == I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd == I2CMD_TIMETO_CREATEUI)
    {
        GENIE2_REGISTER_UI(new QGenieWirelessSetupWizard(),QString("1600:") + QString::number(L_WIRELESSEXPORT_BIGBT) + ":WirelessExport:cmd1");
    }
    else if(cmd == I2CMD_TIMETO_START)
    {
    }
    else if(cmd ==I2CMD_UI_ACTION)
    {
        qDebug()<<"ui action";
    }
    else if(cmd == EPCC_ROUTERCONFIGUI){
        bool bok = false;
        int iPara = paras.toInt(&bok);
        QWidget *resultWidget = 0;
        if(bok){
            if(iPara){
                resultWidget = m_guestExportWizard = new QGenieWirelessSetupWizard();

            }else{
                resultWidget = m_mainExportWizard = new QGenieWirelessSetupWizard();

            }
        }

        if(resultWidget){
            QVariant value;
            value.setValue((void*)resultWidget);
            return value;
        }
    }
    else if(cmd == EPCC_WIRELESSEXPORT_DATA){
        QList<QVariant> paraList = paras.toList();
        if(paraList.size() != 5){
            return QVariant();
        }
        bool bok = false;
        int guestExport = paraList[0].toInt(&bok);

        if(bok){
            QString ssid = paraList[1].toString();
            QString pwd = paraList[2].toString();
            QString auth = paraList[3].toString();
            QString cipher = paraList[4].toString();

            if(guestExport){
                if(m_guestExportWizard){
                    m_guestExportWizard->SetExportData(ssid, pwd, auth, cipher);
                }
            }else{
                if(m_mainExportWizard){
                    m_mainExportWizard->SetExportData(ssid, pwd, auth, cipher);
                }
            }
        }

    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
    }
    else if(cmd ==EPCC_TURNTO_EXPORT)
    {
        if(m_mainExportWizard)
        {
            m_mainExportWizard->init();
        }
    }
    return QVariant();
}


QString GeniePlugin_WSetupPlugin::translateText(int idx)
{
    return GENIE2_GET_TEXT(idx);
}

void GeniePlugin_WSetupPlugin::fun_statistics(const QString & fun)
{
    GENIE2_STATISTICS(fun);
}

void GeniePlugin_WSetupPlugin::output_log(const QString &log)
{
    GENIE2_LOG(log);
}


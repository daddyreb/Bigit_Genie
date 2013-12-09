#include "GeniePlugin_SmartWifi.h"
#include "SmartWifiWidget.h"
#include "language_SmartWifi.h"
#include "version.h"

GeniePlugin_SmartWifi::GeniePlugin_SmartWifi()
{
}

GeniePlugin_SmartWifi::~GeniePlugin_SmartWifi()
{

}

QVariant GeniePlugin_SmartWifi::command(int cmd,const QVariant &paras)
{
    if(cmd == I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd == I2CMD_VERSIONNUMBER){
        return QString(VERSION_STRING);
    }
    else if(cmd == I2CMD_TIMETO_CREATEUI){
        w = new SmartWifiWidget();
        GENIE2_REGISTER_UI(w, QString("2000:") + QString::number(L_SMARTWIFI_BIGBT));
    }
    return QVariant();
}

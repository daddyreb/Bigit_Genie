#ifndef _GENIEPLUGIN_SMARTWIFI_H_
#define _GENIEPLUGIN_SMARTWIFI_H_

#include <QObject>
#include "genie2_interface.h"

class SmartWifiWidget;

class GeniePlugin_SmartWifi : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_SmartWifi.json")
public:
    GeniePlugin_SmartWifi();
    ~GeniePlugin_SmartWifi();
    QVariant command(int cmd, const QVariant &paras);

    SmartWifiWidget *w;
};

#endif

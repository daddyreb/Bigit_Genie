#ifndef GeniePlugin_AIRPRINT_H
#define GeniePlugin_AIRPRINT_H

#include <QObject>
#include "airprintwidget.h"
#include "genie2_interface.h"


class GeniePlugin_Airprint : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Airprint.json")
public:
    GeniePlugin_Airprint();
    ~GeniePlugin_Airprint();
    QVariant command(int cmd,const QVariant &paras);

     AirprintWidget *w;




};

#endif // GeniePlugin_AIRPRINT_H

#ifndef GeniePlugin_Resource_H
#define GeniePlugin_Resource_H

#include <QObject>
#include "genie2_interface.h"


class GeniePlugin_Resource : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Resource.json")
public:
    GeniePlugin_Resource();
    virtual ~GeniePlugin_Resource(){}
public:
    virtual QVariant command(int cmd,const QVariant &paras);


};

#endif // GeniePlugin_Resource_H

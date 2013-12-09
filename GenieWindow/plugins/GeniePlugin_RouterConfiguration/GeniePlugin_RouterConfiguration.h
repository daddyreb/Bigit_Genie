#ifndef GeniePlugin_RouterConfiguration_H
#define GeniePlugin_RouterConfiguration_H

#include <QObject>
#include "genie2_interface.h"
class QGenieRCWirelessObject;
class QGenieGaccObject;
class QGenieDelayProcessLogic;
class QGenieTrafficMeterObject;
class QGenieFupdateObject;
class QGenieRCTopologyObject;
class QGenieQosObject;
class QGenieRebootObject;

class GeniePlugin_RouterConfiguration : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_RouterConiguration.json")
public:
    GeniePlugin_RouterConfiguration();
    virtual ~GeniePlugin_RouterConfiguration();
public:
    virtual QVariant command(int cmd,const QVariant &paras);
protected:
    QGenieRCWirelessObject *mObj;
    QGenieGaccObject *mGaccObj;
    QGenieFupdateObject *mFupObj;
    QGenieDelayProcessLogic *mDelayProcessLogic;
    QGenieTrafficMeterObject *mTMObj;
    QGenieRCTopologyObject *mTpObj;
	QGenieQosObject *mQosObj; //kai.yan add
    QGenieRebootObject* mRebootObj;
};

#endif // GeniePlugin_RouterConfiguration_H

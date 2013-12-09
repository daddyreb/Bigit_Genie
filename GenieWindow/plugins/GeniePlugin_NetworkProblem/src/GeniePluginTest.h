#ifndef GENIEPLUGINTEST_H
#define GENIEPLUGINTEST_H
#include <QObject>
#include "genie2_interface.h"

class GeniePlugin_NetWorkProblem : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_NetworkProblem.json")
public:
    GeniePlugin_NetWorkProblem();
    virtual ~GeniePlugin_NetWorkProblem(){}
public:
    virtual QVariant command(int cmd,const QVariant &paras);
};
#endif // GeniePluginTest_H

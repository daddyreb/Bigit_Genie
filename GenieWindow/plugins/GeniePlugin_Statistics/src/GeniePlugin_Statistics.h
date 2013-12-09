#ifndef GENIEPLUGIN_TESTUI_H
#define GENIEPLUGIN_TESTUI_H

#include <QObject>
#include "genie2_interface.h"

class QGenieDataContainner;
class QGenieStaticInterface;

#define UUID_OF_THIS "{6b864d58-7d53-489d-a558-1684e2fb2f60}"

class GeniePlugin_Statistics : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Statistics.json")
public:
    GeniePlugin_Statistics();
    virtual ~GeniePlugin_Statistics();

public:
    virtual QVariant command(int cmd, const QVariant &paras);

protected:
    QGenieDataContainner *mContainner;
    QGenieStaticInterface *mInterface;
};

#endif // GeniePlugin_TestUi_H

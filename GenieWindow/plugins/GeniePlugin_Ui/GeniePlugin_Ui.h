#ifndef GeniePlugin_Ui_H
#define GeniePlugin_Ui_H

#include <QObject>
#include "genie2_interface.h"
class QGenieUiObject;
enum UI_FLAGS
{
    UIF_HAVE_MODEL=(1<<0)
    ,UIF_RC_BUSY=(1<<1)
};

class GeniePlugin_Ui : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Ui.json")
public:
    GeniePlugin_Ui();
    virtual ~GeniePlugin_Ui();
public:
    virtual QVariant command(int cmd,const QVariant &paras);


    QGenieUiObject *mUiObj;


};

#endif // GeniePlugin_Ui_H

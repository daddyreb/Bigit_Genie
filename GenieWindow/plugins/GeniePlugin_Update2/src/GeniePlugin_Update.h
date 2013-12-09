#ifndef GeniePlugin_Update_H
#define GeniePlugin_Update_H

#include <QObject>
#include "genie2_interface.h"
class QGenieUpdateObject;

//class MainWindow;  //Test use

class GeniePlugin_Update : public QObject, public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Update.json")
public:
    GeniePlugin_Update();
	virtual ~GeniePlugin_Update();
public:
    virtual QVariant command(int cmd, const QVariant &paras);
protected:
    QGenieUpdateObject *mSoftObj;

    //MainWindow *mFirmwareUpdate;
};

#endif // GeniePlugin_Update_H

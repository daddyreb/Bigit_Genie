#ifndef __IPLUGININF2_H
#define __IPLUGININF2_H
#include <QtCore>
class IPluginInf2
{
public:
    virtual QVariant command(int cmd,const QVariant &paras=QVariant())=0;
};
Q_DECLARE_INTERFACE(IPluginInf2,"com.netgear.GeniePlugin.IPluginInf2/1.0")
#endif // IPluginInf_H

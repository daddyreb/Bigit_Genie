#ifndef GeniePlugin_Test_H
#define GeniePlugin_Test_H

#include <QObject>
#include "genie2_interface.h"
class QGenieTestForm;

class GeniePlugin_Test : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
public:
    GeniePlugin_Test();
    virtual ~GeniePlugin_Test();
public:
    virtual QVariant command(int cmd,const QVariant &paras);

    QGenieTestForm *mTestForm;
};

#endif // GeniePlugin_Test_H

#ifndef QGENIEFEEDBACKPLUGIN_H
#define QGENIEFEEDBACKPLUGIN_H

#include <QObject>
#include "genie2_interface.h"
class QGenieTestForm;

class QGenieFeedBackPlugin : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
public:
    QGenieFeedBackPlugin();
    virtual ~QGenieFeedBackPlugin(){}
public:
    virtual QVariant command(int cmd,const QVariant &paras);

    QGenieTestForm *mTestForm;
};
#endif // QGENIEFEEDBACKPLUGIN_H

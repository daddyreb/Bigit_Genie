#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

//#include <QtPlugin>
#include <QtCore>
#include <QObject>

#include "QGenieBasePluginLib.h"
class TestPlugin:public QGenieBasePluginLib
{
   Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    TestPlugin();
    virtual ~TestPlugin();
};

#endif // TESTPLUGIN_H

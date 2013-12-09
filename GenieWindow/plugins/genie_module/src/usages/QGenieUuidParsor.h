#ifndef QGeniePluginParsor_H
#define QGeniePluginParsor_H

class PlgMsg;
class IPluginInf;
#include <QUuid>
#include <QString>
class QGeniePluginParsor
{
public:
    QGeniePluginParsor();
    bool parsePlugin(IPluginInf* pi, PlgMsg *msg);
    QString mResult;
    void cleanup();
protected:
    bool parseUuid(QString s,QUuid *uuid);
    bool parseBind(QString s,bool *bind);
    bool parseHaveWnd(QString s,bool *haveWnd);
    bool parseVersion(QString s,QString *version);
};

#endif // QGeniePluginParsor_H

#ifndef QGENIEINTERFACETOYMANAGER_H
#define QGENIEINTERFACETOYMANAGER_H


class QGenieInterfaceToyManager:public Singleton<QGenieInterfaceToyManager>
        ,public IToyManager
{
public:
    QGenieInterfaceToyManager();
    virtual void setCommand(QString cmd);
};

#endif // QGENIEINTERFACETOYMANAGER_H

#ifndef QGENIEINTERFACESOAPMANAGER_H
#define QGENIEINTERFACESOAPMANAGER_H

class NetgearSoapAPI;

class QGenieInterfaceSoapManager:public ISoapManager,public Singleton<QGenieInterfaceSoapManager>
{
public:
    QGenieInterfaceSoapManager();
    ~QGenieInterfaceSoapManager();
    NetgearSoapSchedule *getSoapAPI();
private:
    NetgearSoapSchedule *Instance();
    NetgearSoapSchedule * schedule;
    friend class QGenieTrayManager;
};

#endif // QGENIEINTERFACESOAPMANAGER_H

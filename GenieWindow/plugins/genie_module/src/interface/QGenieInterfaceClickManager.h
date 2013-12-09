#ifndef QGENIEINTERFACECLICKMANAGER_H
#define QGENIEINTERFACECLICKMANAGER_H

#define ADD_CLICK(s) do{\
    QGenieInterfaceClickManager *clkm=QGenieInterfaceClickManager::GetSingletonPtr();\
    if(clkm){\
        clkm->click(s);\
    }\
}while(0)

#define ADD_SEARCH(s) do{\
    QGenieInterfaceClickManager *clkm=QGenieInterfaceClickManager::GetSingletonPtr();\
    if(clkm){\
        clkm->rememberSearchStr(s);\
    }\
}while(0)

class QGenieInterfaceClickManager:public IClickManager
        ,public Singleton<QGenieInterfaceClickManager>
{
public:
    QGenieInterfaceClickManager();
    virtual void click(const QString &key);	//when button from plugin clicked,can this to let
    virtual void registMaxValue(const QString &key,int v);
    virtual void rememberSearchStr(const QString &s);
    void notifySaveData();
protected:
//    int mCount;
    IPluginInf *getStatisticPi();

};


#endif // QGENIEINTERFACECLICKMANAGER_H

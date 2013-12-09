#ifndef QGenieInterfaceMobileManager_H
#define QGenieInterfaceMobileManager_H



class QGenieInterfaceMobileManager:public IMobileManager,public Singleton<QGenieInterfaceMobileManager>
{
public:
    QGenieInterfaceMobileManager();
    ~QGenieInterfaceMobileManager();
    virtual void * get(const QString &name);
    virtual void set(const QString &name,void *);
};

#endif // QGenieInterfaceMobileManager_H

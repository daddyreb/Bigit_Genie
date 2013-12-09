#ifndef NETGEARSOAP_H
#define NETGEARSOAP_H

#include <QObject>
#include <QMap>

#include "IManagerHandler.h"
#include "ISoapManager.h"
#include "netgearsoapschedule.h"


class NetgearSoap : public QObject
{
    Q_OBJECT

public:
    NetgearSoap();
    ~NetgearSoap();

    void getMacAddr(const QString &para);


private slots:
    void dealMACData(QtSoapMessage &data, QUuid &uuid, QString &id);

signals:
    void GetMACFinish(QString &mMAC);


public:
    static IManagerHandler *sHandler;

private:
    ISoapManager *mSoapManager;
    NetgearSoapSchedule *mSoapApi;
    QUuid mUuid;
    QString getID;
    int intID;
    bool setSoapFlag;

    QString mac_addr;

};




#endif // NETGEARMACFROMSOAP_H

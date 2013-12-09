#ifndef WIFIUTIL_H
#define WIFIUTIL_H

#include <QString>
#include <QObject>

class ExtenderUtilWiFi : public QObject
{
    Q_OBJECT
public:
    explicit ExtenderUtilWiFi(QObject *parent = 0);
    ~ExtenderUtilWiFi();

public:
    // get the name of NIC
    QString getWiFiNicUsable();

    // connect the WLAN: NETGEAR_EXT
    bool ConnectInitExtender();

    // connect WLAN of given
    bool connectToWlan(QString &SSID, QString &password);

    bool checkWifiConnected();

    // get the wlan gateway
    bool getInterfaceGateway();

    // get the IP
    bool getIP(QString &);

    bool isSSIDConnected(QString &SSID);
};

#endif // WIFIUTIL_H

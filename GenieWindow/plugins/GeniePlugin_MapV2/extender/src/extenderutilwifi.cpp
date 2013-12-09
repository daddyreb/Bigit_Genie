#include <QDebug>
#include <QPoint>
#include <QPluginLoader>
#include <QDir>
#include <QCoreApplication>
#include <QHostInfo>

#include "extenderutilwifi.h"

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
DiagnoseInterface *diagnose_inf = 0;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
WiFiApiInterface *wifiApi_inf = 0;
#endif

extern QString g_host;

ExtenderUtilWiFi::ExtenderUtilWiFi(QObject *parent) :
    QObject(parent)
{

#ifdef Q_OS_WIN
    QString dllName = qApp->applicationDirPath() + "/DiagnosePlugin.dll";
    qDebug() << dllName;
    QPluginLoader loader(dllName);

    diagnose_inf = qobject_cast<DiagnoseInterface*>(loader.instance());
    qDebug() << "DiagnosePlugin.dll loader: " << loader.isLoaded();

#elif defined Q_OS_MACX
    QString app_dir_path = qApp->applicationDirPath();
    QDir plugins_dir(app_dir_path);
    QPluginLoader loader(plugins_dir.absolutePath() + "/libWiFiApiPlugin.dylib");
    wifiApi_inf = qobject_cast<WiFiApiInterface*>(loader.instance());
    if(wifiApi_inf)
        qDebug() << "DiagnosePlugin.dll loader" << loader.isLoaded();
    else
        qDebug() << "Open the libWiFiApiPlugin.dylib error.";
#endif

}

ExtenderUtilWiFi::~ExtenderUtilWiFi()
{

}

QString ExtenderUtilWiFi::getWiFiNicUsable()
{
    QString nicguid;
    QStringList  nic_list;

    bool bHaveNIC = false;

#ifdef Q_OS_WIN
    bHaveNIC = (diagnose_inf != 0) && diagnose_inf->GetWiFiInterfaces(nic_list);
#elif defined Q_OS_MACX
    bHaveNIC = (wifiApi_inf != 0) && wifiApi_inf->AllWLanInterfaces(nic_list);
#endif

    if (bHaveNIC) {
        for(int y = 0 ; y < nic_list.size() ; ++y) {
            bool bIsNICConnected = false;
#ifdef Q_OS_WIN
            bIsNICConnected = (diagnose_inf->IsWlanNicConnected(nic_list[y]) >= 0);
#elif defined Q_OS_MACX
            bIsNICConnected = wifiApi_inf->IsWlanNicActive(nic_list[y]);
#endif
            if(bIsNICConnected && (y != 0)) {
                nic_list.swap(0,y);
                break;
            }
        }
    }

    if(nic_list.size() > 0) {
        nicguid = nic_list[0];
    }

    //qDebug() << "Interfacename = " << nicguid;

    return nicguid;
}

bool ExtenderUtilWiFi::ConnectInitExtender()
{
    QString strNicName = getWiFiNicUsable();
    qApp->processEvents();
#ifdef Q_OS_WIN
    return diagnose_inf->ConnectToWlan(strNicName, "NETGEAR_EXT", "", 0);
#elif defined Q_OS_MACX
    return wifiApi_inf->AssociateToNetwork(strNicName, "NETGEAR_EXT", "");
#endif
}

bool ExtenderUtilWiFi::connectToWlan(QString &ssid, QString &pwd)
{
    qDebug() << "start connect the wlan";
    QString strNicName = getWiFiNicUsable();
    qApp->processEvents();
#ifdef Q_OS_WIN
    return diagnose_inf->ConnectToWlan(strNicName, ssid, pwd, 0);
#elif defined Q_OS_MACX
    return wifiApi_inf->AssociateToNetwork(strNicName, ssid, pwd, "");
#endif
}

bool ExtenderUtilWiFi::checkWifiConnected()
{
    QString strNicName;
    strNicName = getWiFiNicUsable();
#ifdef Q_OS_WIN
    return diagnose_inf->IsWlanNicConnected(strNicName);
#elif defined Q_OS_MACX
    return wifiApi_inf->IsWLanNicConnected(strNicName);
#endif
}

bool ExtenderUtilWiFi::getInterfaceGateway()
{
    QString strNicName;
    QStringList getwayList;

    strNicName = getWiFiNicUsable();

    bool bHaveGateway = false;
#ifdef Q_OS_WIN
    bHaveGateway = diagnose_inf->GetInterfaceGateway(strNicName, getwayList);
#elif defined Q_OS_MACS
    //
#endif

    if (bHaveGateway) {
        bool bGatewayOK = false;
        for (int i = 0; i < getwayList.size(); ++i) {
            bGatewayOK = diagnose_inf->IsNicGatewaysOk(strNicName, 0);
            if (bGatewayOK) {
                //g_host = getwayList.at(i);
                return true;
            }
        }
    }

    return false;
}

bool ExtenderUtilWiFi::getIP(QString &hostName)
{
    QHostInfo info = QHostInfo::fromName(hostName);
    if (!info.addresses().isEmpty()) {
        QString ip = info.addresses().first().toString();
        g_host = ip;
        return true;
    } else {
        return false;
    }
}

bool ExtenderUtilWiFi::isSSIDConnected(QString &SSID)
{
    return diagnose_inf->IsSsidConnected(SSID);
}

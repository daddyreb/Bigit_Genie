#include "netgearsoapapi.h"
#include "networktransactionmanager.h"
#include "soapreturnhandler.h"
#include "helperroutines.h"

#include <QtDebug>

#define SOAP_DEFAULT_TIMEOUT_MS 20

#define INIT_SOAP_RETURNHANDLER \
    SoapReturnHandler soapReturnHandler; \
    m_thisThreadLocker.lock(); \
    soapReturnHandler.moveToThread(this); \
    m_thisThreadLocker.unlock();

#define SOAP_SUBMIT_GUARD(statement) \
    m_initMutex.lock(); \
    if(!m_initCompleted){ \
    m_initMutex.unlock(); \
    return SOAP_NOT_INITIALIZED; \
    } \
    (statement); \
    m_initMutex.unlock();


NETGEARSoapApi::NETGEARSoapApi(QObject *parent) :
    QThread(parent)
{
    m_initMutex.lock();
    m_initCompleted = false;
    m_initMutex.unlock();
}

NETGEARSoapApi::~NETGEARSoapApi()
{
    Stop();
}

bool NETGEARSoapApi::Start()
{
    m_initMutex.lock();

    m_thisThreadLocker.lock();
    this->start();
    m_thisThreadLocker.unlock();

    while(!m_initCompleted){
        m_initCdt.wait(&m_initMutex);
    }
    m_initMutex.unlock();

    return true;
}

void NETGEARSoapApi::Stop()
{
    m_initMutex.lock();

    m_thisThreadLocker.lock();
    this->quit();
    this->wait();
    m_thisThreadLocker.unlock();

    m_initCompleted = false;
    m_initMutex.unlock();
}

void NETGEARSoapApi::SubmitTransaction(const QString &host,
                                       quint16 port,
                                       const QString &module,
                                       const QString &method,
                                       const QMap<QString, QString> *param,
                                       const QObject *resultReciver,
                                       const char *slot,
                                       int timeoutSeconds)
{
    emit SubmitTransactionSignal(host, port, module, method, param, resultReciver, slot, timeoutSeconds);
}

void NETGEARSoapApi::SubmitTransaction(const QString &module,
                                       const QString &method,
                                       const QMap<QString, QString> *param,
                                       const QObject *resultReciver,
                                       const char *slot,
                                       int timeoutSeconds)
{
    emit SubmitTransactionSignal(module, method, param, resultReciver, slot, timeoutSeconds);
}

void NETGEARSoapApi::run()
{
    NetworkTransactionManager netTransactionManager;

    connect(this, SIGNAL(SubmitTransactionSignal(QString,quint16,QString,QString,const QMap<QString,QString>*,const QObject*,const char*,int)),
            &netTransactionManager, SLOT(SubmitTransaction(QString,quint16,QString,QString,const QMap<QString,QString>*,const QObject*,const char*,int)));

    connect(this, SIGNAL(SubmitTransactionSignal(QString,QString,const QMap<QString,QString>*,const QObject *,const char*,int)),
            &netTransactionManager, SLOT(SubmitTransaction(QString,QString,const QMap<QString,QString>*,const QObject *,const char*,int)));

    m_initMutex.lock();
    m_initCompleted = true;
    m_initCdt.wakeAll();
    m_initMutex.unlock();

    exec();

    qDebug() << "NETGEARSoapApi thread exit";
}

bool NETGEARSoapApi::InitAttachDeviceEntry(const QStringList &devComponents, AttachDeviceEntry &adentry)
{
    adentry.m_devIP.clear();
    adentry.m_devName.clear();
    adentry.m_devMac.clear();
    adentry.m_isWired = true;
    adentry.m_linkSpeed = 0;
    adentry.m_signalStrength = 0;
    adentry.m_allowOrBlock = true;

    if(devComponents.count() > 3){
        if(IsMacAddrValid(devComponents[3].trimmed())){
            adentry.m_devMac = NormalizeValidMacAddrFormat(devComponents[3].trimmed());
        }else{
            return false;
        }
    }

    if(devComponents.count() >= 2){
        adentry.m_devIP = devComponents[1].trimmed();
    }

    if(devComponents.count() >= 3){
        adentry.m_devName = devComponents[2].trimmed();
    }

    if(devComponents.count() == 5){
        QStringList typeList;
        typeList << "wired" << "wireless";
        if(!typeList.contains(devComponents[4], Qt::CaseInsensitive)){
            return false;
        }
        adentry.m_isWired = devComponents[4].contains("wired",Qt::CaseInsensitive);
    }

    if(devComponents.count() >= 6){
        adentry.m_linkSpeed = devComponents[5].trimmed().toInt();
    }

    if(devComponents.count() >= 7){
        adentry.m_signalStrength = devComponents[6].trimmed().toInt();
    }

    if(devComponents.count() >= 8){
        adentry.m_allowOrBlock = devComponents[7].contains("Allow",Qt::CaseInsensitive);
    }

    return true;
}

int NETGEARSoapApi::GetAttachDevice(const QString &host, quint16 port, QList<AttachDeviceEntry> &attachDevs)
{
    INIT_SOAP_RETURNHANDLER;

    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "DeviceInfo", "GetAttachDevice", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK == soapCode){
        attachDevs.clear();

        //qDebug() << "NETGEARSoapApi GetAttachDevice Result:" << *resultData;
        QString newAttachDevice = (*resultData)["NewAttachDevice"];
        QStringList parts = newAttachDevice.split("@");
        int devCount = parts.count();
        if(devCount >= 2){
            for(int idx = 1 ; idx < devCount ; ++idx){
                QStringList devComponents = parts[idx].split(";");
                if(devComponents.count() >= 2){
                    AttachDeviceEntry adentry;
                    if(InitAttachDeviceEntry(devComponents, adentry)){
                        attachDevs.append(adentry);
                    }
                }
            }
        }
    }else{
        qDebug() << "NETGEARSoapApi GetAttachDevice Soap return failed!!!";
    }

    delete resultData;

    return soapCode;
}


int NETGEARSoapApi::GetUSBType(const QString &host, quint16 port, QStringList &usbTypes)
{
    INIT_SOAP_RETURNHANDLER;


    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "USBPortInfo", "GetUSBType", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)), SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK == soapCode){
        //qDebug() << "NETGEARSoapApi GetUSBType Result:" << *resultData;
        QString usbTypeList = (*resultData)["USBTypeList"];
        QStringList parts = usbTypeList .split("@");
        if(parts.count() >= 2){
            parts = parts[1].split(";");
            int usbCount = parts.count();
            for(int idx = 0 ; idx < usbCount ; ++idx){
                usbTypes.append(parts[idx].trimmed());
            }
        }
    }

    delete resultData;

    return soapCode;
}

int NETGEARSoapApi::GetUSBDesc(const QString &host, quint16 port, QStringList &usbDescs)
{
    INIT_SOAP_RETURNHANDLER;

    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "USBPortInfo", "GetUSBDesc", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)), SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK == soapCode){
        //qDebug() << "NETGEARSoapApi GetUSBDesc Result:" << *resultData;
        QString usbDescList = (*resultData)["USBDescList"];
        QStringList parts = usbDescList .split("@");
        if(parts.count() >= 2){
            parts = parts[1].split(";");
            int usbCount = parts.count();
            for(int idx = 0 ; idx < usbCount ; ++idx){
                usbDescs.append(parts[idx].trimmed());
            }
        }
    }

    delete resultData;

    return soapCode;
}

int NETGEARSoapApi::GetWPSConfigureState(const QString &host ,quint16 port, bool &configured)
{
    configured = false;

    INIT_SOAP_RETURNHANDLER;

    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "WLANConfiguration", "GetWPSConfigureState", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)), SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK == soapCode){
        //qDebug() << "NETGEARSoapApi GetWPSConfigureState Result:" << *resultData;
        QString wpsState = (*resultData)["NewWPSConfigureState"];
        configured = (wpsState.trimmed() != "0");
    }

    delete resultData;

    return soapCode;
}

int NETGEARSoapApi::GetWLANInfo(const QString &host, quint16 port, WLANInfo &wlanInfo)
{
    INIT_SOAP_RETURNHANDLER;

    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "WLANConfiguration", "GetInfo", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK == soapCode){

        wlanInfo.m_enabled = ((*resultData)["NewEnable"].trimmed() != "0");
        wlanInfo.m_ssidBroadcast = ((*resultData)["NewSSIDBroadcast"].trimmed() != "0");
        wlanInfo.m_statusUp = ((*resultData)["NewStatus"].trimmed().compare("up",Qt::CaseInsensitive) == 0);
        wlanInfo.m_ssid = (*resultData)["NewSSID"].trimmed();
        wlanInfo.m_region = (*resultData)["NewRegion"].trimmed();
        wlanInfo.m_channel = (*resultData)["NewChannel"].trimmed();
        wlanInfo.m_wirelessMode = (*resultData)["NewWirelessMode"].trimmed();
        wlanInfo.m_basicEncryptionModes = (*resultData)["NewBasicEncryptionModes"].trimmed();
        wlanInfo.m_wepAuthType = (*resultData)["NewWEPAuthType"].trimmed();
        wlanInfo.m_wpaEncryptionModes = (*resultData)["NewWPAEncryptionModes"].trimmed();
        wlanInfo.m_wlanMACAddress = (*resultData)["NewWLANMACAddress"].trimmed();

        if(IsMacAddrValid(wlanInfo.m_wlanMACAddress)){
            wlanInfo.m_wlanMACAddress = NormalizeValidMacAddrFormat(wlanInfo.m_wlanMACAddress);
        }else{
            qDebug () << "Soap GetWLANInfo ->WLANMAC is invalid!!!";
        }

        //get password
        delete resultData;
        resultData = 0;

        QStringList wepModes, wpaModes;
        wepModes << "wep" << "wep64" << "wep128";
        wpaModes << "wpa2-psk" << "wpa-psk/wpa2-psk" << "wpa-psk" << "mixed wpa";

        if(wepModes.contains(wlanInfo.m_basicEncryptionModes, Qt::CaseInsensitive)){
            SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "WLANConfiguration", "GetWEPSecurityKeys", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));
            soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);
            if(SOAP_OK == soapCode){
                wlanInfo.m_wlanPassphrase = (*resultData)["NewWEPPassphrase"].trimmed();
                wlanInfo.m_wepKey = (*resultData)["NewWEPKey"].trimmed();
            }else{
                wlanInfo.m_wlanPassphrase.clear();
                qDebug() << "NETGEARSoapApi WLANConfiguration GetWEPSecurityKeys Soap return failed!!!";
            }
        }else if(wpaModes.contains(wlanInfo.m_basicEncryptionModes, Qt::CaseInsensitive)){
            SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "WLANConfiguration", "GetWPASecurityKeys", 0, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));
            soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);
            if(SOAP_OK == soapCode){
                wlanInfo.m_wlanPassphrase = (*resultData)["NewWPAPassphrase"].trimmed();
            }else{
                wlanInfo.m_wlanPassphrase.clear();
                qDebug() << "NETGEARSoapApi WLANConfiguration GetWPASecurityKeys Soap return failed!!!";
            }
        }else{
            wlanInfo.m_wlanPassphrase.clear();
        }

    }else{
        qDebug() << "NETGEARSoapApi WLANConfiguration GetInfo Soap return failed!!!";
    }

    delete resultData;

    return soapCode;
}

int NETGEARSoapApi::SetWLANInfo(const QString &host, quint16 port, const WLANInfo &wlanInfo)
{
    int soapCode = SOAP_UNKNOW_ERROR;
    if((soapCode = ConfigurationStarted(host, port)) != SOAP_OK){
        return soapCode;
    }

    QMap<QString, QString> paras;
    paras["NewSSID"] = wlanInfo.m_ssid;
    paras["NewRegion"] = wlanInfo.m_region;
    paras["NewChannel"] = wlanInfo.m_channel;
    paras["NewWirelessMode"] = wlanInfo.m_wirelessMode;

    QString soapMethod;
    bool parasValid = false;

    if(!wlanInfo.m_basicEncryptionModes.compare("none", Qt::CaseInsensitive)){

        soapMethod = "SetWLANNoSecurity";
        parasValid = true;

    }else if(wlanInfo.m_wpaEncryptionModes.compare("none", Qt::CaseInsensitive)/* &&
             !wlanInfo.m_wlanPassphrase.isEmpty()*/){

        if(!wlanInfo.m_basicEncryptionModes.compare("wep", Qt::CaseInsensitive)){

            if(/*!wlanInfo.m_wlanPassphrase.isEmpty() &&
                    (wlanInfo.m_wlanPassphrase.length() <= 32) &&*/
                    (wlanInfo.m_wepKey.length() == 10 || wlanInfo.m_wepKey.length() == 26)){

                //soapMethod = "SetWLANWEPByPassphrase";
                soapMethod = "SetWLANWEPByKeys";
                paras["NewWEPAuthType"] = wlanInfo.m_wepAuthType;
                paras["NewWEPLength"] = (wlanInfo.m_wepKey.length() == 10 ? "64" : "128");
                //paras["NewWEPPassphrase"] = wlanInfo.m_wlanPassphrase;

                paras["NewKey1"] = wlanInfo.m_wepKey;
                paras["NewKey2"] = wlanInfo.m_wepKey;
                paras["NewKey3"] = wlanInfo.m_wepKey;
                paras["NewKey4"] = wlanInfo.m_wepKey;

                parasValid = true;

            }
        }else if(!wlanInfo.m_wpaEncryptionModes.isEmpty() &&
                 (wlanInfo.m_wlanPassphrase.length() >= 8) &&
                 (wlanInfo.m_wlanPassphrase.length() <= 64)){
            soapMethod = "SetWLANWPAPSKByPassphrase";
            paras["NewWPAEncryptionModes"] = wlanInfo.m_wpaEncryptionModes;
            paras["NewWPAPassphrase"] = wlanInfo.m_wlanPassphrase;
            if(wlanInfo.m_wpaEncryptionModes.contains("WPA-PSK")){
                paras["NewWirelessMode"] = "g & b";//54Mbps
            }

            parasValid = true;
        }

    }

    if(!parasValid){
        qDebug () << "NETGEAR soap api SetWLANInfo paramter not support!!!";
        return SOAP_INVALID_ARGS;
    }

    INIT_SOAP_RETURNHANDLER;
    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "WLANConfiguration", soapMethod, &paras, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);
    delete resultData;

    ConfigurationFinished(host, port);

    if(SOAP_OK != soapCode){
        qDebug () << "NETGEAR soap api SetWLANInfo failed!!!";
    }

    return soapCode;
}

int NETGEARSoapApi::ConfigurationStarted(const QString &host, quint16 port)
{
    QMap<QString, QString> paras;
    paras["NewSessionID"] = "58DEE6006A88A967E89A";

    INIT_SOAP_RETURNHANDLER;
    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "DeviceConfig", "ConfigurationStarted", &paras, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));


    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK != soapCode){
        qDebug () << "NETGEAR soap api ConfigurationStarted failed!!!";
    }

    delete resultData;

    return soapCode;
}

int NETGEARSoapApi::ConfigurationFinished(const QString &host, quint16 port)
{
    QMap<QString, QString> paras;
    paras["NewStatus"] = "ChangesApplied";

    INIT_SOAP_RETURNHANDLER;
    SOAP_SUBMIT_GUARD(SubmitTransaction(host, port, "DeviceConfig", "ConfigurationFinished", &paras, &soapReturnHandler , SLOT(OnSoapReturn(int, const QMap<QString,QString>*)),  SOAP_DEFAULT_TIMEOUT_MS));

    const QMap<QString, QString> *resultData = 0;
    int soapCode = soapReturnHandler.WaitForSoapReturn(&resultData);

    if(SOAP_OK != soapCode){
        qDebug () << "NETGEAR soap api ConfigurationFinished failed!!!";
    }

    delete resultData;

    return soapCode;
}

#include "netscanningcontroller.h"
#include "device.h"
#include "mapdevicetype.h"
#include "mapuitilities.h"

#include <QtAlgorithms>
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QtDebug>
#include <QUiLoader>
#include "networkmapview.h"
#include "topologyview.h"
#include "blockdevicemanager.h"
#include "puttyfuncs.h"

extern quint32 sMapFlags;
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
NetScanningController::NetScanningController(QObject *parent) :
        QObject(parent),
        m_customDataCacheMap(0),
        m_poolNumber(-1),
        m_curAdpterId(-1),
        m_isDataValid(false)
#ifdef POWERLINE_REVISION
        ,m_curStartIdx(-1),
        m_curTotalPowerLineInSubDevsList(0),
        m_curTotalPowerLine(0)
#endif

{
    qRegisterMetaType<Device>("Device");

    connect(&m_scp,SIGNAL(signal_nodeDataReturned(SVT_NotifyType)),
            this,SLOT(onNewData(SVT_NotifyType)));
    connect(&m_scp,SIGNAL(signal_ifaceDataReturned(SVT_NotifyType)),
            this,SLOT(onInfInformation(SVT_NotifyType)));
    connect(&m_scp,SIGNAL(signal_countChanged(int)),
            this,SLOT(onCountChanged(int)));
	//add by caiyujun
	connect(&m_scp,SIGNAL(signal_otherDevCountChanged(QString,int)),
			this,SLOT(onOtherDeviceCountChanged(QString,int)));
	//add end
    connect(&m_scp,SIGNAL(signal_newDeviceAdded(SVT_NotifyType)),
            this,SIGNAL(newDeviceNotify(SVT_NotifyType)));
//    connect(&m_scp,SIGNAL(signal_NetgearInfo(QMap<int,QString>)),
//            this,SLOT(onNetgearInfo(QMap<int,QString>)));
    connect(&m_scp,SIGNAL(signal_gatewayChanged(QString)),this,SIGNAL(gatewayChanged(QString)));
    if(sMapFlags&MF_IN_DBG_MOD)
    {
        GENIE2_REGISTER_DBG_UI(m_scp.getDbgUi());
    }
}

NetScanningController::~NetScanningController()
{
    clearSubscribeDevsList();

    delete m_customDataCacheMap;
}

void NetScanningController::clearSubscribeDevsList()
{
    qDeleteAll(m_subscribeDevsList);
    m_subscribeDevsList.clear();
}

void NetScanningController::resetSubscribeDevsList()
{
    foreach(Device *pdev , m_subscribeDevsList)
    {
        pdev->clear();
    }
}

QString NetScanningController::parseDeviceName(const QMap<int,QString> &data)
{
    if(!data[NIE_LOCALNAME].isEmpty())
    {
        return data[NIE_LOCALNAME];
    }
    else if(!data[NIE_CURRENTSETTING_MODEL].isEmpty())
    {
        return data[NIE_CURRENTSETTING_MODEL];
    }
    else if(!data[NIE_NETBIOS_MACHOSTNAME].isEmpty())
    {
        return data[NIE_NETBIOS_MACHOSTNAME];
    }
    else if(!data[NIE_NETBIOS_HOSTNAME].isEmpty())
    {
        return data[NIE_NETBIOS_HOSTNAME];
    }
    else if(!data[NIE_ATTACHDEVICE_NAME].isEmpty())
    {
        return data[NIE_ATTACHDEVICE_NAME];
    }

    return data[NIE_UPNP_FRIENDLYNAME];
}

int NetScanningController::parseDeviceType(const QMap<int,QString> &data)
{
    int tmpType = MDT_NETWORKDEV;

    if(!data[NIE_LOCALOSVERSION].isEmpty())
    {
        if(getWindowsLocalOSVersionCompositedString().contains(data[NIE_LOCALOSVERSION],Qt::CaseInsensitive))
        {
            return MDT_WINDOWSPC;
        }
    }
    else if(!data[NIE_ISDEFAULT_GATEWAY].compare("y",Qt::CaseInsensitive) ||
            !data[NIE_ISGATEWAY].compare("y",Qt::CaseInsensitive))
    {
        return MDT_ROUTER;
    }

    if(!data[NIE_NETBIOS_MACVERSION].isEmpty())
    {
        if(data[NIE_NETBIOS_MACVERSION].contains("macmini",Qt::CaseInsensitive))
        {
            return MDT_MACMINI;
        }
        else if(data[NIE_NETBIOS_MACVERSION].contains("macbook",Qt::CaseInsensitive))
        {
            return MDT_MACBOOK;
        }
        else if(data[NIE_NETBIOS_MACVERSION].contains("macpro",Qt::CaseInsensitive))
        {
            return MDT_MACPRO;
        }
        else if(data[NIE_NETBIOS_MACVERSION].contains("imac",Qt::CaseInsensitive))
        {
            return MDT_IMAC;
        }
    }

    if(!data[NIE_NETBIOS_VERSION].isEmpty())
    {
        if(data[NIE_NETBIOS_VERSION].contains("windows",Qt::CaseInsensitive))
        {
            return MDT_WINDOWSPC;
        }
    }

    if((tmpType = parseUpnpDeviceType(data)) != MDT_NETWORKDEV)
    {
        return tmpType;
    }

    if(data[NIE_NETBIOS_VERSION].contains("unix",Qt::CaseInsensitive))
    {
        return MDT_UNIXPC;
    }

    return parseDeviceTypeWithAttachDevice(data);
}

int NetScanningController::parseUpnpDeviceType(const QMap<int,QString> &data)
{
    int deviceType = MDT_NETWORKDEV;

    static const char* mediadevices[] =
    {
        "MediaRenderer",
        "MediaServer",
        NULL
    };

    static const char* printers[] =
    {
        "Printer",
        "PrinterAnnexA",
        NULL
    };

    static const char* cameras[] =
    {
        "Basic",
        "DigitalSecurityCamera",
        NULL
    };

    static const char* scanners[] =
    {
        "Scanner",
        NULL
    };

    static const char* routers[] =
    {
        "WANDevice",
        "WANConnectionDevice",
        "InternetGatewayDevice",
        "WLANAccessPointDevice",
        "LANDevice",
        NULL
    };

    static const char* netstoragedevices[] =
    {
        "NetworkStorageDevice",
        NULL
    };

    static const char *powerlinedevices[]={
        "WFADevice"  ,
        NULL
    };


    static const char** upnptypes[] =
    {
        mediadevices,
        printers,
        cameras,
        scanners,
        routers,
        netstoragedevices,
//        powerlinedevices,
        NULL
    };

    static const int devicetypes[] =
    {
        MDT_MEDIADEV,
        MDT_PRINTER,
        MDT_CAMERA,
        MDT_SCANNER,
        MDT_ROUTER,
        MDT_STORAGE,
//        MDT_POWERLINE

    };

    for(int i = 0 ; upnptypes[i] != NULL ; ++i)
    {
        for(int j = 0 ; upnptypes[i][j] != NULL ; ++j)
        {
            if(data[NIE_UPNP_DEVICETYPE].contains(upnptypes[i][j],Qt::CaseInsensitive))
            {
                deviceType = devicetypes[i];
                goto outLabel;
            }
        }
    }

    outLabel:

    return deviceType;
}


int NetScanningController::parseDeviceTypeWithAttachDevice(const QMap<int,QString> &data)
{
    static const char* keypatterns[] =
    {
        ".*(ps|wii|xbox).*",
        ".*ipod.*",
        ".*ipad.*",
        ".*iphone.*",
        ".*macmini.*",
        ".*macpro.*",
        ".*macbook.*",
        ".*imac.*",
        ".*android.*",
        "^wn[0-9]{0,4}(rpt|rp).*",
        "^((WNDR|WNR|DGND|DGN|JWNR|WNDRMAC|WGR|CG|MBRN|WPN|VGN|DEVG|DG)[0-9]{0,4}).*",
        NULL
    };

    static const int devicetypes[] =
    {
        MDT_GAMINGCONSOLE,
        MDT_IPODTOUCH,
        MDT_IPAD,
        MDT_IPHONE,
        MDT_MACMINI,
        MDT_MACPRO,
        MDT_MACBOOK,
        MDT_IMAC,
        MDT_ANDROIDDEVICE,
        MDT_REPEATER,
        MDT_ROUTER
    };

    for(int k = 0 ; keypatterns[k] != NULL ; ++k )
    {
        QRegExp regExp(keypatterns[k],Qt::CaseInsensitive);
        if(regExp.exactMatch(data[NIE_ATTACHDEVICE_NAME]) ||
           regExp.exactMatch(data[NIE_CURRENTSETTING_MODEL]))
        {
            return devicetypes[k];
        }
    }

    return MDT_NETWORKDEV;
}

QString NetScanningController::parseDeviceOS(const QMap<int,QString> &data)
{
    if(!data[NIE_LOCALOSVERSION].isEmpty())
    {
#ifdef Q_OS_WIN
        int idx = getWindowsLocalOSVersionStringList().indexOf(data[NIE_LOCALOSVERSION]);
        if(idx >= 0)
        {
            QStringList osList = getWindowsLocalOSStringList();
            if(idx < osList.size())
            {
                return osList[idx];
            }
        }
#else
        return data[NIE_LOCALOSVERSION];
#endif
    }

    if(!data[NIE_NETBIOS_VERSION].isEmpty())
    {
        if(data[NIE_NETBIOS_VERSION].contains("windows",Qt::CaseInsensitive))
        {
            if(data[NIE_NETBIOS_VERSION].contains("5.1"))
            {
                return "Windows XP";
            }
            else if(data[NIE_NETBIOS_VERSION].contains("6.0"))
            {
                return "Windows Vista";
            }
            else if(data[NIE_NETBIOS_VERSION].contains("6.1"))
            {
                return "Windows 7";
            }
            else
            {
                return data[NIE_NETBIOS_VERSION];
            }
        }
        else if(data[NIE_NETBIOS_VERSION].contains("mac",Qt::CaseInsensitive))
        {
            return "Mac OS X";
        }
    }

    if(!data[NIE_NETBIOS_MACVERSION].isEmpty())
    {
        return "Mac OS X";
    }

    return QString();
}


int NetScanningController::parseDeivceSignalStrenth(const QMap<int,QString> &data)
{
    if(data.contains(NIE_ATTACHDEVICE_SIGNALSTRENGTH))
    {
        bool bok = false;
        int tmp = data[NIE_ATTACHDEVICE_SIGNALSTRENGTH].toInt(&bok);
        return (bok ? tmp : -1);
    }

    return -1;
}

bool NetScanningController::parseDeivceWiredProperty(const QMap<int,QString> &data)
{
    if(data[NIE_ISHOST].compare("y",Qt::CaseInsensitive) && !data[NIE_ATTACHDEVICE_ISWIRED].isEmpty())
    {
        return (data[NIE_ATTACHDEVICE_ISWIRED].compare("y",Qt::CaseInsensitive) == 0);
    }

    return (data[NIE_ISWIRED].compare("y",Qt::CaseInsensitive) == 0);
}

QString NetScanningController::loadCustomName(const QString &key)
{
    QString result;

    if((m_customDataCacheMap || loadCustomDataCacheMap()) &&
       m_customDataCacheMap->contains(key))
    {
        if((*m_customDataCacheMap)[key].size() >0)
        {
            result = (*m_customDataCacheMap)[key][0];
        }
    }

    return result;
}

int NetScanningController::loadCustomDeviceType(const QString &key)
{
    int result = MDT_DEVICETYPE_COUNT;

    if((m_customDataCacheMap || loadCustomDataCacheMap()) &&
       m_customDataCacheMap->contains(key))
    {
        if((*m_customDataCacheMap)[key].size() >1)
        {
            bool bok = false;
            result = (*m_customDataCacheMap)[key][1].toInt(&bok);
            result = ((!bok || result == -1) ? MDT_DEVICETYPE_COUNT : result);
        }
    }

    return result;
}

int NetScanningController::loadCustomConnectionType(const QString &key)
{
    int result = MCT_COUNT;

    if((m_customDataCacheMap || loadCustomDataCacheMap()) &&
       m_customDataCacheMap->contains(key))
    {
        if((*m_customDataCacheMap)[key].size() >2)
        {
            bool bok = false;
            result = (*m_customDataCacheMap)[key][2].toInt(&bok);
            result = ((!bok || result == -1) ? MCT_COUNT : result);
        }
    }
    return result;


}

bool NetScanningController::saveCustomData(const QString &key, const QString &customName
                                           , int customType, int customConnectionType)
{
    qDebug() << "Try save custom data key = " << key
            << " custom name = " << customName
            << "custom type = " << customType;

    if(m_customDataCacheMap || loadCustomDataCacheMap())
    {
        customType = (customType == MDT_DEVICETYPE_COUNT ? -1 : customType);
        QString strCustomType;
        strCustomType.setNum(customType);
        QString strCustomConnectionType;
        strCustomConnectionType.setNum(customConnectionType);

        (*m_customDataCacheMap)[key].clear();
        (*m_customDataCacheMap)[key].append(customName);
        (*m_customDataCacheMap)[key].append(strCustomType);
        (*m_customDataCacheMap)[key].append(strCustomConnectionType);

    }

    return storeCustomDataCacheMap();
}

bool NetScanningController::storeCustomDataCacheMap()
{
    bool bresult = false;

    if(m_customDataCacheMap)
    {
        QFile file(getCustomDataStoreFilePath());
        if((bresult = file.open(QFile::WriteOnly)))
        {
            QDataStream ds(&file);
            ds.setVersion(QDataStream::Qt_4_6);
            ds << *m_customDataCacheMap;
        }
    }

    return bresult;
}

bool NetScanningController::loadCustomDataCacheMap()
{
    qDebug () << "Custom store file path = " << getCustomDataStoreFilePath();
    if(!m_customDataCacheMap)
    {
        QFile file(getCustomDataStoreFilePath());
        if(file.open(QFile::ReadWrite))
        {
            m_customDataCacheMap = new QMap<QString,QList<QString> >();
            QDataStream ds(&file);
            ds.setVersion(QDataStream::Qt_4_6);
            ds >> *m_customDataCacheMap;
        }
    }

    return (m_customDataCacheMap != 0);
}

void NetScanningController::deviceFromRawData(Device &dev,
                                              const QMap<int,QString> &data , int idx)
{
	Q_UNUSED(idx);

    QString macAddr = data[NIE_MAC].toUpper();
    dev.clear();

    dev[DP_NAME]                 = parseDeviceName(data);
    dev[DP_TYPE]                 = parseDeviceType(data);

    dev[DP_CUSTOMNAME]           = loadCustomName(macAddr);
    dev[DP_CUSTOMTYPE]           = loadCustomDeviceType(macAddr);
    dev[DP_CUSTOMWIRED]          =loadCustomConnectionType(macAddr);
    dev[DP_PHYSICALADDR]         = macAddr;

    //add by caiyujun
    dev[DP_TURBOFLAG]            = false;
    //add end
    dev[DP_IP]                   = data[NIE_IP];
    dev[DP_OS]                   = parseDeviceOS(data);
    dev[DP_UPNP_DEVICETYPE]      = data[NIE_UPNP_DEVICETYPE];
    dev[DP_UPNP_DESCRIPTION]     = data[NIE_UPNP_MODELDISCRIPTION];
    dev[DP_UPNP_MODEL]           = data[NIE_UPNP_MODELNAME];
    dev[DP_UPNP_MANUFACTURER]    = data[NIE_UPNP_MANUFACTURER];
    dev[DP_UPNP_MANUFACTURERURL] = data[NIE_UPNP_MANUFACTURER_URL];

    dev[DP_VMFLAG]               = bool(data[NIE_ISVIRTUALMACHINE].compare("y",Qt::CaseInsensitive) == 0);
    dev[DP_STATUS]               = bool(data[NIE_ONLINE].compare("y",Qt::CaseInsensitive) == 0);
    dev[DP_ISWIRED]              = parseDeivceWiredProperty(data);

    dev[DP_FIRMWARE]             = data[NIE_CURRENTSETTING_FIRMWARE];
    dev[DP_ISLOCALHOST]          = bool(data[NIE_ISHOST].compare("y",Qt::CaseInsensitive) == 0);
    //dev[DP_ISDEFAULTGATEWAY]     = (idx == 1);//bool(data[NIE_ISDEFAULT_GATEWAY].compare("y",Qt::CaseInsensitive) == 0);
	dev[DP_ISDEFAULTGATEWAY]     =  bool(data[NIE_ISGATEWAY].compare("y",Qt::CaseInsensitive) == 0);

    dev[DP_WIRELESS_SIGNALSTRENGTH] = (dev[DP_ISWIRED].toBool()? int(-1) : parseDeivceSignalStrenth(data));
    dev[DP_ATTACHDEVICE_LINKRATE]   = data[NIE_ATTACHDEVICE_LINKSPEED];
    dev[DP_CURRENTSETTING_MODEL]    = data[NIE_CURRENTSETTING_MODEL];
    dev[DP_ATTACHDEVICE_BLOCKED]    = (data[NIE_ATTACHDEVICE_BLOCKED].compare("y",Qt::CaseInsensitive) ==0)?1:
                                      ((data[NIE_ATTACHDEVICE_BLOCKED].compare("n",Qt::CaseInsensitive) ==0)?0:-1);
#ifdef USE_RDP
    dev[DP_RDP_ENABLE] = (data[NIE_RDP_ENABLE].compare("y",Qt::CaseInsensitive)==0);
#endif
#ifdef USE_PUTTY
    bool putty_enabled=(data[NIE_PUTTY_ENABLE].compare("y",Qt::CaseInsensitive)==0);
    bool putty_exist=puttyExeExists()||winscpExeExists();
    dev[DP_PUTTY_ENABLE]=putty_enabled && putty_exist;
    if(putty_enabled)
    {
        dev[DP_PUTTY_HINT]=getPuttyHint(data[NIE_IP],data[NIE_NETBIOS_HOSTNAME]);
        dev[DP_WINSCP_HINT]=GetWinSCPHint(data[NIE_IP],data[NIE_NETBIOS_HOSTNAME]);
    }
#endif
}

void NetScanningController::onNewData(SVT_NotifyType data)
{

    //    qDebug () << "Network scanning controller receive new data ,size = " << data.size();
    //    qDebug () << "Network scanning controller receive new data ,data = " << data;

    m_isDataValid = true;

    if(data.size() > 0 && data[0].contains(NIE_LOCALPOOLNUMBER) && data[0].contains(NIE_LOCAL_NICIDX))
    {
        bool bok1 = false ,bok2 = false;
        int poolNumber = data[0][NIE_LOCALPOOLNUMBER].toInt(&bok1);
        int adpIdx = data[0][NIE_LOCAL_NICIDX].toInt(&bok2);

        m_isDataValid = !(bok1 && bok2 && (poolNumber < m_poolNumber || adpIdx != m_curAdpterId));

        qDebug () << "poolNumber = " << poolNumber << ",m_poolNumber = " << m_poolNumber;
        qDebug () << "adpIdx = " << adpIdx << ",m_curAdpterId = " << m_curAdpterId;
    }

    if(!m_isDataValid)
    {
        qDebug () << "Old subscrible data skipped!";
        return;
    }

    resetSubscribeDevsList();//clear garbage data cached last time

    int idx = 0;

    for(idx = 0 ; idx < data.size() ; ++idx)
    {
        if(idx >= m_subscribeDevsList.size())
        {
            m_subscribeDevsList.append(new Device());
        }

        deviceFromRawData(*(m_subscribeDevsList[idx]),data[idx],idx);
    }

    int tmpCount = m_subscribeDevsList.size();

    for(int k = idx ; k < tmpCount ; ++k)
    {
        delete m_subscribeDevsList.takeAt(idx);
    }

    Q_ASSERT(m_subscribeDevsList.size() == data.size());

#ifdef POWERLINE_REVISION
    if(m_curStartIdx == 0)
    {
        m_curTotalPowerLineInSubDevsList = 0;
        updatePowerLineInSubDevsList();
    }
#endif
    emit newData(m_subscribeDevsList);

}

void NetScanningController::onInfInformation(SVT_NotifyType data)
{
    qDebug() << "Network sanning controller interface information size = " << data.size();

    for(int idx = 0 ; idx < data.size() ; ++idx)
    {
        qDebug() << "nic " << idx << "idx = " << data[idx][IIE_IDX].toInt();
    }

    emit infInformation(data);

    emit routerTypeChanged();
}

void NetScanningController::onCountChanged(int count)
{
    if(m_isDataValid)
    {
        qDebug () << "Network scanning controller count changed new count = " << count;
        emit deviceCountChanged(count);
    }
    else
    {
        qDebug () << "Old count skiped";
    }
}

void NetScanningController::onOtherDeviceCountChanged(QString deviceIp, int count)
{
	if(m_isDataValid)
    {
        emit otherDeviceCountChanged(deviceIp,count);
    }
}

#ifdef POWERLINE_REVISION
void NetScanningController::onPowerLineStateChanged(int totalPowerLine)
{
    Q_ASSERT (totalPowerLine >= 0);
    int oldTotalPowerLine = m_curTotalPowerLine;
    m_curTotalPowerLine = totalPowerLine;

    updatePowerLineInSubDevsList();

    if(m_curStartIdx == 0)
    {
        if((oldTotalPowerLine != m_curTotalPowerLine) && (m_subscribeDevsList.size() >= 2))
        {
            emit newData(m_subscribeDevsList);
        }
    }
}


void NetScanningController::updatePowerLineInSubDevsList()
{
    if(m_curTotalPowerLine > m_curTotalPowerLineInSubDevsList)
    {

        if(m_subscribeDevsList.size() >= 2)
        {
            for(int i = 0 ; i < (m_curTotalPowerLine - m_curTotalPowerLineInSubDevsList) ; ++i)
            {
                Device *powerLine = new Device();
                (*powerLine)[DP_PHYSICALADDR] = QString("FF:FF:FF:FF:FF:FF");
                (*powerLine)[DP_IP] = QString("0.0.0.0");
                m_subscribeDevsList.append(powerLine);
            }
            m_curTotalPowerLineInSubDevsList = m_curTotalPowerLine;
        }
    }
    else if(m_curTotalPowerLine < m_curTotalPowerLineInSubDevsList)
    {
        for(int k = 0 ; k < (m_curTotalPowerLineInSubDevsList - m_curTotalPowerLine) ; ++k)
        {
            delete m_subscribeDevsList.takeLast();
        }
        m_curTotalPowerLineInSubDevsList = m_curTotalPowerLine;
    }
}
#endif

void NetScanningController::startScan()
{
    m_scp.startControl();
}

//void NetScanningController::refreshNetgearInfo()
//{
//    m_scp.refreshNetgearInfo();
//}

//modify by caiyujun
//void NetScanningController::onSubscribeData(int adpIdx,int startIdx,int count)
void NetScanningController::onSubscribeData(int adpIdx,int startIdx,int count,bool bExtender,unsigned long extenderIp)
{
    bool bAdpChanged=(m_curAdpterId != adpIdx);
    if(bAdpChanged)
    {
        emit deviceCountChanged(0);
    }

    m_curAdpterId = adpIdx;

#ifdef POWERLINE_REVISION
    m_curStartIdx = startIdx;
#endif

    BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
    {
        bdm->cancelLoginDialog();
        if(bAdpChanged)
        {
            bdm->resetUi();
        }
    }

	//modify by caiyujun
    m_poolNumber = m_scp.subscrib(adpIdx,startIdx,count,bExtender,extenderIp);
	//modify end
}
//modify end

//void NetScanningController::onNetgearInfo(const QMap<int, QString> &data)
//{
//    emit currentSettingData(data);
//}

//void NetScanningController::onGatewayChanged(QString data)
//{
//    emit gatewayChanged(data);
//}

void NetScanningController::onNode2Device(int idx,Device *dev)
{
    Q_ASSERT ((idx >= 0) && (idx < m_subscribeDevsList.size()));
    *dev = *m_subscribeDevsList[idx];
}

//add by caiyujun
void NetScanningController::onIp2Device(QString strIp, Device *device)
{
    for (int i=0; i<m_subscribeDevsList.size(); i++)
    {
        if (m_subscribeDevsList[i]->isDevice(strIp))
        {
            *device = *m_subscribeDevsList[i];
        }
    }
}

void NetScanningController::onBonjourTypeChanged(const Device &dev)
{
    qDebug() << "on Type Changed";
    QString key = dev[DP_PHYSICALADDR].toString();

    if((!key.isEmpty()))
    {
        foreach(Device *pdev , m_subscribeDevsList)
        {
            Q_ASSERT(pdev);

            if((*pdev)[DP_PHYSICALADDR].toString() == key)
            {
                (*pdev)[DP_TYPE] = dev[DP_TYPE];
                qDebug () << "Modified device ,key = " << key
                        << "data updated";
                break;
            }
        }
    }
    saveCustomData(dev[DP_PHYSICALADDR].toString(),
                   dev[DP_CUSTOMNAME].toString(),dev[DP_CUSTOMTYPE].toInt()
                   ,dev[DP_CUSTOMWIRED].toInt());
}
//add end

void NetScanningController::onDeviceDataChanged(const Device &dev)
{
    QString key = dev[DP_PHYSICALADDR].toString();

    if((!key.isEmpty()))
    {
        foreach(Device *pdev , m_subscribeDevsList)
        {
            Q_ASSERT(pdev);

            if((*pdev)[DP_PHYSICALADDR].toString() == key)
            {
                (*pdev)[DP_CUSTOMNAME] = dev[DP_CUSTOMNAME];
                (*pdev)[DP_CUSTOMTYPE] = dev[DP_CUSTOMTYPE];
                (*pdev)[DP_CUSTOMWIRED]= dev[DP_CUSTOMWIRED];

                emit newData(m_subscribeDevsList);

                qDebug () << "Modified device ,key = " << key
                        << "data updated";
                break;
            }
        }

        //save change
        saveCustomData(dev[DP_PHYSICALADDR].toString(),
                       dev[DP_CUSTOMNAME].toString(),dev[DP_CUSTOMTYPE].toInt()
                       ,dev[DP_CUSTOMWIRED].toInt());
    }
}

void NetScanningController::onRefresh()
{
    m_poolNumber = m_scp.refresh();

    emit deviceCountChanged(0);
}

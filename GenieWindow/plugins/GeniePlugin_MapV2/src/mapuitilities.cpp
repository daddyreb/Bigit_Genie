#include "mapuitilities.h"
#include "mapdevicetype.h"

#include <QtGlobal>
#include <QtDebug>
#include <QDir>
#include <QApplication>
#include <QDesktopServices>
#include "genie2_interface.h"

#define IMAGES_DEVICES_PATHPREFIX "map/devices/"
#define IMAGES_OTHERS_PATHPREFIX "map/others/"
#define MAP_CUSTOMDATA_STOREFILENAME "mapcustomdata.dat"

static const char* windows_localosversionstrings[] =
{
    "5.1 Build 2600 D",
    "5.1 Build 2600 S",
    "6.0 Build 6000 ",
    "6.0 Build 6001 S",
    "6.0 Build 6002 S",
    "6.1 Build 7600 ",
	"6.1 Build 7601 S",
	"6.2 Build 9200 "
};

static const char* windows_localosstrings[] =
{
    "Windows XP",
    "Windows XP(Service Pack 3)",
    "Windows Vista",
    "Windows Vista(Service pack 1)",
    "Windows Vista(Service pack 2)",
    "Windows 7",
	"Windows 7(Service pack 1)",
	"Windows 8"
};

//must be match with device type enum
static const char* devicetypes[] =
{
    "Internet",
    "Network Device",
    "Windows PC",
    "MacBook",
    "iMac",
    "Mac Mini",
    "Mac Pro",
    "Linux PC",
    "Unix PC",
    "Gaming Console",
    "Media Device",
    "Camera",
    "Smart Phone",
    "Tablet",
    "Printer",
    "Scanner",
    "Storage",
    "Router",
    "iPad",
    "iPodTouch",
    "iPhone",
    "Bridge",
    "Repeater",
    "Cable STB",
    "Satellite STB",
    "SlingBox",
    "Other STB",
    "TV",
    "Switch",
    "Android Tablet",
    "Android Phone",
    "Android Device",
    "Blu-ray Player",
    "DVR",
    "Powerline",
    "Amazonkindle", //kai.yan add
    "iPadmini",     //kai.yan add
    "iPhone5"       //kai.yan add

};

//must be match with image role enum
static const char* imageroles[] =
{
    "Normal",
    "Selected",
    "Offline",
    "Details"
};

//must be match with map image index enum
static const char* mapimages[] =
{
    "VmFlag"
    ,"BlockFlag"
#ifdef USE_RDP
    ,"RdpFlag"
#endif
#ifdef USE_PUTTY
    ,"puttyFlag"  //for Putty
    ,"puttyFlag" //for WinSCP
#endif
    //add by caiyujun
    ,"TurboFlag"
	,"DeviceCount"
    //add end
};

QString getCustomDataStoreFilePath()
{
    return getConfigurationDir() + "/" +MAP_CUSTOMDATA_STOREFILENAME;
}

QString getConfigurationDir()
{
    QDir appDataDir=QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString     configDir;


//#ifdef Q_OS_WIN
    const QString genieDir = "NETGEARGenie";
//#elif defined Q_OS_MAC
//    const QString genieDir = ".NETGEARGenie";
//#else
//    const QString genieDir;
//#endif

    if((!genieDir.isEmpty()) &&
       //(appDataDir.exists(genieDir) || appDataDir.mkdir(genieDir)) &&
	   appDataDir.exists(genieDir))
    {
        configDir = appDataDir.absolutePath();
    }
    else
    {
        configDir = QApplication::applicationDirPath();
    }

    return configDir;
}

QString getWindowsLocalOSVersionCompositedString()
{
    QString resultString;

    int count = (sizeof(windows_localosversionstrings) / sizeof(windows_localosversionstrings[0]));
    for(int idx = 0 ; idx < count ; ++idx)
    {
        resultString += windows_localosversionstrings[idx];
        if(idx != (count - 1))
        {
            resultString += "##";
        }
    }

    return resultString;
}

QStringList getWindowsLocalOSVersionStringList()
{
    QStringList resultList;

    int count = (sizeof(windows_localosversionstrings) / sizeof(windows_localosversionstrings[0]));
    for(int idx = 0 ; idx < count ; ++idx)
    {
        resultList.append(windows_localosversionstrings[idx]);
    }
    return resultList;
}

QStringList getWindowsLocalOSStringList()
{
    QStringList resultList;

    int count = (sizeof(windows_localosstrings) / sizeof(windows_localosstrings[0]));
    for(int idx = 0 ; idx < count ; ++idx)
    {
        resultList.append(windows_localosstrings[idx]);
    }
    return resultList;
}

QString getDeviceTypeImagePath(int deviceType, int imageRole, const QString &suffix)
{
    Q_ASSERT(sizeof(devicetypes) / sizeof(devicetypes[0]) == MDT_DEVICETYPE_COUNT);
    Q_ASSERT(sizeof(imageroles) / sizeof(imageroles[0]) == DTIR_IMAGEROLE_COUNT);

    Q_ASSERT((deviceType >= 0) && (deviceType < MDT_DEVICETYPE_COUNT));
    Q_ASSERT((imageRole >= 0) && (imageRole < DTIR_IMAGEROLE_COUNT));

    QString strDeviceType(devicetypes[deviceType]);
    QString strImageRole(imageroles[imageRole]);

    strDeviceType = strDeviceType.simplified();
    strDeviceType = strDeviceType.replace(' ',"");

    return (GENIE2_RES(IMAGES_DEVICES_PATHPREFIX) + strDeviceType + strImageRole + (suffix.isEmpty()?"":".") + suffix);
}

QString getLocalHostImagePath(int imageRole, const QString &suffix)
{
    Q_ASSERT(sizeof(imageroles) / sizeof(imageroles[0]) == DTIR_IMAGEROLE_COUNT);
    Q_ASSERT((imageRole >= 0) && (imageRole < DTIR_IMAGEROLE_COUNT));

    QString platformTag;
#ifdef Q_OS_MACX
    platformTag = "_Mac";
#endif
    return (GENIE2_RES(IMAGES_DEVICES_PATHPREFIX) + QString("LocalHost") + imageroles[imageRole] + platformTag + (suffix.isEmpty()?"":".") + suffix);
}

QString getCentralRouterImagePath(int imageRole, const QString &suffix)
{
    Q_ASSERT(sizeof(imageroles) / sizeof(imageroles[0]) == DTIR_IMAGEROLE_COUNT);
    Q_ASSERT((imageRole >= 0) && (imageRole < DTIR_IMAGEROLE_COUNT));

    return (GENIE2_RES(IMAGES_DEVICES_PATHPREFIX) + QString("CentralRouter") + imageroles[imageRole] + (suffix.isEmpty()?"":".") + suffix);
}

QString getWirelessSignalImagePath(int signal, const QString &suffix)
{
    if(signal < 0 || signal > 5)
    {
        qDebug() << "Wireless signal invalid ,signal = " << signal;
        return QString();
    }

    QString strSignal;
    strSignal.setNum(signal);

    return GENIE2_RES(IMAGES_OTHERS_PATHPREFIX) + "WirelessSignal_" + strSignal + (suffix.isEmpty()?"":".") + suffix;
}

QString getImagePath(int imageIdx, const QString &suffix)
{
    Q_ASSERT(sizeof(mapimages) / sizeof(mapimages[0]) == MII_COUNT);
    Q_ASSERT (imageIdx >= 0 && imageIdx < MII_COUNT);

    return GENIE2_RES(IMAGES_OTHERS_PATHPREFIX) + mapimages[imageIdx] + (suffix.isEmpty()?"":".") + suffix;

}


//--
QString getDeviceTypeText(int deviceType)
{
    Q_ASSERT(sizeof(devicetypes) / sizeof(devicetypes[0]) == MDT_DEVICETYPE_COUNT);
    Q_ASSERT((deviceType >= 0) && (deviceType < MDT_DEVICETYPE_COUNT));

    return devicetypes[deviceType];
}


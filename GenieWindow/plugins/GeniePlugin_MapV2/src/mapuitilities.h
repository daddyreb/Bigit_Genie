#ifndef MAPUITILITIES_H
#define MAPUITILITIES_H

#include <QString>
#include <QList>
#include <QStringList>

enum DEVICE_IMAGE_ROLE
{
    DTIR_NORMAL,
    DTIR_SELECTED,
    DTIR_OFFLINE,
    DTIR_DETAILVIEWICON,
    DTIR_IMAGEROLE_COUNT
};

enum MAP_IMAGE_INDEX
{
    MII_VMFLAG,
    MII_BLOCKFLAG,
#ifdef USE_RDP
    MII_RDPFLAG,
#endif
#ifdef USE_PUTTY
    MII_PUTTYFLAG,
    MII_WINSCPFLAG,
#endif
    //add by caiyujun
    MII_TURBOFLAG,
	MII_DEVICECOUNT,
    //add end
    MII_COUNT
};

QString getCustomDataStoreFilePath();
QString getConfigurationDir();

QString getWindowsLocalOSVersionCompositedString();
QStringList getWindowsLocalOSVersionStringList();
QStringList getWindowsLocalOSStringList();

QString getDeviceTypeImagePath(int deviceType, int imageRole, const QString &suffix = QString("png"));
QString getLocalHostImagePath(int imageRole, const QString &suffix = QString("png"));
QString getCentralRouterImagePath(int imageRole, const QString &suffix = QString("png"));
QString getWirelessSignalImagePath(int signal, const QString &suffix = QString("png"));
QString getImagePath(int imageIdx, const QString &suffix = QString("png"));

//--
QString getDeviceTypeText(int deviceType);

#endif // MAPUITILITIES_H

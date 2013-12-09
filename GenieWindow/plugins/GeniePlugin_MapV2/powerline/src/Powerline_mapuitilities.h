#ifndef MAPUITILITIES_H
#define MAPUITILITIES_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QWidget>
#include <QDebug>

enum DEVICE_IMAGE_ROLE
{
    DTIR_NORMAL,
    DTIR_SELECTED,
    DTIR_OFFLINE,

    DTIR_IMAGEROLE_COUNT
};

enum MAP_IMAGE_INDEX
{
    MII_VMFLAG,
    MII_BLOCKFLAG,
    MII_COUNT
};


//Powerline start
enum PL_IMAGE_ROLE
{
	PIR_SMALL,
	PIR_LARGE,

	PIR_NONE,

	PIR_COUNT
};
enum PL_IMAGE_STATE
{
	PIS_ON,
	PIS_OFF,
	PIS_NONE,

	PIS_COUNT
};
QString getPLDevTypeImagePath(QString deviceType, int nState, int nRole, const QString &suffix = QString("png"));
QString getPLOtherDevImagePath(int deviceType, int nState = PIS_NONE, int nRole = PIR_NONE, const QString &suffix = QString("png"));
QString getDlgPLDevTypeImagePath(QString deviceType, int nState, const QString &suffix = QString("png"));
//End

QString getWirelessSignalImagePathStr(int signal, const QString &suffix = QString("png"));	//No use
QString getImagePathStr(int imageIdx, const QString &suffix = QString("png"));


//--
//QString getDeviceTypeText(int deviceType);


void setFramelessStyle(QWidget *w, bool bModal = true);
int getStringWidth(const QString &str, const QWidget *widget = NULL);
QString getDeviceTypeTxt(int deviceType);
#endif // MAPUITILITIES_H

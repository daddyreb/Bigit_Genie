#include "Powerline_mapuitilities.h"
#include "PowerlineDefine.h"
#include <QtGlobal>
#include <QtDebug>
#include <QDir>
#include <QtGui/QDesktopServices>
#include <QApplication>
#include <QDialog>




//static const char* windows_localosversionstrings[] =
//{
//    "5.1 Build 2600 D",
//    "5.1 Build 2600 S",
//    "6.0 Build 6000 ",
//    "6.0 Build 6001 S",
//    "6.0 Build 6002 S",
//    "6.1 Build 7600 ",
//    "6.1 Build 7601 S"
//};
//
//static const char* windows_localosstrings[] =
//{
//    "Windows XP",
//    "Windows XP(Service Pack 3)",
//    "Windows Vista",
//    "Windows Vista(Service pack 1)",
//    "Windows Vista(Service pack 2)",
//    "Windows 7",
//    "Windows 7(Service pack 1)"
//};

//must be match with device type enum
static const char* devicetypes[] =
{
	"PowerLine",
	"security-pub-M",
	"security-priv-M",
	"hub",
	"storage",
	"printer",
	"sound",
	"UnKnown",
	"WNDR4500Normal_1",
	"internet",
	"usb",
	"LocalHost"
};
static const char *devHubTypes[] =
{
	"hub",
	"storage",
	"printer",
	"sound",
	"UnKnown",
};

//must be match with map image index enum
static const char* mapimages[] =
{
    "VmFlag",
    "BlockFlag"

};

//must be match with powerline image role enum
static const char *router_role[] =
{
	"Normal",
	"Selected",
	"Offline",
};
static const char *pl_image_role[] =
{
	"-S",			//Small image
	"-L",			//Large image
	""				//None
};
//must be match with powerline image state enum
static const char *pl_image_state[] =
{
	"-ON",
	"-OFF",
	""			//None
};
////must be match with powerline image type enum
//static const char *pl_image_type[] =
//{
//	"DEFAULT",
//	"XAV"
//};

////powerline get image path function
QString getPLDevTypeImagePath(QString deviceType, int nState, int nRole, const QString &suffix /* = QString */)
{
	Q_ASSERT(sizeof(pl_image_role) / sizeof(pl_image_role[0]) == PIR_COUNT);
	Q_ASSERT(sizeof(pl_image_state) / sizeof(pl_image_state[0]) == PIS_COUNT);

	Q_ASSERT((nState >= 0) && (nState < PIS_COUNT));
	Q_ASSERT((nRole >= 0) && (nRole < PIR_COUNT));

	QString strDevState(pl_image_state[nState]);
	QString strDevRole(pl_image_role[nRole]);
	strDevState = strDevState.simplified();
	strDevRole  = strDevRole.simplified();
	deviceType  = deviceType.simplified();
	deviceType  = deviceType.replace(' ', "");

	return (IMAGES_PLDEV_PATHPREFIX + deviceType + strDevState + strDevRole + (suffix.isEmpty() ? "" : ".") + suffix);
}
QString getPLOtherDevImagePath(int deviceType, int nState, int nRole, const QString &suffix)
{
	Q_UNUSED(nRole);

	Q_ASSERT(sizeof(devicetypes) / sizeof(devicetypes[0]) == GDT_DEVICETYPE_COUNT);
	Q_ASSERT(sizeof(pl_image_state) / sizeof(pl_image_state[0]) == PIS_COUNT);
	
	Q_ASSERT((deviceType >= 0) && (deviceType < GDT_DEVICETYPE_COUNT));
	Q_ASSERT((nState >= 0) && (nState < PIS_COUNT));
	Q_ASSERT((nRole >= 0) && (nRole < PIR_COUNT));
	

	QString strDevState(pl_image_state[nState]);
	QString strDevRole(pl_image_role[nRole]);
	QString strDevImage(devicetypes[deviceType]);

	strDevImage = strDevImage.simplified();
	strDevState	= strDevState.simplified();
	strDevRole	= strDevRole.simplified();
	strDevImage = strDevImage.replace(' ', "");

	return (IMAGES_PLOTHERDEV_PATHPREFIX + strDevImage + strDevState + strDevRole + (suffix.isEmpty() ? "" : ".") + suffix);
}
QString getDlgPLDevTypeImagePath(QString deviceType, int nState, const QString &suffix )
{
	Q_ASSERT((nState >= 0) && (nState < PIS_COUNT));
	QString strDevState(pl_image_state[nState]);

	deviceType  = deviceType.simplified();
	deviceType  = deviceType.replace(' ', "");
	strDevState	= strDevState.simplified();
	


	return (IMAGES_DLG_PLDEV_PATHPREFIX + deviceType + strDevState + "-S" + (suffix.isEmpty() ? "" : ".") + suffix);
}
//END


QString getWirelessSignalImagePathStr(int signal, const QString &suffix)
{
    if(signal < 0 || signal > 5)
    {
        qDebug() << "Wireless signal invalid ,signal = " << signal;
        return QString();
    }

    QString strSignal;
    strSignal.setNum(signal);
	QString temp = IMAGES_OTHERS_PATHPREFIX;
	temp += "WirelessSignal_";
	temp += strSignal;
	temp += (suffix.isEmpty()?"":".");
	temp += suffix;

    return temp;
}

QString getImagePathStr(int imageIdx, const QString &suffix)
{
    Q_ASSERT(sizeof(mapimages) / sizeof(mapimages[0]) == MII_COUNT);
    Q_ASSERT (imageIdx >= 0 && imageIdx < MII_COUNT);

	QString temp;
	temp = IMAGES_OTHERS_PATHPREFIX;
	temp += mapimages[imageIdx];
	temp += (suffix.isEmpty()?"":".");
	temp += suffix;
    return temp;

}




void setFramelessStyle(QWidget *w, bool bModal)
{
#ifdef Q_OS_MACX
	Qt::WindowFlags flag= (Qt::Sheet| Qt::FramelessWindowHint);
#else
	QDialog *dlg=qobject_cast<QDialog *>(w);
	Qt::WindowFlags flag= (dlg)?(Qt::Dialog| Qt::FramelessWindowHint):(Qt::FramelessWindowHint);
#endif

	if(dlg && bModal)
	{
		dlg->setModal(true);
		//dlg->setWindowModality(Qt::WindowModal);
	}
	w->setWindowFlags(flag);
	w->setAttribute(Qt::WA_TranslucentBackground);

	//enableMove(w);

	QFile f(/*GENIE2_RES(*/":styles/framelessframe.qss")/*)*/;
	bool btemp=f.open(QIODevice::ReadOnly);
	Q_ASSERT(btemp);
	QString stemp=f.readAll();
	//qDebug() << stemp;

	w->setStyleSheet(stemp);
}

int getStringWidth(const QString &str, const QWidget *widget)
{
	int nWidth = 0;

	if(widget != NULL)
	{
		QFont tmpFont(widget->font());
		QFontMetrics tmpFtMe(tmpFont);
		nWidth = tmpFtMe.width(str);
	}
	else
	{
		QFont tmpFont(QApplication::font());
		QFontMetrics tmpFtMe(tmpFont);
		nWidth = tmpFtMe.width(str);
	}
	

	return nWidth;
}

QString getDeviceTypeTxt(int deviceType)
{
	Q_ASSERT(sizeof(devHubTypes) / sizeof(devHubTypes[0]) == DHT_COUNT);
	Q_ASSERT((deviceType >= 0) && (deviceType < DHT_COUNT));

	return devHubTypes[deviceType];
}

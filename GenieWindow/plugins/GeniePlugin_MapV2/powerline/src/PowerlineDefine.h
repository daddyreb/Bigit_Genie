#ifndef TOPOLOGYDEFINE_H
#define TOPOLOGYDEFINE_H


#include <QString>
#include <QPointF>

enum DEV_HUB_TYPE
{
	DHT_HUB,
	DHT_STORAGE,
	DHT_PRINTER,
	DHT_SOUND,
	DHT_UNKNOWN,

	DHT_COUNT
};
enum DEVICE_TYPE_
{
    GDT_POWERLINE,

    GDT_UNLOCKPIC,
    GDT_LOCKPIC,

	GDT_HUB,
	GDT_STORAGE,
	GDT_PRINTER,
	GDT_SOUND,
	GDT_UNKNOWN,

	GDT_ROUTER,
	GDT_INTERNET,

	GDT_USB,
	GDT_LOCALDEV,

    GDT_DEVICETYPE_COUNT
};

#define POWERLINE_DEFAULT_DEV "DEFAULT"
#define POWERLINE_UNKNOWN_DEV "UNKNOWN"


#define PER_PAGE_DATACOUNT 4		//主页面中每页显示的powerline个数
#define PER_PAGE_NUM 4				//详情中显示的子设备个数

#define POWERLINE_SAFEDEL_LATER(p) do{if(p != NULL){p->deleteLater(); p = NULL;}}while(0)
#define POWERLINE_SAFEDEL(p) do{if(p != NULL){delete p; p = NULL;}}while(0)


//#define D_TOPOLOGYVIEW_STYLE	".Powerline_DetailDlg{border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;}"
//#define TOPOLOGYVIEW_STYLE		".PowerlineView{border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;}"

#define IMAGES_DEVICES_PATHPREFIX ":/PowerLineImage/"
#define IMAGES_PLDEV_PATHPREFIX ":/PowerLineImage/PLineDev/"
#define IMAGES_PLOTHERDEV_PATHPREFIX ":/PowerLineImage/OtherDev/"
#define IMAGES_OTHERS_PATHPREFIX ":/map/others/"
#define IMAGES_DLG_PLDEV_PATHPREFIX ":/PowerLineImage/PLineDevDetail/"



#define DEFAULT_DLG_WIDTH   350
#define DEFAULT_DLG_HEIGHT  145
#define DEFAULT_DLG_HEIGHT_EX 167

#define TEXT_MAX_LEN 12
#define MAX_LENGTH 63

#define PLC_MAINPIPE_PLUGIN_FILE (QCoreApplication::applicationDirPath() + QDir::separator() + "PLCMainPipePlugin.dll")
#define PLC_DRIVER_DIR (QCoreApplication::applicationDirPath() + QDir::separator() + "drivers")


#define DEFAULT_KEY "HomePlugAV"

#define DEFAULT_PIC_PATH ":/PowerLineImage/password.png"
#define OLD_TEXT_LEN "OLD_TEXT_LEN"

//QGenieIface2ShareObject::sSingleInstance &&QGenieIface2ShareObject::sSingleInstance->mHandler
//#define POWERLINE_GET_TEXT(lan, defaultString) ((QGenieIface2ShareObject::sSingleInstance &&QGenieIface2ShareObject::sSingleInstance->mHandler) ? \
//	GENIE2_GET_TEXT(lan) : defaultString)
//#define POWERLINE_TRANSLATE_LABEL(lab, lan, defaultString)

#include "language_Map.h"
#define DEFAULT_CONFIG_FILE  "config.ini"
#define HUB_CONFIG_FILE	"hubConfig.ini"
#define CONFIG_DEFAULT_KEY "plugin/ShowMsgBox"
#include <QSettings>
#include <QApplication>
static int loadConfigurationItem(const QString &key)
{
	//value=GENIE2_GET_SETTING(key);
	QString filePath = QApplication::applicationDirPath() + "/" + DEFAULT_CONFIG_FILE;
	QSettings settings(filePath,QSettings::IniFormat);
	return (settings.value(key).toInt());
}

static void storeConfigurationItem(const QString &key,const QVariant &value)
{
	//GENIE2_SET_SETTING(key,value);
	QString filePath = QApplication::applicationDirPath() + "/" + DEFAULT_CONFIG_FILE;
	QSettings settings(filePath,QSettings::IniFormat);
	settings.setValue(key,value);  
}
#ifdef TEST_PL_CODE
#	define POWERLINE_GET_TEXT(lan, defaultstr) defaultstr
#	define POWERLINE_TRANSLATE_LABEL(lab, lan, defaultstr) (lab->setText(defaultstr))
#	define POWERLINE_TRANSLATE_BUTTON(lab, lan, defaultstr) POWERLINE_TRANSLATE_LABEL(lab, lan, defaultstr)
#	define POWERLINE_TRANSLATE_RADIO(lab, lan, defaultstr) POWERLINE_TRANSLATE_LABEL(lab, lan, defaultstr)
#	define POWERLINE_GET_SETTING(key) loadConfigurationItem(key)
#	define POWERLINE_SET_SETTING(key, value) storeConfigurationItem(key, value)
#	define POWERLINE_RES(_res) _res
#else
#   include "genie2_interface.h"
#	define POWERLINE_GET_TEXT(lan, str) GENIE2_GET_TEXT(lan)
#	define POWERLINE_TRANSLATE_LABEL(lab, lan, defaultstr) GENIE2_TRANSLATE_LABEL(lab, lan)
#	define POWERLINE_TRANSLATE_BUTTON(lab, lan, defaultstr) GENIE2_TRANSLATE_BUTTON(lab, lan)
#	define POWERLINE_TRANSLATE_RADIO(lab, lan, defaultstr) GENIE2_TRANSLATE_RADIOBUT(lab, lan)
#	define POWERLINE_GET_SETTING(key) GENIE2_GET_SETTING(key)
#	define POWERLINE_SET_SETTING(key, value) GENIE2_SET_SETTING(key, value)
#	define POWERLINE_RES(_res) GENIE2_RES(_res)
#endif

#define DEAL_PSTR(p) ((p)->isEmpty() ? "N/A" : p)
#define DEAL_STR(p) ((p).isEmpty() ? "N/A" : p)
#define DEAL_BOOL(p, str1, str2) ((p) ? str1 : str2)
#define DEAL_PSTR_EX(p, str1, str2) ((p)->isEmpty() ? str1 : str2)
#define DEAL_STR_EX(p, str1, str2) ((p).isEmpty() ? str1 : str2)
#define DEAL_EQU_EX(p, str1, str2, str3) ((p == str1) ? str2 : str3)


#endif // TOPOLOGYDEFINE_H

#ifndef _SETTING_DEF_H_____
#define _SETTING_DEF_H_____

#define ORGANIZATION_NAME "NETGEAR"
#define APPNAME     "Genie"

#include "selfdef.h"


#ifdef USE_APP_PATH
#define GENIE_DIR (QDir(qApp->applicationDirPath()))
#else
#define GENIE_DIR (QDir::current())
#endif
#define GENIE_PATH (GENIE_DIR.absolutePath()+"/")

//bool values
#define SETTING_START_WITH_SYSTEM "frame/startwithsystem"
#define SETTING_START_MINIMIZE "frame/startminimize"
#define SETTING_HAVE_UPDATE "launch/haveupdate"
#define SETTING_USE_LOG "launch/uselog"
        //for traffic
        #define SETTING_TRAFFIC_START_MINIMIZE "traffics/startminimize"

//int value
#define SETTING_UPDATE_INTERVAL "frame/updateinterval"

//string date
#define SETTING_INSTALL_DATE "frame/install_date"



//#define CONFIG_FILE_NAME (GENIE_PATH+"genie.ini")
#ifdef PLATFORM_WIN32
#define INI_BASE (QDir::home().absolutePath()+"/NETGEARGenie/")
#else
#define INI_BASE (QDir::home().absolutePath()+"/.NETGEARGenie/")
#endif
#define CHANGABLE_INI_FNAME (INI_BASE+"genie.ini")
#define PERMANENT_INI_FNAME (INI_BASE+"config.ini")


#define UPDATE_IP "update/update_addr"
//#define UPDATE_IP_DEFAULT "192.168.2.225"
#define UPDATE_IP_DEFAULT "updates1.netgear.com"

#define UPDATE_DESP_FILE "update/desp"
#ifdef PLATFORM_WIN32
#define UPDATE_DESP_FILE_DEFAULT "/netgeargenie/update/update_{la}.txt"
#else
#define UPDATE_DESP_FILE_DEFAULT "/netgeargenie/mac/update/update_{la}.txt"
#endif

#define UPDATE_PORT "update/update_port"
#define UPDATE_PORT_DEFAULT 80

#define LAUNCH_HAVEUPDATE "launch/update"
#define LAUNCH_HAVEUPDATE_DEFAULT false

#define AD_ADDR "AD/ad_addr"
//#define AD_ADDR_DEFAULT "192.168.2.225:80"
#define AD_ADDR_DEFAULT "lrs.netgear.com"

#define AD_PORT "AD/ad_port"
#define AD_PORT_DEFAULT 80

#define AD_DESP_ADDR "AD/disp"
#define AD_DESP_ADDR_DEFAULT "/netgeargenie/ad/advsetting.txt"

#define TRAY_STRATEGY "TRAY/strategy"
#ifdef _WIN32
#define TRAY_STRATEGY_DEFAULT 2
#else
#define TRAY_STRATEGY_DEFAULT 1
#endif

//#define AD_PORT "adv/port"
//#define AD_PORT_DEFAULT 80

#define PLUGIN_TRAFFIC_WARRING "plugin/traffic_warring"
#define PLUGIN_TRAFFIC_WARRING_DEFAULT true

#define PLUGIN_TRAFFIC_THRESHOLD "plugin/traffic_threshold"
#define PLUGIN_TRAFFIC_THRESHOLD_DEFAULT  10

#define PLUGIN_STATICS_IP "statistics/statics_addr"
#define PLUGIN_STATICS_IP_DEFAULT  "lrs.netgear.com"

#define PLUGIN_STATICS_PORT "statistics/statics_port"
#define PLUGIN_STATICS_PORT_DEFAULT 80

#define PLUGIN_MAP_SHOWNOTIFY "plugin/map_notify"
#define PLUGIN_MAP_SHOWNOTIFY_DEFAULT false

#define PLUGIN_ROUTECONF_USER "plugin/routerconf_user"
#define PLUGIN_ROUTECONF_USER_DEFAULT "admin"

#define PLUGIN_ROUTECONF_PWD "plugin/routeconf_pwd"
#define PLUGIN_ROUTECONF_PWD_DEFAULT "password"

#define PLUGIN_ROUTECONF_SAVEPWD "plugin/routeconf_savepwd"
#define PLUGIN_ROUTECONF_SAVEPWD_DEFAULT true

#define PLUGIN_STATICS_DISP "statistics/statics_disp"
#define PLUGIN_STATICS_DISP_DEFAULT ""

#define PLUGIN_INTERNET_DETAIL "Internet/Details"
#define PLUGIN_INTERNET_DETAIL_DEFAULT 0

#define SOAP_SET_INTERVAL "soap/setInterval"
#define SOAP_SET_INTERVAL_DEFAULT 50

#define SOAP_GET_INTERVAL "soap/getInterval"
#define SOAP_GET_INTERVAL_DEFAULT 5

#define SHOW_NETWORK_CHANGE "plugin/shownetworkchange"
#define SHOW_NETWORK_CHANGE_DEFAULT true

#define INSTALL_INFO_COMMITED "install_info/commited"
#define INSTALL_INFO_COMMITED_DEFAULT false

#define INSTALL_INFO_DATE "install_info/date"
#define INSTALL_INFO_DATE_DEFAULT (QDate::currentDate().toString("yyyy-MM-dd"))

#define INSTALL_INFO_TIME "install_info/time"
#define INSTALL_INFO_TIME_DEFAULT (QTime::currentTime().toString("hh:mm:ss"))

#ifdef PLATFORM_MACOSX
#define MACOS_AUTOSTART "plugin/macos_autostart"
#define MACOS_AUTOSTART_DEFAULT true

#endif

#define TITLE_URL "mainframe/title_url"
#define TITLE_URL_DEFAULT "http://www.netgear.com/genie"

#define ISETTING_GACC_MAC "plugin/routeconf_gacc_mac"
#define ISETTING_GACC_TIME "plugin/routeconf_gacc_time"
#define ISETTING_INTERNET_DETAIL "Internet/Details"
#define GET_SETTING_INT(FLAG) (SINGLE_SETTING->value(FLAG,FLAG##_DEFAULT).toInt())
#define GET_SETTING_STR(FLAG) (SINGLE_SETTING->value(FLAG,FLAG##_DEFAULT).toString())
#define GET_SETTING_BOOL(FLAG) (SINGLE_SETTING->value(FLAG,FLAG##_DEFAULT).toBool())

#define SET_SETTING_VALUE(FLAG,value) do{SINGLE_SETTING->setValue(FLAG,value);}while(0)
//#define SET_DEFAULT_TOFILE(FLAG) QSettings(CONFIG_FILE_NAME,QSettings::IniFormat).setValue(FLAG,FLAG##_DEFAULT);

#define SET_DEFAULT_TOFILE(FLAG) QGenieSettingHelper().setValue(FLAG,FLAG##_DEFAULT);

#define INIT_ALL_SETTINGS() do{\
    SET_DEFAULT_TOFILE(TITLE_URL)\
    SET_DEFAULT_TOFILE(UPDATE_IP)\
    SET_DEFAULT_TOFILE(UPDATE_PORT)\
    SET_DEFAULT_TOFILE(UPDATE_DESP_FILE)\
    SET_DEFAULT_TOFILE(AD_ADDR)\
    SET_DEFAULT_TOFILE(AD_PORT)\
    SET_DEFAULT_TOFILE(AD_DESP_ADDR)\
    SET_DEFAULT_TOFILE(TRAY_STRATEGY)\
    SET_DEFAULT_TOFILE(PLUGIN_STATICS_IP)\
    SET_DEFAULT_TOFILE(PLUGIN_STATICS_PORT)\
    SET_DEFAULT_TOFILE(PLUGIN_STATICS_DISP)\
    SET_DEFAULT_TOFILE(INSTALL_INFO_COMMITED)\
    SET_DEFAULT_TOFILE(INSTALL_INFO_DATE)\
    SET_DEFAULT_TOFILE(INSTALL_INFO_TIME)\
    }while(0)

#ifdef PLATFORM_MACOSX
#define INIT_ALL_SETTINGS_PERMANENT() do{\
    SET_DEFAULT_TOFILE(PLUGIN_MAP_SHOWNOTIFY)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_USER)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_PWD)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_SAVEPWD)\
    SET_DEFAULT_TOFILE(PLUGIN_INTERNET_DETAIL) \
    SET_DEFAULT_TOFILE(PLUGIN_TRAFFIC_WARRING) \
    SET_DEFAULT_TOFILE(PLUGIN_TRAFFIC_THRESHOLD) \
    SET_DEFAULT_TOFILE(SHOW_NETWORK_CHANGE) \
    SET_DEFAULT_TOFILE(MACOS_AUTOSTART) \
    }while(0)
#else
#define INIT_ALL_SETTINGS_PERMANENT() do{\
    SET_DEFAULT_TOFILE(PLUGIN_MAP_SHOWNOTIFY)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_USER)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_PWD)\
    SET_DEFAULT_TOFILE(PLUGIN_ROUTECONF_SAVEPWD)\
    SET_DEFAULT_TOFILE(PLUGIN_INTERNET_DETAIL) \
    SET_DEFAULT_TOFILE(PLUGIN_TRAFFIC_WARRING) \
    SET_DEFAULT_TOFILE(PLUGIN_TRAFFIC_THRESHOLD) \
    SET_DEFAULT_TOFILE(SHOW_NETWORK_CHANGE) \
    }while(0)
#endif


#endif

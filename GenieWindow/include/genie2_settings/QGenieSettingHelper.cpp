#include "QGenieSettingHelper.h"
//#include "ksetting_def.h"
#ifdef GENIE2_MAINFRAME
#include "../genie2_interface/genie2_interface.h"
#endif
#include <QDesktopServices>
#include <QtCore>

//#ifdef Q_OS_WIN32
//#define INI_BASE (QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/")
#define INI_BASE (QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/")
//#else
//#define INI_BASE (QDir::home().absolutePath()+"/.NETGEARGenie/")
//#endif
#define CHANGABLE_INI_FNAME (INI_BASE+"genie.ini")
#define PERMANENT_INI_FNAME (INI_BASE+"config.ini")

QGenieSettingHelper::QGenieSettingHelper()

{
    mPermanentDict<<"plugin"
                 <<"Internet"
                <<"Internet_Wireless"
               <<"language"
              <<"PLC_Device_Pwd";

    if(!QDir::home().exists(INI_BASE))
    {
        QDir::home().mkdir(INI_BASE);
    }
    Q_ASSERT(QDir::home().exists(INI_BASE));
    mQSettings=new QSettings(CHANGABLE_INI_FNAME,QSettings::IniFormat);
    mQSettingPermanent=new QSettings(PERMANENT_INI_FNAME,QSettings::IniFormat);
}

QGenieSettingHelper::~QGenieSettingHelper()
{
    delete mQSettings;
    delete mQSettingPermanent;
}

QVariant    QGenieSettingHelper::value ( const QString & key, const QVariant & defaultValue )
{
    QVariant v;
    if(isPermanent(key))
    {
        v=mQSettingPermanent->value(key,defaultValue);
    }
    else
    {
        v=mQSettings->value(key,defaultValue);
    }
    return v;
}

void	QGenieSettingHelper::setValue ( const QString & key, const QVariant & value )
{
    if(isPermanent(key))
    {
        mQSettingPermanent->setValue(key,value);
    }
    else
    {
        mQSettings->setValue(key,value);
    }
}

bool QGenieSettingHelper::isPermanent(const QString &key)
{
    int i=key.indexOf("/");
    return (i>=0)&& mPermanentDict.contains(key.left(i));
}


void QGenieSettingHelper::tryRecoverIniFile()
{
#ifdef GENIE2_MAINFRAME
    if(!QFile::exists(CHANGABLE_INI_FNAME))
    {
        struct _tTemp{const char *name;QVariant v;}
        cParas[]={
            {TITLE_URL,"http://www.netgear.com/genie"}
            ,{UPDATE_IP,"updates1.netgear.com"}
            ,{UPDATE_PORT,80}

#ifdef Q_OS_WIN32
            ,{UPDATE_DESP_FILE,"/netgeargenie/update/update_{la}.txt"}
#else
             ,{UPDATE_DESP_FILE,"/netgeargenie/mac/update/update_{la}.txt"}
#endif
            ,{SUPDATE_RETRYTIME,3}
            ,{FUPDATE_RETRYTIME,3}
            ,{AD_ADDR,"lrs.netgear.com"}
            ,{AD_PORT,80}
            ,{AD_DESP_ADDR,"/netgeargenie/ad/advsetting.txt"}
			,{AD_AREADESP_ADDR, "/netgeargenie/ad/adv_area_support.txt"}
            ,{PLUGIN_STATICS_IP,"lrs.netgear.com"}
            ,{PLUGIN_STATICS_PORT,80}
            ,{PLUGIN_STATICS_DISP,""}
            ,{INSTALL_INFO_COMMITED,false}
            ,{INSTALL_INFO_DATE,(QDate::currentDate().toString("yyyy-MM-dd"))}
            ,{INSTALL_INFO_TIME,(QTime::currentTime().toString("hh:mm:ss"))}
            ,{FOGOT_PWD_LINK,"https://genie.netgear.com/UserProfile/#ForgotPasswordPlace:"}
            ,{SINGNUP_LINK,"https://genie.netgear.com/UserProfile/#NewUserPlace:"}
            ,{RSVALT_LINK,"http://updates1.netgear.com/readysharevault/ReadySHAREVault-install.exe"}
            ,{SETTING_FCML_LINK,"genie.netgear.com"}
            ,{APPSTORE_LINK,"https://genie.netgear.com/UserProfile/?deviceCategory=DEVICE_CAT_ROUTER"}
           };
        QSettings setting(CHANGABLE_INI_FNAME,QSettings::IniFormat);
        for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
        {
            setting.setValue(cParas[i].name,cParas[i].v);
        }
    }
    if(!QFile::exists(PERMANENT_INI_FNAME))
    {
        struct _tTemp{char *name;QVariant v;}
        cParas[]={
            {PLUGIN_MAP_SHOWNOTIFY,false}
            ,{PLUGIN_ROUTECONF_USER,"admin"}
            ,{PLUGIN_ROUTECONF_PWD,"password"}
            ,{PLUGIN_ROUTECONF_SAVEPWD,"true"}
			,{PLUGIN_ROUTECONF_SAVEIP , "false"}//kai.yan add
            ,{PLUGIN_SMARTNETWORK_USER,""}//larkvm@gmail.com
            ,{PLUGIN_SMARTNETWORK_PWD,""}//12345678
            ,{PLUGIN_SMARTNETWROK_ISREMOTE,false}
            ,{PLUGIN_SMARTNETWORK_UNLOCKED,false}
            ,{PLUGIN_SMARTNETWORK_SAVEPWD,false}
            ,{PLUGIN_SMARTNETWORK_SAVECPSECUR,false}
            ,{PLUGIN_INTERNET_DETAIL,0}
            ,{PLUGIN_TRAFFIC_WARRING,true}
            ,{PLUGIN_TRAFFIC_THRESHOLD,10}
            ,{SHOW_NETWORK_CHANGE,true}
            ,{LANGUAGE_SETTING,-1}
#ifdef Q_OS_WIN32
            ,{PLUGIN_AIRPRINT_PRINTERLIST,""}
#else
            ,{MACOS_AUTOSTART,true}
#endif


        };
        QSettings setting(PERMANENT_INI_FNAME,QSettings::IniFormat);
        for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
        {
            setting.setValue(cParas[i].name,cParas[i].v);
        }
    }
#endif
}


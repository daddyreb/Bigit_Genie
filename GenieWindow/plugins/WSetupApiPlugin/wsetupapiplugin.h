#ifndef WSETUPAPIPLUGIN_H
#define WSETUPAPIPLUGIN_H

#include <QObject>
#include "wsetupapiinterface.h"

class WSetupApiPlugin : public QObject,public WSetupApiInterface
{
    Q_OBJECT
    Q_INTERFACES(WSetupApiInterface)
    Q_PLUGIN_METADATA(IID "com.dragonflow.WSetupApiPlugin.WSetupApiInterface/1.0" FILE "WSetupApiPlugin.json")
public:
    WSetupApiPlugin();
    virtual ~WSetupApiPlugin();
public:
    virtual  bool  get_export_information(QString &str_nicguid,QString &str_ssid,QString  &str_profilename);
    virtual  bool  export_to_file(const QString &str_nicguid,const QString &str_ssid,
                         const QString &str_profilename,const QString &str_filename);
    virtual  bool  export_to_file2(const QString &str_ssid,
                         const QString &str_password,const QString &str_filename);
    virtual  bool  export_to_file2_v2(const QString &str_ssid,const QString &str_password,
                         const QString &str_auth,const QString &str_cipher,const QString &str_filename);

    virtual  bool  get_logicaldriver_letters(QStringList &letters);

    virtual  bool  is_logicaldriver_removable(const QString &ldriver);
    virtual  bool  is_logicaldriver_cdrom(const QString &ldriver);
    virtual  bool  is_logicaldriver_system(const QString &ldriver);
};

#endif // WSETUPAPIPLUGIN_H

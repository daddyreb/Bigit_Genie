#ifndef WSETUPAPIINTERFACE_H
#define WSETUPAPIINTERFACE_H
#include <QString>
#include <QStringList>

class WSetupApiInterface
{
public:
    virtual  ~WSetupApiInterface(){}
public:
    virtual  bool  get_export_information(QString &str_nicguid,QString &str_ssid,QString  &str_profilename)=0;
    virtual  bool  export_to_file(const QString &str_nicguid,const QString &str_ssid,
                         const QString &str_profilename,const QString &str_filename)=0;
    virtual  bool  export_to_file2(const QString &str_ssid,
                         const QString &str_password,const QString &str_filename)=0;
    virtual  bool  export_to_file2_v2(const QString &str_ssid,const QString &str_password,
                         const QString &str_auth,const QString &str_cipher,const QString &str_filename)=0;

    virtual  bool  get_logicaldriver_letters(QStringList &letters)=0;

    virtual  bool  is_logicaldriver_removable(const QString &ldriver)=0;
    virtual  bool  is_logicaldriver_cdrom(const QString &ldriver)=0;
    virtual  bool  is_logicaldriver_system(const QString &ldriver)=0;
};

Q_DECLARE_INTERFACE(WSetupApiInterface,"com.dragonflow.WSetupApiPlugin.WSetupApiInterface/1.0")
#endif // WSETUPAPIINTERFACE_H

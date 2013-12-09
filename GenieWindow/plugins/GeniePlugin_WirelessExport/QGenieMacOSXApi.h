#ifndef QGENIEMACOSXAPI_H
#define QGENIEMACOSXAPI_H

#include <QStringList>

class QGenieMacOSXApi
{
public:
    QGenieMacOSXApi();
public:
    static QStringList get_volumes_list();
    static bool export_to_file(const QString &ssid,const QString &pwd,
                        const QString &auth,const QString &cipher,const QString &filename);
    static QString get_save_path(const QString &save_dir,const QString &filename);

private:
    static void build_config_string(const QString &ssid,const QString &pwd,bool bnetworkKey_or_passPhrase,
                                    const QString &auth,const QString &cipher,QString &cfg_str);
    static bool save_to_file(const QString &filename,const QString &cfg_str);
};

#endif // QGENIEMACOSXAPI_H

#ifndef QGENIEWLANKEYPERSISTENT_H
#define QGENIEWLANKEYPERSISTENT_H

#include <QString>

class QGenieWlanKeyPersistent
{
public:
    QGenieWlanKeyPersistent();
public:
    static QString load_key(const QString &ssid);
    static void save_key(const QString &ssid,const QString &key);
};

#endif // QGENIEWLANKEYPERSISTENT_H

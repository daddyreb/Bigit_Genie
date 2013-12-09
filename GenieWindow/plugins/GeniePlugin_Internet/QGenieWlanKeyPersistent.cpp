#include "QGenieWlanKeyPersistent.h"
#include "genie2_interface.h"
#include <QVariant>
#include <QByteArray>

#define INI_SECTION "Internet_Wireless"

QGenieWlanKeyPersistent::QGenieWlanKeyPersistent()
{
}

QString QGenieWlanKeyPersistent::load_key(const QString &ssid)
{
    QByteArray byteKey(ssid.toStdString().c_str());

    QString strkey(INI_SECTION);
    strkey += "/";
    strkey += byteKey.toBase64().data();

    QString str_data = GENIE2_GET_SETTING(strkey).toString();

    return QString::fromLatin1(QByteArray::fromBase64(str_data.toStdString().c_str()));
}

void  QGenieWlanKeyPersistent::save_key(const QString &ssid,const QString &key)
{
    QByteArray byteKey(ssid.toStdString().c_str());
    QByteArray byteData(key.toStdString().c_str());

    QString strkey(INI_SECTION);
    strkey += "/";
    strkey += byteKey.toBase64().data();

    QString str_data = byteData.toBase64().data();
    GENIE2_SET_SETTING(strkey,str_data);
}

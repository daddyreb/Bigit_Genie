#ifndef QGENIESETTINGHELPER_H
#define QGENIESETTINGHELPER_H
#include <QSettings>
#include <QStringList>

class QGenieSettingHelper
{
public:
    QGenieSettingHelper();
    ~QGenieSettingHelper();
    QVariant	value ( const QString & key, const QVariant & defaultValue = QVariant() ) ;
    void	setValue ( const QString & key, const QVariant & value );

    QSettings *mQSettings;
    QSettings *mQSettingPermanent;
    QStringList mPermanentDict;
     void tryRecoverIniFile();
 protected:
    bool isPermanent(const QString &key);

};

#endif // QGENIESETTINGHELPER_H

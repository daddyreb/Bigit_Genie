#ifndef QGENIESETTINGS_H
#define QGENIESETTINGS_H

#include <QSettings>
#include <QMutex>
class QGenieSettingHelper;

class QGenieSettings
{
public:
    QGenieSettings();
    ~QGenieSettings();
    QVariant	value ( const QString & key, const QVariant & defaultValue = QVariant() ) ;
    void	setValue ( const QString & key, const QVariant & value );
    void setValueToDisk(const QString & key, const QVariant & value );
    void writeToDisk();
    void setingCmd(const QVariant &para_list);

protected:
    QMap<QString,QVariant> mMap;
    QMutex mMutex;
public:
    QGenieSettingHelper *mSettingHelper;

};

#endif // QGENIESETTINGS_H

#ifndef QLAUNCHFILEDISCRIPTION_H
#define QLAUNCHFILEDISCRIPTION_H
#include <QString>
#include <QSettings>

class QLaunchFileDiscription
{
public:
    QLaunchFileDiscription(QString para);
    ~QLaunchFileDiscription();
    QString getPath();
    QString getReleaseNote();
    QString getMd5();
    QString getVersionNum();
    QString getHost();
    int getPort();
protected:
    QString para;
    QSettings mSettings;
    QMap<QString,QString> mSettingMap;
};

#endif // QLAUNCHFILEDISCRIPTION_H

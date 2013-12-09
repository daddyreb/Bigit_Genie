#ifndef QGENIEWIFIPROFILEMANAGERCONTROLLER_H
#define QGENIEWIFIPROFILEMANAGERCONTROLLER_H
////this file is bond to win32///////////////

#include <QObject>
#include <QString>
#include <QStringList>

class QGenieWiFiProfileManagerController : public QObject
{
    Q_OBJECT
public:
    explicit QGenieWiFiProfileManagerController(QObject *parent = 0);

private:
    QString     m_curInterface;
    QStringList m_profileList;

private:
    QString getProfileInfo(const QString &profileXml,const QString &nodeType);
public:
    bool updateProfileList(QStringList &profiles);
    bool deleteProfile(int index);
    bool addProfile(const QString &ssid, const QString &auth,
                    const QString &cipher, const QString &pwd, bool nBroadcast);
    bool moveUpProfile(int index);
    bool moveDownProfile(int index);

    bool getProfileAuthAndConnType(const QString &profileName,QString &auth,bool &binfrastructure);
signals:
public slots:

};

extern QGenieWiFiProfileManagerController g_WifiProfileManager;

#endif // QGENIEWIFIPROFILEMANAGERCONTROLLER_H

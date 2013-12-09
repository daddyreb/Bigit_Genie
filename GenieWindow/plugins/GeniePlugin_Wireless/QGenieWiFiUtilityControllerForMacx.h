#ifndef QGENIEWIFIUTILITYCONTROLLERFORMACX_H
#define QGENIEWIFIUTILITYCONTROLLERFORMACX_H

#include <QObject>
#include <QLocalSocket>
#include <QString>
#include <QByteArray>
#include <QStringList>

#include "QGeniePrivilegeProcess.h"

class QGenieWiFiUtilityControllerForMacx : public QObject
{
    Q_OBJECT
public:
    explicit QGenieWiFiUtilityControllerForMacx(QObject *parent = 0);
private:
    QGeniePrivilegeProcess      m_utilityProcess;
    QLocalSocket                m_socket;
    QByteArray                  m_cmdLineBuffer;
    QStringList                 m_cmdLines;
public slots:
    bool init();
//private:
//    QString getFullServerName();
private:
    bool readCommandLine(QString &cmd);
    bool writeCommandLine(const QString &cmd);
    void reset();
signals:

private slots:
    void finished (int exitCode,QProcess::ExitStatus exitStatus );
public:
    bool updateProfileList(QStringList &profiles);
    bool deleteProfile(int index);
    bool addProfile(const QString &ssid,
                    const QString &sec,
                    const QString &pwd);
    bool moveUpProfile(int index);
    bool moveDownProfile(int index);
    bool getProfileAuthAndConnType(const QString &profileName,QString &auth,bool &binfrastructure);

private:
    bool executeSimpleCmd(const QString &action, int index);
};

extern QGenieWiFiUtilityControllerForMacx g_WifiProfileManager;

#endif // QGENIEWIFIUTILITYCONTROLLERFORMACX_H

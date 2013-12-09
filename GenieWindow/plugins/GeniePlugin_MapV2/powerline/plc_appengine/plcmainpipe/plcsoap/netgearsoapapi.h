#ifndef NETGEARSOAPAPI_H
#define NETGEARSOAPAPI_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QStringList>

#include "mainpipedefines.h"


class NETGEARSoapApi : private QThread
{
    Q_OBJECT
public:
    explicit NETGEARSoapApi(QObject *parent = 0);
    ~NETGEARSoapApi();

public:
    bool Start();
    void Stop();

public:
    int GetAttachDevice(const QString &host, quint16 port, QList<AttachDeviceEntry> &attachDevs);
    int GetUSBType(const QString &host, quint16 port, QStringList &usbTypes);
    int GetUSBDesc(const QString &host, quint16 port, QStringList &usbDescs);
    int GetWPSConfigureState(const QString &host, quint16 port, bool &configured);
    int GetWLANInfo(const QString &host, quint16 port, WLANInfo &wlanInfo);
    int SetWLANInfo(const QString &host, quint16 port, const WLANInfo &wlanInfo);

private:
    int ConfigurationStarted(const QString &host, quint16 port);
    int ConfigurationFinished(const QString &host, quint16 port);

protected:
    virtual void run();

signals:
    void SubmitTransactionSignal(const QString &module,
                                 const QString &method,
                                 const QMap<QString, QString> *param,
                                 const QObject *resultReceiver,
                                 const char* slot,
                                 int timeoutSeconds);

    void SubmitTransactionSignal(const QString &host,
                                 quint16 port,
                                 const QString &module,
                                 const QString &method,
                                 const QMap<QString, QString> *param,
                                 const QObject *resultReceiver,
                                 const char* slot,
                                 int timeoutSeconds);

private:
    void SubmitTransaction(const QString &host,
                           quint16 port,
                           const QString &module,
                           const QString &method,
                           const QMap<QString, QString> *param,
                           const QObject *resultReceiver,
                           const char* slot,
                           int timeoutSeconds = -1);
    void SubmitTransaction(const QString &module,
                           const QString &method,
                           const QMap<QString, QString> *param,
                           const QObject *resultReceiver,
                           const char* slot,
                           int timeoutSeconds = -1);

private:
    bool InitAttachDeviceEntry(const QStringList &devComponents, AttachDeviceEntry &adentry);

private:
    QMutex m_initMutex;
    QWaitCondition m_initCdt;
    bool m_initCompleted;

    QMutex m_thisThreadLocker;//protect the base QThread object when access from mutilthread
};

#endif // NETGEARSOAPAPI_H

#ifndef QGENIECONNECTTOWLANTHREAD_H
#define QGENIECONNECTTOWLANTHREAD_H

#include <QThread>
#include <QString>

class QGenieConnectToWlanThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieConnectToWlanThread(QObject *parent = 0);
    ~QGenieConnectToWlanThread();
protected:
    QString         m_strnicguid;
    QString         m_strssid;
    QString         m_strpwd;
    bool            m_buseprofile;
private:
#ifdef Q_OS_WIN
    bool waitdhcp_forgateway(const QString& nicguid);
#endif
protected:
    void run();
public:
    bool begin_connect(const QString &nicguid,const QString &ssid,const QString &pwd,bool buseprofile = false);
signals:
    void connect_complete(bool bok);
public slots:

};

#endif // QGENIECONNECTTOWLANTHREAD_H

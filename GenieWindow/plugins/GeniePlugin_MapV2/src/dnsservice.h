#ifndef DNSSERVICE_H
#define DNSSERVICE_H

#include <QThread>

class dnsservice : public QThread
{
    Q_OBJECT
public:
    explicit dnsservice(QObject *parent = 0);

    void run();
    void stopService();
signals:
    void srvStarted(bool bSrvStarted);

public slots:
	void onRebootService();
};

#endif // DNSSERVICE_H

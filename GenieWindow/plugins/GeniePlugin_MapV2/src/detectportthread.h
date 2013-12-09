#ifndef DETECTPORTTHREAD_H
#define DETECTPORTTHREAD_H

#include <QThread>
#include <QString>

class detectportthread : public QThread
{
    Q_OBJECT
public:
    explicit detectportthread(QString strIp, QString strServiceName, QObject *parent = 0);
    void run();

signals:
    void portOffline(QString strIp, QString strServiceName);

public slots:

private:
    QString m_strIp;
    QString m_strServiceName;

};

#endif // DETECTPORTTHREAD_H

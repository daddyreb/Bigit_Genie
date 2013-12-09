#ifndef QGENIEWZCMONITORTHREAD_H
#define QGENIEWZCMONITORTHREAD_H

#include <QThread>

class QGenieWzcMonitorThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieWzcMonitorThread(QObject *parent = 0);
    ~QGenieWzcMonitorThread();
protected:
    int m_iwzc_runningstate;
    int m_iwzc_autoconfstate;
    volatile bool m_bexit;
protected:
    void run();
signals:
    void wzc_runningstate_changed(bool brunning);
    void wzc_autoconfstate_changed(const QString &nic_guid,bool enabled);
public slots:
public:
    void end_monitor();

};

#endif // QGENIEWZCMONITORTHREAD_H

#ifndef QGENIEREFRESHWIRELESSSTATETHREAD_H
#define QGENIEREFRESHWIRELESSSTATETHREAD_H

#include <QThread>

class QGenieRefreshWirelessStateThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieRefreshWirelessStateThread(QObject *parent = 0);
    ~QGenieRefreshWirelessStateThread();
protected:
    volatile bool            m_bexit;
    int                      m_cur_signalLevel;
public:
    bool begin_refresh();
    void end_refresh();
protected:
    void run();
signals:
    void wirelessstate_changed(const QString &nic, int newstate, int signallevel = 0);
    void update_ssid_connected(const QString &nic);

};

#endif // QGENIEREFRESHWIRELESSSTATETHREAD_H

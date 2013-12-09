#ifndef REFRESHSTATUSTHREAD_H
#define REFRESHSTATUSTHREAD_H

#include <QThread>
#include <QSemaphore>


class QGenieConnectionStateDetecter;

class QGenieRefreshStatusThread : public QThread
{
    Q_OBJECT
public:
    QGenieRefreshStatusThread(QObject *parent = 0);
    ~QGenieRefreshStatusThread();
protected:
    volatile bool                   m_bexit;
    bool                            m_state_changed;
    int                             m_Internet_Status;
    QGenieConnectionStateDetecter   *m_detecter;
protected:
    virtual void run();
public:
    void end_refresh();
signals:
    void internet_state_changed(int new_state);
};

#endif // REFRESHSTATUSTHREAD_H

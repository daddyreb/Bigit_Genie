#ifndef QGENIETHREADTIMER_H
#define QGENIETHREADTIMER_H

#include <QThread>
#include <QMutex>

class QGenieThreadTimer : public QThread
{
Q_OBJECT
public:
    explicit QGenieThreadTimer(QObject *parent = 0);
    ~QGenieThreadTimer();
protected:
    int     m_interval_seconds;
    int     m_interval_counter;
    bool    m_bexit;
    QMutex  m_interval_mulock;
public:
    void setInterval(int seconds);
    void start();
    void stop();
protected:
    virtual void run();
signals:
    void timeout();
public slots:

};

#endif // QGENIETHREADTIMER_H

#include "QGenieThreadTimer.h"
#include <QtDebug>
#include <assert.h>

QGenieThreadTimer::QGenieThreadTimer(QObject *parent) :
    QThread(parent),
    m_interval_seconds(0),
    m_interval_counter(0),
    m_bexit(false)
{
}

QGenieThreadTimer::~QGenieThreadTimer()
{
    this->stop();
    this->wait();
}

void QGenieThreadTimer::setInterval(int seconds)
{
    assert (seconds > 0);
    m_interval_mulock.lock();
    m_interval_counter = m_interval_seconds = seconds;
    m_interval_mulock.unlock();
}

void QGenieThreadTimer::start()
{
    m_interval_mulock.lock();
    m_interval_counter = m_interval_seconds;
    m_interval_mulock.unlock();

    if(!QThread::isRunning())
    {
        m_bexit = false;
        QThread::start();
    }
}

void QGenieThreadTimer::stop()
{
    m_bexit = true;
}


void QGenieThreadTimer::run()
{
    while(!m_bexit)
    {
        QThread::msleep(1000);//delay one second
        m_interval_mulock.lock();
        if(--m_interval_counter <= 0)
        {
            emit timeout();
            m_interval_counter = m_interval_seconds;//reset counter
            qDebug () << "time out";
        }
        m_interval_mulock.unlock();
    }
}

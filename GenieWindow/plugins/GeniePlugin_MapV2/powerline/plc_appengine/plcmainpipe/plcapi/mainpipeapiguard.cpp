#include "mainpipeapiguard.h"
#include <QMutex>
#include <QWaitCondition>

MainPipeApiGuard::MainPipeApiGuard(int *refCountP, QMutex *locker, QWaitCondition *cdt) :
    m_refCountP(refCountP)
  ,m_locker(locker)
  ,m_cdt(cdt)
  ,m_progressOk(false)
{
    m_locker->lock();
    ++*m_refCountP;
    m_progressOk = (*m_refCountP > 1);
    m_locker->unlock();
}

MainPipeApiGuard::~MainPipeApiGuard()
{
    m_locker->lock();
    if(--*m_refCountP <= 0){
        m_cdt->wakeAll();
    }
    m_locker->unlock();
}

bool MainPipeApiGuard::CanProgress() const
{
    return m_progressOk;
}

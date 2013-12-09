//mac os x platform implementaion
#include "QGeniePrivilegeProcess.h"
#include <QtDebug>
#include <QMetaType>
#include <Security/Security.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

QGeniePrivilegeProcess::QGeniePrivilegeProcess(QObject *parent) :
        QThread(parent),
        m_state(QProcess::NotRunning),
        m_pipe(NULL),
        m_pid(-1),
        m_bstart_complete(false),
        m_bfinished(false)
{
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
}

void QGeniePrivilegeProcess::run()
{
    AuthorizationRef    myAuthorizationRef = NULL;
    OSStatus            myStatus           = noErr;
    const char          **params           = NULL;

    if( m_pipe != NULL )
    {
        fclose(m_pipe);
        m_pipe = NULL;
    }

    myStatus = AuthorizationCreate (NULL, kAuthorizationEmptyEnvironment,
                                    kAuthorizationFlagDefaults, &myAuthorizationRef);

    if(myStatus != noErr)
    {
        goto failed_to_start;
    }


    params = new const char*[m_arglist.count()+1];

    for(int i = 0 ; i < m_arglist.count() ; ++i)
    {
        params[i] = new char[strlen(m_arglist[i].toStdString().c_str())+1];
        strcpy((char*)params[i],m_arglist[i].toStdString().c_str());
    }

    params[m_arglist.count()] = NULL;

    myStatus = AuthorizationExecuteWithPrivileges(myAuthorizationRef,m_pragram.toStdString().c_str(),kAuthorizationFlagDefaults,(char* const*)params,&m_pipe);

    for(int i = 0;i < m_arglist.count(); ++i)
    {
        delete [] params[i];
    }

    delete [] params;
    params = NULL;

    AuthorizationFree (myAuthorizationRef,
                       kAuthorizationFlagDestroyRights);

    if(noErr != myStatus)
    {
        goto failed_to_start;
    }

    //-------------
    if( fread(&m_pid,sizeof(pid_t),1,m_pipe) == 1 )
    {
        //start ok
        m_mutex_state.lock();
        m_state = QProcess::Running;
        m_mutex_state.unlock();

        m_mutex_start.lock();
        m_bstart_complete = true;
        m_mutex_start.unlock();

        emit started();

        m_condition_start.wakeAll();

        again:
        if( waitpid(m_pid,NULL,0) != m_pid )
        {
            if (errno == EINTR)
            {
                goto again;
            }
            else
            {
                //exception occured so exit thread
                m_mutex_state.lock();
                m_state = QProcess::NotRunning;
                m_mutex_state.unlock();

                m_mutex_finish.lock();
                m_bfinished = true;
                m_mutex_finish.unlock();

                emit finished(0,QProcess::NormalExit);

                m_condition_finish.wakeAll();
            }

        }
        else
        {
            //process finished
            m_mutex_state.lock();
            m_state = QProcess::NotRunning;
            m_mutex_state.unlock();

            m_mutex_finish.lock();
            m_bfinished = true;
            m_mutex_finish.unlock();

            emit finished(-1,QProcess::CrashExit);

            m_condition_finish.wakeAll();
        }

        return;
    }


    failed_to_start:
    m_mutex_state.lock();
    m_state = QProcess::NotRunning;
    m_mutex_state.unlock();

    m_mutex_start.lock();
    m_bstart_complete = true;
    m_mutex_start.unlock();

    m_condition_start.wakeAll();

    m_mutex_finish.lock();
    m_bfinished = true;
    m_mutex_finish.unlock();

    m_condition_finish.wakeAll();

    emit error(QProcess::FailedToStart);

}

void QGeniePrivilegeProcess::start ( const QString & program, const QStringList & arguments )
{
    if( m_state != QProcess::NotRunning )
    {
        return;
    }

    assert ( !program.isEmpty() );

    m_pragram           = program;
    m_arglist           = arguments;

    m_pid               = -1;
    m_bstart_complete   = false;
    m_bfinished         = false;
    m_state             = QProcess::Starting;

    QThread::start();
}

QProcess::ProcessState	QGeniePrivilegeProcess::state () const
{
    m_mutex_state.lock();
    QProcess::ProcessState result = m_state;
    m_mutex_state.unlock();

    return result;
}

bool QGeniePrivilegeProcess::waitForFinished ( int msecs )
{
    bool bret = false;

    m_mutex_finish.lock();
    if(!m_bfinished)
    {
        bret = m_condition_finish.wait(&m_mutex_finish);
    }
    m_mutex_finish.unlock();

    return bret;
}

bool QGeniePrivilegeProcess::waitForStarted ( int msecs )
{
    bool bret = true;

    m_mutex_start.lock();

    if(!m_bstart_complete)
    {
        bret = m_condition_start.wait(&m_mutex_start);
    }

    m_mutex_start.unlock();

    if(bret)
    {
        m_mutex_state.lock();
        bret = (m_state == QProcess::Running);
        m_mutex_state.unlock();
    }

    return bret;
}

void QGeniePrivilegeProcess::kill ()
{
    if(m_pid > 0)
    {
        ::kill(m_pid,SIGTERM);
    }
}

#ifndef QGENIEPRIVILEGEPROCESS_H
#define QGENIEPRIVILEGEPROCESS_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QString>
#include <QStringList>

class QGeniePrivilegeProcess : public QThread
{
    friend class QGenieProcessMonitorThread;

    Q_OBJECT
public:
    explicit QGeniePrivilegeProcess(QObject *parent = 0);
protected:
    QString                     m_pragram;
    QStringList                 m_arglist;
    QProcess::ProcessState      m_state;
    FILE                        *m_pipe;
    pid_t                       m_pid;
public:
    void start ( const QString & program, const QStringList & arguments );
    QProcess::ProcessState state () const;
    bool waitForFinished ( int msecs = 30000 );
    bool waitForStarted ( int msecs = 30000 );
signals:
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void started ();
    void error ( QProcess::ProcessError error );
public slots:
    void kill ();
protected:
    void run();
protected:
    QWaitCondition  m_condition_start;
    QMutex          m_mutex_start;
    bool            m_bstart_complete;

    QWaitCondition  m_condition_finish;
    QMutex          m_mutex_finish;
    bool            m_bfinished;

    mutable QMutex  m_mutex_state;
};

#endif // QGENIEPRIVILEGEPROCESS_H

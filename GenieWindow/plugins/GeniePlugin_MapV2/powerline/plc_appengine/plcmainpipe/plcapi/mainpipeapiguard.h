#ifndef MAINPIPEAPIGUARD_H
#define MAINPIPEAPIGUARD_H

class QMutex;
class QWaitCondition;

class MainPipeApiGuard
{
public:
    MainPipeApiGuard(int *refCountP, QMutex *locker, QWaitCondition *cdt);
    ~MainPipeApiGuard();
    bool CanProgress() const;

private:
    MainPipeApiGuard(const MainPipeApiGuard &other);
    MainPipeApiGuard & operator = (const MainPipeApiGuard &right);

private:
    int *m_refCountP;
    QMutex *m_locker;
    QWaitCondition *m_cdt;
    bool m_progressOk;
};

#endif // MAINPIPEAPIGUARD_H

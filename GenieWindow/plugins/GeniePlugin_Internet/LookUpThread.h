#ifndef LOOKUPTHREAD_H
#define LOOKUPTHREAD_H

#include <QThread>
#include <QHostInfo>

class LookUpThread : public QThread
{
    Q_OBJECT
public:
    LookUpThread();
protected:
    void run();
signals:
    void SendStatus(bool );
};

#endif // LOOKUPTHREAD_H

#ifndef PLCDEVICESOAPTHREAD_H
#define PLCDEVICESOAPTHREAD_H

#include <QThread>

class PLCDevice;
class MainPipeInterface;

class PLCDeviceSoapThread : public QThread
{
    Q_OBJECT
public:
    explicit PLCDeviceSoapThread(QObject *parent = 0);
    ~PLCDeviceSoapThread();

signals:
    
public slots:

public:
    void begin(PLCDevice *device, MainPipeInterface *mainPipeInf);

private:
    PLCDevice *m_device;
    MainPipeInterface *m_mainPipeInf;

protected:
    void run();
};

#endif // PLCDEVICESOAPTHREAD_H

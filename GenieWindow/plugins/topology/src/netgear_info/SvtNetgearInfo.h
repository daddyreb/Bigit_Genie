#ifndef SVTNETGEARINFO_H
#define SVTNETGEARINFO_H

#include <QThread>
#include <QHostInfo>



class SVT_NetgearInfoThread : public QThread
{
    Q_OBJECT
public:
    SVT_NetgearInfoThread();
//    void startGetNetgearInfo(const QString &addr);
protected:
    void run();

protected:
    QString mAddr;

};

#endif // SVTNETGEARINFO_H

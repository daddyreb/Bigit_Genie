#ifndef QGENIEREFRESHVISIBLENETWORKTHREAD_H
#define QGENIEREFRESHVISIBLENETWORKTHREAD_H

#include <QThread>
#include <QList>
#include <QString>

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
#elif defined Q_OS_MACX
#include <QMap>
#endif

#include <QMutex>
#include <QWaitCondition>

class QGenieRefreshVisibleNetworkThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieRefreshVisibleNetworkThread(QObject *parent = 0);
    ~QGenieRefreshVisibleNetworkThread();
private:
    enum {VNT_WAIT,VNT_RUN,VNT_QUIT};
    int m_vntState;

    QMutex          m_mutex;
    QWaitCondition  m_condition;
private:
#ifdef Q_OS_WIN
    QList<VisibleNetwork>              *m_lpvisible_network;
#elif defined Q_OS_MACX
    QList<QMap<QString,QString> >      *m_lpnetwork_maplist;
#endif

protected:
    void run();
    bool scanNetworks(QString &nicguid);
public:
#ifdef Q_OS_WIN
    void set_out_vnlist(QList<VisibleNetwork>  *pvn_list);
#elif defined Q_OS_MACX
    void set_out_vn_maplist(QList<QMap<QString,QString> >  *pvn_maplist);
#endif

signals:
    void refreshCompleted(bool bok,const QString &nicguid);

public slots:
    void start(Priority priority = InheritPriority);
    void quit();
};

#endif // QGENIEREFRESHVISIBLENETWORKTHREAD_H

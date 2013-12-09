#ifndef QGENIEREFRESHVISIBLENETWORKTHREAD_H
#define QGENIEREFRESHVISIBLENETWORKTHREAD_H

#include <QThread>
#include <QList>
#include <QMap>
#include <QString>

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
#elif defined Q_OS_MACX
#endif


class QGenieRefreshVisibleNetworkThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieRefreshVisibleNetworkThread(QObject *parent = 0);
    ~QGenieRefreshVisibleNetworkThread();
private:
#ifdef Q_OS_WIN
    QList<VisibleNetwork>               *m_lpvisible_network;
#elif defined Q_OS_MACX
    QList<QMap<QString,QString> >       *m_lpvisible_networkmaplist;
#endif
    QString                             m_strnic_guid;
protected:
    void run();
public:
    bool begin_refresh();
#ifdef Q_OS_WIN
    void set_out_vnlist(QList<VisibleNetwork>  *pvn_list);
#elif defined Q_OS_MACX
    void set_out_vnmaplist(QList<QMap<QString,QString> > *pvn_maplist);
#endif
    void set_nicguid(const QString &nicguid);
signals:
    void refresh_complete(bool bok);
public slots:

};

#endif // QGENIEREFRESHVISIBLENETWORKTHREAD_H

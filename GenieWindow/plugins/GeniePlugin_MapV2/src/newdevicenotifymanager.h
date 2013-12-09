#ifndef NEWDEVICENOTIFYMANAGER_H
#define NEWDEVICENOTIFYMANAGER_H

#include <QObject>
#include "topology.h"

class NewDeviceNotifyWidget;

class NewDeviceNotifyManager : public QObject
{
    Q_OBJECT
public:
    explicit NewDeviceNotifyManager(QObject *parent = 0);
    ~NewDeviceNotifyManager();

private:
    bool                    m_isNotifying;
    NewDeviceNotifyWidget   *m_ndnWidget;
    SVT_NotifyType          m_cachedNewDevicesPending2Notify;

private:
    void notifyOver();

signals:
    void translateText(int idx ,QString *text);

public slots:
    void onNewDeviceNotify(SVT_NotifyType data);
private slots:
    void onNotifyTimeout();
    void onNotifyCompleted(int countRemain);
};

#endif // NEWDEVICENOTIFYMANAGER_H

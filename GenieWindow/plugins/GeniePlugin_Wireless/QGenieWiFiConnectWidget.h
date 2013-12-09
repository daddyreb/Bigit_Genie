#ifndef QGENIEWIFICONNECTWIDGET_H
#define QGENIEWIFICONNECTWIDGET_H

#include "QGenieVisibleNetworkWidget.h"
#ifdef ENABLE_WIFIPROFILE_MANAGER
#include "QGenieWiFiProfileManagerWidget.h"
#endif
#include <QWidget>

class QGenieWiFiConnectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGenieWiFiConnectWidget(QWidget *parent = 0);

private:
    QGenieVisibleNetworkWidget      *m_vnWidget;
#ifdef ENABLE_WIFIPROFILE_MANAGER
    QGenieWiFiProfileManagerWidget  *m_wpmWidget;
#endif
signals:

public slots:
    void onManager();
    void onNetworkList();
public:
#ifdef Q_OS_WIN
    void wzc_autoconfstate_changed(const QString &nic_guid,bool enabled);
#endif

#ifdef ENBABLE_UPDATEWIRELESS_REALTIME
public:
    void try_refreshnetwork();
#endif
};

#endif // QGENIEWIFICONNECTWIDGET_H

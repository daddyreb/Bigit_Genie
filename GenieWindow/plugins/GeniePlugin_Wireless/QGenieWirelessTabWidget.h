#ifndef QGENIEWIRELESSTABWIDGET_H
#define QGENIEWIRELESSTABWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QEvent>

#include "QGenieWirelessChannelView.h"
#include "QGenieWiFiConnectWidget.h"

#ifdef Q_OS_WIN
#include "QGenieWzcMonitorThread.h"
#endif

class QGenieWirelessTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGenieWirelessTabWidget(QWidget *parent = 0);
    ~QGenieWirelessTabWidget();
protected:
    QTabWidget                                  *m_tabwidget;
protected:
    QGenieWirelessChannelView                   *m_wchannel_view;
    QGenieWiFiConnectWidget                     *m_wificonnect_widget;
    QWidget                                     *m_psudo_view;
    QWidget                                     *m_routerconfiguration_widget;

    int                                         m_cur_tab_index;
    int                                         m_wireless_state;

#ifdef Q_OS_WIN
    QGenieWzcMonitorThread                      m_wzcmonitor_thread;
#endif

    QString                                     m_current_ssid;

protected:
    void init_tab();
    void retranslateUi();
public:
    void update_bigbutton_ui();
    void enableChangeChannel(bool enable);
    void enablegotoguestaccess(bool enable);

protected:
    virtual void changeEvent(QEvent *);
signals:

private slots:
#ifdef Q_OS_WIN
    void wzc_runningstate_changed(bool brunning);
    void wzc_autoconfstate_changed(const QString &nic_guid,bool enabled);
#endif
    void wirelessstate_changed(const QString &nic, int newstate,int signallevel);
    void currentChanged ( int index ) ;
protected slots:
    void update_ssid_connected(const QString &ssid);
public slots:
    void notifyRaise();

};

#endif // QGENIEWIRELESSTABWIDGET_H

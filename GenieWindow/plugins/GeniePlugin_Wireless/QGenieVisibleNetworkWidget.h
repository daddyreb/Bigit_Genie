#ifndef QGENIEVISIBLENETWORKWIDGET_H
#define QGENIEVISIBLENETWORKWIDGET_H

#include "QGenieInputPasswordWidget.h"
#include "QGenieAvailableNetworklist.h"
#include "QGenieRefreshVisibleNetworkThread.h"
#include "QGenieConnectToWlanThread.h"
#include "QGenieProgressPanel.h"

#include <QtCore>
#include <QtGui>
//#include <QStackedLayout>

class QGenieVisibleNetworkWidget : public QWidget
{
    Q_OBJECT
public:
    QGenieVisibleNetworkWidget(QWidget *parent=0);
    ~QGenieVisibleNetworkWidget();
public:
#ifdef Q_OS_WIN
    void wzc_autoconfstate_changed(const QString &nic_guid,bool enabled);
#endif
protected:
    QPushButton                                   *refresh_btn;
    QPushButton                                   *connect_btn;
    QPushButton                                   *manager_btn;
    QLabel                                        *optprompt_lb;
    QGenieAvailableNetworkList                    *available_network_list;
    QGenieProgressPanel                           *m_progress_panel;
    QGenieAvailableNetworkItem                    *m_cur_toconnect_item;
protected:
    QGenieRefreshVisibleNetworkThread              m_refreshvn_thread;

#ifdef Q_OS_WIN
    QList<VisibleNetwork>                          m_visible_network;
#elif defined Q_OS_MACX
    QList<QMap<QString,QString> >                  m_network_maplist;
#endif

    QGenieConnectToWlanThread                      m_connectwlan_thread;
    bool                                           m_bconnect_or_disconnect;
    bool                                           m_bconnecting_or_refreshing;
    int                                            m_optprompt_index;
protected:
    void retranslateUi();
    void assert_connect_btn_state();
protected:
    virtual void changeEvent(QEvent *event);
    virtual void  showEvent ( QShowEvent * event ) ;
private:
    void do_connect_disconnect(const QString &ssid,const QString &pwd,bool buseprofile = false);
    void beginRefresh();
    //void refresh_netgearrouter();
    bool connect_wireless_tryagin();
protected slots:
    void connectBtnClicked();
    void refreshBtnClicked();
    void networklist_itemselectionchanged();
    void networklist_itemdoubleclicked(QListWidgetItem * item );
    void refresh_complete(bool bok);
    void connect_complete(bool bok);

    void onRefreshCompleted(bool bok , const QString &nicguid);
protected:
    QString m_strnicguid;
    QString m_strssid;
    QString m_strPwd;
#ifdef ENBABLE_UPDATEWIRELESS_REALTIME
public:
    void try_refreshnetwork();
#endif
signals:
    void Manager();
};

#endif //QGENIEVISIBLENETWORKWIDGET_H

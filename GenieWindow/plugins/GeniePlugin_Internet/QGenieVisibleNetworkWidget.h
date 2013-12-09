#ifndef QGENIEVISIBLENETWORKWIDGET_H
#define QGENIEVISIBLENETWORKWIDGET_H

#include "QGenieInputPasswordWidget.h"
#include "QGenieAvailableNetworkList.h"
#include "QGenieRefreshVisibleNetworkThread.h"
#include "QGenieProgressPanel.h"
#include "QGenieDialogInnerWidget.h"

//#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QEvent>
//#include <QStackedLayout>

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
#elif defined Q_OS_MACX
#endif

class QGenieVisibleNetworkWidget : public /*QDialog*/QGenieDialogInnerWidget
{
    Q_OBJECT
public:
    QGenieVisibleNetworkWidget(const QString &strnicguid,QWidget *parent=0);
protected:
    QPushButton                                             *refresh_btn;
    QPushButton                                             *connect_btn;
    QPushButton                                             *close_btn;
    QLabel                                                   *optprompt_lb;
    //    QTreeWidget                                         *network_list;
    QGenieAvailableNetworkList                              *available_network_list;
    QGenieInputPasswordWidget                               *inputpassword_widget;
    //QStackedLayout                                      *network_list_refresh_layout;
    //QLabel                                                  *refreshing_prompt_lb;
    QGenieProgressPanel                                     *m_progress_panel;
protected:
    QGenieRefreshVisibleNetworkThread                       m_refreshvn_thread;
#ifdef Q_OS_WIN
    QList<VisibleNetwork>                                   m_visible_network;
#elif defined Q_OS_MACX
    QList<QMap<QString,QString> >                           m_visible_networkmaplist;
#endif
    int                                                     m_optprompt_text_idx;
protected:
    void retranslateUi();
    void assert_connect_btn_state();
    //    bool valid_password(const QString &auth,const QString &cipher,const QString &key);
protected:
    virtual void changeEvent(QEvent *event);
public:
    virtual QString get_dlg_title();
private:
    //    void refresh_network_list(const QString &strNicGuid);
    void refresh_availablenetwork_list(const QString &strNicGuid);
protected slots:
    void connect_btn_clicked(bool checked);
    void refresh_btn_clicked(bool checked);
    void close_btn_clicked(bool checked);
    void networklist_itemselectionchanged();
    void networklist_itemdoubleclicked(QListWidgetItem * item );
    void refresh_complete(bool bok);
protected:
    QString m_strnicguid;
    QString m_strssid;
    QString m_strPwd;
public:
    void set_nic_interface(const QString &strnicguid);
    bool get_connect_detail(QString &strSsid,QString &strPwd);
};

#endif //QGENIEVISIBLENETWORKWIDGET_H

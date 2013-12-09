#ifndef QGENIEWIRELESSTAB_H
#define QGENIEWIRELESSTAB_H

#include <QWidget>
#include <QPushButton>
#include <QStackedLayout>
#include <QEvent>

#include "IPluginInf.h"
#include "QGenieWirelessChannelView.h"
#include "QGenieVisibleNetworkWidget.h"

class QGenieWirelessTab : public QWidget
{
Q_OBJECT
public:
    explicit QGenieWirelessTab(QWidget *parent = 0);
protected:
//    QPushButton                               *m_internet_btn;
    QPushButton                               *m_wirelessexport_btn;
    QPushButton                               *m_wirelesschannel_btn;
    QPushButton                               *m_wirelessconnect_btn;
    QPushButton                               *m_routerconfiguration_btn;

    QStackedLayout                          *m_tab_layout;

//    IPluginInf                                    *m_internet_plugin;
    IPluginInf                                    *m_wirelessexport_plugin;
    IPluginInf                                    *m_routerconfiguration_plugin;

//    QWidget                                     *m_internet_widget;
    QWidget                                     *m_wirelessexport_widget;
    QGenieWirelessChannelView    *m_wchannel_view;
    QGenieVisibleNetworkWidget    *m_vn_widget;
    QWidget                                     *m_routerconfiguration_widget;

protected:
    void init_tab();
    void retranslateUi();
protected:
    virtual void changeEvent(QEvent *);
signals:

public slots:
    void btns_clicked(QAbstractButton *btn);
};

#endif // QGENIEWIRELESSTAB_H

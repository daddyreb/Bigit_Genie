#ifndef QGENIECURCHANNELWIDGET_H
#define QGENIECURCHANNELWIDGET_H

#include "QGenieProgressPanel.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QEvent>

class QGenieCurChannelWidget : public QWidget
{
    friend class QGenieWirelessChannelView;
Q_OBJECT
public:
    explicit QGenieCurChannelWidget(QWidget *parent = 0);
protected:
    QLabel                                      *m_cur_network_lb;
    QLabel                                      *m_cur_channel_lb;
    QGenieProgressPanel                         *m_progress_panel;
    QLabel                                      *m_prompt_lb;
    QPushButton                                 *m_refresh_btn;
    QPushButton                                 *m_changechannel_btn;

    int                                          m_cur_channel;
    QString                                      m_cur_ssid;
    bool                                         m_bchannelrefresh_ok;
    bool                                         m_bshowautochannel;
    bool                                         m_changechannel_enable;
private:
    bool refresh_netgearrouter();
protected:
    virtual void changeEvent(QEvent *event);
    void retranslateUi();
protected:
    void begin_refresh();
signals:
    void refresh_request();
private slots:
    void refresh_btn_clicked();
    void changechannel_clicked();
    void onViewCurrentPage(int curPage);
public:
    void loadData(bool brefreshok, const QString &curssid = "", int curchannel = -1, bool bautochannel = false);
    void enableChangeChannel(bool enable);
};

#endif // QGENIECURCHANNELWIDGET_H

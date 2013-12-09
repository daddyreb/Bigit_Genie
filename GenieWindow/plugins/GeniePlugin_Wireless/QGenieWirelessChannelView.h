#ifndef QGENIEWIRELESSCHANNELVIEW_H
#define QGENIEWIRELESSCHANNELVIEW_H

#include <QFrame>
#include <QShowEvent>
#include <QTimer>

#include "QGenieCurChannelWidget.h"
#include "QGenieHistogramWidget.h"
#include "QGenieRefreshWChannelViewThread.h"
#include "WifiChannelData.h"


class QGenieWirelessChannelView : public QFrame
{
    Q_OBJECT
public:
    explicit QGenieWirelessChannelView(QWidget *parent = 0);
    ~QGenieWirelessChannelView();
protected:
    QGenieCurChannelWidget              *m_curchannel_widget;
    QGenieHistogramWidget               *m_histogram_widget;
    QGenieRefreshWChannelViewThread     m_refresh_thread;
protected:
    virtual void showEvent(QShowEvent *event);
private:
    void buildChannelTooltip(WifiChannelData *data, int channel, QString &strTooltip);
    void buildSsids2Tooltips(WifiChannelData *data, QStringList &tipsList);
    void buildChannelDataList(WifiChannelData *data, QList<unsigned int> &dataList);
    void buildLabelDataList(WifiChannelData *data, QList<int> &labelList);
private slots:
    void on_refresh_request();
    void refresh_complete(bool bok, WifiChannelData *data);

public:
    void enableChangeChannel(bool enable);
};

#endif // QGENIEWIRELESSCHANNELVIEW_H

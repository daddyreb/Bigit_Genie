#ifndef QGENIEWIRELESSCHANNELLEGENDWIDGET_H
#define QGENIEWIRELESSCHANNELLEGENDWIDGET_H


#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QEvent>

class QGenieWirelessChannelLegendWidget : public QFrame
{
Q_OBJECT
public:
    explicit QGenieWirelessChannelLegendWidget(QWidget *parent = 0);
protected:
    QFrame          *legendFrame;
    QVBoxLayout     *legendFrameVerLayout;
    //QLabel          *legendLB;
    QHBoxLayout     *legendHorLayout;
    QSpacerItem     *horizontalSpacer;
    QVBoxLayout     *legendColorVerLayout;
    QHBoxLayout     *horizontalLayout;
    QLabel          *legendColorMyLB;
    QLabel          *myWiFiLB;
    QHBoxLayout     *horizontalLayout_2;
    QLabel          *legendColorOtherLB;
    QLabel          *otherWiFiLB;
protected:
   void setupUi();
   void retranslateUi();
protected:
   void changeEvent(QEvent *event);
signals:

public slots:

};

#endif // QGENIEWIRELESSCHANNELLEGENDWIDGET_H

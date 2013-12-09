#include "QGenieWirelessChannelLegendWidget.h"
#include "GeniePlugin_Wireless.h"

QGenieWirelessChannelLegendWidget::QGenieWirelessChannelLegendWidget(QWidget *parent) :
    QFrame(parent)
{
    setupUi();
}

void QGenieWirelessChannelLegendWidget::setupUi()
{
    //this->setWindowOpacity(0.1);

    this->setObjectName(QString::fromUtf8("this"));
    this->setGeometry(QRect(50, 80, 100, 30));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    //this->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
    this->setFrameShape(QFrame::StyledPanel);
    this->setFrameShadow(QFrame::Raised);
    legendFrameVerLayout = new QVBoxLayout(this);
    legendFrameVerLayout->setSpacing(1);
    legendFrameVerLayout->setContentsMargins(0, 0, 0, 0);
    legendFrameVerLayout->setObjectName(QString::fromUtf8("thisVerLayout"));
    //legendLB = new QLabel(this);
    //legendLB->setObjectName(QString::fromUtf8("legendLB"));
    //legendLB->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\256\213\344\275\223\";"));

    //legendFrameVerLayout->addWidget(legendLB);
    this->setFrameShadow(QFrame::Plain);
    this->setFrameShape(QFrame::NoFrame);

    legendHorLayout = new QHBoxLayout();
    legendHorLayout->setObjectName(QString::fromUtf8("legendHorLayout"));
    horizontalSpacer = new QSpacerItem(6, 6, QSizePolicy::Fixed, QSizePolicy::Minimum);

    legendHorLayout->addItem(horizontalSpacer);

    legendColorVerLayout = new QVBoxLayout();
    legendColorVerLayout->setSpacing(1);
    legendColorVerLayout->setObjectName(QString::fromUtf8("legendColorVerLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setAlignment(Qt::AlignLeft);
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    legendColorMyLB = new QLabel(this);
    legendColorMyLB->setObjectName(QString::fromUtf8("legendColorMyLB"));
    sizePolicy.setHeightForWidth(legendColorMyLB->sizePolicy().hasHeightForWidth());
    legendColorMyLB->setSizePolicy(sizePolicy);
    legendColorMyLB->setMinimumSize(QSize(10, 5));
    legendColorMyLB->setMaximumSize(QSize(10, 5));
    legendColorMyLB->setStyleSheet(QString::fromUtf8("background-color: rgb(0,113,215);"));

    horizontalLayout->addWidget(legendColorMyLB);

    myWiFiLB = new QLabel(this);
    //myWiFiLB->setWordWrap(true);
    myWiFiLB->setObjectName(QString::fromUtf8("myWiFiLB"));
    myWiFiLB->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\256\213\344\275\223\";"));

    horizontalLayout->addWidget(myWiFiLB);


    legendColorVerLayout->addLayout(horizontalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setAlignment(Qt::AlignLeft);
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    legendColorOtherLB = new QLabel(this);
    legendColorOtherLB->setObjectName(QString::fromUtf8("legendColorOtherLB"));
    sizePolicy.setHeightForWidth(legendColorOtherLB->sizePolicy().hasHeightForWidth());
    legendColorOtherLB->setSizePolicy(sizePolicy);
    legendColorOtherLB->setMinimumSize(QSize(10, 5));
    legendColorOtherLB->setMaximumSize(QSize(10, 5));
    legendColorOtherLB->setStyleSheet(QString::fromUtf8("background-color: rgb(238,210,51);"));

    horizontalLayout_2->addWidget(legendColorOtherLB);

    otherWiFiLB = new QLabel(this);
    //otherWiFiLB->setWordWrap(true);
    otherWiFiLB->setObjectName(QString::fromUtf8("otherWiFiLB"));
    otherWiFiLB->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\256\213\344\275\223\";"));


    horizontalLayout_2->addWidget(otherWiFiLB);


    legendColorVerLayout->addLayout(horizontalLayout_2);


    legendHorLayout->addLayout(legendColorVerLayout);


    legendFrameVerLayout->addLayout(legendHorLayout);


    retranslateUi();

} // setupUi

void QGenieWirelessChannelLegendWidget::retranslateUi()
{    
    //legendLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCV_WIFI_LEGEND_TEXT)/*"Legend"*/);
    legendColorMyLB->setText(QString());
    myWiFiLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCV_WIFI_MYWIFI_TEXT)/*"My WiFi"*/);
    legendColorOtherLB->setText(QString());
    otherWiFiLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCV_WIFI_OTHERWIFI_TEXT)/*"Other's WiFi"*/);
} // retranslateUi

void QGenieWirelessChannelLegendWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QFrame::changeEvent(event);
}

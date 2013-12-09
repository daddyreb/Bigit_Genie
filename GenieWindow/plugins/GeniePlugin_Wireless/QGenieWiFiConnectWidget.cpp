#include "QGenieWiFiConnectWidget.h"
#include <QStackedLayout>
#include "genie2_interface.h"

QGenieWiFiConnectWidget::QGenieWiFiConnectWidget(QWidget *parent) :
    QWidget(parent)
{
    QStackedLayout *layOut = new QStackedLayout(this);

    m_vnWidget = new QGenieVisibleNetworkWidget(this);
    m_vnWidget->setObjectName("InnerDialog");
    layOut->addWidget(m_vnWidget);
    connect(m_vnWidget,SIGNAL(Manager()),this,SLOT(onManager()));

#ifdef ENABLE_WIFIPROFILE_MANAGER
    m_wpmWidget = new QGenieWiFiProfileManagerWidget(this);
    layOut->addWidget(m_wpmWidget);
    connect(m_wpmWidget,SIGNAL(networkList()),this,SLOT(onNetworkList()));
#endif

}

void QGenieWiFiConnectWidget::onManager()
{
#ifdef ENABLE_WIFIPROFILE_MANAGER
    m_wpmWidget->beginManage();

    QStackedLayout *layOut = qobject_cast<QStackedLayout *>(layout());

    if(layOut)
    {
        GENIE2_SET_FOCUS(m_wpmWidget);
        layOut->setCurrentWidget(m_wpmWidget);
    }
#endif
}

void QGenieWiFiConnectWidget::onNetworkList()
{
    QStackedLayout *layOut = qobject_cast<QStackedLayout *>(layout());

    if(layOut)
    {
        GENIE2_SET_FOCUS(m_vnWidget);
        layOut->setCurrentWidget(m_vnWidget);
    }
}

#ifdef Q_OS_WIN
void QGenieWiFiConnectWidget::wzc_autoconfstate_changed(const QString &nic_guid,bool enabled)
{
    m_vnWidget->wzc_autoconfstate_changed(nic_guid,enabled);
}
#endif

#ifdef ENBABLE_UPDATEWIRELESS_REALTIME
void QGenieWiFiConnectWidget::try_refreshnetwork()
{
    m_vnWidget->try_refreshnetwork();
}
#endif

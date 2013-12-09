#include "QGenieWiFiProfileManagerWidget.h"
#include <QStackedLayout>
#include "genie2_interface.h"

#ifdef Q_OS_MACX
#include "QGenieWiFiUtilityControllerForMacx.h"
#endif

QGenieWiFiProfileManagerWidget::QGenieWiFiProfileManagerWidget(QWidget *parent) :
    QWidget(parent),
    m_profileWidget(0),
    m_newProfileWidget(0)
{
    m_profileWidget = new QGenieWiFiProfileWidget(this);
    m_newProfileWidget = new QGenieWiFiCreateProfileWidget(this);

    connect(m_profileWidget,SIGNAL(requestAddProfile()),
            this,SLOT(showCreateProfileWidget()));
    connect(m_profileWidget,SIGNAL(networkList()),this,SIGNAL(networkList()));
    connect(m_profileWidget,SIGNAL(error()),this,SIGNAL(networkList()));
    connect(m_newProfileWidget,SIGNAL(addProfileCompleted(bool)),this,SLOT(addProfileCompleted(bool)));

    QStackedLayout *layOut = new QStackedLayout(this);
    layOut->addWidget(m_profileWidget);
    layOut->addWidget(m_newProfileWidget);

    layOut->setCurrentWidget(m_profileWidget);

}

void QGenieWiFiProfileManagerWidget::showCreateProfileWidget()
{
    m_newProfileWidget->resetUi();
    GENIE2_SET_FOCUS(m_newProfileWidget);
    qobject_cast<QStackedLayout *>(layout())->setCurrentWidget(m_newProfileWidget);

}

void QGenieWiFiProfileManagerWidget::showProfileListWidget()
{
    GENIE2_SET_FOCUS(m_profileWidget);
    qobject_cast<QStackedLayout *>(layout())->setCurrentWidget(m_profileWidget);
}

void QGenieWiFiProfileManagerWidget::addProfileCompleted(bool added)
{
    Q_UNUSED(added)

    m_profileWidget->addProfileCompleted(true);
    showProfileListWidget();
}


void QGenieWiFiProfileManagerWidget::beginManage()
{
#ifdef Q_OS_MACX
    g_WifiProfileManager.init();
#endif
    m_profileWidget->init();
    showProfileListWidget();
}

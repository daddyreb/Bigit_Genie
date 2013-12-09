#include "newdevicenotifymanager.h"
#include "newdevicenotifywidget.h"

#include <QtDebug>


NewDeviceNotifyManager::NewDeviceNotifyManager(QObject *parent) :
        QObject(parent),
        m_isNotifying(false),
        m_ndnWidget(0)
{
}

NewDeviceNotifyManager::~NewDeviceNotifyManager()
{
    delete m_ndnWidget;
}

void NewDeviceNotifyManager::onNewDeviceNotify(SVT_NotifyType data)
{
    if(data.size() == 0)
    {
        return;
    }

    if(0 == m_ndnWidget)
    {
        m_ndnWidget = new NewDeviceNotifyWidget(qobject_cast<QWidget*>(parent()));

        connect(m_ndnWidget,SIGNAL(completed(int)),
                this,SLOT(onNotifyCompleted(int)));
        connect(m_ndnWidget,SIGNAL(timeout()),
                this,SLOT(onNotifyTimeout()));

        connect(m_ndnWidget,SIGNAL(translateText(int,QString*)),
                this,SIGNAL(translateText(int,QString*)));
    }

    if(!m_isNotifying)
    {
        m_ndnWidget->notifyNewDevice(data.takeFirst()[NIE_IP]);
        m_isNotifying = true;
    }

    m_cachedNewDevicesPending2Notify.append(data);

}

void NewDeviceNotifyManager::onNotifyTimeout()
{
    qDebug () << "NewDeviceNotifyManager onNotifyTimeout";

    notifyOver();
}

void NewDeviceNotifyManager::onNotifyCompleted(int countRemain)
{
    qDebug () << "NewDeviceNotifyManager onNotifyCompleted count = "
            << countRemain;

    if(m_isNotifying && (countRemain > 0) &&
       (m_cachedNewDevicesPending2Notify.size() > 0))
    {
        m_ndnWidget->notifyNewDevice(m_cachedNewDevicesPending2Notify.takeFirst()[NIE_IP],
                                     countRemain);
        qDebug () << "next new device";
    }
    else
    {
        notifyOver();
    }
}

void NewDeviceNotifyManager::notifyOver()
{
    m_isNotifying = false;
    m_cachedNewDevicesPending2Notify.clear();

    m_ndnWidget->close();
    delete m_ndnWidget;
    m_ndnWidget = 0;
}

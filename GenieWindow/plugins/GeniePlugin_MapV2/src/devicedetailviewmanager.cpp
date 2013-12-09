#include "devicedetailviewmanager.h"

#include "devicedetailview.h"
#include "devicedetailviewframe.h"

#include <QtDebug>

DeviceDetailViewManager::DeviceDetailViewManager(QWidget *detailViewParent , QObject *parent) :
    QObject(parent)
{
    m_detailView = new DeviceDetailView();
    m_detailView->setFocusPolicy(Qt::NoFocus);

    m_detailViewFrame = new DeviceDetailViewFrame(m_detailView,detailViewParent);

    connect(m_detailView,SIGNAL(requestDisplayNewTitle(QString)),
            m_detailViewFrame,SLOT(setFrameTitle(QString)));
    connect(m_detailView,SIGNAL(requestDisplayNewLogo(QPixmap)),
            m_detailViewFrame,SLOT(setFrameLogo(QPixmap)));
    connect(m_detailView,SIGNAL(translateText(int,QString*)),
            this,SIGNAL(translateText(int,QString*)));
    connect(m_detailView,SIGNAL(requestRouterImagePath(QString,int,QString*)),
            this,SIGNAL(requestRouterImagePath(QString,int,QString*)));

    connect(m_detailView,SIGNAL(viewDetailsOver(Device,bool)),
            this,SIGNAL(viewDetailsOver(Device,bool)));
}

DeviceDetailViewManager::~DeviceDetailViewManager()
{
    delete m_detailView;
    delete m_detailViewFrame;
}

void DeviceDetailViewManager::viewDeviceDetails(const Device &dev)
{
    m_detailView->viewDeviceDetails(dev);
    m_detailViewFrame->centerInParent();
}

void DeviceDetailViewManager::requestViewDeviceDetailsOver()
{
    m_detailView->excuteViewDetailsOver();
}

void DeviceDetailViewManager::onNewData(const QList<Device *> &data)
{
    m_detailView->onNewData(data);
}

void DeviceDetailViewManager::raise()
{
    m_detailViewFrame->raise();
}

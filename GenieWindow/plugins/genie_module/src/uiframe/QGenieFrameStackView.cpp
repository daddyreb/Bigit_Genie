#include "stable.h"

QGenieFrameStackView::QGenieFrameStackView(QWidget *parent)
    :QStackedWidget(parent)
{
//    this->setMinimumSize(620,396);
//    this->setMaximumSize(626,403);
  //  this->setMaximumSize(620,396);
    QGenieStackManager *stackManager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackManager);
    stackManager->setStackView(this);

    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    proxymanager->setStackView(this);

    setCurrentIndex(0);
    setProperty(STYLE_PROPERTY_HINT_SUB,SHSE_FRAME_STACK);


}


#include "stable.h"


QGeniePageHome::QGeniePageHome(QWidget *parent)
    :QWidget(parent)
{
    mUi.setupUi(this);
//    mTimer.setInterval(500);
//    QGenieBigButtonManager * bigbuttonmanager=QGenieBigButtonManager::GetSingletonPtr();
//    ASSERT(bigbuttonmanager);
//    connect(&mTimer,SIGNAL(timeout()),bigbuttonmanager ,SLOT(slot_updateAllState()));
//    mTimer.start();
}

 QGeniePageHome::~QGeniePageHome()
 {
//      mTimer.stop();
 }

void	QGeniePageHome::showEvent ( QShowEvent * event )
{
    qDebug("QGeniePageHome::showEvent");
    QWidget::showEvent(event);
  //  QGenieBigButtonManager * bigbuttonmanager=QGenieBigButtonManager::GetSingletonPtr();
//    bigbuttonmanager->updateAllState(true);

}



void QGeniePageHome::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        mUi.retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

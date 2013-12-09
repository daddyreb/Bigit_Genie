#include "stable.h"

QGenieFrameAD::QGenieFrameAD(QWidget *parent)
    :QFrame(parent)
{
    mUi.setupUi(this);
    mUi.stack->setCurrentIndex(0);

}

void QGenieFrameAD::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        mUi.retranslateUi(this);
    }
    QFrame::changeEvent(event);
}

/*
void QGenieFrameAD::changePage(QString s)
{
    if(s.isEmpty())
    {
         mUi.stack->setCurrentIndex(0);
    }
    else
    {
        mUi.stack->setCurrentIndex(1);
        QUrl url = QUrl(s);
        mUi.webView->load(url);
        mUi.webView->page()->setContentEditable(false);
    }
}
*/

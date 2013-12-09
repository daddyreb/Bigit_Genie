#include "stable.h"
#define MAX_SEARCH_LEN 64

QGenieFrameSearch::QGenieFrameSearch(QWidget *parent)
    :QFrame(parent)
{
    mUi.setupUi(this);
    mUi.lineEdit->setMaxLength(MAX_SEARCH_LEN);
    connect(mUi.lineEdit,SIGNAL(returnPressed()),this,SLOT(slot_openUrl()));
    connect(mUi.search_go,SIGNAL(pressed()),this,SLOT(slot_openUrl()));
}

void QGenieFrameSearch::slot_openUrl()
{


    qDebug("QGenieFrameSearch::slot_openUrl()");
    QString txturl = mUi.lineEdit->text();
    if(txturl!="")
    {
        QUrl url("http://kb.netgear.com/app/answers/list/kw/"+txturl);
        //   QUrl url("http://www.dlink.com.cn");
        GENIE_LOG(QString("open url ")+url.path());
        QDesktopServices::openUrl(url);
        ADD_CLICK("H5");
        ADD_SEARCH(txturl);
    }


}

void QGenieFrameSearch::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        DEFINE_LANGUAGE_MANAGER
        SET_TEXT(mUi.search_go,L_MAIN_SEARCH_GO);
    }
    QFrame::changeEvent(event);
}


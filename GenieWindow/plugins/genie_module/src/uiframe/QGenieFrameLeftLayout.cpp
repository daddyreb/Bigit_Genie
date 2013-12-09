#include "stable.h"

QButtonGroup QGenieFrameLeftLayout::mButGroup;

QGenieFrameLeftLayout::QGenieFrameLeftLayout(QWidget *parent)
    :QFrame(parent)
    ,mButNum(0)
{
    //mUi.scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mUi.scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mUi.setupUi(this);
}



void QGenieFrameLeftLayout::addButton(QPushButton *bt)
{    
    mButGroup.addButton(bt);
    mButGroup.setId(bt,mButNum);
    mButNum++;
    mUi.verticalLayout->addWidget(bt);
   // SET_FIX_SIZE(TEST_BUT,bt);
}

void QGenieFrameLeftLayout::deleteButton(QPushButton *bt)
{
    qDebug("QGenieFrameLeftLayout::deleteButton(QPushButton *bt)");
    mButGroup.removeButton(bt);
    mButGroup.setId(bt,mButNum);
    mButNum--;
    mUi.verticalLayout->removeWidget(bt);
   // SET_FIX_SIZE(TEST_BUT,bt);
}

void QGenieFrameLeftLayout::checkButton(int idx)
{
    QAbstractButton *btn=mButGroup.button(idx);
    ASSERT(btn);
    btn->setChecked(true);
}

void QGenieFrameLeftLayout::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        mUi.retranslateUi(this);
    }
    QFrame::changeEvent(event);
}

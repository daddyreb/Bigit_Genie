#include "QSetTextHelper.h"
QSetTextHelper::QSetTextHelper(QWidget *w)
{
    mLabel=qobject_cast<QLabel *>(w);
    mButton=qobject_cast<QPushButton *>(w);
    mRdtButton=qobject_cast<QRadioButton *>(w);
    mChkBox=qobject_cast<QCheckBox *>(w);
    mTabBar=qobject_cast<QTabBar *>(w);
}
bool QSetTextHelper::isOk()
{
    return mLabel||mButton||mRdtButton||mChkBox||mTabBar;
}
void QSetTextHelper::setText(const QString &text)
{
    if(mLabel){mLabel->setText(text);}
    if(mButton){mButton->setText(text);}
    if(mRdtButton){mRdtButton->setText(text);}
    if(mChkBox){mChkBox->setText(text);}
    if(mTabBar){mTabBar->setTabText(mTabBar->currentIndex(),text);}
}
QString QSetTextHelper::text()
{
    if(mLabel){return mLabel->text();}
    if(mButton){return mButton->text();}
    if(mRdtButton){return mRdtButton->text();}
    if(mChkBox){return mChkBox->text();}
    if(mTabBar){return mTabBar->tabText(mTabBar->currentIndex());}
    return QString();
}


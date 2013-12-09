#include "stable.h"

#define  LINK_PAGE_HINT "linkpage"

QGenieButtonStackViewController::QGenieButtonStackViewController(QWidget *parent)
    :QPushButton(parent)
    ,mLinkWidget(NULL)
    ,mLeftButton(NULL)
    ,mControllerType(CT_LEFT_BUTTON)
    ,mStrIdx(-1)
    ,mLeftPanIdx(-1)
{
    connect(this,SIGNAL(pressed()),this,SLOT(slot_Click()));
}

void QGenieButtonStackViewController::slot_Click ( )
{  

    this->click();

}

void QGenieButtonStackViewController::click()
{
    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);
   // stackmanager->changeStackViewIdx(page);
   // ASSERT(mLinkWidget);
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    if(!(mControllerType == CT_LEFT_BUTTON && isChecked()))
    {
        const plug_inf *p= proxymanager->getPluginInfFromLeftBut(this);
        if(p && p->pi)
        {
            p->pi->notifyBigButton(mNotifyPara);
        }
        ADD_CLICK((mControllerType == CT_LEFT_BUTTON)?"H2":"H3");
    }


    if(mLinkWidget)
    {
        stackmanager->changeStackViewIdx(mLinkWidget);

    }
    if(mLeftButton)
    {
        mLeftButton->checkThis();
    }
}

void QGenieButtonStackViewController::checkThis()
{
    ASSERT(mControllerType==CT_LEFT_BUTTON);
    setChecked(true);
    QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
    ASSERT(leftpan);
    ASSERT(mLeftPanIdx >=0);
    leftpan->changeStackIdx(mLeftPanIdx);

}

void QGenieButtonStackViewController::setLink(QWidget *w,QGenieButtonStackViewController *leftbutton)
{
 //   ASSERT(mLinkWidget == NULL);
  //      ASSERT(w);
    if(!leftbutton)
    {
        leftbutton =this;
    }
    mLinkWidget=w;
    mLeftButton=leftbutton;
}

//void QGenieButtonStackViewController::setNotifyPara(QString s)
//{
//    mNotifyPara=s;
//}

void QGenieButtonStackViewController::parsePara(QString s)
{
    QStringList lst=s.split(":");
    if(lst.size() >=1)
    {
        mNotifyPara=lst.at(0);
    }
    if(lst.size() >=2)
    {
        bool btemp;
        int itemp=lst.at(1).toInt(&btemp);
        if(btemp)
        {
            mStrIdx=itemp;
         //   changeLan();
        }
        else{
            mStrIdx=-1;
            mStr=lst.at(1);
        }
    }
	else
	{
		mStrIdx=-1;
		mStr=mNotifyPara;

	}
}

void QGenieButtonStackViewController::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange && mControllerType== CT_LEFT_BUTTON)
    {
        changeLan();

      //  mUi.retranslateUi(this);
    }
    QPushButton::changeEvent(event);
}

void QGenieButtonStackViewController::changeLan()
{
    DEFINE_LANGUAGE_MANAGER

    if(mStrIdx>=0)
    {
        QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(proxymanager);
        const plug_inf *p=proxymanager->getPluginInfFromLeftBut(this);
        if(p)
        {
            SET_LABEL_TEXT(this,mStrIdx,p->uuid);
        }
        else
        {
            SET_TEXT(this,mStrIdx);
        }
    }
    else{
        setText(mStr);
    }

    qDebug() << "recieved message QEvent::LanguageChange";
}

#include "stable.h"
#define BUTTON_PER_PAGE 11

QGenieFrameLeftPanel::QGenieFrameLeftPanel(QWidget *parent)
    :QFrame(parent)
    ,mPageNum(0)
{
    //mUi.scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mUi.scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mUi.setupUi(this);
    connect(mUi.but_last,SIGNAL(clicked()),this,SLOT(slot_ShowLast()));
    connect(mUi.but_next,SIGNAL(clicked()),this,SLOT(slot_ShowNext()));
    mUi.but_last->setVisible(false);
    mUi.but_next->setVisible(false);
    mUi.stack->setCurrentIndex(0);
}



void QGenieFrameLeftPanel::addButton(QGenieButtonStackViewController *bt, QWidget *ww)
{
   // mUi.verticalLayout->addWidget(bt);

    bool enableNavigation=false;

    bt->setLink(ww,bt);

  //  mButtonHash.insert(ww,bt);
    QGenieFrameLeftLayout *layout=qobject_cast<QGenieFrameLeftLayout *> (mUi.stack->widget(mPageNum));

    int num=layout->getCount();
    ASSERT(num <= BUTTON_PER_PAGE);

    QGenieFrameLeftLayout *curlayout=NULL;
    if(num == BUTTON_PER_PAGE)
    {
        curlayout = new QGenieFrameLeftLayout();
        mUi.stack->addWidget(curlayout);
        enableNavigation=(mPageNum == 0);
        mUi.but_next->setEnabled(true);
        mPageNum++;
    }
    else
    {
        curlayout =layout;
    }

    curlayout->addButton(bt);
 //   bt->setParent(curlayout);

    if(enableNavigation)
    {
        mUi.but_last->setVisible(true);
        mUi.but_next->setVisible(true);
    }
    qDebug("QGenieFrameLeftPanel::addButton %d",num);
    bt->mLeftPanIdx=mPageNum;
}

void QGenieFrameLeftPanel::changeStackIdx(int idx)
{
    mUi.stack->setCurrentIndex(idx);
}

void QGenieFrameLeftPanel::slot_ShowLast()
{
    int curidx=mUi.stack->currentIndex();
    if(--curidx >=0)
    {
        mUi.stack->setCurrentIndex(curidx);
        if(curidx==0)
        {
          //  mUi.but_last->setDisabled(true);
        }
        mUi.but_next->setEnabled(true);
    }

}

void QGenieFrameLeftPanel::slot_ShowNext()
{
    int curidx=mUi.stack->currentIndex();
    int allpage=mUi.stack->count();
    if((++curidx) <allpage)
    {
        mUi.stack->setCurrentIndex(curidx);
        if(curidx==(allpage-1))
        {
           // mUi.but_next->setDisabled(true);
        }
        mUi.but_last->setEnabled(true);
    }
}





void QGenieFrameLeftPanel::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        mUi.retranslateUi(this);
    }
    QFrame::changeEvent(event);
}

#ifdef ENABLE_MULTI_LEFT_BUTTON
QList<QGenieButtonStackViewController *> QGenieFrameLeftPanel::createButton(plug_inf *p,QString text,QWidget *linkW)
{
    QList<QGenieButtonStackViewController *> result_list;
    QStringList text_list = text.split("#");

    foreach(QString text_tmp,text_list)
    {
        QGenieButtonStackViewController *but = new QGenieButtonStackViewController(this);

        but->parsePara(text_tmp);
        but->setProperty(STYLE_PROPERTY_HINT,SHE_BUTTON_COLOR);
        but->setCheckable(true);
        but->setFocusPolicy(Qt::NoFocus);
        result_list.append(but);
    }


    if(p)
    {
        QList<int> prio_list;
        QStringList templst=p->pi->bigButtonParas();
        if(templst.size() >0)
        {

            for(int i=0;i<templst.size();i++)
            {
                QString temps=templst[i];
                QStringList tempslt2=temps.split(":");
                if(tempslt2.size() >=2)
                {
                    bool btemp;
                    int prio=tempslt2.at(1).toInt(&btemp);
                    if(btemp)
                    {
                        prio_list.append(prio);
                    }
                }

            }
        }
        else
        {
            //if no info in bigButtonParas
            prio_list.append(p->pi->priority());
        }

        while(prio_list.size() < result_list.size())
            prio_list.append(p->pi->priority());

        int idx = 0;
        foreach(QGenieButtonStackViewController *but,result_list)
        {
            mPrioLeftButMap.insert(prio_list[idx++],but);
            mLeftButlinkWMap.insert(but,linkW);
        }
    }
    else
    {
        foreach(QGenieButtonStackViewController *but,result_list)
        {
            addButton(but,linkW);
        }
    }

    return result_list;

}

void QGenieFrameLeftPanel::updateCachedLeftButtonUi()
{
    QList<QGenieButtonStackViewController *> leftbuts = mPrioLeftButMap.values();
    foreach(QGenieButtonStackViewController *but,leftbuts)
    {
        addButton(but,mLeftButlinkWMap.value(but));
    }
}

#else
QGenieButtonStackViewController *QGenieFrameLeftPanel::createButton(QString text,QWidget *linkW)
{
    QGenieButtonStackViewController *but=new QGenieButtonStackViewController(this);

    but->parsePara(text);
 //   but->setProperty(LINK_PAGE_HINT,mPages.size()-1);
    but->setProperty(STYLE_PROPERTY_HINT,SHE_BUTTON_COLOR);
    but->setCheckable(true);
    but->setFocusPolicy(Qt::NoFocus);
    addButton(but,linkW);
    return but;

}
#endif


#ifdef UNLOAD_PI
    void QGenieFrameLeftPanel::unloadPi(IPluginInf *pi)
    {
        QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(proxymanager);
        QGeniePagePluginProxy* mPage=proxymanager->getPagePluginProxy(pi);
        QPushButton *bt=mButtonHash.value((QWidget *)mPage);

        for(int i=0;i<mPageNum;i++)
        {
              QWidget *tempW=mUi.stack->widget(i);
              QGenieFrameLeftLayout *leftlayout = qobject_cast<QGenieFrameLeftLayout *>(tempW);
              ASSERT(tempW);
              leftlayout->deleteButton(bt);
        }
        delete bt;


        //todo here
    }
#endif

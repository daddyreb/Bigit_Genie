#include "stable.h"




QGenieStackManager::QGenieStackManager()
    :mStackView(NULL)
    ,mMovieView(NULL)
    ,mHomeView(NULL)
    ,mPageNum(0)
    ,mPageCur(0)
    ,mPageNext(0)
    ,mPageLast(0)
    ,mMovieOn(false)
    ,mHomeBut(NULL)
#ifdef CAN_LOCK_STACK
    ,mCanAutoChangePage(true)
#endif
    ,mInProcess(true)
{


}


QGenieStackManager::~QGenieStackManager()
{

}

QWidget *QGenieStackManager::getCurView()
{
    return mPageCur;
}

QWidget *QGenieStackManager::getNextView()
{
    return mPageNext;
}

 void QGenieStackManager::realChangeStackViewIdx()
 {
     if(!mInProcess)
     {
         return;
     }
     mStackView->setCurrentWidget(mPageNext);
	 mStackView->update();
     QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
     ASSERT(leftpan);
     leftpan->setDisabled(false);
     mPageLast=mPageCur;
     mPageCur=mPageNext;
     mPageNext=NULL;
#ifdef PLATFORM_MACOSX
     mPageLast->clearFocus();
     mPageCur->setFocus();
#endif
#ifdef CAN_LOCK_STACK
     static QSet<QUuid> sUuidSet;
     if(sUuidSet.isEmpty())
     {
         sUuidSet<<QUuid(PLUGIN_UUID_NETCONFIG)<<QUuid(PLUGIN_UUID_WIRELESS);
     }
     QGeniePageHome *home=QGeniePageHome::GetSingletonPtr();
     ASSERT(home);
     if(mPageCur != home)
     {
         QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
         ASSERT(proxymanager);
         QGeniePagePluginProxy *page=(QGeniePagePluginProxy *)mPageCur;
         plug_inf *p=proxymanager->getPluginInf(page->mPluginInf);
         if(p )
         {
             mCanAutoChangePage=!(sUuidSet.contains(p->uuid));
         }

     }
#endif
 }

 void QGenieStackManager::slot_NotifyRaiseOrLower()
 {
     QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
     ASSERT(proxymanager);
     bool notifyraise=(mPageCur && (mPageCur!=mMovieView) &&(mPageCur != mHomeView));
     if(notifyraise)
     {
         QGeniePagePluginProxy *page=static_cast<QGeniePagePluginProxy*>(mPageCur);
         if(page)
         {
            IPluginInf *pi=proxymanager->getPluginInf(page);
            if(pi)
            {
                pi->notifyRaise();
            }
         }
     }

     bool notifylower=(mPageLast && (mPageLast!=mMovieView) && (mPageLast!= mHomeView));
     if(notifylower)
     {
         QGeniePagePluginProxy *page=static_cast<QGeniePagePluginProxy*>(mPageLast);
         if(page)
         {
            IPluginInf *pi=proxymanager->getPluginInf(page);
            if(pi)
            {
                pi->notifyLower();
            }
         }
     }
 }



void QGenieStackManager::setStackView(QGenieFrameStackView *stackview)
{
    ASSERT(mStackView==NULL);
    mStackView=stackview;


    createBasicPage();

}


QWidget *QGenieStackManager::createStackView(int type)
{
    ASSERT(0);
    return NULL;
}


void QGenieStackManager::changeStackViewIdx(QWidget *w)
{
    if(!mInProcess)
    {
        return;
    }
    if(NULL ==w )
    {
        w=mHomeView;
    }
    ASSERT(w);

    if(mPageCur == w || mMovieView==mStackView->currentWidget())
    {
        return;
    }
    QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
    ASSERT(leftpan);
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);






#ifdef MOVIE_WHEN_PAGE_CHANGE
    mPageNext=w;
   mMovieView->setNext();
   mStackView->setCurrentWidget(mMovieView);
   leftpan->setDisabled(true);
#else
   mPageNext=w;
  leftpan->setDisabled(true);
  realChangeStackViewIdx();
#endif
  QTimer::singleShot(1,this,SLOT(slot_NotifyRaiseOrLower()));

}

//QGenieButtonStackViewController * QGenieStackManager::createHomeButton(QWidget *parent)
//{
//
//}



//void QGenieStackManager::realRegistPluginProxy()
//{
//    ASSERT(0); //MOVE TO plugin manger
//    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
//    ASSERT(proxymanager);
//
//    QList<IPluginInf *> tempinf=mPiCache.values();
//    while (!tempinf.isEmpty())
//    {
//
//       IPluginInf  *pi=tempinf.takeFirst();
//        QGeniePagePluginProxy * proxyW=proxymanager->createPage(pi);
//       registPluginWidget(proxyW,pi);
//    }
//}



#ifdef UNLOAD_PI
    void QGenieStackManager::unloadPi(IPluginInf *pi)
    {
        QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(proxymanager);
        QGeniePagePluginProxy *mPage=proxymanager->getPagePluginProxy(pi);
        if(mPage)
        {
            mStackView->removeWidget(mPage);
        }
    }

#endif

void QGenieStackManager::createBasicPage()
{
    //create page home and page movie
    ASSERT(mHomeView ==NULL );
    ASSERT(mStackView);
    ASSERT(mMovieView ==NULL);
    mHomeView = new QGeniePageHome(mStackView);
//    home->hide();
    mStackView->addWidget(mHomeView);
    mPageCur=mHomeView;


    mMovieView=new QGeniePageMovie(mStackView);
    mStackView->addWidget(mMovieView);

    QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
    ASSERT(leftpan);

    ASSERT(mHomeBut == NULL);
#ifdef ENABLE_MULTI_LEFT_BUTTON
    mHomeBut= leftpan->createButton(NULL,"Home:"+QString::number(L_MAIN_LEFTBT_HOME),mHomeView)[0];
#else
    mHomeBut= leftpan->createButton("Home:"+QString::number(L_MAIN_LEFTBT_HOME),mHomeView);
#endif
    mHomeBut->setChecked(true);


}






#include "stable.h"


#define  BIG_BUTTON_TYPE_HINT "but_type"

QGenieBigButtonManager::QGenieBigButtonManager()
{    
    qDebug()<<"QGenieBigButtonManager::QGenieBigButtonManager()";
    for(int i=0;i<BBT_NUM;i++)
    {
        mBigButtons[i]=NULL;
    }
    mTime.start();
}

void QGenieBigButtonManager::registBigButton(QGenieButtonBig *b)
{
    qDebug("registBigButton");
    mBigButtonsCache.append(b);
}

void QGenieBigButtonManager::realRegistBigButton()
{
    qDebug("realRegistBigButton()");
    while (!mBigButtonsCache.isEmpty())
    {
       QGenieButtonBig  *b=mBigButtonsCache.takeFirst();
       QVariant va=b->property(BIG_BUTTON_TYPE_HINT);
       bool r;
       int type=va.toInt(&r);
       ASSERT(r);
       ASSERT( type >=0 && type < BBT_NUM );
       ASSERT(mBigButtons[type]==NULL);
       mBigButtons[type]=b;
      // b->setType("admin");
    }

}

void QGenieBigButtonManager::setType(int idx,QString s)
{
    ASSERT(mBigButtons[idx]);
    mBigButtons[idx]->setType(s);
}


//void QGenieBigButtonManager::slot_SetState(int idx,int state)
//{
//    ASSERT(idx >=0 && idx < BBT_NUM);
//    ASSERT(mBigButtons[idx]);


//    mBigButtons[idx]->setState((BIG_BUTTON_STATUS)state);
//}

//void QGenieBigButtonManager::loadStyleSheet()
//{
//        qDebug(" QGenieBigButtonManager::loadStyleSheet()");

//        for(int i=0;i<BBT_NUM;i++)
//        {
//            ASSERT(mBigButtons[i]);
//            mBigButtons[i]->loadStyleSheet();
//        }
//}

QGenieButtonBig *QGenieBigButtonManager::getBigButton(int idx)
{
    ASSERT( idx >=0 && idx < BBT_NUM );
    ASSERT(mBigButtons[idx]!=NULL);
    return mBigButtons[idx];
}



void QGenieBigButtonManager::setLink(int idx,QGeniePagePluginProxy *w,QGenieButtonStackViewController *leftbutton)
{
        ASSERT(idx >=0 && idx <BBT_NUM);
        mBigButtons[idx]->setLink(w,leftbutton);
}

//void QGenieBigButtonManager::updateAllState(bool force)
//{
//    for(int i=0;i<BBT_NUM;i++)
//    {
//       // ASSERT(mBigButtons[i]);
//        if(mBigButtons[i])
//        {
//            updateState(mBigButtons[i],force);
//        }
//    }
//}
//void QGenieBigButtonManager::slot_updateAllState()
//{
//    QGenieFrameStackView *stackview=QGenieFrameStackView::GetSingletonPtr();
//    ASSERT(stackview);

//    QGeniePageHome *pagehome=QGeniePageHome::GetSingletonPtr();
//    ASSERT(pagehome);

//    if(stackview && pagehome && stackview->currentWidget() == pagehome)
//    {
//        updateAllState();
//    }
//}

//void QGenieBigButtonManager::updateState(int idx)
//{
//    ASSERT(idx >=0 && idx <BBT_NUM);
//    QGenieButtonBig *but=mBigButtons[idx];
//    ASSERT(but);
//    updateState(but);
//}

//void QGenieBigButtonManager::updateState(QGenieButtonBig *but,bool force)
//{
//    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
//    ASSERT(proxymanager);
//    if(!force && !but->canUpdateStateNow(mTime.elapsed()))
//    {
//        return;
//    }

//    const plug_inf *p=proxymanager->getPluginInf(but);
////    ASSERT(p);
//    if(p && p->state == PMS_WND_CREATED && p->pi)
//    {
//         int state=p->pi->getStatus(but->mNotifyPara);
//         ASSERT(state>=0 && state <BBS_NUM);
//         if(state >=0 && state <BBS_NUM)
//         {
//             but->setState((BIG_BUTTON_STATUS)state);
//         }

//    }
//}









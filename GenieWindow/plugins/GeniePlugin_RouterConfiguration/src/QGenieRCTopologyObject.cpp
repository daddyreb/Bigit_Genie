#include "QGenieRCTopologyObject.h"
#include "genie2_interface.h"
#include "GeniePlugin_RouterConfiguration.h"
#include "QGenieRCWirelessObject.h"
#include "genie2_netgear.h"
#include "SvtFcmlError.h"
#include "language_RouterConfig.h"
#include "QGenieTopoItems.h"
#include <QUiLoader>
#include <QtCore>
#include "QPiObject.h"
#include <math.h>

extern quint64 sRCFlags;
#define ITEM_NUM_PERPAGE 7



QGenieTopoDeviceCache::QGenieTopoDeviceCache()
    :mFlags(0)
    ,mLinkSpeed(0)
    ,mSignalStrength(0)
    ,mType(NT_NETWORKDEVICE)
{
}

void QGenieTopoDeviceCache::setFlag(DEVICE_CACHE_FLAGS flag, bool bSet)
{
    if(bSet)
    {
        mFlags |=flag;
    }
    else
    {
        mFlags &= ~flag;
    }
}

/*
 **********************************************************************
 **********************************************************************
 **********************************************************************
 **********************************************************************
 */
QGenieRCTopoDetailViewManager::QGenieRCTopoDetailViewManager()
    :mW(NULL)
    ,mShortcut(NULL)
{

}

QGenieRCTopoDetailViewManager::~QGenieRCTopoDetailViewManager()
{

}


void QGenieRCTopoDetailViewManager::slot_startShowDetail(QGenieTopoDeviceCache *cache)
{
    if(!cache || cache->mType==QGenieTopoDeviceCache::NT_INTERNET)
    {
        return;
    }
    QGenieRCTopologyObject *topoo=QGenieRCTopologyObject::GetSingletonPtr();
    Q_ASSERT(topoo);
    QWidget *pw=topoo->getUi();
    if(!mW)
    {
        bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/rctopo_detail.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);

        QWidget *w=loader.load(&f,pw);
        Q_ASSERT(w);
        QList<QPushButton *> btns=w->findChildren<QPushButton*>();
        foreach(QPushButton *btn,btns)
        {
            connect(btn,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
        }
        QFile cssFile(GENIE2_RES("ui/detailviewframe.qss"));
        if (cssFile.open(QIODevice::ReadOnly)) {
            w->setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
        }
        QStackedWidget *stack=w->findChild<QStackedWidget*>("stackedWidget");
        Q_ASSERT(stack);
        stack->setCurrentIndex(0);


        mW=w;
//        translateUi();
//        setContentText(mW,cache);会完成翻译工作
    }
    if(!mShortcut)
    {
        mShortcut = new QShortcut(pw);
        mShortcut->setContext(Qt::ApplicationShortcut);
        mShortcut->setKey(Qt::Key_Escape);
        connect(mShortcut,SIGNAL(activated()),this,SLOT(slot_escapKey()));
    }
    setContentText(mW,cache);
    mW->show();

    topoo->setLockState(true);
    mW->move((pw->width()-mW->width())/2,(pw->height()-mW->height())/2);
    mW->raise();
}

void QGenieRCTopoDetailViewManager::slot_btnClicked()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(btn);
    QString obj_name=btn->objectName();
    if(obj_name=="but_right")
    {
        slot_escapKey();
    }
    else if(obj_name=="but_left")
    {
        QStackedWidget *stack=mW->findChild<QStackedWidget *>("stackedWidget");
        Q_ASSERT(stack);
        if(0==stack->currentIndex())
        {
            stack->setCurrentIndex(1);
            translateButtons();
        }
        else
        {

        }

    }
}
enum LABEL_TYPE
{
    LT_CUSTOM_NAME
    ,LT_TYPE
    ,LT_CONNECTION
    ,LT_IP
    ,LT_MAC
    ,LT_SIGNAL_STRENGTH
    ,LT_LINKRATE

    ,LT_P2_CUSTOM_NAME
    ,LT_P2_TYPE
    ,LT_P2_IP
    ,LT_P2_MAC
    ,LT_P2_SIGNAL_STRENGTH
    ,LT_P2_LINKRATE
    ,LT_NUM
};

const char *cLeftLabels[LT_NUM]={"l1","l2","l3","l4","l5","l6","l7"
                                 ,"p2l1","pp2l2","p2l3","p2l4","p2l5","p2l6"};
const char *cRightLabels[LT_NUM]={"r1","r2","r3","r4","r5","r6","r7"
                                  ,"none","none","p2r3","p2r4","p2r5","p2r6"};
const int cLeftLan[LT_NUM]=
{L_ATTACHED_DEVICE_CUSTOMNAME,L_ATTACHED_DEVICE_TYPE,L_ATTACHED_DEVICE_STATUS,L_ATTACHED_DEVICE_IP,L_ATTACHED_DEVICE_MACADDRESS,L_ATTACHED_DEVICE_SIGNALSTRENGTH,L_ATTACHED_DEVICE_LINKRATE
 ,L_ATTACHED_DEVICE_CUSTOMNAME,L_ATTACHED_DEVICE_TYPE,L_ATTACHED_DEVICE_IP,L_ATTACHED_DEVICE_MACADDRESS,L_ATTACHED_DEVICE_SIGNALSTRENGTH,L_ATTACHED_DEVICE_LINKRATE
};

void QGenieRCTopoDetailViewManager::setContentText(QWidget *w, QGenieTopoDeviceCache *cache)
{
    Q_ASSERT(cache);
    QLabel *title=w->findChild<QLabel*>("frameTitleLabel");
    title->setText(cache->mName.isEmpty()?cache->mIp:cache->mName);
    //ip,

    const QString cRightLan[LT_NUM]={
        title->text()
        ,QString::number(cache->mType)
        ,((cache->mFlags&QGenieTopoDeviceCache::DCF_WIRED)?"wired":"wireless")
        ,cache->mIp
        ,cache->mMac
        ,QString::number(cache->mLinkSpeed)
        ,QString::number(cache->mSignalStrength)

        ,QString()
        ,QString()
        ,cache->mIp
        ,cache->mMac
        ,QString::number(cache->mLinkSpeed)
        ,QString::number(cache->mSignalStrength)

    };

    for(int i=0;i<LT_NUM;i++)
    {
        QLabel *lab_l=w->findChild<QLabel*>(cLeftLabels[i]);
        QLabel *lab_r=w->findChild<QLabel *>(cRightLabels[i]);

        bool bUseless=(
                    ((i==LT_LINKRATE)&&!(cache->mFlags&QGenieTopoDeviceCache::DCF_HAVE_LINK_RATE))
                    ||((i==LT_SIGNAL_STRENGTH)&&!(cache->mFlags&QGenieTopoDeviceCache::DCF_HAVE_SIGNAL_DATA))
                    ||((i==LT_P2_LINKRATE)&&!(cache->mFlags&QGenieTopoDeviceCache::DCF_HAVE_LINK_RATE))
                    ||((i==LT_P2_SIGNAL_STRENGTH)&&!(cache->mFlags&QGenieTopoDeviceCache::DCF_HAVE_SIGNAL_DATA))

                    );
        if(lab_l)
        {
            lab_l->setText(GENIE2_GET_TEXT(cLeftLan[i]));
            lab_l->setVisible(!bUseless);
        }
        if(lab_r)
        {
            lab_r->setText(cRightLan[i]);
            lab_r->setVisible(!bUseless);
        }
    }
    translateButtons();
}

void QGenieRCTopoDetailViewManager::translateUi()
{
    if(!mW)
    {
        return;
    }
    for(int i=0;i<LT_NUM;i++)
    {
        QLabel *lab_l=mW->findChild<QLabel*>(cLeftLabels[i]);
//        QLabel *lab_r=qFindChild<QLabel *>(w,cRightLabels[i]);
        lab_l->setText(GENIE2_GET_TEXT(cLeftLan[i]));
    }

    translateButtons();

}

void QGenieRCTopoDetailViewManager::translateButtons()
{
    if(!mW)
    {
        return;
    }
    QStackedWidget *stack=mW->findChild<QStackedWidget*>("stackedWidget");
    QPushButton *but_left=mW->findChild<QPushButton*>("but_left");
    QPushButton *but_right=mW->findChild<QPushButton*>("but_right");
    Q_ASSERT(stack&&but_left&&but_right&&stack->currentIndex()<2);
    const int cLans[][2]={{L_ATTACHED_DEVICE_MODIFY,L_ATTACHED_DEVICE_CLOSE}
                          ,{L_ATTACHED_DEVICE_APPLY,L_ATTACHED_DEVICE_CLOSE}};
    int idx=stack->currentIndex();
    but_left->setText(GENIE2_GET_TEXT(cLans[idx][0]));
    but_right->setText(GENIE2_GET_TEXT(cLans[idx][1]));
}

void QGenieRCTopoDetailViewManager::slot_escapKey()
{
    if(mW)
    {
        mW->hide();
        mW->deleteLater();
        mW=NULL;
        QGenieRCTopologyObject::GetSingleton().setLockState(false);
    }
}



/*
 **********************************************************************
 **********************************************************************
 **********************************************************************
 **********************************************************************
 */
QGenieRCTopologyObject::QGenieRCTopologyObject(QObject *parent)
    :QObject(parent)
    ,mW(NULL)
    ,mControlPanel(NULL)
    ,mRefreshSession(-1)
    ,mGraphicsView(NULL)
    ,mScene(NULL)
    ,mStack(NULL)
    ,mPi(NULL)
    ,mStart(0)
    ,mDetailViewManager(new QGenieRCTopoDetailViewManager())
    ,mRouterData(new QGenieTopoDeviceCache)
    ,mInternetData(new QGenieTopoDeviceCache)
{
    mRouterData->mType=QGenieTopoDeviceCache::NT_CENTERROUTER;
    mInternetData->mType=QGenieTopoDeviceCache::NT_INTERNET;
}

QGenieRCTopologyObject::~QGenieRCTopologyObject()
{
    foreach(QGenieTopoNode *node,mForeGroundList)
    {
        delete node;
    }
    foreach(QGenieTopoNode *node,mBackgroundList)
    {
        delete node;
    }
    foreach(QSharedPointer<QGenieTopoDeviceCache> dev,mDataCaches)
    {
        dev.clear();
    }
    mForeGroundList.clear();
    mRouterData.clear();
    mInternetData.clear();
    delete mDetailViewManager;
}

QGenieTopoNode *QGenieRCTopologyObject::getBackgroundNode()
{
    QGenieTopoNode *node=NULL;
    if(!mBackgroundList.isEmpty())
    {
        node=mBackgroundList.back();
        mBackgroundList.pop_back();
    }
    else
    {
        node=new QGenieTopoNode();
    }
    Q_ASSERT(node);
    mForeGroundList.append(node);
    return node;
}

void QGenieRCTopologyObject::addNodeToBackground(QGenieTopoNode *node)
{
    Q_ASSERT(!mBackgroundList.contains(node));
    Q_ASSERT(mForeGroundList.contains(node));
    mForeGroundList.removeAll(node);
    mBackgroundList.append(node);
    node->mData.clear();
}
enum INFO_TYPE
{
    IT_IP
    ,IT_NAME
    ,IT_MAC
    ,IT_WIRED
    ,IT_LINKRATE
    ,IT_SIGNALRATE
    ,IT_BLOCKED
};
void QGenieRCTopologyObject::refreshUiData1(const QStringList &lst)
{
	qDebug()<<lst;
    foreach(QSharedPointer<QGenieTopoDeviceCache> dev,mDataCaches)
    {
        dev.clear();
    }
    mDataCaches.clear();
    int c=lst.count();
    int rows=(c>1)?lst[1].toInt():0;
    int columns=(c>2)?lst[2].toInt():0;

    QString ns;
    for(int i=0;i<rows;i++)
    {
        QSharedPointer<QGenieTopoDeviceCache> p(new QGenieTopoDeviceCache);
        mDataCaches.append(p);
        for(int j=0;j<columns;j++)
        {
            int cur=3+i*columns+j;
            const QString& s=(cur<c)?lst[cur]:ns;
            switch(j)
            {
            case IT_IP:
                p->mIp=s;
                break;
            case IT_NAME:
                p->mName=s;
                break;
            case IT_MAC:
            {
                p->mMac=s;
                QString temps=s;
                temps.replace(":","");
                p->setFlag(QGenieTopoDeviceCache::DCF_ISVM,temps.startsWith("000c29",Qt::CaseInsensitive));
            }
                break;
            case IT_WIRED:
                p->setFlag(QGenieTopoDeviceCache::DCF_WIRED,(0==s.compare("wired",Qt::CaseInsensitive)));
                break;
            case IT_LINKRATE:
                p->mLinkSpeed=s.toInt()
                        ;p->setFlag(QGenieTopoDeviceCache::DCF_HAVE_LINK_RATE,true);
                break;
            case IT_SIGNALRATE:
                p->mSignalStrength=s.toInt();
                p->setFlag(QGenieTopoDeviceCache::DCF_HAVE_SIGNAL_DATA,true);
                break;
            case IT_BLOCKED:
                p->setFlag(QGenieTopoDeviceCache::DCF_BLOCKED,(0!=s.compare("allow",Qt::CaseInsensitive)));
                break;
            }

        }
    }
    mStart=0;
    refreshUiData2(mStart,ITEM_NUM_PERPAGE);


}

void QGenieRCTopologyObject::refreshUiData2(int start, int count)
{
    foreach(QGenieTopoNode *node,mForeGroundList)
    {
        mScene->removeItem(node);
        node->setToolTip("");
    }

    mBackgroundList.append(mForeGroundList);
    mForeGroundList.clear();
    int allcount=mDataCaches.count();


    //7@1;192.168.1.2;geines-Mac-mini;28:37:37:12:09:2F;wireless;54;86;Allow@2;192.168.1.6;xgywz-PC;00:1D:0F:33:08:1C;wireless;54;87;Allow@3;192.168.1.10;Android_359836041571187;A8:26:D9:CB:2E:A4;wireless;54;79;Allow@4;192.168.1.11;Android_357710047293728;64:A7:69:3F:E6:E9;wireless;54;67;Allow@5;192.168.1.14;android_8eb092bb95e89cf;C8:AA:21:80:A8:F1;wireless;54;87;Allow@6;192.168.1.15;--;28:37:37:12:C6:9D;wireless;54;86;Allow@7;192.168.1.29;PC2011110909BPO;14:E6:E4:06:F8:E8;wireless;54;92;Allow


    const qreal cR=208.0;
    const qreal cHWRate=(111.0/208.0);
    const qreal PI = 3.14159265358979323846;



    QGenieTopoNode *node=getBackgroundNode();

//    node->setType(QGenieTopoNode::NT_INTERNET);
    node->setDevData(mInternetData);
    mScene->addItem(node);
    node->setZValue(1);
    node->setPos(cR,0.0);

    node=getBackgroundNode();
//    node->setType(QGenieTopoNode::NT_CENTERROUTER);
    node->setDevData(mRouterData);
    mScene->addItem(node);
    node->setZValue(2);
    node->setPos(0.0,0.0);

    const int cNodeCount=qMin(start+count,allcount)-start;
    const qreal cAngleStep=PI*2/(cNodeCount+1);

    qreal curAngle=2*PI;

#if 0
    //不应该有Genie节点
    curAngle-=cAngleStep;
    node=getBackgroundNode();
    node->setType(QGenieTopoNode::NT_GENIE);
    mScene->addItem(node);
    node->setPos(cR*cos(curAngle),cR*sin(curAngle)*cHWRate);
#endif


    for(int i=0;i<cNodeCount;i++)
    {
        curAngle-=cAngleStep;
        node=getBackgroundNode();
        Q_ASSERT(i<allcount);
//        node->setType(QGenieTopoNode::NT_NETWORKDEVICE);
        node->setDevData(mDataCaches[start+i]);

        mScene->addItem(node);
        node->setZValue(1);
        node->setPos(cR*cos(curAngle),cR*sin(curAngle)*cHWRate);
    }

    updateControlPanel(start,cNodeCount,allcount);


}

QWidget *QGenieRCTopologyObject::getUi()
{
    if(!mW)
    {
		bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/rctopology.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        w->hide();
        Q_ASSERT(w);
        mW=w;
        mGraphicsView=mW->findChild<QGraphicsView *>("graphicsView");
        mStack=mW->findChild<QStackedWidget*>("stackedWidget");
        Q_ASSERT(mGraphicsView && mStack);
        mPi=new QPiObject(mStack,"pi");
        mPi->stopAnimation();
        mStack->setCurrentIndex(0);

        mScene = new QGraphicsScene(this);
        mScene->setSceneRect(QRectF(-257.443,-37.3571 ,515.985,74.7142) );
        mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
        mGraphicsView->setScene(mScene);
//        mScene->setBackgroundBrush(Qt::blue);
        QList<QPushButton*> btns=w->findChildren<QPushButton*>();
        foreach(QPushButton *btn,btns)
        {
            connect(btn,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
        }



    }
    return mW;
}

void QGenieRCTopologyObject::updateControlPanel(int start,int count,int allcount)
{
    QWidget *pw=getUi();
    if(!mControlPanel)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/rctopo_controlpan.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f,pw);
		w->show();
        w->raise();

        QList<QPushButton *>bts=w->findChildren<QPushButton *>();
        foreach(QPushButton *bt,bts)
        {
            bt->setFixedSize(18,18);
        }

        QFile fqss(GENIE2_RES("ui/rctopo_controlpan.qss"));
        btemp=fqss.open(QIODevice::ReadOnly);
        Q_ASSERT(btemp);
        QString stemp=fqss.readAll();
        w->setStyleSheet(stemp);

        w->move(pw->width()-w->width(),pw->height()-w->height());
        QList<QPushButton*> btns=w->findChildren<QPushButton*>();
        foreach(QPushButton *btn,btns)
        {
            connect(btn,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
        }
        mControlPanel=w;
    }
    if(allcount<=ITEM_NUM_PERPAGE)
    {
        mControlPanel->hide();
    }
    else
    {
        mControlPanel->show();
        const char *cParas[]={"but_first","but_prev","but_next","but_last"};
        QList<QPushButton *>pan_buts;
        for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
        {
            QPushButton *btn=mControlPanel->findChild<QPushButton*>(cParas[i]);
            Q_ASSERT(btn);
            pan_buts.append(btn);
        }
        pan_buts[0]->setEnabled(start>0);
        pan_buts[1]->setEnabled(start>0);
        pan_buts[2]->setEnabled((start+count)<allcount);
        pan_buts[3]->setEnabled((start+count)<allcount);

        QLabel *lab=mControlPanel->findChild<QLabel*>("label");
        int cur=(start/ITEM_NUM_PERPAGE)+1;
        int all=allcount/ITEM_NUM_PERPAGE+((allcount%ITEM_NUM_PERPAGE)?1:0);
        lab->setText(QString("%1/%2").arg(cur).arg(all));
    }
   
//    return mControlPanel;
}

void QGenieRCTopologyObject::setLockState(bool lock)
{
    if(mGraphicsView)
    {
        mGraphicsView->setEnabled(!lock);
    }
    if(mControlPanel)
    {
        mControlPanel->setEnabled(!lock);
    }
    QPushButton *but=mW->findChild<QPushButton *>("but_refresh");
    if(but)
    {
        but->setEnabled(!lock);
    }
}

void QGenieRCTopologyObject::slot_RefreshData(bool remote)
{
    sRCFlags &= ~RCF_MUST_REFRESH_TOPO;
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

//    mPi->startAnimation();
//    setError(-1);
//    QStackedWidget *stack_tm=qFindChild<QStackedWidget*>(mW,"stack_tm");
//    Q_ASSERT(stack_tm);
//    stack_tm->setCurrentIndex(0);
    if(remote)
    {
        sRCFlags |= RCF_IN_REMOTE_REFRESHING;
        mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                         ->netgearCommand(INetgearInterface::IC_SM_ATTACHDEVICE
                                          ,QStringList()<<rcwo->getCpName(),this,SLOT(slot_DataReturned(int,QStringList)));
    }
    else
    {
//        Q_ASSERT(0);
        sRCFlags |= RCF_IN_LOCAL_REFRESHING;
        mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                         ->netgearCommand(INetgearInterface::IC_ATTACHDEVICE
                                          ,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));
    }
    mStack->setCurrentIndex(1);
    mPi->startAnimation();
}

void QGenieRCTopologyObject::slot_DataReturned(int session, const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    bool btemp=false;
    int length=lst.count();
    int rvalue=(length>0)?lst[0].toInt(&btemp):-1;
    bool requestOk=((0==rvalue) && btemp);

    if(sRCFlags & RCF_REMOTE_LOGEDIN)
    {
        Q_ASSERT(sRCFlags &RCF_REMOTE);



        bool wantReLoginCP=((FE_WRONG_USER_OR_PWD==rvalue) && btemp);
        bool wantReAuthen=((FE_NOT_AUTHENTICATIED==rvalue)&&btemp);

        if(requestOk)
        {
            QTellLPCHelper::GetSingleton().tryTellLPC();
            rcwo->tryUpdateFirmware();
        }

        if(wantReAuthen)
        {
            rcwo->setLoginError(L_RTCF_MSG_PLEASERELOGIN,true);
            rcwo->logOut(true);
            return;
        }
        if(wantReLoginCP  )
        {
            if(rcwo->tryReloginCP())
            {
                mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                                 ->netgearCommand(INetgearInterface::IC_ATTACHDEVICE
                                                  ,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));

            }
            else
            {
                sRCFlags|=RCF_MUST_REFRESH_TOPO;
                sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;
//                mPi->stopAnimation();
            }
            rcwo->updateUi();
            return;
        }
    }

    if(sRCFlags & RCF_REMOTE)
    {
        sRCFlags &=~RCF_IN_REMOTE_REFRESHING;
    }
    else
    {
        sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
    }

    if(requestOk)
    {
        refreshUiData1(lst);
    }
    else
    {
        sRCFlags |= RCF_MUST_REFRESH_TOPO;
    }
    mStack->setCurrentIndex(0);
    mPi->stopAnimation();
//    if(session==mSessionTMOption)
//    {
//        sessionOptionReturned(lst);
//    }
//    else if(session==mSessionTMStatistics)
//    {
//        sessionStatisticsReturned(lst);
//    }
    rcwo->updateUi();



}

void QGenieRCTopologyObject::slot_btnClicked()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(btn);
    QString obj_name=btn->objectName();
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    if(obj_name=="but_refresh")
    {
        slot_RefreshData(sRCFlags&RCF_REMOTE);
        rcwo->_updateLockState();
    }
    else if(obj_name=="but_first")
    {
        mStart=0;
        refreshUiData2(mStart,ITEM_NUM_PERPAGE);
    }
    else if(obj_name=="but_prev")
    {
        Q_ASSERT(mStart >= ITEM_NUM_PERPAGE);
        mStart-=ITEM_NUM_PERPAGE;
        refreshUiData2(mStart,ITEM_NUM_PERPAGE);
    }
    else if(obj_name=="but_next")
    {
        mStart+=ITEM_NUM_PERPAGE;
        Q_ASSERT(mStart<mDataCaches.count());
        refreshUiData2(mStart,ITEM_NUM_PERPAGE);
    }
    else if(obj_name=="but_last")
    {
        mStart=(mDataCaches.count()/ITEM_NUM_PERPAGE)*ITEM_NUM_PERPAGE;
        refreshUiData2(mStart,ITEM_NUM_PERPAGE);
    }


}

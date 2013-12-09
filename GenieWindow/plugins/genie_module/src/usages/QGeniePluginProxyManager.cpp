#include "stable.h"

#ifdef PLATFORM_MACOSX
#define PLUGIN_DIR "../PlugIns"
#else
#define PLUGIN_DIR "."
#endif

struct _sBigButtonCache
{
    int priority;
    QString text;
    plug_inf * p;
#ifdef ENABLE_MULTI_LEFT_BUTTON
    int bbt_index;
#endif
};

QGeniePluginProxyManager::QGeniePluginProxyManager()
    :mStackView(NULL)
    ,mLoadOk(false)
    //,mCurrentStackIdx(0)
   // ,mPluginDiagnose(NULL)
  //  ,mCurrentLoader(NULL)
{
    qDebug()<<"QGeniePluginProxyManager::QGeniePluginProxyManager()";
    //mTimeLineForNoneWndPlugin=new QTimeLine(10000,this);
  //  mParsor=new QGeniePluginParsor(this);
  //  connect(mTimeLineForNoneWndPlugin,SIGNAL(finished()),this,SLOT(slot_startupAllNoneWndPlugins()));
#ifdef CAL_VERSION
     mCalVersion=QString(VERSION_STR_RIGHT).toInt();
#endif
}

QGeniePluginProxyManager::~QGeniePluginProxyManager()
{
    qDebug("~QGeniePluginProxyManager()");
}



QGeniePagePluginProxy *QGeniePluginProxyManager::createPage(IPluginInf *pi)
{
 //   static int sCurnum=0;
    ASSERT(mStackView);
#ifdef SIMPLE_FRAME
    QGeniePagePluginProxy *retproxy= new QGeniePagePluginProxy(NULL);
    QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(traymanager);
    connect(traymanager->mRestoreAction,SIGNAL(triggered()),retproxy,SLOT(showNormal()));
#else
    QGeniePagePluginProxy *retproxy= new QGeniePagePluginProxy(mStackView);
#endif
 //   retproxy->setIdx(sCurnum);

    QGenieBigButtonManager* bbtmanager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(bbtmanager);




    QWidget *w=pi->widget(retproxy);
	ASSERT(w);
    w->setProperty(STYLE_PROPERTY_HINT_SUB,SHSE_FRAME_PLUGIN);
    ASSERT(mStackView);
    w->setParent(mStackView);
    retproxy->setPluginWidget(w,pi);
    retproxy->setAutoFillBackground(true);

    //mProxyHash.insert(retproxy,pi);

    int idx=mStackView->count();
    //mStackIdxMap.insert(idx,retproxy);

    return retproxy;
}
const plug_inf *QGeniePluginProxyManager::getPluginInf(QGenieButtonBig *bgbt)
{
    return mInfoManager.mBigButtonIdx.value(bgbt,NULL);
}

plug_inf *QGeniePluginProxyManager::getPluginInf(IPluginInf *pi)
{
    return mInfoManager.mPiIdx.value(pi,NULL);
}

const plug_inf *QGeniePluginProxyManager::getPluginInfFromLeftBut(QGenieButtonStackViewController * but)
{
    return mInfoManager.mLeftButtonIdx.value(but,NULL);
}

plug_inf *QGeniePluginProxyManager::getPluginInf(QUuid uuid)
 {
    if(uuid.isNull())
    {
        return NULL;
    }
     return mInfoManager.mUuidIdx.value(uuid,NULL);
 }

void QGeniePluginProxyManager::setStackView(QGenieFrameStackView *stackview)
{
    ASSERT(stackview);
    ASSERT(mStackView==0);
    mStackView=stackview;
//    connect(stackview,SIGNAL(currentChanged(int)),this,SLOT(slot_StackIdxChanged(int)));
}

IPluginInf * QGeniePluginProxyManager::getPluginInf(QGeniePagePluginProxy *w)
{
    plug_inf * inf=mInfoManager.mPageIdx.value(w);
//    ASSERT(inf);
    if(inf && inf->state==PMS_WND_CREATED)
    {
        return inf->pi;
    }
 //   ASSERT(inf);
    return NULL;
}

QGeniePagePluginProxy *QGeniePluginProxyManager::getPagePluginProxy(IPluginInf *pi)
{
    plug_inf *p=mInfoManager.mPiIdx.value(pi);
    ASSERT(p);
    if(p)
    {
        return p->page;
    }
    return NULL;
}



//void QGeniePluginProxyManager::registNoneWndPlugin(IPluginInf *pi)
//{
//    mNoneWndPlugins.insert(pi);
//}

//void QGeniePluginProxyManager::startupAllNoneWndPlugins()
//{
//    QList<IPluginInf *> templist=mNoneWndPlugins.toList();
//    QList<IPluginInf *>::iterator i;
//
//    for(i=templist.begin();i!=templist.end();i++)
//    {
//        IPluginInf *pi=*i;
//        ASSERT(pi);
//        pi->startup();
//    }
//
////    mTimeLineForNoneWndPlugin->setStartFrame(0);
////    mTimeLineForNoneWndPlugin->setDirection(QTimeLine::Forward);
////    mTimeLineForNoneWndPlugin->start();
//}

void QGeniePluginProxyManager::slot_StartupAllNoneWndPlugins()
{
//    QList<IPluginInf *> templist=mNoneWndPlugins.toList();
//    QList<IPluginInf *>::iterator i;
//
//    for(i=templist.begin();i!=templist.end();i++)
//    {
//        IPluginInf *pi=*i;
//        ASSERT(pi);
//        pi->startup();
//    }
}

bool QGeniePluginProxyManager::isValitPluginFileName(QString s) const
{
    qDebug()<<s;
#ifdef _WIN32
    QRegExp m("GeniePlugin*.dll",Qt::CaseInsensitive);
#elif defined _MACOS_
    QRegExp m("libGeniePlugin*.dylib",Qt::CaseInsensitive);
#else
    QRegExp m("libGeniePlugin*.so",Qt::CaseInsensitive);
#endif
    m.setPatternSyntax(QRegExp::Wildcard);
    return (m.exactMatch(s));
}




void QGeniePluginProxyManager::addAllPluginToUi()
{
    QGenieInterfaceManagerHandler *handler=QGenieInterfaceManagerHandler::GetSingletonPtr();
    ASSERT(handler);

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);

    QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
    ASSERT(leftpan);

    QGenieBigButtonManager *bigbutmanager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(bigbutmanager);

    QGenieInerTextManager *innertextmanager=QGenieInerTextManager::GetSingletonPtr();
    ASSERT(innertextmanager);


    QGeniePluginParsor parsor;
    QMap<int,plug_inf *> mSortMap;
    QMap<int,_sBigButtonCache *> sortBigBut;

    for(int i=0;i<mInfoManager.mInfList.size();i++)
    {

        //[1] load plugin inf`
        plug_inf *p =mInfoManager.mInfList.at(i);
        GENIE_LOG(QString("load plugin ")+p->filename);
        QString stemp=QString("")+GENIE_PATH +PLUGIN_DIR+"/"+p->filename;
        if(!p->loader)
        {

            p->loader=new QPluginLoader(stemp);
        }

        p->loader->setFileName(stemp);
        qDebug()<<stemp;
        QObject *temp = p->loader->instance();
        if(!temp)
        {
            p->loader->unload();
            p->state=PMS_ERROR;
            continue;
        }
        p->pi = qobject_cast<IPluginInf *>(temp);
        if(!p->pi)
        {
            p->loader->unload();
            p->state=PMS_ERROR;
            continue;
        }


        //[2] parse plugin inf
        parsor.clearResult();

        PlgMsg msg;
        if(!parsor.parsePlugin(p->pi,&msg))
        {
            p->version=msg.versionStr;
            p->loader->unload();
            p->pi=NULL;
            continue;
        }
        p->version=msg.versionStr;
        p->pi->setHandler(handler);
        p->state=PMS_LOADED;
        p->uuid=msg.uid;
        p->havewnd=msg.haveW;
//        p->priority=msg.priority;
        //for some bug
        mInfoManager.mUuidIdx.insert(p->uuid,p);

        GENIE_LOG(QString("VERSION:")+msg.versionStr);
        GENIE_LOG(QString("DATE:")+p->pi->getInfo(PLUGIN_INF_CREATE_DATE));

        //calculate version number
#ifdef CAL_VERSION
        QStringList slst=msg.versionStr.split(".");
        int siz=slst.size();
        if(siz > 0)
        {
            bool btemp;
            int itemp=slst.at(siz-1).toInt(&btemp);
            if(btemp)
            {
                mCalVersion+=itemp;
                qDebug()<< "version added "<<itemp <<" when "<<p->filename;
            }
        }
#endif

        //get inner text info
        QString innerText=p->pi->getInfo(PLUGIN_INF_INNER_LANGUAGE);
        if("" != innerText)
        {
            innertextmanager->parsePara(innerText,p->uuid);
        }


        if(msg.haveW)
        {
            mSortMap.insert(p->pi->priority(),p);
            QStringList templst=p->pi->bigButtonParas();
            if(templst.size() >0)
            {

                for(int i=0;i<templst.size();i++)
                {
                    QString temps=templst[i];
                    QStringList tempslt2=temps.split(":");
                    if(tempslt2.size() >=2)
                    {
                     //   QString temptext=tempslt2.at(0);
                        bool btemp;
                        int prio=tempslt2.at(1).toInt(&btemp);
                        if(btemp)
                        {
                            _sBigButtonCache * st=new _sBigButtonCache;
                            st->priority=prio;
                            st->text=temps;
                            st->p = p;
#ifdef ENABLE_MULTI_LEFT_BUTTON
                            st->bbt_index = i;
#endif
                            sortBigBut.insert(st->priority,st);
                        }
                    }

                }
            }
            else
            {
                //if no info in bigButtonParas
                _sBigButtonCache *st=new _sBigButtonCache;
                st->priority=p->pi->priority();
                st->text=p->pi->text();
                st->p=p;
                sortBigBut.insert(st->priority,st);
            }

        }
        else
        {
            p->page=NULL;
//delay startup
  //          p->pi->startup();
        }
        GENIE_LOG(QString("load plugin end ")+p->filename);
    }//for

//    bigbutmanager->loadStyleSheet();

    QList<plug_inf *> pis=mSortMap.values();
    while(!pis.isEmpty())
    {
        plug_inf *p=pis.takeAt(0);
        GENIE_LOG(QString("add plugin to ui ")+p->filename);
        QGeniePagePluginProxy *w=createPage(p->pi);
        p->page=w;

        QString text=p->pi->text();
        w->resize(mStackView->size());
//        
#ifdef SIMPLE_FRAME
        w->setWindowTitle(text.split(":").at(0));
        w->show();
#ifdef ENABLE_MULTI_LEFT_BUTTON
        p->leftpanbut.clear();
#else
        p->leftpanbut=NULL;
#endif
#else
        mStackView->addWidget(w);

#ifdef ENABLE_MULTI_LEFT_BUTTON
        p->leftpanbut=leftpan->createButton(p,text,w);
        foreach(QGenieButtonStackViewController *leftbut,p->leftpanbut)
        {
            mInfoManager.mLeftButtonIdx.insert(leftbut,p);
            leftbut->changeLan();
        }
#else
        p->leftpanbut=leftpan->createButton(text,w);
        mInfoManager.mLeftButtonIdx.insert(p->leftpanbut,p);
        p->leftpanbut->changeLan();
#endif

#endif


       // leftpan->addButton(createButton(leftpan,text),w);


        p->state=PMS_WND_CREATED;
        GENIE_LOG(QString("add plugin to ui end")+p->filename);
    }

#ifdef ENABLE_MULTI_LEFT_BUTTON
    leftpan->updateCachedLeftButtonUi();
#endif

    //add bigbuttons
#ifndef SIMPLE_FRAME
    GENIE_LOG("start register bigbutton");
    QList<_sBigButtonCache *> cachelist=sortBigBut.values();
    while(!cachelist.isEmpty())
    {
        _sBigButtonCache *cac=cachelist.takeFirst();

        static int idx=0;
        if(idx < BBT_NUM)
        {
            QGenieButtonBig *bgbt=bigbutmanager->getBigButton(idx);
            bigbutmanager->getBigButton(idx)->parseConfigString(cac->text);
            cac->p->butbig.append(bgbt);
#ifdef ENABLE_MULTI_LEFT_BUTTON
            bgbt->setLink(cac->p->page,cac->p->leftpanbut[cac->bbt_index < cac->p->leftpanbut.size()?cac->bbt_index:0]);
#else
            bgbt->setLink(cac->p->page,cac->p->leftpanbut);
#endif

            //use cached states
            int cacheStateIdx=cac->p->butbig.size()-1;
//            int cacheState=cac->p->stateCache.value(cacheStateIdx,-1);
            int cachesubState=cac->p->substateCache.value(cacheStateIdx,-1);
            QString cachStyle=cac->p->styleCache.value(cacheStateIdx,"");
            QString bkgStyle=cac->p->bkgCache.value(cacheStateIdx,"");
//            if(cacheState!=-1)
//            {
//                bgbt->setState((BIG_BUTTON_STATUS)cacheState);
//                qDebug()<<"use cache state "<<cacheStateIdx<<":"<<cacheState;
//            }
            if(cachesubState!=-1)
            {
                bgbt->setSubState((ENUM_BIGBT_SUB_STATE)cachesubState);
            }
            if(cachStyle != "")
            {
                bgbt->setUserStyle(cachStyle);
            }
            if(bkgStyle != "")
            {
                bgbt->setBKG(bkgStyle);

            }

            idx++;
        }


        delete cac;
    }
    GENIE_LOG("end register bigbutton");
#endif



    mInfoManager.updateIdxs();
    mInfoManager.clearAllCaches();

    QGenieFrameDbgPluginInfo *dbg_pan=QGenieFrameDbgPluginInfo::GetSingletonPtr();
    ASSERT(dbg_pan);
    mInfoManager.synToModel(dbg_pan->ui->tableWidget->model());

}
void QGeniePluginProxyManager::startupAllPi()
{
    for(int i=0;i<mInfoManager.mInfList.size();i++)
    {
        plug_inf *p=mInfoManager.mInfList.at(i);
        if(p && (p->state==PMS_LOADED||p->state ==PMS_WND_CREATED) /*&& p->havewnd==false*/)
        {
            ASSERT(QGenieLaunchInf::sLaunchManager);
            bool btemp=false;
#if 0
            if(QUuid(PLUGIN_UUID_NETTRAFFIC)==p->uuid)
            {
               btemp=QApplication::arguments().contains("-trafficmini");
               qDebug()<<"NetTraffic Started !!"<<btemp;
            }
#endif
            p->pi->startup(btemp);
#if 0
            p->state=PMS_STARTED;
            if(btemp)
            {
                p->flag|=START_MINI;
            }
#endif
        }
    }
}

void QGeniePluginProxyManager::loadPi(QString filename)
{
    plug_inf *p=mInfoManager.mFileNameIdx.value(filename);
    if(p && (p->state==PMS_INIT)||(p->state==PMS_UNLOADED))
    {
        QGenieInterfaceManagerHandler *handler=QGenieInterfaceManagerHandler::GetSingletonPtr();
        ASSERT(handler);
        QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
        ASSERT(leftpan);


        bool bReCreateButtons=(p->state ==PMS_INIT);
        if(p->loader ==NULL)
        {
            p->loader=new QPluginLoader(p->filename);
        }
        p->loader->setFileName(filename);

        QObject *temp = p->loader->instance();
        if(!temp)
        {
            p->loader->unload();
            p->state=PMS_ERROR;
            return;
        }
        p->pi = qobject_cast<IPluginInf *>(temp);
        if(!p->pi)
        {
            p->loader->unload();
            p->state=PMS_ERROR;
            return;
        }


        //[2] parse plugin inf
        QGeniePluginParsor parsor;
        parsor.clearResult();
        PlgMsg msg;
        if(!parsor.parsePlugin(p->pi,&msg))
        {
            p->loader->unload();
            p->state=PMS_ERROR;
            return;
        }
        p->pi->setHandler(handler);

        p->uuid=msg.uid;
        p->havewnd=msg.haveW;
        p->state=PMS_LOADED;

        if(p->havewnd)
        {
            QGeniePagePluginProxy *w=createPage(p->pi);
            p->page=w;

            QString text=p->pi->text();
            w->resize(mStackView->size());
            mStackView->addWidget(w);

#ifdef ENABLE_MULTI_LEFT_BUTTON
            if(bReCreateButtons && (p->leftpanbut.size() == 0))
            {
                p->leftpanbut = leftpan->createButton(p,text,w);
            }
            else if(p->leftpanbut.size() != 0)
            {
                foreach(QGenieButtonStackViewController *leftbut,p->leftpanbut)
                {
                    leftbut->setLink(w);
                    leftbut->setEnabled(true);
                }
            }
#else
            if(bReCreateButtons && (p->leftpanbut==NULL))
            {
                p->leftpanbut=leftpan->createButton(text,w);
            }
            else if(p->leftpanbut)
            {
                p->leftpanbut->setLink(w);
                p->leftpanbut->setEnabled(true);
            }
#endif


            for(int i=0;i<p->butbig.size();i++)
            {
                QGenieButtonBig *bigbut=p->butbig.at(i);
                bigbut->setEnabled(true);
                bigbut->setLink(w);
            }
//            if( p->butbig)
//            {
//                p->butbig->setEnabled(true);
//                p->butbig->setLink(w);
//            }
            p->state=PMS_WND_CREATED;



        }
        else
        {
            p->pi->startup((p->flag&START_MINI)!=0);
        }
        mInfoManager.updateIdxs();
      }
}


void QGeniePluginProxyManager::unloadPi(QString filename)
{
    plug_inf *p=mInfoManager.mFileNameIdx.value(filename);
    if(p && (p->state == PMS_LOADED ||p->state ==PMS_WND_CREATED ||
             p->state==PMS_STARTED))
    {
        QGenieFrameLeftPanel *leftpan=QGenieFrameLeftPanel::GetSingletonPtr();
        ASSERT(leftpan);

        QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
        ASSERT(stackmanager);
        stackmanager->mHomeBut->setChecked(true);
        leftpan->changeStackIdx(0);

        //change to home
        stackmanager->changeStackViewIdx(NULL);

        ASSERT(p->pi);
        ASSERT(mStackView);

        QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
        ASSERT(lanmanager);

        lanmanager->unloadPi(p->pi);


        QGenieInterfaceTrayManager *trayinterface=QGenieInterfaceTrayManager::GetSingletonPtr();
        ASSERT(trayinterface);
        trayinterface->unloadPi(p->pi);

        QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
        ASSERT(traymanager);
        traymanager->unloadPi(p->pi);


        QGenieInerTextManager *innertextmanager=QGenieInerTextManager::GetSingletonPtr();
        ASSERT(innertextmanager);
        innertextmanager->unloadPi(p->uuid);

        if(p->page)
        {
            mStackView->removeWidget(p->page);
            SAFE_DELETE(p->page);
        }
//        if(p->butbig)
//        {
//            p->butbig->setDisabled(true);
//            p->butbig->setLink(NULL);
//        }

        for(int i=0;i<p->butbig.size();i++)
        {
            QGenieButtonBig *bigbut=p->butbig.at(i);
            bigbut->setEnabled(false);
            bigbut->setLink(NULL);
        }
#ifdef ENABLE_MULTI_LEFT_BUTTON
        foreach(QGenieButtonStackViewController *leftbut,p->leftpanbut)
        {
            if(leftbut)
            {
                leftbut->setLink(NULL);
                leftbut->setDisabled(true);
            }
        }
#else
        if(p->leftpanbut)
        {
            p->leftpanbut->setLink(NULL);
            p->leftpanbut->setDisabled(true);
        }
#endif
        p->lanTexts.clear();

        p->state=PMS_UNLOADED;
        p->loader->unload();
        p->pi=NULL;

    }
}


void QGeniePluginProxyManager::detectWidgets()
{
    mLoadOk=false;

    QDir tempdir=GENIE_DIR;
    if (!tempdir.cd(PLUGIN_DIR))
    {
        qDebug("can not find the plugin path");
        return;
    }
    qDebug()<<tempdir.absoluteFilePath(".");



    foreach (QString fileName, tempdir.entryList(QDir::Files))
    {
        if(!isValitPluginFileName(fileName))
        {
            continue;
        }
        mInfoManager.createLineFromFileName(fileName);
    }

    addAllPluginToUi();
    mLoadOk=true;
}
#ifdef CAL_VERSION
QString QGeniePluginProxyManager::calVersionStr() const
{
//    QString ret="0000";
//    QString temp=QString::number(mCalVersion);
//    int len=temp.size();
//    ASSERT(len<=4);
//    ret=ret.replace(4-len,len,temp);
//    return ret;

    return QString::number(mCalVersion);
}
#endif



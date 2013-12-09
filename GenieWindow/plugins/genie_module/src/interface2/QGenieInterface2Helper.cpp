#include "stable.h"
#include "genie2_singlestart.h"
#include "genie2_interface.h"
#ifdef Q_OS_MACX
#define PLUGIN_DIR "../PlugIns"
#else
#define PLUGIN_DIR "."
#endif
#include "QGenieInterface2LogManager.h"
#include "QGenieLaunchInf.h"

bool isValidPluginFileName(const QString &s)
{
#ifdef Q_OS_WIN32
    QRegExp m("GeniePlugin*.dll",Qt::CaseInsensitive);
#elif defined Q_OS_MACX
    QRegExp m("libGeniePlugin*.dylib",Qt::CaseInsensitive);
#else
    QRegExp m("libGeniePlugin*.so",Qt::CaseInsensitive);
#endif
    m.setPatternSyntax(QRegExp::Wildcard);
    return (m.exactMatch(s));
}

static int sCurIdx=0;
static QHash<QUuid,int> sCacheHash;
int indexOfUuid(const QUuid &uuid)
{
	qDebug()<<uuid.toString();
    if(sCacheHash.contains(uuid))
    {
        return sCacheHash.value(uuid);
    }
    sCacheHash.insert(uuid,++sCurIdx);

    return sCurIdx;
}

QUuid uuidOfIndex(int index)
{
    return (sCacheHash.key(index,QUuid()));
}

void initPriorityList(QList<QUuid> &ret_lst)
{
    ret_lst.clear();
    QString fname=NETGEAR_DIR+"priority.dat";
    QFile file(fname);
    if(QDir::home().exists(fname) )
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QDataStream ds(&file);
            ds.setVersion(QDataStream::Qt_4_6);
            ds >> ret_lst;
        }
    }
    else
    {
        ret_lst <<QUuid(PLUGIN_UUID_RESOURCE)
                <<QUuid(PLUGIN_UUID_UI)
                <<QUuid(PLUGIN_UUID_INTERNET)
                <<QUuid(PLUGIN_UUID_WIRELESS)
                <<QUuid(PLUGIN_UUID_NETCONFIG)
                <<QUuid(PLUGIN_UUID_MAP)
                <<QUuid(PLUGIN_UUID_LPC);
        if(file.open(QIODevice::WriteOnly))
        {
            QDataStream ds(&file);
            ds.setVersion(QDataStream::Qt_4_6);
            ds << ret_lst;
        }
    }

}

struct _tPluginInfo
{
    int uuid;
    QPluginLoader *loader;
    QString filename;
    IPluginInf2 *pi;
    int versionNumber;
    short priority;
};

class _PluginInfoHelper
{
    _PluginInfoHelper()
        :mUiPlugin(NULL)
        ,mFrameVersion(-1)
    {

    }
    ~_PluginInfoHelper(){

        QList<QUuid> uuid_list;
        initPriorityList(uuid_list);
        QList<int> iuuid_list;
        int c=uuid_list.count();
        for(int i=0;i<c;i++)
        {
            iuuid_list<<indexOfUuid(uuid_list[i]);
        }
        Q_ASSERT(iuuid_list.count()==c);

        QList<_tPluginInfo *> list=mInfoMap.values();
        foreach(_tPluginInfo *p,list)
        {
            if(!iuuid_list.contains(p->uuid)&&p->loader)
            {
                p->loader->unload();
                delete p->loader;
            }
        }
        for(int i=c-1;i>=0;i--)
        {
            _tPluginInfo *p=mInfoMap.value(iuuid_list[i]);
            if(p && p->loader)
            {
                p->loader->unload();
                delete p->loader;
            }
        }
        foreach(_tPluginInfo *p,list)
        {
            delete p;
        }

    }
    IPluginInf2 * loadFile(const QString &fname)
    {

        QPluginLoader *loader=new QPluginLoader(fname);
#ifdef Q_OS_MACX
        QString fulname=qApp->applicationDirPath()+"/../PlugIns/"+fname;
        loader->setFileName(fulname);
#else
        loader->setFileName(fname);
#endif

        QObject *o=loader->instance();
        IPluginInf2 *pi = qobject_cast<IPluginInf2 *>(o);

		int itemp=pi?rememberPiInfo(pi,fname,loader):0;
        switch(itemp)
        {
        case 0:
            //error
            loader->unload();
            GENIE2_SAFEDELETE(loader);
            return NULL;
        case 1:
            //success
            return pi;
        case 2:
            //already remmembed

            Q_ASSERT(0);
            GENIE2_SAFEDELETE(loader);

        default:
            Q_ASSERT(0);

        };


        return NULL;
    }
    //return 0.error    1.success   2.already remmembed
    int rememberPiInfo(IPluginInf2 *pi,const QString &fname,QPluginLoader *loader)
    {

        QString stemp=pi->command(I2CMD_UUID).toString();
        int uuid=indexOfUuid(QUuid(stemp));
        bool btemp;
        int version=pi->command(I2CMD_VERSIONNUMBER).toString().replace(".","").toInt(&btemp,10);
        Q_ASSERT(btemp);
        qDebug()<<"rememberPiInfo():"<<version;
        if(mFrameVersion <0)
        {
            mFrameVersion=QString(VERSION_STRING).replace(".","").toInt(&btemp,10);
            Q_ASSERT(mFrameVersion>=0 && btemp);
        }
        qDebug()<<"mFrameVersion:"<<mFrameVersion;
        if(mFrameVersion/100==version/100)
        {
            if(!mInfoMap.contains(uuid))
            {
                _tPluginInfo *pInfo=new _tPluginInfo;
                pInfo->uuid=uuid;
                pInfo->priority=-1;
                pInfo->versionNumber=version;
                pInfo->filename=fname;
                pInfo->loader=loader;
                pInfo->pi=pi;
                mInfoMap.insert(uuid,pInfo);
                return 1;
            }
            else
            {
                return 2;
            }
        }
        return 0;


    }

    IPluginInf2 *getUiPlugin()
    {
        if(mUiPlugin)
        {
            return mUiPlugin;
        }
        _tPluginInfo *pInfo=mInfoMap.value(indexOfUuid(PLUGIN_UUID_UI),NULL);
        return (mUiPlugin=((pInfo)?(pInfo->pi):NULL));
    }

    IPluginInf2 *getPlugin(int uuid_idx)
    {
        return mInfoMap.contains(uuid_idx)?mInfoMap[uuid_idx]->pi:NULL;
    }

    IPluginInf2 *getPlugin(const QUuid &uuid)
    {
        return getPlugin(indexOfUuid(uuid));
    }

    QString getPluginFileName(int uuid_idx)
    {
        return mInfoMap.contains(uuid_idx)?mInfoMap[uuid_idx]->filename:"";
    }

    void initUi()
    {
        _tPluginInfo *pInfo=NULL;
        IPluginInf2 *ui_pi=getUiPlugin();
        //step1 tell plugin ui to prepare mainframe
        if(!ui_pi)
        {
            return;
        }
        ui_pi->command(I2CMD_CREATE_MAINFRAME);


        //step2 tell plugins in the priority list to prepare ui
        QList<QUuid> uuid_list;
        initPriorityList(uuid_list);
        for(int i=0;i<uuid_list.size();i++)
        {
            int index=indexOfUuid(uuid_list[i]);
            pInfo=mInfoMap.value(index,NULL);
            if(pInfo)
            {
                pInfo->priority=i;
                pInfo->pi->command(I2CMD_TIMETO_CREATEUI);
            }
        }

        //step3 tell other plugins to prepare ui
        QMap<int,_tPluginInfo *>::Iterator i=mInfoMap.begin();
        while(i!=mInfoMap.end())
        {
            _tPluginInfo *pInfo=i.value();
            if(pInfo && pInfo->pi && pInfo->priority <0)
            {
                pInfo->pi->command(I2CMD_TIMETO_CREATEUI);
            }
            i++;
        }

        //step4 tell ui plugin load ui ok
        ui_pi->command(I2CMD_CREATEUI_END);

        //step5 tell all plugin can start logic now
        tellAllPlutinMessage(I2CMD_TIMETO_START,0);


    }

    void tellAllPlutinMessage(int cmd,const QVariant &paras)
    {
        QMap<int,_tPluginInfo *>::Iterator i=mInfoMap.begin();
        while(i!=mInfoMap.end())
        {
            _tPluginInfo *pInfo=i.value();
            if(pInfo && pInfo->pi /*&& pInfo->priority <0*/)
            {
                pInfo->pi->command(cmd,paras);
            }
            i++;
        }
    }


    QMap<int,_tPluginInfo *> mInfoMap;
    IPluginInf2 *mUiPlugin;
    int mFrameVersion;
    friend class QGenieInterface2Helper;
};



QGenieInterface2Helper::QGenieInterface2Helper()
    :mInfoHelper(new _PluginInfoHelper())
    ,mSingleProcessHandler(new QGenieSingleProcessHandler())
{
    bool multi=QCoreApplication::arguments().contains("-multi");
    mbConflictWithOtherProcess=(!multi && !mSingleProcessHandler->tryCreate());
//    mbConflictWithOtherProcess=true;
    if(!mbConflictWithOtherProcess)
    {
        mSingleProcessHandler->startWatch();
        connect(mSingleProcessHandler,SIGNAL(signal_OtherProcessStarted()),this,SLOT(slot_otherSameProcess()));
#ifdef USE_LAZYLOAD
        mCurLazyPi=0;
        connect(&mLazyTimer,SIGNAL(timeout()),this,SLOT(slot_LazyLoad()));
#endif
        if(!QCoreApplication::arguments().contains("-mini"))
        {
//            QGenieLaunchInf *launchinf=QGenieLaunchInf::GetSingletonPtr();
//            Q_ASSERT(launchinf);
            QGenieSplashManager *splashm=QGenieSplashManager::GetSingletonPtr();
            Q_ASSERT(splashm);
            splashm->showSplash();
        }
    }
    QGenieLaunchInf *thispi=QGenieLaunchInf::GetSingletonPtr();
    Q_ASSERT(thispi);
    mInfoHelper->rememberPiInfo(thispi,"Genie.dll",NULL);

}

QGenieInterface2Helper::~QGenieInterface2Helper()
{
    GENIE2_SAFEDELETE(mInfoHelper);
    disconnect(mSingleProcessHandler,SIGNAL(signal_OtherProcessStarted())
               ,NULL,NULL);
    GENIE2_SAFEDELETE(mSingleProcessHandler);
#ifdef USE_LAZYLOAD
    mLazyTimer.stop();
    disconnect(&mLazyTimer,SIGNAL(timeout()),NULL,NULL);
#endif
}

#ifdef USE_LAZYLOAD
void QGenieInterface2Helper::slot_LazyLoad()
{
    QTime t=QTime::currentTime();
    QMap<int,_tPluginInfo *> &infomap=mInfoHelper->mInfoMap;
    Q_ASSERT(infomap.count());
    QList<int> keys=infomap.keys();
    int c=keys.count();
    int idx=-1;
    mCurLazyPi=(mCurLazyPi+1)%c;
    for(int i=0;i<c;i++)
    {
        int tempidx=(mCurLazyPi+i)%c;
        if(!mCompletePis.contains(keys[tempidx]))
        {
            idx=tempidx;
            bool needreenter=GENIE2_TELL_OTHERPLUGIN(keys[idx],I2CMD_LAYZYLOAD,0).toBool();
            GENIE2_LOG(QString("lazy load cmd to plugin %1").arg(keys[idx]));
            if(!needreenter)
            {
                mCompletePis.insert(keys[idx]);
                break;
            }
            GENIE2_LOG(QString("lazy load %1 used %2 msecs").arg(keys[idx]).arg(t.msecsTo(QTime::currentTime())));
        }
    }

    if(idx <0)
    {
        GENIE2_LOG("lazy load end");
        mLazyTimer.stop();
    }



            //Í£Ö¹ timer µÄÂß¼­
}

#endif

void QGenieInterface2Helper::loadAllPlugins()
{
    QDir tempdir=QDir(qApp->applicationDirPath());
    if (!tempdir.cd(PLUGIN_DIR))
    {
        qDebug("can not find the plugin path");
        return;
    }
    foreach (QString fileName, tempdir.entryList(QDir::Files))
    {
         if(!isValidPluginFileName(fileName))
        {
            continue;
        }
        mInfoHelper->loadFile(fileName);
    }

    mInfoHelper->initUi();
#ifdef USE_LAZYLOAD
    mLazyTimer.start(500);
#endif


}

void QGenieInterface2Helper::registerUi(const QVariant &para)
{
    IPluginInf2 *ui_pi=mInfoHelper->getUiPlugin();
    if(ui_pi)
    {
        ui_pi->command(I2CMD_REGISTER_UI,para);
    }
}

void QGenieInterface2Helper::registerDBGUi(const QVariant &para)
{
    IPluginInf2 *ui_pi=mInfoHelper->getUiPlugin();
    if(ui_pi)
    {
        ui_pi->command(I2CMD_REGISTER_DBG_UI,para);
    }
}

QVariant QGenieInterface2Helper::tellPluginMessage(const QVariant &para)
{
    QList<QVariant> list=para.toList();
    Q_ASSERT(list.size()==3);
    bool btemp;
    QVariant ret;

    IPluginInf2 *pi;
    int idx=list[0].toInt(&btemp);
    if(btemp)
    {
        pi=mInfoHelper->getPlugin(idx);
    }
    else
    {
        pi=mInfoHelper->getPlugin(QUuid(list[0].toString()));
    }
    int cmd=list[1].toInt(&btemp);
    Q_ASSERT(btemp);
    if(pi)
    {
        ret=pi->command(cmd,list[2]);
    }
    return ret;

}

QVariant QGenieInterface2Helper::tellPluginMessage(int uuid, int message,const QVariant &paras)
{
    IPluginInf2 *pi=mInfoHelper->getPlugin(uuid);
    if(pi)
    {
        return pi->command(message,paras);
    }
    return QVariant();
}

void QGenieInterface2Helper::tellAllPlutinMessage(int cmd, const QVariant &paras)
{
    mInfoHelper->tellAllPlutinMessage(cmd,paras);
}

QVariant QGenieInterface2Helper::getImageName(const QVariant &paras)
{
    Q_ASSERT(paras.type()==QVariant::Int);
    bool btemp;
    int type=paras.toInt(&btemp);
    const char *cParas[]={
        "images/route/%1big_1_1.png"
        ,"images/route/%1big_2_1.png"
        ,"images/route/%1big_3_1.png"
        ,"images/route/%1big_3_2.png"
        ,"images/route/%1Normal_1.png"
        ,"images/route/%1Normal_2.png"
        ,"images/route/%1Selected_1.png"
        ,"images/route/%1Selected_2.png"
        ,"images/route/%1OffLine_1.png"
    };
    if(!btemp || (type>=sizeof(cParas)/sizeof(cParas[0])))
    {
        type=0;
    }
    const QString &dbg_model=QGenieIface2ShareObject::sSingleInstance->mDBGRouteModel;
    QString router=dbg_model.isEmpty()
                   ?GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString()
                       :dbg_model;
    int i1=router.indexOf(QRegExp("-|_"));
    if(i1 >0)
    {
        router=router.left(i1);
    }
    QRegExp regx("v(\\d+)",Qt::CaseInsensitive);
    router.replace(regx,"");
    if(router=="" || router=="N/A")
    {
        //set default png name
        router="DEFAULT_NONETGEAR";
    }

    return GENIE2_RES(cParas[type]).arg(router);
}

QString QGenieInterface2Helper::getPluginFileName(int uuid)
{
    return mInfoHelper->getPluginFileName(uuid);
}

//bool QGenieInterface2Helper::rememberPi(IPluginInf2 *pi, const QString &fname)
//{
//    return mInfoHelper->rememberPiInfo(pi,fname);
//}
int QGenieInterface2Helper::loadPlugin(const QString &fname)
{

    if(QDir(qApp->applicationDirPath()
#ifdef Q_OS_MACX
        +"/../PlugIns"
#endif
        ).exists(fname))
    {
        QList<_tPluginInfo *> lst=mInfoHelper->mInfoMap.values();
        foreach(_tPluginInfo *info,lst)
        {
            if(info->filename==fname)
            {
                return info->uuid;
            }
        }
        IPluginInf2 *pi=mInfoHelper->loadFile(fname);
        if(pi)
        {

            QUuid uuid=pi->command(I2CMD_UUID).toString();
            return uuid.isNull()?-1:indexOfUuid(uuid);
        }

    }
        return -1;
}

void QGenieInterface2Helper::initDBGModel(const QVariant &paras)
{
    QAbstractItemModel *model=static_cast<QAbstractItemModel *>(paras.value<void *>());
    Q_ASSERT(model);
    model->removeRows(0,model->rowCount());
    int itemp=model->rowCount();
//    model->insertRows(itemp,1);
//    model->setData(model->index(itemp,0),"Genie.dll");
//    model->setData(model->index(itemp,1),VERSION_STRING);

    QMap<int,_tPluginInfo *>::Iterator i=mInfoHelper->mInfoMap.begin();
    while(i!=mInfoHelper->mInfoMap.end())
    {
        _tPluginInfo *pInfo=i.value();
        if(pInfo )
        {
            itemp=model->rowCount();
            model->insertRows(itemp,1);
            model->setData(model->index(itemp,0),pInfo->filename);
            int &version=pInfo->versionNumber;
            model->setData(model->index(itemp,1),
                           QString("").sprintf("%2d.%2d.%2d.%2d",version/1000000,(version/10000)%100,(version/100)%100,version%100));
            model->setData(model->index(itemp,2),pInfo->priority);
            model->setData(model->index(itemp,3)
                           ,QString("[%1]%2").arg(pInfo->uuid).arg(uuidOfIndex(pInfo->uuid).toString()));
        }
        i++;
    }
}


void QGenieInterface2Helper::slot_otherSameProcess()
{
    GENIE2_LOG("popup QGenieInterface2Helper::slot_otherSameProcess()");
    GENIE2_UI_COMMAND(UICMD_SHOW_MAINFRAME,0);
}

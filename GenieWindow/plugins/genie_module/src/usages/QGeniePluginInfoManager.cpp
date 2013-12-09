#include "stable.h"

plug_inf::plug_inf():
        loader(NULL)
        ,state(PMS_INIT)
        ,pi(NULL)
        ,page(NULL)
   //     ,butbig(NULL)
#ifndef ENABLE_MULTI_LEFT_BUTTON
        ,leftpanbut(NULL)
#endif
        ,flag(0)
{

}

QGeniePluginInfoManager::QGeniePluginInfoManager()
{
}


QGeniePluginInfoManager::~QGeniePluginInfoManager()
{
    while(!mInfList.isEmpty())
    {
        bool btemp=false;
        plug_inf *p=mInfList.takeAt(0);
        GENIE_LOG(QString("start delete plugin ")+p->filename);
        if(p->loader)
        {
            btemp=p->loader->unload();
            if(!btemp)
            {
                btemp=p->loader->unload();
            }
        }
        SAFE_DELETE(p->loader);
//        SAFE_DELETE(p->pi);
        GENIE_LOG(QString("end delete plugin")+p->filename+" "+QString::number(btemp));
        SAFE_DELETE(p);
    }
}

void QGeniePluginInfoManager::createLineFromFileName(QString filename)
{
    plug_inf *temp=mFileNameIdx.value(filename,NULL);
    ASSERT(temp==NULL);

    plug_inf *p=new plug_inf();
    p->filename=filename;
    mInfList.append(p);
    mFileNameIdx.insert(filename,p);
#ifdef USE_DEBUG_PAN
    QGenieFrameDebug *dbgpan=QGenieFrameDebug::GetSingletonPtr();
    ASSERT(dbgpan);
    dbgpan->mUi.comboBox->addItem(filename);
#endif

}

plug_inf *QGeniePluginInfoManager::getLineFromFileName(QString filename)
{
    return mFileNameIdx.value(filename,NULL);
}

void QGeniePluginInfoManager::updateIdxs()
{
    mPiIdx.clear();
    mPageIdx.clear();
    mLeftButtonIdx.clear();
    mBigButtonIdx.clear();
    mUuidIdx.clear();
    for(int i=0;i<mInfList.size();i++)
    {
        plug_inf *p=mInfList.at(i);
        mPiIdx.insert(p->pi,p);
        if(p->page)
        {
            mPageIdx.insert(p->page,p);
        }
#ifdef ENABLE_MULTI_LEFT_BUTTON
        foreach(QGenieButtonStackViewController * leftbut,p->leftpanbut)
        {
            if(leftbut)
            {
                mLeftButtonIdx.insert(leftbut,p);
            }
        }
#else
        if(p->leftpanbut)
        {
            mLeftButtonIdx.insert(p->leftpanbut,p);
        }
#endif


        for(int i=0;i<p->butbig.size();i++)
        {
            QGenieButtonBig *but=p->butbig[i];
            mBigButtonIdx.insert(but,p);
        }

        mUuidIdx.insert(p->uuid,p);
    }
}

void QGeniePluginInfoManager::clearAllCaches()
{
    for(int i=0;i<mInfList.size();i++)
    {
        plug_inf *p=mInfList.at(i);
//        p->stateCache.clear();
        p->styleCache.clear();
        p->substateCache.clear();
        p->bkgCache.clear();
    }
}

void QGeniePluginInfoManager::synToModel(QAbstractItemModel *model)
{
    if(!model)
    {
        return;
    }
    QString stemp;
    model->removeRows(0,model->rowCount());
    foreach (plug_inf *p, mInfList) {
        if(!p){continue;}
        int itemp=model->rowCount();
        model->insertRows(itemp,1);
        QModelIndex idx=model->index(itemp,0);
        model->setData(idx,p->filename);
        idx=model->index(itemp,1);
        model->setData(idx,p->version);

        stemp="";
        idx=model->index(itemp,2);
        if(p->pi)
        {
            stemp=QString::number(p->pi->priority());
        }
        model->setData(idx,stemp);

        const char *cStrs[]={
          "PMS_INIT","PMS_LOADED","PMS_WND_CREATED","PMS_UNLOADED",
            "PMS_STARTED","PMS_ERROR"
        };
        int state=p->state;
        if(state <0 || state >=sizeof(cStrs)/sizeof(cStrs[0]))
        {
            state=PMS_ERROR;
        }
        idx=model->index(itemp,3);
        model->setData(idx,cStrs[state]);

        stemp="";
        idx=model->index(itemp,4);
        if(p->pi)
        {
            QStringList lst=p->pi->bigButtonParas();
            foreach(QString s,lst)
            {
                stemp+=(stemp=="")?s:(QString("\n%1").arg(s));
            }
        }
        model->setData(idx,stemp);



    }
}

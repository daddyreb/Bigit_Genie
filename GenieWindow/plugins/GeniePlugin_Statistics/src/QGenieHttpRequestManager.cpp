#include "QGenieHttpRequestManager.h"
#include "SvtNetworkTool.h"
#include "genie2_interface.h"

QGenieHttpRequestThread::QGenieHttpRequestThread(QGenieHttpRequestManager *manager)
    :mManager(manager)
{

}

void QGenieHttpRequestThread::run()
{
    while(!mManager->mExit)
    {
        bool isempty;
        QString path;
        int session;
        mManager->mMutex.lock();
        isempty=mManager->mSessionCache.isEmpty();
        if(!isempty)
        {
            path=mManager->mPathCache.front();
            session=mManager->mSessionCache.front();
            mManager->mPathCache.pop_front();
            mManager->mSessionCache.pop_front();
        }
        mManager->mMutex.unlock();
        if(isempty)
        {
            break;
        }
        SVT_Http http(&(mManager->mExit));
        const QByteArray &retarray=http.getHttpResult(mManager->mAddress,path
                                                      ,mManager->mPort);

        QString body=retarray;
        bool ok=(
                    !body.isEmpty() &&(body.indexOf("errorcode")<0)
                );
        emit signal_Done(session,!ok,body);

    }
} 


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



QGenieHttpRequestManager::QGenieHttpRequestManager(QObject *parent)
    :QObject(parent)
    ,mSavedCurSession(0)
    ,mThread(this)
    ,mExit(false)
{
    connect(&mThread,SIGNAL(signal_Done(int,bool,QString))
            ,this,SIGNAL(signal_Done(int,bool,QString)));
    mAddress=GENIE2_GET_SETTING(PLUGIN_STATICS_IP).toString();
    mPort=GENIE2_GET_SETTING(PLUGIN_STATICS_PORT).toInt();
}

QGenieHttpRequestManager::~QGenieHttpRequestManager()
{
    mExit=true;
    mThread.wait();
}

int QGenieHttpRequestManager::addRequest(const QString &path)
{
    mMutex.lock();
    int session=mSavedCurSession++;
    mSessionCache.append(session);
    mPathCache.append(path);

    mThread.start();
    mMutex.unlock();
	return session;
}

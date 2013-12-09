#include "QGenieTray_Communication_Verify.h"

#ifdef SECURE_LOCALSOCKET
#define MAX_CACHE_TABLE_LEN 20
#define MAX_RETRY_COUNT 10
#define CACHE_TABLE_LEN 10
#define RETRY_COUNT 3
#define WAIT_TIME   300
#define MAX_SESSION_NUMBER 2000000000

QGenieTray_Communication_Verify::QGenieTray_Communication_Verify(QObject *parent) :
    QObject(parent),
    mLocalSock(NULL),
    mWaitTime(WAIT_TIME),
    mRetryCount(RETRY_COUNT),
    mCacheLen(CACHE_TABLE_LEN),
    mRetSession(-1)
{
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(slot_timeout()));

  
    mTimer.start(WAIT_TIME);
}
QGenieTray_Communication_Verify::~QGenieTray_Communication_Verify()
{
    mLocalSock = NULL;
}

//==================================
//  slot funtion
//==================================

////////////////////////////////////////////////////////////
//        NAME: slot_timeout()
// DESCRIPTION: time out.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-23
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::slot_timeout()
{
    if(mCacheList.isEmpty())
    {
        return;
    }

    QString retry;
    QStringList lst;
    int nRetry = 0;
    QString tmp;

    tmp = mCacheList[0].toString();
    lst = tmp.split(";");
    mCacheList.pop_front();
	if(lst.length() < 6)
	{
		return;
	}
	retry = lst[3];

    if(retry.toInt() > 0)
    {
        //retry > 0
        nRetry = retry.toInt();
        nRetry -= 1;
        retry.clear();
        retry = QString("%1;%2;%3;%4;%5;%6").arg(lst[0], lst[1], lst[2]).arg(nRetry).arg(lst[4], lst[5]);
        mCacheList.push_front(retry);

        uiCommand();

    }

}
//==================================
//  set funcion
//==================================

////////////////////////////////////////////////////////////
//        NAME: setLocalSocket()
// DESCRIPTION: set Local socket.
//   ARGUMENTS: const QLocalSocket &localSock.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::setLocalSocket(QLocalSocket *localSock)
{
//    Q_ASSERT(!mLocalSock);
    mLocalSock = localSock;
}

////////////////////////////////////////////////////////////
//        NAME: setCacheTableLen()
// DESCRIPTION: Set Cache Table length.
//   ARGUMENTS: int len.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::setCacheTableLen(int len)
{
    if(len >= MAX_CACHE_TABLE_LEN)
    {
        mCacheLen = MAX_CACHE_TABLE_LEN;
    }
    else
    {
        mCacheLen = (len > 0) ? len : CACHE_TABLE_LEN;
    }
}
////////////////////////////////////////////////////////////
//        NAME: setRetryCount()
// DESCRIPTION: Set command send repeat frequency.
//   ARGUMENTS: int count.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::setRetryCount(int count)
{
    if(count >= MAX_RETRY_COUNT)
    {
        mRetryCount = MAX_RETRY_COUNT;
    }
    else
    {
        mRetryCount = (count > 0) ? count : RETRY_COUNT;
    }
}
////////////////////////////////////////////////////////////
//        NAME: setWaitTime()
// DESCRIPTION: Set waiting for the command return time.
//   ARGUMENTS: int sec.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::setWaitTime(int sec)
{
    if(sec < WAIT_TIME)
    {
        mWaitTime = (sec > 0) ? sec : WAIT_TIME;
    }
}

////////////////////////////////////////////////////////////
//        NAME: addNewData()
// DESCRIPTION: Add a new data to cache table.
//   ARGUMENTS: const QString newData.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
int QGenieTray_Communication_Verify::addNewData(const QString newData, ROLE role)
{
    QString lst = newData;
    int nSession = 0;
    int nLen = mCacheList.count();

    if(nLen >= mCacheLen || newData.isEmpty())
    {
		Q_ASSERT("addNewData");
        return -1;
    }

    //////Assign a new session number
    static int g_session = 0;
    nSession = ((++g_session) >= MAX_SESSION_NUMBER) ? (g_session = 0) : g_session;

    //    //////Get rid of a repeated item
    //    for(int i = 0; i < nLen; i++)
    //    {
    //        lst = mCacheList[i].split(",");
    //        if(lst[2] == session)
    //        {
    //            return true;
    //        }
    //    }

    lst += QString(";%1;%2;%3").arg(mRetryCount).arg(nSession).arg(role);
    mCacheList.append(lst);

    return nSession;
}
////////////////////////////////////////////////////////////
//        NAME: clearCacheTable()
// DESCRIPTION: clear cache table.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::clearCacheTable()
{
    mCacheList.clear();
}
////////////////////////////////////////////////////////////
//        NAME: initData()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::initData()
{
    mLocalSock = NULL;
    mWaitTime = WAIT_TIME;
    mRetryCount = RETRY_COUNT;
    mCacheLen = CACHE_TABLE_LEN;
    mCacheList.clear();
	mRetSession = -1;
}
////////////////////////////////////////////////////////////
//        NAME: initData()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
bool QGenieTray_Communication_Verify::delData(int session)
{
    int nLen = mCacheList.count();
    QStringList lst;

    if(nLen <= 0)
    {
        return false;
    }

    for(int i = 0; i < nLen; i++)
    {
        lst = mCacheList[i].toString().split(";");
        if((lst.count()>4) && (lst[4].toInt() == session))
        {
            mCacheList.removeAt(i);
            return true;
        }
    }

    return false;
}
////////////////////////////////////////////////////////////
//        NAME: chkCurId()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-22
////////////////////////////////////////////////////////////
bool QGenieTray_Communication_Verify::chkCurId(int session)
{
    bool bRet = delData(session);
    return bRet;
}
////////////////////////////////////////////////////////////
//        NAME: chkCurId()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-23
////////////////////////////////////////////////////////////
void QGenieTray_Communication_Verify::uiCommand()
{
    if(mLocalSock == NULL ||mCacheList.isEmpty())
    {
        return;
    }

    QString tmp = mCacheList[0].toString();
    if(mLocalSock->state() == QLocalSocket::ConnectedState)
    {
        QDataStream out(mLocalSock);
        out.setVersion(QDataStream::Qt_4_6);
        out << tmp;
    }
}
/***********************************************************
* DESCRIPTION: overload function.
*      AUTHOR: Chensi
*        DATA: 2012-2-23
***********************************************************/
void QGenieTray_Communication_Verify::uiCommand(const QString &objname)
{
    qDebug() << "QGenieTray_Communication_Verify::uiCommand() " << objname;

//    QString strData = QString("act,") + objname;

    int nPos = mCacheList.count();
    int nRet = addNewData(objname, SENDER);
    if(nRet == -1)
    {
        return;
    }

    QString tmp = mCacheList[nPos].toString();

    if(mLocalSock == NULL)
    {
        return;
    }

    if(mLocalSock->state()==QLocalSocket::ConnectedState)
    {
        QDataStream out(mLocalSock);
        out.setVersion(QDataStream::Qt_4_6);
        out << tmp;
    }
}
////////////////////////////////////////////////////////////
//        NAME: chkCurId()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-23
////////////////////////////////////////////////////////////
int QGenieTray_Communication_Verify::respond(int session)
{
    if(mLocalSock == NULL)
    {
        return -1;
    }

	if(mRetSession == session || mRetSession > session)
	{
		return -1;
	}
	mRetSession = session;

    ////action,command,para,retryCount,session,role
    QString tmp = QString("%1;%2;%3;%4;%5;%6").arg("act", "respondMsg", "1").arg(0).arg(session).arg(RECEIVER);

    if(mLocalSock->state()==QLocalSocket::ConnectedState)
    {
        QDataStream out(mLocalSock);
        out.setVersion(QDataStream::Qt_4_6);
        out << tmp;
    }

	return 0;	////0 :ok  -1: err
}
////////////////////////////////////////////////////////////
//        NAME: chkCurId()
// DESCRIPTION: init.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-23
////////////////////////////////////////////////////////////
int QGenieTray_Communication_Verify::getCmdAndPara(QString &para, int role)
{
    QStringList lst = para.split(";");
    if(lst.count() < 6)
    {
        return -1;
    }

	switch(role)
	{
	case 0:

    para.clear();
    para = lst[1];    //command

    return lst[2].toInt();
	case 1:
		para.clear();
		para = lst[2];
		return lst[1].toInt();
	default:
		break;
	}
	return -1;
}
int QGenieTray_Communication_Verify::getRole(QString &para)
{
    QStringList lst = para.split(";");
    if(lst.count() < 6)
    {
        return UNKNOW;
    }

    return lst[5].toInt();
}

int QGenieTray_Communication_Verify::getSession(QString &para)
{
    QStringList lst = para.split(";");
    if(lst.count() < 6)
    {
        return -1;
    }

    return lst[4].toInt();
}
#endif

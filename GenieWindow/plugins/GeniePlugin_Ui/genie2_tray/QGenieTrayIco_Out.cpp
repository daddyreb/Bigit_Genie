#include "QGenieTrayManager.h"
#include "QGenieTrayIco_Out.h"
#include "trayico_out_cmds.h"



#define TRAY_UUID "_NETGEARGenieTray_%1_%2_"
#define CMD_CACHE_SIZE 10
static int sIdx=0;
#define SERVICE_WAITSECS_BEFORE_RESTART 20


#define MAX_SEND_COUNT 3


//#include "genie2_interface.h"
QGenieTrayIco_Out::QGenieTrayIco_Out(QGenieTrayManager *manager)
    :mTrayManager(manager)
    ,mLocalSever(NULL)
    ,mLocalSocket(NULL)
    ,mSession(0)
{
    startService();
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_RestartServer()));
//    connect(&mRetryTimer,SIGNAL(timeout()),this,SLOT(slot_retryLostCmd()));
    //    connect(&mLocalSock,SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),this,SLOT(slot_LocalSock_StateChange(QLocalSocket::LocalSocketState)));
}

QGenieTrayIco_Out::~QGenieTrayIco_Out()
{
    if(mLocalSocket)
    {
        disconnect(mLocalSocket,SIGNAL(disconnected()),NULL,NULL);
        mLocalSocket->abort();
    }
    if(mLocalSever)
    {
        disconnect(mLocalSever,SIGNAL(newConnection()),NULL,NULL);
        delete mLocalSever;
    }

    mTimer.stop();
//    mRetryTimer.stop();
}

void QGenieTrayIco_Out::startService()
{
    if(mLocalSocket)
    {
        disconnect(mLocalSocket,SIGNAL(disconnected()),NULL,NULL);
        mLocalSocket->abort();
        mLocalSocket->deleteLater();
        mLocalSocket=NULL;
    }
    if(mLocalSever)
    {
        mLocalSever->close();
        disconnect(mLocalSever,SIGNAL(newConnection()),NULL,NULL);

        mLocalSever->deleteLater();
    }


    mSession=0;
    mCommandCache.clear();
    mLocalSever=new QLocalServer();
    connect(mLocalSever,SIGNAL(newConnection()),this,SLOT(slot_SockNewConnection()));

    //    connect(mLocalSever,SIGNAL(disconnected()),this,SLOT(slot_SockDisconnected()));
    //    connect(mLocalSever,SIGNAL(error(QLocalSocket::LocalSocketError)),this,SLOT(slot_SockError(QLocalSocket::LocalSocketError)));
    QString sockname=QString(TRAY_UUID).arg(QUuid::createUuid().toString()).arg(sIdx++);
#ifdef DBG_MODE
    sockname="test_genie";
#endif
    mTimer.setSingleShot(true);
    mTimer.start(1000*SERVICE_WAITSECS_BEFORE_RESTART);

    mLocalSever->listen(sockname);
    QStringList para;
    para<<"-start"<<sockname;
    QProcess::startDetached("genie2_tray.exe",para);
    Q_ASSERT(QDir::current().exists("genie2_tray.exe"));


}

void QGenieTrayIco_Out::uiCommand(int cmd,const QString &para)
{
//    slot_retryLostCmd();
    QVariantList cmd_package ;
    cmd_package<< cmd <<para;
    if(mLocalSocket && mLocalSocket->state()==QLocalSocket::ConnectedState)
    {
        real_uiCommand(cmd,para);
    }
    else
    {
        mCommandCache.append(QVariant(cmd_package));
    }
}

//return 0.ok   1.error     2.timeout
void QGenieTrayIco_Out::real_uiCommand(int cmd, const QString &para)
{
    if(!mLocalSocket || !(mLocalSocket->state()==QLocalSocket::ConnectedState))
        return;

	QByteArray lba;
	QDataStream out(&lba, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	QString stemp;
	stemp=QString("%1*%2#%3").arg(QString::number(mSession++),QString::number(cmd),para);

#ifdef _DEBUG
	QFile f("./genie_log.txt");
	if(f.open(QIODevice::Append))
	{
		QTextStream s(&f);
		s<<stemp<<"\n";
		f.close();
	}
#endif

	out <<stemp;

	if(mLocalSocket->write(lba)!=lba.length())
		qDebug()<<"send tray msg failed!";
}

void QGenieTrayIco_Out::slot_SockNewConnection()
{


    qDebug()<<"Server::slot_Connected()";
    //    mConnected=true;
    //  mExitTimer.stop();
    mTimer.stop();

    QLocalSocket *clientConnection = mLocalSever->nextPendingConnection();
    if(mLocalSocket)
	{
		return;
	}

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    connect(clientConnection,SIGNAL(readyRead()),
            this,SLOT(slot_SockRead()));
    //#ifndef DBG_MODE
    connect(clientConnection,SIGNAL(disconnected()),this,SLOT(slot_RestartServer()));
    //#endif
    connect(clientConnection,SIGNAL(destroyed(QObject*)),this,SLOT(slot_LocalSocketDeleted(QObject *)));

    mLocalSocket=clientConnection;


    emit mTrayManager->canInitNow();
    sendCachedCmds();

}



void QGenieTrayIco_Out::slot_RestartServer()
{
    /*********CHENSI 2012-2-23**********/
    startService();
}

void QGenieTrayIco_Out::slot_SockRead()
{
    QLocalSocket *clientConnection=qobject_cast<QLocalSocket*> (sender());
    Q_ASSERT(clientConnection);
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_6);
    QString stemp;
    while(!in.atEnd())
    {
        in >>stemp;
        QStringList lst=stemp.split(",");

#ifdef SECURE_SEND
        if(lst.count() >= 4)
        {
            if((lst[0] == "act") && (lst[3].toInt() > mSession))
            {
                emit mTrayManager->actionTrigged(QString("%1,%2").arg(lst[1],lst[2]));
                mSession = lst[3].toInt();
            }
        }
#else
        if(lst.count()>=3)
        {
            if(lst[0]=="act")
            {
                emit mTrayManager->actionTrigged(QString("%1,%2").arg(lst[1],lst[2]));
            }
        }
#endif
    }
}

void QGenieTrayIco_Out::slot_LocalSocketDeleted(QObject *obj)
{
    QLocalSocket *sock=static_cast<QLocalSocket *>(obj);
    if(sock==mLocalSocket || !mLocalSocket)
	{
		mLocalSocket=NULL;
	}
}

void QGenieTrayIco_Out::sendCachedCmds()
{

    int c=mCommandCache.count();
    for(int i=0;i<c;i++)
    {
        QVariantList para=mCommandCache[i].toList();
        Q_ASSERT(para.count()>1);
        real_uiCommand(para[0].toInt(),para[1].toString());
    }
    mCommandCache.clear();
}

#ifndef DBG_MODE
#define MESSAGE_SHOWING_TIME 3000

static QString sName="kkbbuyodf;ahf";
static int sSession=0;






#endif

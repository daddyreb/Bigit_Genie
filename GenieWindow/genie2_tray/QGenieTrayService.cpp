#include "QGenieTrayService.h"
#include "genie2_tray.h"
#include <QtCore>
#include "genie2_interface.h"
#include <QPluginLoader>

/*********2012-2-23**********/
#ifdef SECURE_LOCALSOCKET
#include "QGenieTray_Communication_Verify.h"
#endif

#define WAIT_SECS_WHENSTARTUP (60)
#define SESSION_CACHE_SIZE 20
 QGenieTrayService::QGenieTrayService(QObject *parent)
    :QObject(parent)
    ,mTrayManager(new QGenieTrayManager(1))
#ifdef SECURE_LOCALSOCKET
    ,mVerify(new QGenieTray_Communication_Verify(parent))
#endif
    ,mCurrentSession(-1)
{
    QPluginLoader loader("GeniePlugin_Resource.dll");
    loader.setFileName("GeniePlugin_Resource.dll");
    //    loader.instance();
    QObject *o=loader.instance();
    Q_ASSERT(o);

    connect(&mTimer,SIGNAL(timeout()),qApp,SLOT(quit()));
    mTimer.start(WAIT_SECS_WHENSTARTUP);

    connect(mTrayManager,SIGNAL(actionTrigged(QString)),this,SLOT(slot_ActionTrigged(QString)));

    connect(&mLocalSock,SIGNAL(connected()),this,SLOT(slot_SockConnected()));
    connect(&mLocalSock,SIGNAL(disconnected()),qApp,SLOT(quit()));
    connect(&mLocalSock,SIGNAL(error(QLocalSocket::LocalSocketError)),qApp,SLOT(quit()));
    connect(&mLocalSock,SIGNAL(readyRead()),this,SLOT(slot_Read()));
    //qDebug()<<"QGenieTrayService::QGenieTrayService()" << qApp->arguments().value(2);
    mLocalSock.connectToServer(qApp->arguments().value(2));

    /*********CHENSI 2012-2-23**********/
#ifdef SECURE_LOCALSOCKET
    mVerify->setLocalSocket(&mLocalSock);
#endif
}

QGenieTrayService::~QGenieTrayService()
{
    delete mTrayManager;
}


void QGenieTrayService::slot_SockConnected()
{
    mTimer.stop();
}


void QGenieTrayService::slot_Read()
{
    qDebug()<<"Server::slot_ReadCommand()";

    QLocalSocket *clientConnection=qobject_cast<QLocalSocket*> (sender());
    Q_ASSERT(clientConnection==&mLocalSock);
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_6);
    while(!in.atEnd())
    {
#ifdef SECURE_LOCALSOCKET
        /*********CHENSI 2012-2-23**********/
        //        int itemp;
        //        QString stemp;
        //        in >>itemp>>stemp;
        //        mTrayManager->uiCommand(itemp,stemp);
        QString stemp;
        in >> stemp;

        int nRole = mVerify->getRole(stemp);
        if(nRole == QGenieTray_Communication_Verify::UNKNOW)
        {
            return;
        }
        int nSession = mVerify->getSession(stemp);
        if(nRole == QGenieTray_Communication_Verify::SENDER)
        {
            //respond

            int nRet = mVerify->respond(nSession);

            int itemp = mVerify->getCmdAndPara(stemp);
			if(itemp == -1 || nRet == -1)
			{
				return;
			}
            mTrayManager->uiCommand(itemp, stemp);
        }
        else
        {
            //verify respond
            bool bRet = mVerify->chkCurId(nSession);
        }
#else
        QString stemp;
        bool btemp;
        in >>stemp;
        int istar=stemp.indexOf("*");
        int ipound=stemp.indexOf("#");
        int len=stemp.length();
#ifdef _DEBUG
        QFile f("./genie2_tray_log.txt");
        if(f.open(QIODevice::Append))
        {
            QTextStream s(&f);
            s<<stemp<<"\n";
            f.close();
        }

#endif

        int session=stemp.left(istar).toInt(&btemp);
        if(istar>0 && ipound >istar && (session >= mCurrentSession))
        {
            mCurrentSession=session;
            int itemp=stemp.mid(istar+1,ipound-istar-1).toInt();
            mTrayManager->uiCommand(itemp,stemp.right(len-ipound-1));

        }
        else
        {
#ifdef _DEBUG
        QFile f("./genie2_tray_log.txt");
        if(f.open(QIODevice::Append))
        {
            QTextStream s(&f);
            s<<"parse cmd erro and quit\n";
            f.close();
        }

#endif
            qApp->quit();
        }

#endif

    }
}

void QGenieTrayService::slot_ActionTrigged(const QString &objname)
{
#ifdef SECURE_LOCALSOCKET
    qDebug() << "QGenieTrayService::slot_ActionTrigged() " << objname;

    //    if(mLocalSock.state()==QLocalSocket::ConnectedState)
    //    {
    //        QDataStream out(&mLocalSock);
    //        out.setVersion(QDataStream::Qt_4_6);
    //        out <<QString("act,")+objname;
    //    }

    /*********CHENSI 2012-2-23**********/
    QString tmp = QString("act;") + objname;
    mVerify->uiCommand(tmp);
#else
    if(mLocalSock.state()==QLocalSocket::ConnectedState)
    {
//        QVariantList cmd_package;
//        cmd_package<<int(TIOC_ACTION_TRIGGED)<<objname;
        QDataStream out(&mLocalSock);
        out.setVersion(QDataStream::Qt_4_6);
        out <<QString("act,")+objname;
    }
#endif
}

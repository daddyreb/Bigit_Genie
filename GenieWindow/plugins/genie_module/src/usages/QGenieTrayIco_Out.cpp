#include "stable.h"

#define MESSAGE_SHOWING_TIME 3000

static QString sName="kkbbuyodf;ahf";
static int sSession=0;

QGenieTrayIco_Out::QGenieTrayIco_Out(QGenieTrayManager *traymanager)
    :mLocalSocket(NULL)
    ,mTrayManager(traymanager)
    ,mOn(true)
//    ,mTrayMenu(NULL)
//    ,mQuitAction(NULL)
//    ,mPluginMenuHere(NULL)
//    ,mUpdateAction(NULL)
//    ,mRestoreAction(NULL)
//    ,mSelfTrayMessage(NULL)
//    ,mMoviewManager(NULL)
//    ,mDialogExit(NULL)
{
    //mTrayIcon->show();

    connect(&mStartProcessTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    startup();
}

QGenieTrayIco_Out::~QGenieTrayIco_Out()
{
    mStartProcessTimer.stop();
    SAFE_DELETE(mLocalSocket);
}

void QGenieTrayIco_Out::slot_StartProcess()
{
    if(!mStartProcessTimer.isActive())
    {
        if(mLocalSocket)
        {
            mLocalSocket->abort();
            mLocalSocket->close();
			disconnect(mLocalSocket,NULL);
			delete mLocalSocket;
        }
        

        mLocalSocket= new QLocalSocket(this);
        connect(mLocalSocket,SIGNAL(connected()),this,SLOT(slot_Connected()));
        connect(mLocalSocket, SIGNAL(readyRead()), this, SLOT(slot_ReadyRead()));
        connect(mLocalSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(displayError(QLocalSocket::LocalSocketError)));
        connect(mLocalSocket,SIGNAL(disconnected()),this,SLOT(slot_Disconnected()));

        sName=QString("%1_%2").arg(QUuid::createUuid().toString()).arg(sSession++);
        QStringList para;
		para<<"-start"<<sName;
         QProcess::startDetached("genie_tray.exe",para);
         mStartProcessTimer.setSingleShot(true);
         mStartProcessTimer.start(5000);
     }
}

void QGenieTrayIco_Out::sendCommand(const QString &cmd)
{
    ASSERT(mLocalSocket);
	int test=mLocalSocket->state();
    if(mLocalSocket->state()!=QLocalSocket::ConnectedState)
    {
        mCommandList <<cmd;
        if(!mStartProcessTimer.isActive())
        {
            mLocalSocket->abort();
            mLocalSocket->connectToServer(sName);
        }
        qDebug("connect to server");
    }
    else{
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << cmd;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));
        mLocalSocket->write(block);
        mLocalSocket->flush();

        qDebug()<<"send cmd "<<cmd;


    }
}

void QGenieTrayIco_Out::slot_Connected()
{
    qDebug()<<"Client::slot_Connected()";

    foreach (QString cmd, mCommandList) {
        sendCommand(cmd);

    }
    mCommandList.clear();
   //    clientConnection->disconnectFromServer();
}
void QGenieTrayIco_Out::slot_Disconnected()
{
    qDebug()<<"Client::slot_Disconnected()";
    if(mOn)
    {
        QTimer::singleShot(0,this,SLOT(slot_StartProcess()));
    }
}

void QGenieTrayIco_Out::slot_ReadyRead()
{
    qDebug()<<"QGenieTrayIco_Out::slot_ReadyRead()";

    QLocalSocket *clientConnection=qobject_cast<QLocalSocket*> (sender());
    Q_ASSERT(clientConnection);
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    QString command;

    quint16 blocksize;
    while(!in.atEnd())
    {
        qDebug()<<clientConnection->bytesAvailable();
        if (clientConnection->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blocksize;

        if (in.atEnd())
        {
            qDebug()<<"in.atEnd()";
            return;
        }


    #if 0
        in >> array;
        command=array.constData();
    #else
        in >> command;
    #endif
        parseCommand(command);
    }

}

void QGenieTrayIco_Out::slot_Timeout()
{
    if(mTranslateStr != "")
    {
        sendCommand(mTranslateStr);
    }
    else
    {
        sendCommand("check alive");
    }
}

void QGenieTrayIco_Out::parseCommand(const QString &cmd)
{
    qDebug()<<"QGenieTrayIco_Out::parseCommand";
    if("update"==cmd)
    {
 //         mLocalSocket->abort();
      startUpdate();
    }
    else if("exit"==cmd)
    {
        exit();
    }
    else if("show"==cmd)
    {
        showNormal();
    }
    else if("autostart_yes"==cmd || "autostart_no" ==cmd )
    {
        mTrayManager->slot_SetAutoStart(cmd=="autostart_yes");
    }
    else if("shownetworkchange_yes"==cmd || "shownetworkchange_no"==cmd)
    {
        mTrayManager->slot_SetShowNetworkChange("shownetworkchange_yes"==cmd);
    }
    else
    {
        GENIE_LOG(QString("QGenieTrayIco_Out received a unknown message [%1],"
              "so abort then wait reestablish the connection").arg(cmd));
        mLocalSocket->abort();
    }
}

void QGenieTrayIco_Out::displayError(QLocalSocket::LocalSocketError socketError)
{
    QStringList para;
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
       GENIE_LOG("The host was not found. And Restart it");
        break;
    case QLocalSocket::ConnectionRefusedError:
        GENIE_LOG("The connection was refused by the peer. ");
        break;
    case QLocalSocket::PeerClosedError:
        GENIE_LOG("QLocalSocket::PeerClosedError:");
        break;
    default:
        GENIE_LOG(QString("The following error occurred: %1.")
                  .arg(mLocalSocket->errorString()));
    }
    mLocalSocket->abort();

}

void QGenieTrayIco_Out::startMovie()
{
    sendCommand("startmovie");
}

void QGenieTrayIco_Out::stopMovie()
{
    sendCommand("stopmovie");

}

void QGenieTrayIco_Out::setToolTip(const QString &tooltip)
{
    QString cmd=QString("tooltip##%1").arg(tooltip);
    sendCommand(cmd);
}

void QGenieTrayIco_Out::setMessage(const QString &msg)
{
    QString cmd=QString("message##%1").arg(msg);
    sendCommand(cmd);
}

void QGenieTrayIco_Out::setIcon(const QString &ico)
{
    QString cmd=QString("setico##%1").arg(ico);
    sendCommand(cmd);

}

void QGenieTrayIco_Out::retranslateUi(const QStringList &texts)
{
    if(texts.count()<11)
    {
        GENIE_LOG("error QGenieTrayIco_Out::retranslateUi");
        return;
    }
    QString para="";
    foreach(QString tempstr,texts)
    {
        if(para!="")
        {
            para+="##";
        }
        para+=tempstr;
    }
    QString cmd=QString("translate##%1").arg(para);
    sendCommand(cmd);
    mTranslateStr=cmd;

//    mRestoreAction->setText(texts[0]);
//    mUpdateAction->setText(texts[1]);
//    mQuitAction->setText(texts[2]);
//    //todo

}

void QGenieTrayIco_Out::exit()
{
    mOn=false;
    mLocalSocket->abort();
    mTrayManager->exit();
}

void  QGenieTrayIco_Out::addPluginAction(QAction *act)
{
    ASSERT(0);
}

QAction* QGenieTrayIco_Out::addPluginMenu(QMenu *menu)
{
    ASSERT(0);
    return NULL;
}

void QGenieTrayIco_Out::setActionState(const QMap<int ,bool> &para)
{
    QString cmd="setactionchecked";
    for(int i=0;i<_TRAY_PARA_NUM;i++)
    {
        ASSERT(para.contains(i));
        cmd=QString("%1##%2").arg(cmd,para.value(i)?"y":"n");
    }
    sendCommand(cmd);
}

void QGenieTrayIco_Out::startup()
{


    slot_StartProcess();

}

// void QGenieTrayIco_Out::slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason)
// {
//     qDebug()<<"QGenieTrayIco_Out::slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason)";
//     if(mTrayManager)
//     {
//         mTrayManager->trayIconClicked(reason);
//     }

// }

// void QGenieTrayIco_Out::slot_ExitApp()
// {
//    exit();
// }

 void QGenieTrayIco_Out::startUpdate()
 {
     QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
     ASSERT(main);
     main->slot_ShowNormal();
     ASSERT(mTrayManager);
     if(mTrayManager)
     {
         mTrayManager->startUpdate();
     }
 }

 void QGenieTrayIco_Out::setLayoutDirection(Qt::LayoutDirection drt)
 {
     QString cmd="layout##%1";
     cmd=cmd.arg((drt==Qt::LeftToRight)?"0":"1");
     sendCommand(cmd);
 }


 void QGenieTrayIco_Out::showNormal()
 {
     QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
     ASSERT(main);
     main->slot_ShowNormal();


 }



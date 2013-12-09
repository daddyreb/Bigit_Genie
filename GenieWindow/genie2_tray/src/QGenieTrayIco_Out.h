#ifndef QGENIETRAYICO_OUT_H
#define QGENIETRAYICO_OUT_H

#include <QtCore>
#include <QtGui>
#include <QLocalSocket>
#include <QLocalServer>
#include "QGenieTrayManager.h"
#include "IGenieTrayIco.h"
class QLocalSocket;
class QGenieDialogExit;
class QGenieTrayManager;


class QGenieTrayIco_Out:public QObject,public IGenieTrayIco
{
    Q_OBJECT
public:
    QGenieTrayIco_Out(QGenieTrayManager *);
    ~QGenieTrayIco_Out();
    void uiCommand(int cmd,const QString &para=QString());
    void real_uiCommand(int cmd,const QString &para=QString());

    QLocalServer *mLocalSever;
    QLocalSocket *mLocalSocket;
    QGenieTrayManager* mTrayManager;
    QVariantList mCommandCache;
    QTimer mTimer;
//    QTimer mRetryTimer;


    int mSession;


protected:
    void startService();
protected slots:
    void slot_SockNewConnection();
//    void slot_SockDisconnected();
    void slot_RestartServer();
    void slot_SockRead();
    void slot_LocalSocketDeleted(QObject *);
    void sendCachedCmds();

};


#endif // QGenieTrayIco_Out_H

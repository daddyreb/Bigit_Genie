#ifndef QGENIEPLUGIN_BASIC_REMOTE_READYSHARE_H
#define QGENIEPLUGIN_BASIC_REMOTE_READYSHARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>

#include "IManagerHandler.h"
#include "ILogManager.h"

#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QGroupBox>
#include <QRadioButton>

#include <QHttpRequestHeader>
#include <QByteArray>
#include <QDebug>
#include <QHttp>
#include <QAuthenticator>

#include "QProgressIndicator.h"

#include <QTimer>

class QGeniePlugin_Basic_Remote_ReadyShare : public QWidget
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Basic_Remote_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblErrorString;
    int iErrorNum;
    QString strTimeout;

    QLabel* lblReadyShare;
    QLabel* lblDesc;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;
    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
    QPushButton* btnBack;
    QPushButton* btnContinue;

    QString strReadyShare;
    QString strDesc;

    QHttp* http;

    QString strIpInUsing;
    QString strRegUser;
    QString strRegPwd;
    QString strHost;
    QString strRouterAdminPassword;

    void getInviteHtm();
    QString getPassword(QString str);
    QString getUser(QString str);

    QProgressIndicator* qPrg;
    int iCurId;

    QTimer* timerHttp;
    int iTimerCount;

    QLabel* txtBasic;
    QLabel* txtRemote;
   // QLabel* txtPrinting;

signals:
    void preIndexSend(int iPreIndex=4);
    void RegisterOk(QString strUser,QString strPassword);
    void RegisterNotYet();
public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
    void readyRead ( const QHttpResponseHeader & resp );
    void authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    void	requestFinished ( int id, bool error );
    void	requestStarted ( int id );
    void httpTimeout();


};

#endif

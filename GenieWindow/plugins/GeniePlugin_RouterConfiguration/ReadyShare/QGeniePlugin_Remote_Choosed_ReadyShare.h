#ifndef QGENIEPLUGIN_REMOTE_CHOOSED_READYSHARE_H
#define QGENIEPLUGIN_REMOTE_CHOOSED_READYSHARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLineEdit>
#include <QScrollArea> //*scrollArea;


#include <QAuthenticator>
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
#include <QSpacerItem>

#include "QProgressIndicator.h"
#include "Singleton.h"

#include <QTimer>


class QGeniePlugin_Remote_Choosed_ReadyShare : public QWidget,public Singleton<QGeniePlugin_Remote_Choosed_ReadyShare>
{
    Q_OBJECT
public:
    explicit QGeniePlugin_Remote_Choosed_ReadyShare(QWidget *parent = 0);

    void changeEvent(QEvent *e);

    void setupUi();
    void retranslateUi();
    static IManagerHandler *sHandler;
    static QStackedLayout* stackLayout;

    QLabel* lblErrorString;
    int iErrorNum;
    QString strTimeout;
    QString strInvalidUser;

    QLabel* lblReadyShare;

    QLabel* lblDesc;
    QLabel* lblHow;
    QLabel* lblStep1;
    QLabel* lblStep2;
    QLabel* lblStep3;
    QLabel* lblStep4;
    QLabel* lblNote;
    QLabel* lblStart;
    QLabel* lblGetRecount;
    QLabel* lblUser;
    QLabel* lblPwd;

    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *verticalLayout_3;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;

    QLineEdit* txtUser;
    QLineEdit* txtPwd;
    QHttp* http;

   // QLabel* lblDevice;
    //QLabel* lblDevicePash;
    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;
    //QVBoxLayout* vlyDesc;
    QGridLayout* gridAll;
     QPushButton* btnBack;

    QPushButton* btnRegister;

    QString strReadyShare;
    QString strDesc;
    QString strIpInUsing;
    QString strRegUser;
    QString strRegPwd;
    QString strHost;
    QString strRouterAdminPassword;


    QProgressIndicator* qPrg;
    int iCurId;

    QTimer* timerHttp;
    int iTimerCount;

   // QSpacerItem *verticalSpacer_5;
   // QSpacerItem *verticalSpacer_6;
    QFrame* frameLine1;
    QFrame* frameLine2;
signals:
    void ShowPreForm();
    void RegisterOk(QString strUser,QString strPassword);


public slots:
    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );
    void readyRead ( const QHttpResponseHeader & resp );
    void authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    void	requestFinished ( int id, bool error );
    void	requestStarted ( int id );
    void textAnyChanged ( const QString & text );
    void httpTimeout();


};

#endif

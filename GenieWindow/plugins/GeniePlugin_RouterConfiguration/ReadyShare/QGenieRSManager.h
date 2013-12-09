#ifndef QGenieRSManager_H
#define QGenieRSManager_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
 
 
 
 
 
#include <QGroupBox>
#include <QRadioButton>
#include <QProcess>
#include "QProgressIndicator.h"
#include "Singleton.h"
#include <QByteArray>
#include <QDebug>
#include "GenieHttp.h"
#include <QAuthenticator>
#include <QTimer>
#include <QLineEdit>
#include <QStackedLayout>
class PCPForm;

class QGenieRSManager : public QObject, public Singleton<QGenieRSManager>
{
    Q_OBJECT
public:
    explicit QGenieRSManager();
    ~QGenieRSManager();

//    void changeEvent(QEvent *e);

    QWidget * getUi();
    void retranslateUi();
    void init(quint32 flag);


    QLabel* lblErrorString;
    int mErrorNum;

    QGroupBox* grpSwitchType;
    QRadioButton* rdoBasic;
    QRadioButton* rdoRemote;
    QRadioButton* rdoPrinting;
    //////////////////////////////////
    QRadioButton* rdoG9;

    QGridLayout* gridAll;
//    QPushButton* btnBack;
    QPushButton* btnContinue;

    GenieHttp* http;

//    QString strRegUser;
//    QString strRegPwd;
//    QString strRouterAdminPassword;

    void getInviteHtm(bool haslocation);

    QString getPassword(const QString &str);
    QString getUser(const QString &str);

    QProgressIndicator* mPi;
    int mCurId;

    QTimer* mTimer;
//    int iTimerCount;

    PCPForm* mPcpForm;
protected:
    QWidget *mW;
    void showError(int lan);

public:
//    void hideAndDisRadio();
    QString wrapStr(const QString &s);
    void updateTitleButton(quint32 flag);

signals:
//    void preIndexSend(int iPreIndex=4);

//    void RegisterOk(QString strUser,QString strPassword);
//    void RegisterNotYet();
public slots:
//    void BtnBackClicked( bool checked = false );
    void BtnContinueClicked( bool checked = false );

    void readyRead ( QNetworkReply* reply );
    void authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    void requestFinished ( int id, bool error );
//    void requestStarted ( int id );
    void slot_HttpTimeout();

};

#endif // QGenieRSManager_H

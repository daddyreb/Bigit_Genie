#ifndef QGenieRSRemote_H
#define QGenieRSRemote_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "GenieHttp.h"

#include "QProgressIndicator.h"
#include "Singleton.h"




class QGenieRSRemote : public QObject,public Singleton<QGenieRSRemote>
{
    Q_OBJECT
public:
    explicit QGenieRSRemote();
    ~QGenieRSRemote();
//    void changeEvent(QEvent *e);

    QWidget * getUi();
    void retranslateUi();

    int mErrorNum;
    QLineEdit* mTxtUser;
    QLineEdit* mTxtPwd;
    GenieHttp* http;
//    QPushButton* btnBack;
    QLabel *mEdtError;

//    QPushButton* btnRegister;
    QStackedWidget *mStack;

    QProgressIndicator* mPi;
    int mRegisterId;
    int mUnRegisterId;

    QTimer* timerHttp;
    void setRegisterState(bool registered,const QString &username,const QString &password);
//    int iTimerCount;

   // QSpacerItem *verticalSpacer_5;
   // QSpacerItem *verticalSpacer_6;
//    QFrame* frameLine1;
//    QFrame* frameLine2;
    void showError(int lan);
protected:
    QWidget *mW;
    QString mUsername;
    QString mPassword;

    void registerRS();
    void unregisterRS();


public slots:
//    void BtnBackClicked( bool checked = false );
//    void BtnContinueClicked( bool checked = false );
    void slot_butClicked();
    void slot_linkActivated(const QString &url);

    void readyRead ( QNetworkReply * reply);
    void authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    void	requestFinished ( int id, bool error );
//    void	requestStarted ( int id );
    void textAnyChanged ( const QString & text );
    void httpTimeout();

    ///
//    void CountClickR16( const QString &count );
//    void CountClickR17( const QString &count );


};

#endif

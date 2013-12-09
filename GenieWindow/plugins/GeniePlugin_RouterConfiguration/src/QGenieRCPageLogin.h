#ifndef LOGIN_H
#define LOGIN_H

#include <QtGui>
#include <QtEvents>
#include "QProgressIndicator.h"
#include "netgearsoapapi.h"
#include "netgearsoapschedule.h"
#include "inc/genie_interface/ISettings.h"
#include "ILanguageManager.h"
//#include "QThreadIsNetgear.h"
#include "Singleton.h"
#include "ui_QGenieRCPageLogin.h"
#include "QGenieRCLogic.h"
#include "soapclient.h"


class QGenieRCSoapProxy;

class QGenieRCPageLogin : public QWidget,public ISoapClient,public Singleton<QGenieRCPageLogin>
{
    Q_OBJECT
public:
    QGenieRCPageLogin(QWidget* parent=NULL);
    ~QGenieRCPageLogin();
    inline bool isRefreshing()
    {
        return mLoginTimer->isActive();
    }

private:
    void retranslateUi();
    int mErrorTextIdx;
    
public:
    Ui::QGenieRCPageLogin mUi;
//    QGridLayout *gridLayout;
//    QSpacerItem *horizontalSpacer;
//    QSpacerItem *horizontalSpacer_2;
//    QSpacerItem *verticalSpacer_2;
//    QGroupBox *groupBox;
//    QGridLayout *gridLayout_2;
//    QLabel *label_usr;
//    QLineEdit *edt_username;
//    QLabel *label_pwd;
//    QLineEdit *edt_pwd;
//    QWidget *widget;
//    QLabel *mLabelErrShow;
//    QCheckBox *ckb_savepwd;
//    QSpacerItem *verticalSpacer;
//    QSpacerItem *verticalSpacer_3;
//    QFrame *frame;
//    QGridLayout *gridLayout_3;
//    QPushButton *mButLogin;
//    QPushButton *mButCancel;

//    QThreadIsNetgear *mThreadIsNetgear;
    int mLoginTimes;
#ifdef HTTP_LOGIN
    QHttp mLoginHttp;
    int mCurHttpRequestId;
#endif



public:
    

    ISettings* g_settings;

    QTimer* mLoginTimer;

    QUuid uuid;


public:
    ILanguageManager *lanmanager;
    void startUiWaitState();
    void stopUiWaitState(bool bOk=true);
    void GoToLogin();
    void load_key();
    void save_key(const QString& user,const QString& pwd);
    void ReloadPassword();
    bool isPwdEmpty();
    void showError(int idx);
    void hideError();
    virtual void parseSoapResponse(int,QtSoapMessage&);
    virtual void whenSoapError(int cmd,const QString& reson );
    void tryStartLogin(PENDING_CMD pcmd=SLP_NONE,quint32 clearPending=SLP_NONE,int trytime=0);
//    void loginOk(bool ok,int reson);

private slots:

    void slot_Cancel();
    void slot_LoginTimeout();
    void on_but_login_clicked();
    void on_chk_save_clicked(bool ck);



    void PasswordLineEdit(QString);
#ifdef HTTP_LOGIN
    void slot_HttpDone(bool err);
    void slot_HttpRequestFinished(int id,bool error);
#endif


signals:

    void SendGetRouterInfo(int);

protected:
    void keyPressEvent(QKeyEvent *event);
    virtual void changeEvent(QEvent* event);
    friend class QGenieRCSoapProxy;
    friend class QGenieRCLogic;
};

#endif // LOGIN_H

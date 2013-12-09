#ifndef __pages_h__
#define __pages_h__

#include "AsyncOp.h"
#include "LPCNavPage.h"
#include "LPCNavPopup.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QList>
#include <QCheckBox>
#include "oputil.h"

class RouterLoginPopup
	: public LPCNavPopup
{
	Q_OBJECT

public:
	RouterLoginPopup(const QString& username, const QString& password, const QObject *receiver, const char *method);
	virtual ~RouterLoginPopup();

	const QString& username() const { return m_username; }
	const QString& password() const { return m_password; }

	virtual void retranslateUI();
	virtual void onPopup();

signals:
	void internalClose(RouterLoginPopup *popup, bool accepted);

public slots:
	void accept();
	void reject();

private:
	QString m_username;
	QString m_password;
	QLineEdit *m_usernameEdit;
	QLineEdit *m_passwordEdit;
	QLabel *m_promptLabel;
	QLabel *m_usernameLabel;
	QLabel *m_passwordLabel;
};

class MessagePopup
	: public LPCNavPopup
{
	Q_OBJECT

public:
	MessagePopup(const char *caption, const char *content, const QObject *receiver, const char *method);
	virtual ~MessagePopup();

	virtual void retranslateUI();

signals:
	void internalClose(MessagePopup *popup, bool accepted);

public slots:
	void accept();

private:
	QByteArray m_caption;
	QByteArray m_content;
	QLabel *m_contentLabel;
};

class FirmwarePage
	: public LPCNavPage
{
	Q_OBJECT

public:
	FirmwarePage(QWidget *parent);
	virtual ~FirmwarePage();

signals:
	void internalCheck();

public slots:
	void check();
	void aopCheck(AsyncOp *op, bool waitOk);
	void authCheck(RouterLoginPopup *popup, bool accepted);

protected:
	virtual bool prepareImpl();
	virtual bool loadImpl();

private:
	QLabel *m_error1;
	QPushButton *m_tryagain;
};

class IntroPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	IntroPage(QWidget *parent);
	virtual ~IntroPage();

	virtual void retranslateUI();

public slots:
	void nextPage();

protected:
	virtual bool prepareImpl();

private:
	QLabel *m_introPic;
};

class DisclaimerPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	DisclaimerPage(QWidget *parent);
	virtual ~DisclaimerPage();

public slots:
	void prevPage();
	void nextPage();
};

class PreSignInPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	PreSignInPage(QWidget *parent);
	virtual ~PreSignInPage();

public slots:
	void prevPage();
	void nextPage();

protected:
	virtual bool prepareImpl();

private:
	QRadioButton *m_existing;
};

class SignInPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	SignInPage(QWidget *parent);
	virtual ~SignInPage();

	virtual void retranslateUI();

signals:
	void internalRefresh();

public slots:
	void prevPage();
	void nextPage();

protected:
	virtual bool prepareImpl();

private slots:
	void updateUI();
	void aopCheck(AsyncOp *op, bool waitOk);
	void authCheck(RouterLoginPopup *popup, bool accepted);

private:
	QLineEdit *m_usernameEdit;
	QLineEdit *m_passwordEdit;
	QLabel *m_errorLabel;
	QLabel *m_forgotLabel;
	QPushButton *m_nextButton;
};

class CreateAccountPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	CreateAccountPage(QWidget *parent);
	virtual ~CreateAccountPage();

public slots:
	void prevPage();
	void nextPage();
	void checkAvailable();

protected:
	virtual bool prepareImpl();

private slots:
	void updateUI();
	void aopCheck(AsyncOp *op, bool waitOk);
	void accountCheck(AsyncOp *op, bool waitOk);

private:
	bool validateInput();

private:
	QLabel *m_errorPrompt;
	QLabel *m_usernamePrompt;
	QLabel *m_passwordConfirmPrompt;
	QLabel *m_emailPrompt;
	QLabel *m_emailConfirmPrompt;
	QLineEdit *m_usernameEdit;
	QLineEdit *m_passwordEdit;
	QLineEdit *m_passwordConfirmEdit;
	QLineEdit *m_emailEdit;
	QLineEdit *m_emailConfirmEdit;
	QPushButton *m_checkBtn;
	QPushButton *m_nextBtn;
	QRegExp m_usernameRE;
};

class DonePage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	DonePage(QWidget *parent);
	virtual ~DonePage();
};

class ManagePage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	ManagePage(QWidget *parent);
	virtual ~ManagePage();

public slots:
	void toggleLPC();
	void changeSettings();
	void changeCustomSettings();
        void enterBypass(); //20121128 yankai add
        void GetLookupData(const QHostInfo &Host);

protected:
	virtual bool prepareImpl();
	virtual bool loadImpl();

private slots:
	void aopCheck(AsyncOp *op, bool waitOk);
	void aopToggle(AsyncOp *op, bool waitOk);
	void aopCustom(AsyncOp *op, bool waitOk);
	void devicePrompt(MessagePopup *popup, bool accepted);
        void aopSerialNum(AsyncOp *op, bool waitOk);//kai.yan add
        void aopGetDeviceID(AsyncOp *op, bool waitOk); //kai.yan add


private:
	void enableUI(bool enable);
	void updateLPCStatus(bool lpcEnabled);

private:
	QLabel *m_errorLabel;
	QLabel *m_bundle;
	QLabel *m_bundleDetail;
	QLabel *m_account;
	QLabel *m_lpcStatus;
	QPushButton *m_toggleButton;
	QPushButton *m_changeButton;
	QPushButton *m_settingsButton;
	bool m_lpcEnabled;
	int m_lastStatus;
        QLabel *m_bypassAccount; //yankai add
        QPushButton *m_bypassButton; //kai.yan add
        Outfile m_outfile;
};

class SettingsPage
	: public LPCNavPage
{
	Q_OBJECT
	
public:
	SettingsPage(QWidget *parent);
	virtual ~SettingsPage();

public slots:
	void prevPage();
	void nextPage();

protected:
	virtual bool prepareImpl();
	virtual bool loadImpl();

private slots:
	void aopCheck(AsyncOp *op, bool waitOk);
	void aopChange(AsyncOp *op, bool waitOk);
	void radioChanged();

private:
	void loadValues();

private:
	QRadioButton *m_levels[6];
	QLabel *m_errorPrompt;
	QPushButton *m_nextBtn;
	int m_currentLevel;
	bool m_loadOk;
	QList<QCheckBox *> m_customList;//kai.yan add
	QFrame * m_frame_custom;
};

//20121129 yankai add
class BypassUserList
    :public LPCNavPage
{
    Q_OBJECT
public:
        BypassUserList(QWidget *parent);
        virtual ~BypassUserList();

protected:
    virtual bool prepareImpl();
    virtual bool loadImpl();
private slots:
    void gotoManagePage();
    void aopCheck(AsyncOp *op, bool waitOk);
    void logout_callback(AsyncOp *op, bool waitOk);
    void aopGetBypassUers();
    void bypassLogin(QListWidgetItem* item);
    void bypassUserReflesh();
    void bypassLogout();
    void aopGetDeviceID(AsyncOp *op, bool waitOk); //kai.yan add
    void aopSerialNum(AsyncOp *op, bool waitOk);
    void bypassLoginin();
    void bypassCancel();
    void aopCheck_Login(AsyncOp *op, bool waitOk);
    void updateUI();

private:
    QListWidget* bypassUserList;
    QStackedWidget *stackedWidget;
    QLabel *bypassMsg;
    QLabel *m_error;
    Outfile m_outfile;
    QLineEdit *bypassUserName;
    QLineEdit *bypassPassword;
    QLabel *m_errorLabel;
    QPushButton *m_bypassLogin;
	QLabel *lab_user_login;
	QPushButton *m_btnReflesh;

};

//class BypassUserLogin
//    :public LPCNavPage
//{
//    Q_OBJECT
//public:
//    BypassUserLogin(QWidget *parent);
//    virtual ~BypassUserLogin();

//protected:
//    virtual bool prepareImpl();
//    virtual bool loadImpl();
//private slots:
//    void bypassLoginin();
//    void bypassCancel();
//    void aopCheck(AsyncOp *op, bool waitOk);
//    void updateUI();
//private:
//    QLineEdit *bypassUserName;
//    QLineEdit *bypassPassword;
//    QLabel *m_errorLabel;
//    QPushButton * m_bypassLogin;
//    Outfile *outfile;

//};
//end
#endif // __pages_h__

#ifndef __ctrl_h__
#define __ctrl_h__

#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExp>

class CustomUILoader;

class RouterLoginDialog
	: public QDialog
{
	Q_OBJECT

public:
	RouterLoginDialog(const QString& username, const QString& password, QWidget *parent);
	virtual ~RouterLoginDialog();

	const QString& username() const { return m_username; }
	const QString& password() const { return m_password; }

public slots:
	virtual void accept();

private:
	QString m_username;
	QString m_password;
	QLineEdit *m_usernameEdit;
	QLineEdit *m_passwordEdit;
};

class Page
	: public QFrame
{
	Q_OBJECT

public:
	Page(QWidget *parent);
	virtual ~Page();
	virtual void setVisible(bool visible);

	bool prepare();
	bool load() { return loadImpl(); }
	void unload() { unloadImpl(); }

	static void registerPages(CustomUILoader *loader);

protected slots:
	virtual void gotoMainPage();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

	virtual bool prepareImpl() { return true; }
	virtual bool loadImpl() { return true; }
	virtual void unloadImpl() {}

	void navigateTo(const char *pageName);
};

class FirmwarePage
	: public Page
{
	Q_OBJECT

public:
	FirmwarePage(QWidget *parent);
	virtual ~FirmwarePage();

signals:
	void internalCheck();

public slots:
	void quit();
	void check();

protected:
	virtual bool prepareImpl();
	virtual bool loadImpl();

private:
	QLabel *m_error1;
	QPushButton *m_tryagain;
};

class IntroPage
	: public Page
{
	Q_OBJECT

public:
	IntroPage(QWidget *parent);
	virtual ~IntroPage();

public slots:
	void nextPage();

protected:
	virtual bool prepareImpl();
};

class DisclaimerPage
	: public Page
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
	: public Page
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
	: public Page
{
	Q_OBJECT

public:
	SignInPage(QWidget *parent);
	virtual ~SignInPage();

signals:
	void internalRefresh();

public slots:
	void prevPage();
	void nextPage();

protected:
	virtual bool prepareImpl();

private slots:
	void updateUI();

private:
	QLineEdit *m_usernameEdit;
	QLineEdit *m_passwordEdit;
	QLabel *m_errorLabel;
	QLabel *m_forgotLabel;
	QPushButton *m_nextButton;
};

class CreateAccountPage
	: public Page
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
	: public Page
{
	Q_OBJECT

public:
	DonePage(QWidget *parent);
	virtual ~DonePage();
};

class ManagePage
	: public Page
{
	Q_OBJECT

public:
	ManagePage(QWidget *parent);
	virtual ~ManagePage();

public slots:
	void toggleLPC();
	void changeSettings();
	void changeCustomSettings();

protected:
	virtual bool prepareImpl();
	virtual bool loadImpl();

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
};

class SettingsPage
	: public Page
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

private:
	void loadValues();

private:
	QRadioButton *m_levels[5];
	QLabel *m_errorPrompt;
	QPushButton *m_nextBtn;
	int m_currentLevel;
	bool m_loadOk;
};

#endif // __ctrl_h__

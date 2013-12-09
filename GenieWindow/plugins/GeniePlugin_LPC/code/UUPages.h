#ifndef __UUPages_h__
#define __UUPages_h__

#include "UUApp.h"
#include "AsyncOp.h"
#include "common.h"
#include "coreapi.h"
#include "webapi.h"
#include <QtCore/QPointer>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

namespace LPCUserUtility {

class StartupPage
	: public NavPage
{
	Q_OBJECT

public:
	StartupPage(QWidget *parent);
	virtual ~StartupPage();

protected:
	virtual void onLoad();

private Q_SLOTS:
	void onDetectRouterFinished();
	void onRouterAuthFinished();
	void onGetLPCEnableStatusFinished();
	void onGetDefaultIdFinished();
	void onGetChildIdFinished();
	void onGetUsersFinished();
	void onGetUserForChildFinished();

private:
	void handleRouterError(int error);
	void displayError(const char *msg, int webError, bool stopProgress);

private:
	QPointer<QLabel> m_movieLabel;
	QPointer<QLabel> m_statusLabel;
	QPointer<QLabel> m_errorLabel;
	QPointer<QMovie> m_movie;
	QPointer<AsyncOp> m_op;
};

class InfoNoticePage
	: public NavPage
{
	Q_OBJECT

public:
	InfoNoticePage(QWidget *parent);
	virtual ~InfoNoticePage();

protected:
	virtual void onLoad();

private Q_SLOTS:
	void onWebLoginFinished();
	void onAuthRouterFinished();
	void onSetMasqIDFinished();
	void onAuthRouterFinished2();
	void onDeleteMacFinished();

private:
	void loginAction();
	void logoutAction();

private:
	QPointer<QLabel> m_movieLabel;
	QPointer<QLabel> m_statusLabel;
	QPointer<QMovie> m_movie;
	QPointer<AsyncOp> m_op;
};

class MainPage
	: public NavPage
{
	Q_OBJECT

public:
	MainPage(QWidget *parent);
	virtual ~MainPage();

protected:
	virtual void onLoad();

private Q_SLOTS:
	void doLogin();
	void onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
	QPointer<QListWidget> m_accountList;
	QPointer<QLineEdit> m_passwordEdit;
	QPointer<QPushButton> m_loginButton;
};

class RouterPasswordPage
	: public NavPage
{
	Q_OBJECT

public:
	RouterPasswordPage(QWidget *parent);
	virtual ~RouterPasswordPage();

protected:
	virtual void onLoad();
	virtual void keyPressEvent(QKeyEvent *e);

private Q_SLOTS:
	void nextPage();
	void onTextChanged(const QString& text);

private:
	QPointer<QLineEdit> m_usernameEdit;
	QPointer<QLineEdit> m_passwordEdit;
	QPointer<QPushButton> m_okButton;
};

class StatusPage
	: public NavPage
{
	Q_OBJECT

public:
	StatusPage(QWidget *parent);
	virtual ~StatusPage();

protected:
	virtual void onLoad();
	virtual void retranslateUI();

private Q_SLOTS:
	void onPageAction();

private:
	QPointer<QLabel> m_statusLabel;
	QPointer<QPushButton> m_actionButton;
};

class StartupLongErrorPage
	: public NavPage
{
	Q_OBJECT

public:
	StartupLongErrorPage(QWidget *parent);
	virtual ~StartupLongErrorPage();

private:
};

} // namespace LPCUserUtility

#endif // __UUPages_h__

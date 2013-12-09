#ifndef __UUApp_h__
#define __UUApp_h__

#include "ext.h"
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtGui/QApplication>
#include <QtGui/QFrame>
#include <QtGui/QMenuBar>
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QSystemTrayIcon>
#include <QtNetwork/QNetworkAccessManager>

namespace LPCUserUtility {

class MainWindow;
class NavPage;

class AppData
{
public:
	AppData();
	virtual ~AppData();

	const QString& parentDeviceId() const;
	void setParentDeviceId(const QString& parentDeviceId);

	const QString& childDeviceId() const;
	void setChildDeviceId(const QString& childDeviceId);

	const QString& userName() const;
	void setUserName(const QString& userName);

	static const QString DEFAULT_ROUTER_USERNAME;
	static const QString DEFAULT_ROUTER_PASSWORD;

	QString m_soapAction;
	QString m_routerUsername;
	QString m_routerPassword;
	QByteArray m_mac;
	QStringList m_userList;

	enum Mode {
		Mode_LogIn,
		Mode_LogOut,
	};

	Mode m_opMode;

	bool m_loggedIn;
	QString m_webPassword;

private:
	QString m_parentDeviceId;
	QString m_childDeviceId;
	QString m_userName;
};

class App
	: public QApplication
{
	Q_OBJECT

public:
	App(int& argc, char **argv);
	virtual ~App();

	static App *instance();
	static AppData *appData();
	static int main(int argc, char *argv[]);
	int run();

	void navigateTo(const char *pageName);

	QNetworkAccessManager *networkAccessManager() const;
	QString ttt(const char *sourceText, const char *disambiguation = 0, int n = -1);

public Q_SLOTS:
	void realDeactivateUI();
	void activateUI(const QByteArray& hint);

public:
	void deactivateUI();
	void showMessage(const QString& text);
	void setLoginState(bool login);

private Q_SLOTS:
	void cleanup();
	void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void onAbout();
	void onStatus();
	void onLogOut();
	void onLogin();

private:
	AppData *m_appData;
	QPointer<MainWindow> m_mainWnd;
#ifdef UU_USE_TRAY_MENU
	QPointer<QSystemTrayIcon> m_trayIcon;
	QPointer<QMenu> m_trayMenu;
#endif // UU_USE_TRAY_MENU

	QPointer<QAction> m_LogOutAction;
	QPointer<QAction> m_LogInAction;

#ifdef UU_USE_APP_MENU
	QMenuBar m_appMenuBar;
	QMenu m_appMenu;
#endif // UU_USE_APP_MENU

	QNetworkAccessManager *m_nam;
};

class MainWindow
	: public QWidget
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

	void navigateTo(const char *pageName);

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void changeEvent(QEvent *e);

private Q_SLOTS:
	void onNavigateAction();

private:
	QWidget *loadUI(const QString& name, QWidget *parentWidget);

private:
	QPointer<QStackedWidget> m_navHost;

	typedef NavPage *(*NewPageProc)();
	typedef QMap<QByteArray, NewPageProc> PageMap;
	PageMap m_pageMap;
	QPointer<NavPage> m_currentPage;
	CustomUILoader m_uiLoader;
};

class NavPage
	: public QFrame
{
	Q_OBJECT

public:
	NavPage(QWidget *parent);
	virtual ~NavPage();

	void load();
	void unload();

public Q_SLOTS:
	void deactivateUI();
	void exitApp();

protected:
	virtual void onLoad();
	virtual void onUnload();
	virtual void retranslateUI();

	void navigateTo(const char *pageName);

	AppData *appData() const;
	QNetworkAccessManager *nam() const;

	void updateTextUI(QWidget *widget);
	void updateTextUIWork(QWidget *widget);
	void updateWidgetText(QWidget *widget, const QString& text, const QString& fixedMarkup = "");
	QString ttt(const char *sourceText, const char *disambiguation = 0, int n = -1);
	void setLPCText(QWidget *widget, const char *text, bool markup = false);
};

} // namespace LPCUserUtility

#endif // __UUApp_h__

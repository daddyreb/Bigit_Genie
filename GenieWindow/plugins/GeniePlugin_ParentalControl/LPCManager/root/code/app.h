#ifndef __app_h__
#define __app_h__

#include "AsyncOp.h"
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QMenuBar>
#include <QNetworkAccessManager>
#include <QPointer>
#include <QStackedLayout>
#include <QUiLoader>
#include <QWidget>
#include <QtCore/QTimer>

//#define ENABLE_DEBUG_UI

class CustomTranslator;
class CustomUILoader;
class MainWindow;
class Page;

class AppData
{
public:
	QString m_username;
	QString m_password;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_soapAction;
	QString m_deviceId;
	QString m_userToken;
	bool m_inChangeBasicSettings;

	AppData();
	~AppData();
	bool needSetup() const;
	bool needShowAbortButton() const;
	void load();
	void save();
	void reset();
};

class LPCManager
	: public QApplication
{
	Q_OBJECT

public:
	static int main(int argc, char *argv[]);

	LPCManager(int& argc, char **argv);
	virtual ~LPCManager();

	bool init();
	int run();
	QWidget *loadUI(const QString& name, QWidget *parentWidget = NULL);
	void changeLanguage(const QString& langName);
	QNetworkAccessManager *networkAccessManager() const;

	bool wait(AsyncOp *op, const QString& promptText = QString::fromUtf8(""));
	void requestNavigateTo(const QString& pageName);

	AppData *appData() const;

	bool requestUpdate();

private slots:
	void updateRequestFinished();
	void promptUpdate();

private:
	CustomTranslator *m_defaultTranslator;
	CustomTranslator *m_translator;
	CustomUILoader *m_uiLoader;
	QPointer<MainWindow> m_mainWnd;
	QNetworkAccessManager *m_nam;
	AppData *m_appData;
	QNetworkReply *m_updateReply;
	QTimer m_updateTimer;
	QString m_updateDownloadUrl;
	int m_loopLevel;
};

LPCManager *app();
AppData *appData();

class MainWindow
	: public QFrame
{
	Q_OBJECT

public:
	MainWindow();
	virtual ~MainWindow();

	void requestNavigateTo(const QString& pageName);

signals:
	void internalRequestNavigateTo(const QString& pageName);

protected:
	bool event(QEvent *e);

public slots:
	void navigateTo(const QString& pageName);
	void navigateToAct();
	void changeLanguageAct();

#ifdef ENABLE_DEBUG_UI
	void toggleDebugMenuBar();
	void doSslTest();
	void doClearSavedCredentials();
#endif // #ifdef ENABLE_DEBUG_UI

private:
	void onLanguageChanged();

private:
	QStackedLayout *m_navPane;
	Page *m_activePage;
	QString m_activePageName;

#ifdef ENABLE_DEBUG_UI
	QAction *m_langInd;

#ifdef Q_OS_DARWIN
	QMenu *m_debugMenuBar;
#else
	QMenuBar *m_debugMenuBar;
#endif // Q_OS_DARWIN

#endif // ENABLE_DEBUG_UI

};

class WaitPopup
	: public QDialog
{
	Q_OBJECT

public:
	WaitPopup(const QString& promptText, AsyncOp *op, QWidget *parent = NULL);
	virtual ~WaitPopup();

public slots:
	virtual void reject();

private:
	AsyncOp *m_op;
};

#endif // __app_h__

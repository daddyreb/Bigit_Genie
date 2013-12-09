#include "UUApp.h"
#include "UUPages.h"
#include <QtCore/QFile>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QNetworkReply>

#define APP_VERSION_STRING "1.0.0"

int main(int argc, char *argv[])
{
	return LPCUserUtility::App::main(argc, argv);
}

void getRidOfSslErrors(QNetworkReply *reply)
{
	// TODO: safe ?
	reply->ignoreSslErrors();
}

QString markupLink(const QString& text)
{
	QString result;
	//result.replace(QRegExp(QString::fromUtf8("(http://[^ ]+)([ .,!?])")), QString::fromUtf8("<a href=\"\\1\">\\1</a>\\2"));
	static const QString sep = QString::fromLatin1(",!?");
	int offset = 0;
	for (;;) {
		int pos1 = text.indexOf(QLatin1String("http://"), offset);
		if (pos1 < 0) {
			result.append(text.mid(offset));
			//qDebug() << result;
			break;
		} else {
			result.append(text.mid(offset, pos1 - offset));
			//qDebug() << result;
			offset = pos1 + 7;

			bool domain = false;

			while (offset < text.length()) {
				QChar c = text[offset];
				if (c.isSpace() || c.unicode() >= 256 || sep.indexOf(c) >= 0) {
					break;
				}

				if (!domain && c == '/') {
					domain = true;
				}

				if (domain && c == '.') {
					break;
				}

				offset++;
			}

			QString href = text.mid(pos1, offset - pos1);

			result.append(QLatin1String("<a href=\""));
			result.append(href);
			result.append(QLatin1String("\">"));
			result.append(href);
			result.append(QLatin1String("</a>"));

			//qDebug() << text.mid(pos1, offset - pos1);
		}
	}
	return result;
}

namespace LPCUserUtility {

const QString AppData::DEFAULT_ROUTER_USERNAME = QString::fromUtf8("admin");
const QString AppData::DEFAULT_ROUTER_PASSWORD = QString::fromUtf8("password");

AppData::AppData()
	: m_loggedIn(false)
{
}

AppData::~AppData()
{
}

const QString& AppData::parentDeviceId() const
{
	return m_parentDeviceId;
}

void AppData::setParentDeviceId(const QString& parentDeviceId)
{
	m_parentDeviceId = parentDeviceId;
}

const QString& AppData::childDeviceId() const
{
	return m_childDeviceId;
}

void AppData::setChildDeviceId(const QString& childDeviceId)
{
	m_childDeviceId = childDeviceId;
}

const QString& AppData::userName() const
{
	return m_userName;
}

void AppData::setUserName(const QString& userName)
{
	m_userName = userName;
}

App::App(int& argc, char **argv)
	: QApplication(argc, argv)
{
	m_nam = new QNetworkAccessManager();
	m_appData = new AppData();

	QIcon appIcon(QLatin1String(":/LPCModule/images/LPCUserUtility.ico"));

#ifdef UU_USE_TRAY_MENU
	m_trayIcon = new QSystemTrayIcon(this);
	m_trayIcon->setIcon(appIcon);
	connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));

#ifdef UU_TRAY_ICON_AUTO
#else
	m_trayIcon->show();
#endif // UU_TRAY_ICON_AUTO

	m_trayMenu = new QMenu();
	m_trayIcon->setContextMenu(m_trayMenu);

	m_trayMenu->addAction(ttt("About"), this, SLOT(onAbout()));
	m_trayMenu->addAction(ttt("Status"), this, SLOT(onStatus()));
	m_LogOutAction = m_trayMenu->addAction(ttt("Log out"), this, SLOT(onLogOut()));
	m_LogInAction = m_trayMenu->addAction(ttt("Login"), this, SLOT(onLogin()));
	m_trayMenu->addAction(ttt("Exit"), this, SLOT(quit()));

#endif // UU_USE_TRAY_MENU

#ifdef UU_USE_APP_MENU

	m_appMenuBar.addMenu(&m_appMenu);
	QAction *appAct;
	appAct = m_appMenu.addAction(ttt("About"), this, SLOT(onAbout()));
	appAct->setMenuRole(QAction::AboutRole);

	appAct = m_appMenu.addAction(ttt("Status"), this, SLOT(onStatus()));
	appAct->setMenuRole(QAction::ApplicationSpecificRole);

	appAct = m_appMenu.addAction(ttt("Log out"), this, SLOT(onLogOut()));
	appAct->setMenuRole(QAction::ApplicationSpecificRole);
	m_LogOutAction = appAct;

	appAct = m_appMenu.addAction(ttt("Login"), this, SLOT(onLogin()));
	appAct->setMenuRole(QAction::ApplicationSpecificRole);
	m_LogInAction = appAct;

	appAct = m_appMenu.addAction(ttt("Exit"), this, SLOT(quit()));
	appAct->setMenuRole(QAction::QuitRole);

#endif // UU_USE_APP_MENU

	setOrganizationName(QString::fromUtf8("NETGEAR"));
	setOrganizationDomain(QString::fromUtf8("netgear.com"));
	setApplicationName(QString::fromUtf8("LPC User Utility"));
	setApplicationVersion(QString::fromUtf8(APP_VERSION_STRING));
	setWindowIcon(appIcon);

	setLoginState(false);
}

App::~App()
{
#ifdef UU_USE_TRAY_MENU
	delete m_trayIcon;
	delete m_trayMenu;
#endif // UU_USE_TRAY_MENU
	delete m_nam;
}

App *App::instance()
{
	return static_cast<App*>(qApp);
}

AppData *App::appData()
{
	return instance()->m_appData;
}

QNetworkAccessManager *App::networkAccessManager() const
{
	return m_nam;
}

QString App::ttt(const char *sourceText, const char *disambiguation, int n)
{
	return QString::fromAscii(sourceText);
}

int App::main(int argc, char *argv[])
{
	App app(argc, argv);
	return app.run();
}

int App::run()
{
	connect(this, SIGNAL(aboutToQuit()), SLOT(cleanup()));

	m_mainWnd = new MainWindow();
	m_mainWnd->show();
	return exec();
}

void App::cleanup()
{
	if (m_mainWnd) {
		delete m_mainWnd;
	}

	if (m_appData) {
		delete m_appData;
		m_appData = NULL;
	}
}

void App::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
#ifdef UU_TRAY_ICON_AUTO
	if (reason == QSystemTrayIcon::Trigger) {
		if (m_mainWnd && !m_mainWnd->isVisible()) {
			QMetaObject::invokeMethod(this, "activateUI", Qt::QueuedConnection, Q_ARG(QByteArray, appData()->m_loggedIn ? "Status" : "Startup"));
		}
	}
#endif // UU_TRAY_ICON_AUTO
}

void App::onAbout()
{
	QDesktopServices::openUrl(QString::fromUtf8("http://netgear.opendns.com/support/switch/windows"));
}

void App::onStatus()
{
	activateUI("Status");
}

void App::onLogOut()
{
	appData()->m_opMode = AppData::Mode_LogOut;
	activateUI("InfoNotice");
}

void App::onLogin()
{
	activateUI(appData()->m_loggedIn ? "Status" : "Startup");
}

void App::activateUI(const QByteArray& hint)
{
	if (m_mainWnd) {
#ifdef UU_USE_TRAY_MENU
#ifdef UU_TRAY_ICON_AUTO
		m_trayIcon->hide();
#endif // UU_TRAY_ICON_AUTO
#endif // UU_USE_TRAY_MENU
		m_mainWnd->showNormal();
		m_mainWnd->show();
		m_mainWnd->setFocus();
		m_mainWnd->activateWindow();
		navigateTo(hint);
	}
}

void App::deactivateUI()
{
	if (m_mainWnd) {
		m_mainWnd->showMinimized();
	}
}

void App::realDeactivateUI()
{
#ifdef UU_USE_TRAY_MENU
#ifdef UU_TRAY_ICON_AUTO
	m_mainWnd->hide();
	m_trayIcon->show();
#endif // UU_TRAY_ICON_AUTO
#endif // UU_USE_TRAY_MENU
}

void App::showMessage(const QString& text)
{
	QMessageBox::information(m_mainWnd, applicationName(), text);
}

void App::setLoginState(bool login)
{
	appData()->m_loggedIn = login;
	if (login) {
		if (m_LogInAction) {
			m_LogInAction->setVisible(false);
		}
		if (m_LogOutAction) {
			m_LogOutAction->setVisible(true);
		}
	} else {
		if (m_LogInAction) {
			m_LogInAction->setVisible(true);
		}
		if (m_LogOutAction) {
			m_LogOutAction->setVisible(false);
		}
	}
}

void App::navigateTo(const char *pageName)
{
	if (m_mainWnd) {
		m_mainWnd->navigateTo(pageName);
	}
}

template <class T>
QWidget *createPage(const QString& className, QWidget *parent, const QString& name)
{
	Q_UNUSED(className)
	T *w = new T(parent);
	w->setObjectName(name);
	return w;
}

#define INTERNAL_REGISTER_PAGE(page) m_uiLoader.registerFactory(QString::fromUtf8(#page), createPage<page>)

MainWindow::MainWindow()
{
	setWindowTitle(QString::fromUtf8("%1 %2 %3").arg(QApplication::organizationName()).arg(QApplication::applicationName()).arg(QApplication::applicationVersion()));

	QVBoxLayout *rootLayout = new QVBoxLayout();
	setLayout(rootLayout);

	m_navHost = new QStackedWidget();
	rootLayout->addWidget(m_navHost);

	INTERNAL_REGISTER_PAGE(StartupPage);
	INTERNAL_REGISTER_PAGE(InfoNoticePage);
	INTERNAL_REGISTER_PAGE(MainPage);
	INTERNAL_REGISTER_PAGE(RouterPasswordPage);
	INTERNAL_REGISTER_PAGE(StatusPage);
	INTERNAL_REGISTER_PAGE(StartupLongErrorPage);

	const char *pageNames[] = {
		"Startup",
		"InfoNotice",
		"Main",
		"RouterPassword",
		"Status",
		"StartupLongError",
	};
/*
	QMenuBar *menuBar = new QMenuBar();
	rootLayout->setMenuBar(menuBar);
	QMenu *miscMenu = menuBar->addMenu(tr("Misc"));
	for (int i = 0; i < sizeof(pageNames) / sizeof(pageNames[0]); i++) {
		miscMenu->addAction(QString::fromUtf8(pageNames[i]), this, SLOT(onNavigateAction()));
	}
	*/

	navigateTo("Startup");
}

MainWindow::~MainWindow()
{
}

void MainWindow::onNavigateAction()
{
	QAction *act = qobject_cast<QAction*>(sender());
	if (act) {
		navigateTo(act->text().toUtf8());
	}
}

void MainWindow::navigateTo(const char *pageName)
{
	NavPage *newPage = qobject_cast<NavPage*>(loadUI(pageName, NULL));
	if (!newPage) {
		return;
	}

	NavPage *oldPage = m_currentPage;
	m_currentPage = newPage;
	m_navHost->setCurrentIndex(m_navHost->addWidget(m_currentPage));
	if (oldPage) {
		oldPage->unload();
		m_navHost->removeWidget(oldPage);
		oldPage->deleteLater();
	}
	resize(400, 100);
	m_currentPage->load();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	QWidget::closeEvent(e);
//	e->ignore();
//	App::instance()->deactivateUI();
}

void MainWindow::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	if (e->type() == QEvent::WindowStateChange) {
		if (windowState() & Qt::WindowMinimized) {
			QMetaObject::invokeMethod(App::instance(), "realDeactivateUI", Qt::QueuedConnection);
		}
	}
}

QWidget *MainWindow::loadUI(const QString& name, QWidget *parentWidget)
{
	QString uiFilePath = QString::fromUtf8(":/LPCModule/ui/UserUtility%1.ui").arg(name);
	QFile uiFile(uiFilePath);
	if (!uiFile.open(QIODevice::ReadOnly)) {
		return NULL;
	}

	return m_uiLoader.load(&uiFile, parentWidget);
}

NavPage::NavPage(QWidget *parent)
	: QFrame(parent)
{
}

NavPage::~NavPage()
{
}

void NavPage::deactivateUI()
{
	App::instance()->deactivateUI();
}

void NavPage::exitApp()
{
	App::instance()->quit();
}

void NavPage::load()
{
	onLoad();
	retranslateUI();
}

void NavPage::unload()
{
	onUnload();
}

void NavPage::navigateTo(const char *pageName)
{
	App::instance()->navigateTo(pageName);
}

void NavPage::updateTextUI(QWidget *widget)
{
	updateTextUIWork(widget);

	QObjectList::const_iterator it = widget->children().begin();
	QObjectList::const_iterator ie = widget->children().end();
	for (; it != ie; ++it) {
		QWidget *childWidget = qobject_cast<QWidget*>(*it);
		if (childWidget) {
			updateTextUI(childWidget);
		}
	}
}

void NavPage::updateTextUIWork(QWidget *widget)
{
	QVariant vLPCText = widget->property("LPCText");
	QVariant vLPCTextMarkup = widget->property("LPCMarkupText");
	QVariant vLPCTextFixedMarkup = widget->property("LPCTextURL");

	if (vLPCTextMarkup.isValid()) {
		updateWidgetText(widget, markupLink(ttt(vLPCTextMarkup.toByteArray().data())));
	} else if (vLPCText.isValid()) {
		if (vLPCTextFixedMarkup.isValid()) {
			updateWidgetText(widget, ttt(vLPCText.toByteArray().data()), QString::fromUtf8(vLPCTextFixedMarkup.toByteArray()));
		} else {
			updateWidgetText(widget, ttt(vLPCText.toByteArray().data()));
		}
	}
}

void NavPage::updateWidgetText(QWidget *widget, const QString& text, const QString& fixedMarkup)
{
	if (QLabel *lbl = qobject_cast<QLabel*>(widget)) {
		if (!fixedMarkup.isEmpty()) {
			QString t = QString::fromUtf8("<a href=\"%1\">%2</a>").arg(fixedMarkup).arg(text);
			lbl->setText(t);
		} else {
			lbl->setText(text);
		}
	} else if (QAbstractButton *btn = qobject_cast<QAbstractButton*>(widget)) {
		btn->setText(text);
	} else if (QTextEdit *txt = qobject_cast<QTextEdit*>(widget)) {
		txt->setText(text);
	}
}

QString NavPage::ttt(const char *sourceText, const char *disambiguation, int n)
{
	return App::instance()->ttt(sourceText, disambiguation, n);
}

void NavPage::retranslateUI()
{
	updateTextUI(this);
}

void NavPage::setLPCText(QWidget *widget, const char *text, bool markup)
{
	if (markup) {
		widget->setProperty("LPCMarkupText", QByteArray(text));
	} else {
		widget->setProperty("LPCText", QByteArray(text));
	}
	updateTextUIWork(widget);
}

void NavPage::onLoad()
{
}

void NavPage::onUnload()
{
}

AppData *NavPage::appData() const
{
	return App::appData();
}

QNetworkAccessManager *NavPage::nam() const
{
	return App::instance()->networkAccessManager();
}

} // namespace LPCUserUtility

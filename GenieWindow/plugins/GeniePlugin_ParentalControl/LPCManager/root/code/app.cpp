#include "app.h"
#include "common.h"
#include "coreapi.h"
#include "ctrl.h"
#include "ext.h"
#include "logger.h"
#include "oputil.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMovie>
#include <QPushButton>
//#include <QScriptEngine>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "app_config.h"

void importTranslationJS(const QString& srcPath, const QString& dstPath, const QString lang);

static const char *g_mix = "\x3a\x40\xd8\x53\x80";

QString encrypt(const QString& s)
{
	QByteArray ar = s.toUtf8();
	for (int i = 0; i < ar.length(); i++) {
		char c = ar[i];
		c ^= g_mix[i % (sizeof(g_mix) / sizeof(g_mix[0]))];
		ar[i] = c;
	}
	return QString::fromUtf8(ar.toBase64());
}

QString decrypt(const QString& s)
{
	QByteArray ar = QByteArray::fromBase64(s.toUtf8());
	for (int i = 0; i < ar.length(); i++) {
		char c = ar[i];
		c ^= g_mix[i % (sizeof(g_mix) / sizeof(g_mix[0]))];
		ar[i] = c;
	}
	return QString::fromUtf8(ar);
}

LPCManager *app()
{
	return static_cast<LPCManager*>(QCoreApplication::instance());
}

AppData *appData()
{
	return app()->appData();
}

AppData::AppData()
: m_inChangeBasicSettings(false)
{
}

AppData::~AppData()
{
}

bool AppData::needSetup() const
{
	return m_username.isEmpty() || m_deviceId.isEmpty();
}

bool AppData::needShowAbortButton() const
{
	return !m_username.isEmpty() && !m_deviceId.isEmpty() && m_inChangeBasicSettings;
}

void AppData::load()
{
	QSettings settings;
	settings.beginGroup(QString::fromUtf8("data"));
	m_username = decrypt(settings.value(QString::fromUtf8("username")).toString());
	m_password = decrypt(settings.value(QString::fromUtf8("password")).toString());
	m_routerUsername = decrypt(settings.value(QString::fromUtf8("routerUsername"), encrypt(QString::fromUtf8("admin"))).toString());
	m_routerPassword = decrypt(settings.value(QString::fromUtf8("routerPassword"), encrypt(QString::fromUtf8("password"))).toString());
	m_soapAction = decrypt(settings.value(QString::fromUtf8("soapAction")).toString());
	m_deviceId = decrypt(settings.value(QString::fromUtf8("deviceId")).toString());
	m_userToken = decrypt(settings.value(QString::fromUtf8("userToken")).toString());
	settings.endGroup();
}

void AppData::save()
{
	QSettings settings;
	settings.beginGroup(QString::fromUtf8("data"));
	settings.setValue(QString::fromUtf8("username"), encrypt(m_username));
	settings.setValue(QString::fromUtf8("password"), encrypt(m_password));
	settings.setValue(QString::fromUtf8("routerUsername"), encrypt(m_routerUsername));
	settings.setValue(QString::fromUtf8("routerPassword"), encrypt(m_routerPassword));
	settings.setValue(QString::fromUtf8("soapAction"), encrypt(m_soapAction));
	settings.setValue(QString::fromUtf8("deviceId"), encrypt(m_deviceId));
	settings.setValue(QString::fromUtf8("userToken"), encrypt(m_userToken));
	settings.endGroup();
}

void AppData::reset()
{
	m_username.clear();
	m_password.clear();
	m_routerUsername.clear();
	m_routerPassword.clear();
	m_soapAction.clear();
	m_deviceId.clear();
	m_userToken.clear();
	m_inChangeBasicSettings = false;
}

struct LANG_BIND
{
	const char *name;
	int langId;
};

const LANG_BIND g_languages[] = {
	{ "cs", QLocale::Czech },
	{ "da", QLocale::Danish },
	{ "de", QLocale::German },
	{ "el", QLocale::Greek },
	{ "en", QLocale::English },
	{ "es", QLocale::Spanish },
	{ "fi", QLocale::Finnish },
	{ "hu", QLocale::Hungarian },
	{ "it", QLocale::Italian },
	{ "nl", QLocale::Dutch },
	{ "no", QLocale::Norwegian },
	{ "pl", QLocale::Polish },
	{ "pt_BR", QLocale::Portuguese },
	{ "ro", QLocale::Romanian },
	{ "ru", QLocale::Russian },
	{ "sk", QLocale::Slovak },
	{ "sl", QLocale::Slovenian },
	{ "sv", QLocale::Swedish },
};

LPCManager::LPCManager(int& argc, char **argv)
: QApplication(argc, argv), m_updateReply(NULL), m_loopLevel(0)
{
}

LPCManager::~LPCManager()
{
	if (m_updateReply) {
		QNetworkReply *reply = m_updateReply;
		m_updateReply = NULL;
		reply->abort();
		delete reply;
	}

	if (m_mainWnd) {
		delete m_mainWnd;
	}

	if (m_appData) {
		m_appData->save();
		delete m_appData;
	}
}

AppData *LPCManager::appData() const
{
	return m_appData;
}

int LPCManager::main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(LPCManager);

	LPCManager app(argc, argv);
	app.init();
	return app.run();
}

bool LPCManager::init()
{
	setOrganizationName(QString::fromUtf8("NETGEAR"));
	setOrganizationDomain(QString::fromUtf8("netgear.com"));
	setApplicationName(QString::fromUtf8("Live Parental Controls Management Utility"));
	setApplicationVersion(QString::fromUtf8(APP_VERSION_STRING));
	setWindowIcon(QIcon(QString::fromUtf8(":/images/LPCManager.ico")));

	m_defaultTranslator = new CustomTranslator(this);
	m_translator = new CustomTranslator(this);
	m_uiLoader = new CustomUILoader(this);
	m_nam = new QNetworkAccessManager(this);
	m_appData = new AppData();
	m_appData->load();

	// TODO:
/*
	QDir::addSearchPath(QString::fromUtf8("image"), QString::fromUtf8("I:/genie_mem/Genie/1.1/plugins/bind/GeniePlugin_ParentalControl/LPCManager/root/res/images"));
	QDir::addSearchPath(QString::fromUtf8("ui"), QString::fromUtf8("I:/genie_mem/Genie/1.1/plugins/bind/GeniePlugin_ParentalControl/LPCManager/root/res/ui"));
	QDir::addSearchPath(QString::fromUtf8("translation"), QString::fromUtf8("I:/genie_mem/Genie/1.1/plugins/bind/GeniePlugin_ParentalControl/LPCManager/root/res/i18n"));
	QDir::addSearchPath(QString::fromUtf8("css"), QString::fromUtf8("I:/genie_mem/Genie/1.1/plugins/bind/GeniePlugin_ParentalControl/LPCManager/root/res"));
/*/
	QDir::addSearchPath(QString::fromUtf8("image"), QString::fromUtf8(":/images"));
	QDir::addSearchPath(QString::fromUtf8("ui"), QString::fromUtf8(":/ui"));
	QDir::addSearchPath(QString::fromUtf8("translation"), QString::fromUtf8(":/i18n"));
	QDir::addSearchPath(QString::fromUtf8("css"), QString::fromUtf8(":/"));
//*/
	// css
	QFile cssFile(QString::fromUtf8("css:/LPCManager.css"));
	if (!cssFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	QTextStream cssText(&cssFile);
	setStyleSheet(cssText.readAll());

	// default language support
	m_defaultTranslator->loadPhraseBook(QString::fromUtf8("translation:/en.qph"));
	installTranslator(m_defaultTranslator);

	QString langName = QString::fromUtf8("en");

	QLocale::Language lang = QLocale::system().language();
	for (size_t i = 0; i < sizeof(g_languages) / sizeof(g_languages[0]); i++) {
		if (lang == g_languages[i].langId) {
			langName = QString::fromUtf8(g_languages[i].name);
			break;
		}
	}

	changeLanguage(langName);

	Page::registerPages(m_uiLoader);

	return true;
}

int LPCManager::run()
{
	if (requestUpdate()) {
		return 0;
	}

	m_mainWnd = new MainWindow();
	m_mainWnd->resize(800, 600);
	m_mainWnd->show();
	m_mainWnd->navigateTo(QString::fromUtf8("firmware"));
	return exec();
}

bool LPCManager::requestUpdate()
{
#ifdef Q_OS_DARWIN
	//m_checkUpdateFinished = false;

	const char *update_url = "http://updates1.netgear.com/parentalcontrols/mac/lpcupdateinfo.txt";
	//const char *update_url = "file:///f:/work_tree/lpc_deploy/lpcupdateinfo.txt";
	QNetworkRequest req(QString::fromUtf8(update_url));
	m_updateReply = m_nam->get(req);
	connect(m_updateReply, SIGNAL(finished()), SLOT(updateRequestFinished()));

#else
#endif

	return false;
}

int compareVersion(const QByteArray& v1, const QByteArray& v2)
{
	if (v1 == v2) {
		return 0;
	}

	QList<QByteArray> p1 = v1.split('.');
	QList<QByteArray> p2 = v2.split('.');

	if (p1.length() != 3 || p2.length() != 3) {
		return 1;
	}

	int major1 = p1.at(0).toInt();
	int major2 = p2.at(0).toInt();

	if (major1 > major2) {
		return 1;
	}

	if (major1 < major2) {
		return -1;
	}

	int minor1 = p1.at(1).toInt();
	int minor2 = p2.at(1).toInt();

	if (minor1 > minor2) {
		return 1;
	}

	if (minor1 < minor2) {
		return -1;
	}

	int patch1 = p1.at(2).toInt();
	int patch2 = p2.at(2).toInt();

	if (patch1 > patch2) {
		return 1;
	}

	if (patch1 < patch2) {
		return -1;
	}

	return 0;
}

void LPCManager::updateRequestFinished()
{
	if (m_updateReply) {
		QNetworkReply *reply = m_updateReply;
		m_updateReply->deleteLater();
		m_updateReply = NULL;

		if (reply->error() == QNetworkReply::NoError) {

			QByteArray updateVersion;
			QByteArray updateURL;

			QByteArray data = reply->readAll();
			QList<QByteArray> lines = nice_split(data);
			for (int i = 0; i < lines.length(); i++) {
				QByteArray line = lines.at(i);
				QList<QByteArray> items = line.split('=');
				if (items.length() == 2) {
					QByteArray key = items.at(0).toLower();
					if (key == "version") {
						updateVersion = items.at(1);
					} else if (key == "downloadurl") {
						updateURL = items.at(1);
					}
				}
			}

			if (!updateVersion.isEmpty() && !updateURL.isEmpty() && compareVersion(updateVersion, APP_VERSION_STRING) > 0) {
				connect(&m_updateTimer, SIGNAL(timeout()), SLOT(promptUpdate()));
				m_updateTimer.start(1000);
				m_updateDownloadUrl = QString::fromUtf8(updateURL);
			}
		}
	}
}

void LPCManager::promptUpdate()
{
	if (m_loopLevel == 0) {
		m_updateTimer.stop();
		if (QMessageBox::Yes == QMessageBox::question(m_mainWnd, tr("upgrade.confirm.title"), tr("upgrade.confirm"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
			QMessageBox::information(m_mainWnd, tr("upgrade.alert.title"), tr("upgrade.alert"));
			QDesktopServices::openUrl(m_updateDownloadUrl);
			quit();
		}
	}
}

QNetworkAccessManager *LPCManager::networkAccessManager() const
{
	return m_nam;
}

QWidget *LPCManager::loadUI(const QString& name, QWidget *parentWidget)
{
	// TODO:
	QString uiFilePath = QString::fromUtf8("ui:/");
	uiFilePath.append(name);
	if (!name.toLower().endsWith(QString::fromUtf8(".ui"))) {
		uiFilePath.append(QString::fromUtf8(".ui"));
	}

	QFile uiFile(uiFilePath);
	if (!uiFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	return m_uiLoader->load(&uiFile, parentWidget);
}

void LPCManager::changeLanguage(const QString& langName)
{
	if (m_translator) {
		removeTranslator(m_translator);
	} else {
		m_translator = new CustomTranslator(this);
	}

	QString url = QString::fromUtf8("translation:/");
	url.append(langName);
	url.append(QString::fromUtf8(".qph"));
	m_translator->loadPhraseBook(url);
	installTranslator(m_translator);

	if (m_mainWnd) {
		postEvent(m_mainWnd, new QEvent(QEvent::LanguageChange));
	}
}

bool LPCManager::wait(AsyncOp *op, const QString& promptText)
{
	++m_loopLevel;
	WaitPopup popup(promptText, op, m_mainWnd);
	bool retval = popup.exec() == QDialog::Accepted;
	--m_loopLevel;
	return retval;
}

void LPCManager::requestNavigateTo(const QString& pageName)
{
	if (!m_mainWnd) {
		return;
	}

	m_mainWnd->requestNavigateTo(pageName);
}

MainWindow::MainWindow()
: m_activePage(NULL)
{
	setWindowTitle(QString::fromUtf8("%1 %2 %3").arg(QApplication::organizationName()).arg(QApplication::applicationName()).arg(QApplication::applicationVersion()));
	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

#ifdef ENABLE_DEBUG_UI

#ifdef Q_OS_DARWIN
	m_debugMenuBar = new QMenu(this);
#else
	m_debugMenuBar = new QMenuBar();
	layout0->addWidget(m_debugMenuBar);
#endif // Q_OS_DARWIN

	QMenu *menu1 = m_debugMenuBar->addMenu(QString::fromUtf8("nav"));

	QAction *act;

	const char *pageNames[] = {
		"firmware", "intro", "disclaimer", "presignin", "signin", "createaccount", "done", "manage", "settings"
	};

	for (size_t i = 0; i < sizeof(pageNames) / sizeof(pageNames[0]); i++) {
		act = menu1->addAction(QString::fromUtf8(pageNames[i]));
		connect(act, SIGNAL(triggered()), SLOT(navigateToAct()));
	}

	QMenu *menu2 = m_debugMenuBar->addMenu(QString::fromUtf8("language"));

	//act = menu2->addAction(QLocale::countryToString(QLocale::system().country()));
	//act->setEnabled(false);
	m_langInd = menu2->addAction(QString::fromUtf8("current system language: %1").arg(QLocale::languageToString(QLocale::system().language())));
	m_langInd->setEnabled(false);
	menu2->addSeparator();

	for (size_t i = 0; i < sizeof(g_languages) / sizeof(g_languages[0]); i++) {
		act = menu2->addAction(QString::fromUtf8(g_languages[i].name));
		connect(act, SIGNAL(triggered()), SLOT(changeLanguageAct()));
	}

	act = new QAction(this);
	act->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	act->setShortcutContext(Qt::ApplicationShortcut);
	connect(act, SIGNAL(triggered()), SLOT(toggleDebugMenuBar()));
	addAction(act);

	QMenu *menu3 = m_debugMenuBar->addMenu(QString::fromUtf8("misc"));
	act = menu3->addAction(QString::fromUtf8("ssl test..."));
	connect(act, SIGNAL(triggered()), SLOT(doSslTest()));
	act = menu3->addAction(QString::fromUtf8("clear saved credentials"));
	connect(act, SIGNAL(triggered()), SLOT(doClearSavedCredentials()));

	m_debugMenuBar->setVisible(false);

#endif // ENABLE_DEBUG_UI

	m_navPane = new QStackedLayout();
	layout0->addLayout(m_navPane);

	connect(this, SIGNAL(internalRequestNavigateTo(const QString&)), SLOT(navigateTo(const QString&)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onLanguageChanged()
{
#ifdef ENABLE_DEBUG_UI
	m_langInd->setText(QString::fromUtf8("current system language: %1").arg(QLocale::languageToString(QLocale::system().language())));
#endif // ENABLE_DEBUG_UI

	if (!m_activePageName.isEmpty()) {
		navigateTo(m_activePageName);
	}
}

bool MainWindow::event(QEvent *e)
{
	if (e->type() == QEvent::LanguageChange) {
		onLanguageChanged();
		return true;
	}
	return QFrame::event(e);
}

#ifdef ENABLE_DEBUG_UI

void MainWindow::toggleDebugMenuBar()
{
#ifdef Q_OS_DARWIN
	m_debugMenuBar->exec(QCursor::pos());
#else
	m_debugMenuBar->setVisible(!m_debugMenuBar->isVisible());
#endif // Q_OS_DARWIN
}

QString g_sslTestUrl = QString::fromUtf8("https://api.opendns.com/v1/");

void MainWindow::doSslTest()
{
	QInputDialog inputDlg;
	bool ok;
	QString url = QInputDialog::getText(this, QString::fromUtf8("SSL Test"), QString::fromUtf8("Please enter an URL:"), QLineEdit::Normal, g_sslTestUrl, &ok);
	if (ok) {
		g_sslTestUrl = url;
		AsyncOp *op = CoreApi::beginSSLTest(url, app()->networkAccessManager(), this);
		bool waitOk = app()->wait(op, QString::fromUtf8("Commmicating to %1").arg(url));
		op->deleteLater();
		int code;
		if (waitOk) {
			code = op->result().toInt();
		} else {
			// aborted
			code = WTFStatus_Aborted;
		}

		DBGLOG(LOG_INFO, 2, QString::fromUtf8("SSLTest returns: %1").arg(code));
	}
}

void MainWindow::doClearSavedCredentials()
{
	appData()->reset();
	navigateTo(QString::fromUtf8("firmware"));
}

#endif // ENABLE_DEBUG_UI

void MainWindow::requestNavigateTo(const QString& pageName)
{
	emit internalRequestNavigateTo(pageName);
}

void MainWindow::navigateTo(const QString& pageName)
{
	Page *oldPage = m_activePage;
	Page *newPage = qobject_cast<Page*>(app()->loadUI(pageName));
	if (!newPage) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("loadUI failed!"));
		return;
	}

	if (!newPage->prepare()) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("prepare failed!"));
		delete newPage;
		return;
	}

	m_navPane->setCurrentIndex(m_navPane->addWidget(newPage));
	m_activePage = newPage;

	if (oldPage) {
		oldPage->unload();
		oldPage->deleteLater();
	}

	m_activePageName = pageName;
	newPage->load();
}

void MainWindow::navigateToAct()
{
	QAction *act = qobject_cast<QAction*>(sender());
	if (act) {
		navigateTo(act->text());
	}
}

void MainWindow::changeLanguageAct()
{
	QAction *act = qobject_cast<QAction*>(sender());
	if (act) {
		app()->changeLanguage(act->text());
	}
}

void generateQPH()
{
/*	QDir dir("I:/misc_script/opendns_mgm");
	QStringList files = dir.entryList();
	QStringList::const_iterator it = files.begin();
	QStringList::const_iterator ie = files.end();
	for (; it != ie; ++it) {
		QString dst = it->toLower();
		if (dst.endsWith(".js")) {
			int pos = dst.indexOf("-");
			if (pos > 0) {
				dst = dst.mid(pos + 1);
				dst = dst.left(dst.length() - 3);
				QString lang = dst;
				int pos2 = dst.indexOf('-');
				if (pos2 > 0) {
					lang = dst.left(pos2);
					lang.push_back('_');
					lang.append(dst.mid(pos2 + 1).toUpper());
				}
				dst = lang;
				dst.append(".qph");
				importTranslationJS(dir.filePath(*it), dir.filePath(dst), lang);
			}
		}
	}*/
}
/*
static QScriptValue __collectEntry(QScriptContext *context, QScriptEngine *engine, void *arg)
{
	QMap<QString, QString> *m = static_cast<QMap<QString, QString>*>(arg);

	if (context->argumentCount() == 2) {
		m->insert(context->argument(0).toString(), context->argument(1).toString());
	}

	return QScriptValue();
}

void importTranslationJS(const QString& srcPath, const QString& dstPath, const QString lang)
{
	QFile jsFile(srcPath);
	if (!jsFile.open(QIODevice::ReadOnly)) {
		return;
	}

	QFile outFile(dstPath);
	if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return;
	}

	QTextStream stream(&jsFile);
	stream.setCodec(QTextCodec::codecForName("utf-8"));
	QString translationJS = stream.readAll();

	QString js = translationJS;
	js.append(QString::fromUtf8("\nfor (e in translations) { collect(e, translations[e]) }"));

	QMap<QString, QString> tm;

	QScriptEngine engine;

	QScriptValue fun1 = engine.newFunction(__collectEntry, &tm);
	engine.globalObject().setProperty(QString::fromUtf8("collect"), fun1);

	QScriptValue retval = engine.evaluate(js);
	if (retval.isError()) {
		QStringList ls = engine.uncaughtExceptionBacktrace();
		qDebug() << ls;
		return;
	}

	QString xml;
	xml.append(QString::fromUtf8("<!DOCTYPE QPH>\n<QPH language=\""));
	xml.append(lang);
	xml.append(QString::fromUtf8("\">\n"));

	QMap<QString, QString>::const_iterator it = tm.begin();
	QMap<QString, QString>::const_iterator ie = tm.end();
	for (; it != ie; ++it) {
		xml.append(QString::fromUtf8("<phrase>\n\t<source>"));
		xml.append(it.key());
		xml.append(QString::fromUtf8("</source>\n\t<target>"));
		xml.append(it.value());
		xml.append(QString::fromUtf8("</target>\n</phrase>\n"));
	}

	xml.append(QString::fromUtf8("</QPH>\n"));

	QByteArray utf8Text;
	utf8Text.append(QString::fromUtf8("\xef\xbb\xbf"));
	utf8Text.append(xml.toUtf8());

	outFile.write(utf8Text);
}
*/
WaitPopup::WaitPopup(const QString& promptText, AsyncOp *op, QWidget *parent)
: QDialog(parent, Qt::Dialog), m_op(op)
{
	setWindowTitle(tr("general.wait.title"));

	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

	QHBoxLayout *promptPane = new QHBoxLayout();
	layout0->addLayout(promptPane);

	QLabel *gifLabel = new QLabel();
	gifLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	promptPane->addWidget(gifLabel);

	QLabel *prompt = new QLabel(promptText);
	promptPane->addWidget(prompt);
	prompt->setProperty("class", QString::fromUtf8("singleline"));

	promptPane->addStretch();

	QMovie *gifMovie = new QMovie(QString::fromUtf8("image:/icon_working.gif"), "gif", gifLabel);
	gifLabel->setMovie(gifMovie);
	gifMovie->start();

	QHBoxLayout *buttonPane = new QHBoxLayout();
	layout0->addLayout(buttonPane);

	QPushButton *btnAbort = new QPushButton(tr("abort"));

	buttonPane->addStretch();
	buttonPane->addWidget(btnAbort);

	connect(btnAbort, SIGNAL(clicked()), SLOT(reject()));

	connect(op, SIGNAL(finished()), SLOT(accept()));
}

WaitPopup::~WaitPopup()
{
}

void WaitPopup::reject()
{
	m_op->abort();
	QDialog::reject();
}

void getRidOfSslErrors(QNetworkReply *reply)
{
	// TODO: safe ?
	reply->ignoreSslErrors();
}

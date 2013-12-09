#include "env.h"
#include "logger.h"
#include <QtCore/QLocale>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

const char *g_pageNames[] = {
	"firmware",
	"intro",
	"disclaimer",
	"presignin",
	"signin",
	"createaccount",
	"settings",
	"done",
	"manage",
};

struct LANG_BIND
{
	const char *code;
	const char *name;
	int langId;
};

static const LANG_BIND g_languages[] = {
	{ "csy", "cs", QLocale::Czech },
	{ "dan", "da", QLocale::Danish },
	{ "deu", "de", QLocale::German },
	{ "ell", "el", QLocale::Greek },
	{ "enu", "en", QLocale::English },
	{ "esp", "es", QLocale::Spanish },
	{ "fin", "fi", QLocale::Finnish },
	{ "hun", "hu", QLocale::Hungarian },
	{ "ita", "it", QLocale::Italian },
	{ "nld", "nl", QLocale::Dutch },
	{ "nor", "no", QLocale::Norwegian },
	{ "plk", "pl", QLocale::Polish },
	{ "ptb", "pt_BR", QLocale::Portuguese },
	{ "rom", "ro", QLocale::Romanian },
	{ "rus", "ru", QLocale::Russian },
	{ "sky", "sk", QLocale::Slovak },
	{ "slv", "sl", QLocale::Slovenian },
	{ "sve", "sv", QLocale::Swedish },
/*
	{ "fra", "fr", QLocale::French },
	{ "bgr", "bg", QLocale::Bulgarian },
	{ "hrv", "hr", QLocale::Croatian },
	{ "chs", "chs", QLocale::Chinese },
	{ "kor", "ko", QLocale::Korean },
	{ "jpn", "jp", QLocale::Japanese },
	{ "ara", "ar", QLocale::Arabic },
	{ "tur", "tu", QLocale::Turkish },
*/
};

int ak_main(int argc, char **argv)
{
	HomeApp app(argc, argv);
	return app.run();
}

int main(int argc, char **argv)
{
	DBGLOGINIT();
	int retval = ak_main(argc, argv);
	DBGLOGTERM();
	return retval;
}

HomeApp::HomeApp(int& argc, char **argv)
: QApplication(argc, argv)
{
	QWidget *w = new QWidget();
	postEvent(w, new QEvent(QEvent::User));
	delete w;
}

HomeApp::~HomeApp()
{
}

int HomeApp::run()
{
	HomeWindow wnd;
	wnd.show();
	return exec();
}

HomeWindow::HomeWindow()
: m_langCode("enu")
{
	m_translator = new SimpleTranslator();
	m_translator->loadDefaultDict(QString::fromUtf8(":/LPCModule/i18n/en.qph"));

	QVBoxLayout *rootLayout = new QVBoxLayout();
	setLayout(rootLayout);

	QMenuBar *menuBar = new QMenuBar();
	rootLayout->addWidget(menuBar);

	m_tab = new QTabWidget();
	rootLayout->addWidget(m_tab);

	QMenu *languageMenu = menuBar->addMenu(tr("language"));
	QAction *act = languageMenu->addAction(tr("enu"));
	connect(act, SIGNAL(triggered()), SLOT(onLanguageAction()));
	act = languageMenu->addAction(tr("deu"));
	connect(act, SIGNAL(triggered()), SLOT(onLanguageAction()));

	QMenu *testMenu = menuBar->addMenu(tr("test"));
	act = testMenu->addAction(tr("popup1"));
	connect(act, SIGNAL(triggered()), SLOT(onPopup1()));
	act = testMenu->addAction(tr("popup2"));
	connect(act, SIGNAL(triggered()), SLOT(onPopup2()));

	QMenu *navMenu = menuBar->addMenu(tr("nav"));
	for (size_t i = 0; i < sizeof(g_pageNames) / sizeof(g_pageNames[0]); i++) {
		act = navMenu->addAction(QString::fromUtf8(g_pageNames[i]));
		connect(act, SIGNAL(triggered()), SLOT(onNav()));
	}

	QWidget *homePage = new QWidget();
	QVBoxLayout *homeLayout = new QVBoxLayout();
	homePage->setLayout(homeLayout);
	QPushButton *bigButton = new QPushButton(tr("lpc"));
	homeLayout->addWidget(bigButton);
	homeLayout->addStretch();

	m_lpcFrame = new LPCMainFrame(this, this);

	m_tab->addTab(homePage, tr("home"));
	m_tab->addTab(m_lpcFrame, tr("lpc"));

	connect(m_tab, SIGNAL(currentChanged(int)), SLOT(onSwitchTab()));
	connect(bigButton, SIGNAL(clicked()), SLOT(onBigButtonClicked()));

	qApp->postEvent(this, new QEvent(QEvent::LanguageChange));
	resize(600, 480);
}

HomeWindow::~HomeWindow()
{
	m_lpcFrame->detachEnv();
}

void HomeWindow::onSwitchTab()
{
	if (m_tab->currentWidget() == m_lpcFrame) {
		m_lpcFrame->onActivate(true);
	} else {
		m_lpcFrame->onActivate(false);
	}
}

void HomeWindow::onBigButtonClicked()
{
	m_tab->setCurrentWidget(m_lpcFrame);
}

void HomeWindow::onLanguageAction()
{
	QAction *act = qobject_cast<QAction*>(sender());
	if (act) {
		QByteArray langCode = act->text().toUtf8();
		if (m_langCode != langCode) {
			m_langCode = langCode;
			qApp->postEvent(this, new QEvent(QEvent::LanguageChange));
		}
	}
}

void HomeWindow::onPopup1()
{
	m_lpcFrame->push(new TestPopupA(NULL));
}

void HomeWindow::onPopup2()
{
	m_lpcFrame->push(new TestPopupB(NULL));
}

void HomeWindow::onNav()
{
	QAction *act = qobject_cast<QAction*>(sender());
	if (act) {
		m_lpcFrame->navigateTo(act->text());
	}
}

QByteArray HomeWindow::currentLanguageCode()
{
	return m_langCode;
}

void HomeWindow::reportLPCStatus(int status)
{
}

void HomeWindow::changeLanguage()
{
	QByteArray code = currentLanguageCode();

	m_translator->unloadDict();

	const char *name = "en";
	for (int i = 0; i < sizeof(g_languages) / sizeof(g_languages[0]); i++) {
		if (code == g_languages[i].code) {
			name = g_languages[i].name;
			break;
		}
	}

	if (strcmp(name, "en") != 0) {
		m_translator->loadDict(QString::fromUtf8(":/LPCModule/i18n/%1.qph").arg(name));
	}
}

QString HomeWindow::translateText(const char *code)
{
	return m_translator->translate(code);
}

void HomeWindow::queryRouterAuth(QString& username, QString& password)
{
}

TestPopupA::TestPopupA(QWidget *parent)
: LPCNavPopup(parent)
{
	QHBoxLayout *layout0 = new QHBoxLayout();
//	layout0->setContentsMargins(0, 0, 0, 0);
	setLayout(layout0);

	layout0->addWidget(new QPushButton(tr("hello")));
	layout0->addWidget(new QPushButton(tr("world")));

	QPushButton *closeBtn = new QPushButton(tr("close"));
	layout0->addWidget(closeBtn);
	connect(closeBtn, SIGNAL(clicked()), SLOT(pop()));

	setCaption(QString::fromUtf8("TestPopupA"));
}

TestPopupA::~TestPopupA()
{
}

TestPopupB::TestPopupB(QWidget *parent)
: LPCNavPopup(parent)
{
	QVBoxLayout *layout0 = new QVBoxLayout();
//	layout0->setContentsMargins(0, 0, 0, 0);
	setLayout(layout0);

	QPushButton *btn1;
	layout0->addWidget(btn1 = new QPushButton(tr("hello")));
	layout0->addWidget(new QPushButton(tr("world")));

	QPushButton *closeBtn = new QPushButton(tr("close"));
	layout0->addWidget(closeBtn);
	connect(closeBtn, SIGNAL(clicked()), SLOT(pop()));

	setCaption(QString::fromUtf8("TestPopupB"));

	connect(btn1, SIGNAL(clicked()), SLOT(test()));
}

TestPopupB::~TestPopupB()
{
}

void TestPopupB::test()
{
	setCaption(QString::fromUtf8("HaHaa!!!!!!!!!!!!!!!!!!!!!!!!"));
}

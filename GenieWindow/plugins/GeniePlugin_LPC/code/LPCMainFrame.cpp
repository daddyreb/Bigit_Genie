#include "logger.h"
#include "LPCMainFrame.h"
#include "LPCNavPage.h"
#include "LPCNavPopup.h"
#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <QtNetwork/QNetworkReply>
#include <qdebug.h>
void getRidOfSslErrors(QNetworkReply *reply)
{
	// TODO: safe ?
	reply->ignoreSslErrors();
}

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

void LPCRegisterPages(CustomUILoader *loader);

AppData::AppData()
: m_inChangeBasicSettings(false)
{
}

AppData::~AppData()
{
}

bool AppData::needSetup(const QString& deviceId) const
{
	if (deviceId.isEmpty()) {
		return true;
	}

	if (!deviceId.isEmpty() && !m_deviceId.isEmpty() && deviceId != m_deviceId) {
		return true;
	}
	return m_username.isEmpty() || m_deviceId.isEmpty();
}

bool AppData::needShowAbortButton() const
{
	return !m_username.isEmpty() && !m_deviceId.isEmpty() && m_inChangeBasicSettings;
}

void AppData::load()
{
	QSettings settings(QString::fromUtf8("NETGEAR"), QString::fromUtf8("Live Parental Controls Management Utility"));
	settings.beginGroup(QString::fromUtf8("data"));
	m_username = decrypt(settings.value(QString::fromUtf8("username")).toString());
	m_password = decrypt(settings.value(QString::fromUtf8("password")).toString());
	m_routerUsername = decrypt(settings.value(QString::fromUtf8("routerUsername"), encrypt(QString::fromUtf8("admin"))).toString());
	m_routerPassword = decrypt(settings.value(QString::fromUtf8("routerPassword"), encrypt(QString::fromUtf8("password"))).toString());
	m_deviceId = decrypt(settings.value(QString::fromUtf8("deviceId")).toString());
	m_userToken = decrypt(settings.value(QString::fromUtf8("userToken")).toString());
	settings.endGroup();
}

void AppData::save()
{
	QSettings settings(QString::fromUtf8("NETGEAR"), QString::fromUtf8("Live Parental Controls Management Utility"));
	settings.beginGroup(QString::fromUtf8("data"));
	settings.setValue(QString::fromUtf8("username"), encrypt(m_username));
	settings.setValue(QString::fromUtf8("password"), encrypt(m_password));
	settings.setValue(QString::fromUtf8("routerUsername"), encrypt(m_routerUsername));
	settings.setValue(QString::fromUtf8("routerPassword"), encrypt(m_routerPassword));
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
	m_deviceId.clear();
	m_userToken.clear();
	m_inChangeBasicSettings = false;
}

LPCMainFrame::LPCMainFrame(WTFHelper *helper, QWidget *parent)
: QFrame(parent), m_helper(helper), m_bFirstActivate(true)
, m_lpcStatus(LPCSTATUS_UNKNOWN)
{
	m_appData = new AppData();
	m_appData->load();

	updateAppDataRouterAuth();

	m_nam = new QNetworkAccessManager();
	//m_nam->setCookieJar(new QNetworkCookieJar());
	m_coreApi = new CoreApi(m_nam);
	//m_coreApi->setControlPointId(QLatin1String("cp171"));
	//m_coreApi->setSmartNetworkUsername(QLatin1String("larkvm@gmail.com"));
	//m_coreApi->setSmartNetworkPassword(QLatin1String("12345678"));
	//m_coreApi->setFcmlMode(true);
	m_coreApi->routerApi()->setSmartNetworkBaseUrl(m_helper->querySmartNetworkBaseUrl());

	m_uiLoader = new CustomUILoader(this);

	LPCRegisterPages(m_uiLoader);

	m_rootLayout = new QStackedLayout();
	m_rootLayout->setStackingMode(QStackedLayout::StackAll);
	setLayout(m_rootLayout);

	QWidget *navLayer = new QWidget();
	m_navPane = new QStackedLayout();
	navLayer->setLayout(m_navPane);
	m_rootLayout->addWidget(navLayer);

	QFile cssFile(QString::fromUtf8(":/LPCModule/default.css"));
	if (cssFile.open(QIODevice::ReadOnly)) {
		setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
	}

	connect(this, SIGNAL(internalRequestNavigateTo(const QString&,bool)), SLOT(navigateTo(const QString&,bool)), Qt::QueuedConnection);

	reportStatus(LPCSTATUS_UNKNOWN);
}

LPCMainFrame::~LPCMainFrame()
{
	delete m_coreApi;
	delete m_nam;
	m_appData->save();
	delete m_appData;
}

void LPCMainFrame::detachEnv()
{
	m_helper = NULL;
}

CoreApi *LPCMainFrame::coreApi() const
{
	return m_coreApi;
}

QString LPCMainFrame::ttt(const char *sourceText, const char *disambiguation, int n)
{
	if (!m_helper) {
		return QString::fromLatin1(sourceText);
	}
	return m_helper->translateText(sourceText);
}

void LPCMainFrame::reportStatus(int status)
{
	m_lpcStatus = status;
	if (m_helper) {
		m_helper->reportLPCStatus(status);
	}
}

void LPCMainFrame::reportLPCEnableDisable(bool enable)
{
	if (m_helper) {
		m_helper->reportLPCEnableDisable(enable);
	}
}

void LPCMainFrame::reportLPCLevel(int level)
{
	if (m_helper) {
		m_helper->reportLPCLevel(level);
	}
}

void LPCMainFrame::reportLPCBasicSettingClick()
{
	if (m_helper) {
		m_helper->reportLPCBasicSettingClick();
	}
}

void LPCMainFrame::reportLPCCustomSettingClick()
{
	if (m_helper) {
		m_helper->reportLPCCustomSettingClick();
	}
}

void LPCMainFrame::onActivate(bool active)
{
	if (active) {
		if (m_bFirstActivate) {
			m_bFirstActivate = false;
			navigateTo("firmware", false);
		}
	}
}

void LPCMainFrame::changeEvent(QEvent *e)
{
	QFrame::changeEvent(e);
	if (e->type() == QEvent::LanguageChange) {
		changeLanguage();
	}
}

void LPCMainFrame::changeLanguage()
{
	if (m_helper) {
		m_helper->changeLanguage();
	}

	int layerCount = m_rootLayout->count();
	for (int i = 0; i < layerCount; i++) {
		LPCPopupHost *layer = qobject_cast<LPCPopupHost*>(m_rootLayout->widget(i));
		if (layer) {
			layer->retranslateUI();
		}
	}

	if (m_activePage) {
		m_activePage->retranslateUI();
	}

	if (m_helper) {
		m_helper->reportLPCStatus(m_lpcStatus);
	}
}

void LPCMainFrame::navigateTo(const QString& pageName, bool force)
{qDebug()<<"pageName:"<<pageName<<" force:"<<force;
	if (m_activePageName == pageName && !force) {
		return;
	}

	LPCNavPage *oldPage = m_activePage;
	LPCNavPage *newPage = qobject_cast<LPCNavPage*>(loadUI(pageName, NULL));
        if (!newPage) {qDebug()<<"loadUI failed!";
                DBGLOG(LOG_ERROR, 3, QString::fromUtf8("loadUI failed!"));
                return;
        }
        qDebug()<<"**navigateTo success**";
	newPage->init(this);

        if (!newPage->prepare()) {qDebug()<<"prepare failed!";
                DBGLOG(LOG_ERROR, 3, QString::fromUtf8("prepare failed!"));
                delete newPage;
                return;
        }

	m_navPane->setCurrentIndex(m_navPane->addWidget(newPage));
        m_activePage = newPage;

        if (oldPage) {
                oldPage->unload();
                oldPage->deleteLater();
                qDebug()<<"oldPage delete";
        }

	m_activePageName = pageName;
        qDebug()<<"navigateTo----pageName:"<<pageName;
	newPage->load();


}

void LPCMainFrame::push(LPCNavPopup *popup)
{
	popup->internalInit(this);
	LPCPopupHost *host = new LPCPopupHost(popup, NULL);
	m_rootLayout->widget(0)->setEnabled(false);
	m_rootLayout->insertWidget(0, host);
	m_rootLayout->setCurrentWidget(host);
	popup->onPopup();
}

void LPCMainFrame::pop(LPCNavPopup *popup)
{
	LPCPopupHost *host = qobject_cast<LPCPopupHost*>(popup->parent()->parent());
	m_rootLayout->removeWidget(host);
	m_rootLayout->widget(0)->setEnabled(true);
	m_rootLayout->setCurrentIndex(0);
	host->deleteLater();
}

void LPCMainFrame::jumpSmart(const QString& cpid, const QString& smartNetworkUsername, const QString& smartNetworkPassword, const QString& routerUsername, const QString& routerPassword)
{
	if (!cpid.isEmpty()) {
		DBGLOG(LOG_INFO, 1, QString::fromUtf8("jumpSmart %1 %2 %3 %4 %5").arg(cpid, smartNetworkUsername, smartNetworkPassword, routerUsername, routerPassword));
		m_appData->m_routerUsername = routerUsername;
		m_appData->m_routerPassword = routerPassword;
		m_coreApi->setControlPointId(cpid);
		m_coreApi->setSmartNetworkUsername(smartNetworkUsername);
		m_coreApi->setSmartNetworkPassword(smartNetworkPassword);
		m_coreApi->setRouterUsername(routerUsername);
		m_coreApi->setRouterPassword(routerPassword);
		m_coreApi->setFcmlMode(true);
	} else {
		DBGLOG(LOG_INFO, 1, QString::fromUtf8("jumpSmart FCML OFF"));
		m_coreApi->setFcmlMode(false);
	}
	navigateTo("firmware", false);
}

QWidget *LPCMainFrame::loadUI(const QString& name, QWidget *parentWidget)
{
	QString uiFilePath = QString::fromUtf8(":/LPCModule/ui/%1.ui").arg(name);
	QFile uiFile(uiFilePath);
	if (!uiFile.open(QIODevice::ReadOnly)) {
		return NULL;
	}
        qDebug()<<"LPCMainFrame::loadUI--uiFilePath:"<<uiFilePath;
        QWidget* w = m_uiLoader->load(&uiFile, parentWidget);
        if(w)
        {
            qDebug()<<"*LPCMainFrame::loadUI--succeed";
        }else
        {
            qDebug()<<"*LPCMainFrame::loadUI--failed";
        }
        return w;
}

void LPCMainFrame::requestNavigateTo(const QString& pageName, bool force)
{
	emit internalRequestNavigateTo(pageName, force);
}

void LPCMainFrame::wait(AsyncOp *op, const QObject *receiver, const char *method, const QString& promptText)
{
	InternalWaitPopup *popup = new InternalWaitPopup(op, receiver, method, promptText, NULL);
	push(popup);
}

void LPCMainFrame::wait(AsyncOp *op, const QObject *receiver, const char *method, const char *promptId)
{
	InternalWaitPopup *popup = new InternalWaitPopup(op, receiver, method, promptId, NULL);
	push(popup);
}

void LPCMainFrame::updateAppDataRouterAuth()
{
	if (m_helper) {
		m_helper->queryRouterAuth(m_appData->m_routerUsername, m_appData->m_routerPassword);
	}
}

LPCPopupHost::LPCPopupHost(LPCNavPopup *child, QWidget *parent)
: QFrame(parent), m_child(child)
{
	QVBoxLayout *layout0 = new QVBoxLayout();
	layout0->setContentsMargins(0, 0, 0, 0);
	setLayout(layout0);

	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->setContentsMargins(0, 0, 0, 0);

	LPCPopupFrame *frame = new LPCPopupFrame(child, NULL);

	layout0->addStretch();
	layout0->addLayout(layout1);
	layout0->addStretch();

	layout1->addStretch();
	layout1->addWidget(frame);
	layout1->addStretch();
}

LPCPopupHost::~LPCPopupHost()
{
}

void LPCPopupHost::retranslateUI()
{
	m_child->retranslateUI();
}

LPCPopupFrame::LPCPopupFrame(LPCNavPopup *content, QWidget *parent)
: m_tracking(false)
{
	QVBoxLayout *rootLayout = new QVBoxLayout();
	rootLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(rootLayout);

	QHBoxLayout *titleLayout = new QHBoxLayout();
	titleLayout->setContentsMargins(0, 0, 0, 0);
	rootLayout->addLayout(titleLayout);
	rootLayout->addWidget(content);

	m_title = new QLabel(content->caption());
	m_title->setObjectName(QString::fromUtf8("lpcPopupFrameTitle"));
	m_title->setWordWrap(false);
	titleLayout->addWidget(m_title);

	content->internalInit2(this);
}

LPCPopupFrame::~LPCPopupFrame()
{
}

void LPCPopupFrame::changeCaption(const QString& caption)
{
	m_title->setText(caption);
}

void LPCPopupFrame::mouseMoveEvent(QMouseEvent *e)
{
	QFrame::mouseMoveEvent(e);
	if (m_tracking) {
		e->accept();
		int x = e->globalX() - m_dx;
		int y = e->globalY() - m_dy;
		move(x, y);
	}
}

void LPCPopupFrame::mousePressEvent(QMouseEvent *e)
{
	QFrame::mousePressEvent(e);
	if (!m_tracking && e->button() == Qt::LeftButton) {
		e->accept();
		m_tracking = true;
		m_dx = e->globalX() - pos().x();
		m_dy = e->globalY() - pos().y();
	}
}

void LPCPopupFrame::mouseReleaseEvent(QMouseEvent *e)
{
	QFrame::mouseReleaseEvent(e);
	if (m_tracking && e->button() == Qt::LeftButton) {
		e->accept();
		m_tracking = false;
	}
}

InternalWaitPopup::InternalWaitPopup(AsyncOp *op, const QObject *receiver, const char *method, const QString& promptText, QWidget *parent)
: LPCNavPopup(parent), m_op(op)
{
	initCommon();
	connect(this, SIGNAL(internalClose(AsyncOp*, bool)), receiver, method, Qt::QueuedConnection);
	m_promptLabel->setText(promptText);
	retranslateUI();
}

InternalWaitPopup::InternalWaitPopup(AsyncOp *op, const QObject *receiver, const char *method, const char *promptId, QWidget *parent)
: LPCNavPopup(parent), m_op(op), m_promptId(promptId)
{
	initCommon();
	connect(this, SIGNAL(internalClose(AsyncOp*, bool)), receiver, method, Qt::QueuedConnection);
	retranslateUI();
}

InternalWaitPopup::~InternalWaitPopup()
{
}

void InternalWaitPopup::initCommon()
{
	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

	QHBoxLayout *promptPane = new QHBoxLayout();
	layout0->addLayout(promptPane);

	QLabel *gifLabel = new QLabel();
	gifLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	promptPane->addWidget(gifLabel);

	QLabel *prompt = new QLabel();
	prompt->setProperty("LPCTags", QStringList(QString::fromUtf8("SingleLine")));
	promptPane->addWidget(prompt);
	prompt->setProperty("class", QString::fromUtf8("singleline"));

	promptPane->addStretch();

	QMovie *gifMovie = new QMovie(QString::fromUtf8(":/LPCModule/images/icon_working.gif"), "gif", gifLabel);
	gifLabel->setMovie(gifMovie);
	gifMovie->start();

	QHBoxLayout *buttonPane = new QHBoxLayout();
	layout0->addLayout(buttonPane);

	QPushButton *btnAbort = new QPushButton();
    btnAbort->setAutoDefault(true);

	buttonPane->addStretch();
	buttonPane->addWidget(btnAbort);

	m_btnAbort = btnAbort;
	m_promptLabel = prompt;

	connect(btnAbort, SIGNAL(clicked()), SLOT(onAbortButtonClicked()));
	connect(m_op, SIGNAL(finished()), SLOT(onAsyncOpFinished()));
}

void InternalWaitPopup::retranslateUI()
{
	setCaption(ttt("general.wait.title"));
	m_btnAbort->setText(ttt("abort"));
	if (!m_promptId.isEmpty()) {
		m_promptLabel->setText(ttt(m_promptId.data()));
	}
}

void InternalWaitPopup::onAsyncOpFinished()
{
	emit internalClose(m_op, true);
	pop();
}

void InternalWaitPopup::onAbortButtonClicked()
{
	m_op->abort();
	emit internalClose(m_op, false);
	pop();
}

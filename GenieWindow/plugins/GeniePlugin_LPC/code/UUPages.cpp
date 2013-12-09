#include "UUPages.h"
#include <QtGui/QKeyEvent>
#include <QtGui/QMovie>

namespace LPCUserUtility {

StartupPage::StartupPage(QWidget *parent)
	: NavPage(parent)
{
}

StartupPage::~StartupPage()
{
}

void StartupPage::onLoad()
{
	m_movieLabel = qFindChild<QLabel*>(this, QString::fromUtf8("movieLabel"));
	m_statusLabel = qFindChild<QLabel*>(this, QString::fromUtf8("statusLabel"));
	m_errorLabel = qFindChild<QLabel*>(this, QString::fromUtf8("errorLabel"));

	m_movie = new QMovie(QString::fromUtf8(":/LPCModule/images/icon_working.gif"), "gif", m_movieLabel);
	m_movieLabel->setMovie(m_movie);
	m_movie->start();

	m_errorLabel->hide();

	setLPCText(m_statusLabel, "Status: detecting the router");

	m_op = CoreApi::beginDetectRouter(nam(), this);
	connect(m_op, SIGNAL(finished()), SLOT(onDetectRouterFinished()));
}

void StartupPage::onDetectRouterFinished()
{
	m_op->deleteLater();
	int routerError = m_op->property("UURouterError").toInt();
	if (routerError == UURouterError_NoError) {
		appData()->m_mac = m_op->property("varMac").toByteArray();

		if (appData()->m_routerUsername.isEmpty()) {
			appData()->m_routerUsername = AppData::DEFAULT_ROUTER_USERNAME;
			if (appData()->m_routerPassword.isEmpty()) {
				appData()->m_routerPassword = AppData::DEFAULT_ROUTER_PASSWORD;
			}
		}

		if (appData()->m_soapAction.isEmpty()) {
			m_op = CoreApi::beginAutoRouterAuthenticate(appData()->m_routerUsername, appData()->m_routerPassword, nam(), this);
		} else {
			m_op = RouterApi::beginAuthenticate(appData()->m_routerUsername, appData()->m_routerPassword, appData()->m_soapAction, nam(), this);
		}
		connect(m_op, SIGNAL(finished()), SLOT(onRouterAuthFinished()));
	} else {
		handleRouterError(routerError);
	}
}

void StartupPage::handleRouterError(int error)
{
	switch (error) {
	case UURouterError_NoRouter:
		displayError("Error: could not detect the router", UUWebsiteError_NoRouter, true);
		break;
	case UURouterError_NoNetwork:
		displayError("Error: Internet connection not found", UUWebsiteError_NoNetwork, true);
		break;
	case UURouterError_ParentalControlNotEnabled:
		displayError("Error: Parental Controls not enabled on this router", UUWebsiteError_ParentalControlNotEnabled, true);
		break;
	case UURouterError_NoDefaultDeviceId:
		displayError("Error: We have detected a router change. Please restart the User Utility", UUWebsiteError_NoDefaultDeviceId, true);
		break;
	case UURouterError_InvalidRouterPassword:
		navigateTo("RouterPassword");
		break;
	case UURouterError_Unknown:
	default:
		navigateTo("StartupLongError");
		break;
    }
}

void StartupPage::displayError(const char *msg, int webError, bool stopProgress)
{
	if (webError == UUWebsiteError_NoError) {
		// TODO: color
		m_errorLabel->hide();
	} else {
		QByteArray url;
		url.append("http://netgear.opendns.com/support/switch/error");
		url.append(QString::fromUtf8("%1").arg(webError).toUtf8());
		m_errorLabel->setProperty("LPCTextURL", QByteArray(url));
		setLPCText(m_errorLabel, "Learn more about this error", false);
		m_errorLabel->show();
	}
	setLPCText(m_statusLabel, msg);
	if (stopProgress) {
		m_movie->stop();
		m_movieLabel->hide();
	} else {
		m_movie->start();
		m_movieLabel->show();
	}
}

void StartupPage::onRouterAuthFinished()
{
	m_op->deleteLater();
	int routerError = m_op->property("UURouterError").toInt();
	if (routerError == UURouterError_NoError) {
		QVariant varSoapAction = m_op->property("varSoapAction");
		if (varSoapAction.isValid()) {
			appData()->m_soapAction = varSoapAction.toString();
		}
		m_op = CoreApi::beginWrappedGetParentalControlEnableStatus(appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onGetLPCEnableStatusFinished()));
	} else {
		handleRouterError(routerError);
	}
}

void StartupPage::onGetLPCEnableStatusFinished()
{
	m_op->deleteLater();
	QVariant varParentalControlStatus = m_op->property("varParentalControlStatus");
	if (varParentalControlStatus.isValid() && varParentalControlStatus.toBool()) {
		m_op = CoreApi::beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"), appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onGetDefaultIdFinished()));
	} else {
		handleRouterError(UURouterError_ParentalControlNotEnabled);
	}
}

void StartupPage::onGetDefaultIdFinished()
{
	m_op->deleteLater();
	QString parentDeviceID = m_op->property("varDeviceID").toString();
	if (!parentDeviceID.isEmpty()) {
		appData()->setParentDeviceId(parentDeviceID);
		m_op = CoreApi::beginWrappedGetDNSMasqDeviceID(appData()->m_mac, appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onGetChildIdFinished()));
	} else {
		handleRouterError(UURouterError_NoDefaultDeviceId);
	}
}

void StartupPage::onGetChildIdFinished()
{
	m_op->deleteLater();
	appData()->setChildDeviceId(m_op->property("varDeviceID").toString());

	displayError("Status: downloading accounts information", UUWebsiteError_NoError, false);

	m_op = WebApi::beginGetUserForChildDeviceId(appData()->childDeviceId(), nam(), this);
	connect(m_op, SIGNAL(finished()), SLOT(onGetUserForChildFinished()));
}

void StartupPage::onGetUserForChildFinished()
{
	m_op->deleteLater();
	appData()->setUserName(m_op->property("varUserName").toString());

	m_op = WebApi::beginGetUsersForDeviceId(appData()->parentDeviceId(), nam(), this);
	connect(m_op, SIGNAL(finished()), SLOT(onGetUsersFinished()));
}

void StartupPage::onGetUsersFinished()
{
	m_op->deleteLater();
	if (m_op->result().toInt() == WTFStatus_NoError) {
		QStringList userList = m_op->property("varList").toStringList();
		if (!userList.isEmpty()) {
			appData()->m_userList = userList;

			if (appData()->userName().isEmpty()) {
				navigateTo("Main");
			} else {
				App::instance()->setLoginState(true);
				navigateTo("Status");
			}
		} else {
			displayError("Error: no user accounts configured for this device", UUWebsiteError_NoUserAccountsConfigured, true);
		}
	} else {
		displayError("Error: couldn't download information about user accounts", UUWebsiteError_FailedDownloadUserAccounts, true);
	}
}

InfoNoticePage::InfoNoticePage(QWidget *parent)
	: NavPage(parent)
{
}

InfoNoticePage::~InfoNoticePage()
{
}

void InfoNoticePage::onLoad()
{
	m_movieLabel = qFindChild<QLabel*>(this, QString::fromUtf8("movieLabel"));
	m_statusLabel = qFindChild<QLabel*>(this, QString::fromUtf8("statusLabel"));

	m_movie = new QMovie(QString::fromUtf8(":/LPCModule/images/icon_working.gif"), "gif", m_movieLabel);
	m_movieLabel->setMovie(m_movie);
	m_movie->start();

	switch (appData()->m_opMode) {
	case AppData::Mode_LogIn:
		loginAction();
		break;
	case AppData::Mode_LogOut:
		logoutAction();
		break;
	}
}

void InfoNoticePage::loginAction()
{
	setLPCText(m_statusLabel, "Status: logging in...");
	m_op = WebApi::beginGetDeviceChild(appData()->parentDeviceId(), appData()->userName(), appData()->m_webPassword, nam(), this);
	connect(m_op, SIGNAL(finished()), SLOT(onWebLoginFinished()));
}

void InfoNoticePage::onWebLoginFinished()
{
	m_op->deleteLater();
	int result = m_op->result().toInt();
	if (result == WTFStatus_NoError) {
		appData()->setChildDeviceId(m_op->property("varChildDeviceId").toString());
		m_op = RouterApi::beginAuthenticate(appData()->m_routerUsername, appData()->m_routerPassword, appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onAuthRouterFinished()));
	} else if (result == WTFStatus_AuthenticationFailed) {
		App::instance()->showMessage(ttt("You have entered an incorrect password for user %1. Please try again.").arg(appData()->userName()));
		navigateTo("Main");
	} else {
		App::instance()->showMessage(ttt("Failed to obtain user accounts."));
		navigateTo("Main");
	}
}

void InfoNoticePage::onAuthRouterFinished()
{
	m_op->deleteLater();
	int result = m_op->result().toInt();
	if (result == WTFStatus_NoError) {
		m_op = CoreApi::beginWrappedSetDNSMasqDeviceID(appData()->m_mac, appData()->childDeviceId(), appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onSetMasqIDFinished()));
	} else {
		App::instance()->showMessage(ttt("Failed to associate with router. Please reconnect your router and/or restart the User Utility"));
	}
}

void InfoNoticePage::onSetMasqIDFinished()
{
	m_op->deleteLater();
	int result = m_op->result().toInt();
	if (result == WTFStatus_NoError) {
		App::instance()->setLoginState(true);
		//App::instance()->showMessage(ttt("You've logged in succesfully as %1").arg(appData()->userName()));
		//deactivateUI();
		navigateTo("Status");
	} else {
		App::instance()->showMessage(ttt("Failed to associate with router. Please reconnect your router and/or restart the User Utility"));
	}
}

void InfoNoticePage::logoutAction()
{
	setLPCText(m_statusLabel, "Status: logging out...");
	m_op = RouterApi::beginAuthenticate(appData()->m_routerUsername, appData()->m_routerPassword, appData()->m_soapAction, nam(), this);
	connect(m_op, SIGNAL(finished()), SLOT(onAuthRouterFinished2()));
}

void InfoNoticePage::onAuthRouterFinished2()
{
	m_op->deleteLater();
	int result = m_op->result().toInt();
	if (result == WTFStatus_NoError) {
		m_op = CoreApi::beginWrappedDeleteMacAddress(appData()->m_mac, appData()->m_soapAction, nam(), this);
		connect(m_op, SIGNAL(finished()), SLOT(onDeleteMacFinished()));
	} else {
		navigateTo("StartupLongError");
	}
}

void InfoNoticePage::onDeleteMacFinished()
{
	m_op->deleteLater();
	int result = m_op->result().toInt();
	if (result == WTFStatus_NoError) {
		App::instance()->setLoginState(false);
		navigateTo("Status");
	} else {
		navigateTo("StartupLongError");
	}
}

MainPage::MainPage(QWidget *parent)
	: NavPage(parent)
{
}

MainPage::~MainPage()
{
}

void MainPage::onLoad()
{
	m_accountList = qFindChild<QListWidget*>(this, QString::fromUtf8("accountList"));
	m_passwordEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("passwordEdit"));
	m_loginButton = qFindChild<QPushButton*>(this, QString::fromUtf8("loginButton"));

	m_accountList->addItems(appData()->m_userList);

	connect(m_accountList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
	m_loginButton->setEnabled(false);

	if (m_accountList->count() > 0) {
		m_accountList->setCurrentRow(0);
	}

	m_passwordEdit->setFocus();
}

void MainPage::doLogin()
{
	appData()->setUserName(m_accountList->currentItem()->text());
	appData()->m_webPassword = m_passwordEdit->text();
	appData()->m_opMode = AppData::Mode_LogIn;
	navigateTo("InfoNotice");
}

void MainPage::onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	m_loginButton->setEnabled(current != NULL);
}

RouterPasswordPage::RouterPasswordPage(QWidget *parent)
	: NavPage(parent)
{
}

RouterPasswordPage::~RouterPasswordPage()
{
}

void RouterPasswordPage::onLoad()
{
	m_usernameEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("usernameEdit"));
	m_passwordEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("passwordEdit"));
	m_okButton = qFindChild<QPushButton*>(this, QString::fromUtf8("okButton"));

	connect(m_usernameEdit, SIGNAL(textChanged(const QString&)), SLOT(onTextChanged(const QString&)));
	connect(m_passwordEdit, SIGNAL(textChanged(const QString&)), SLOT(onTextChanged(const QString&)));

	m_usernameEdit->setText(appData()->m_routerUsername);
}

void RouterPasswordPage::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
		if (focusWidget() == m_usernameEdit) {
			m_passwordEdit->setFocus();
			return;
		}

		if (focusWidget() == m_passwordEdit) {
			m_okButton->click();
			return;
		}
	}

	return NavPage::keyPressEvent(e);
}

void RouterPasswordPage::nextPage()
{
	appData()->m_routerUsername = m_usernameEdit->text();
	appData()->m_routerPassword = m_passwordEdit->text();
	navigateTo("Startup");
}

void RouterPasswordPage::onTextChanged(const QString& text)
{
	m_okButton->setDisabled(m_usernameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty());
}

StatusPage::StatusPage(QWidget *parent)
	: NavPage(parent)
{
}

StatusPage::~StatusPage()
{
}

void StatusPage::onLoad()
{
	m_statusLabel = qFindChild<QLabel*>(this, QString::fromUtf8("statusLabel"));
	m_actionButton = qFindChild<QPushButton*>(this, QString::fromUtf8("actionButton"));

	if (appData()->m_loggedIn) {
		setLPCText(m_statusLabel, "You're logged in.");
		m_statusLabel->setText(ttt("You're logged in as %1.").arg(appData()->userName()));
		setLPCText(m_actionButton, "Log out");
	} else {
		setLPCText(m_statusLabel, "You are not logged in.");
		setLPCText(m_actionButton, "Login");
	}
}

void StatusPage::retranslateUI()
{
	NavPage::retranslateUI();
	if (appData()->m_loggedIn) {
		m_statusLabel->setText(ttt("You're logged in as %1.").arg(appData()->userName()));
	}
}

void StatusPage::onPageAction()
{
	if (appData()->m_loggedIn) {
		appData()->m_opMode = AppData::Mode_LogOut;
		navigateTo("InfoNotice");
	} else {
		navigateTo("Main");
	}
}

StartupLongErrorPage::StartupLongErrorPage(QWidget *parent)
	: NavPage(parent)
{
}

StartupLongErrorPage::~StartupLongErrorPage()
{
}

} // namespace LPCUserUtility

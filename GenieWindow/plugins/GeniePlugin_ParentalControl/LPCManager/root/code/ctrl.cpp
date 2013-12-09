#include "app.h"
#include "common.h"
#include "coreapi.h"
#include "ctrl.h"
#include "ext.h"
#include "logger.h"
#include "webapi.h"
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRegExp>
#include <QUrl>
#include <QVBoxLayout>

QString markupLink(const QString& text)
{
	QString result = text;
	result.replace(QRegExp(QString::fromUtf8("(http://[^ ]+)([ .,!?])")), QString::fromUtf8("<a href=\"\\1\">\\1</a>\\2"));
	return result;
}

void updateStyleClass(QWidget *widget, const QString& styleClass)
{
	widget->setProperty("class", styleClass);
	widget->setStyleSheet(QString::fromUtf8(".empty {  }"));
}

RouterLoginDialog::RouterLoginDialog(const QString& username, const QString& password, QWidget *parent)
: QDialog(parent)
{
	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

	QLabel *prompt = new QLabel(tr("routerlogin.prompt"));
	layout0->addWidget(prompt);

	QFormLayout *layout1 = new QFormLayout();
	layout0->addLayout(layout1);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	layout0->addWidget(buttonBox);

	m_usernameEdit = new QLineEdit(username);
	layout1->addRow(tr("routerlogin.username"), m_usernameEdit);

	m_passwordEdit = new QLineEdit(password);
	m_passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
	layout1->addRow(tr("routerlogin.password"), m_passwordEdit);

	setWindowTitle(tr("routerlogin.title"));

	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

	m_usernameEdit->setFocus();
}

RouterLoginDialog::~RouterLoginDialog()
{
}

void RouterLoginDialog::accept()
{
	m_username = m_usernameEdit->text();
	m_password = m_passwordEdit->text();
	QDialog::accept();
}

Page::Page(QWidget *parent)
: QFrame(parent)
{
}

Page::~Page()
{
}

void Page::keyPressEvent(QKeyEvent *e)
{
	Qt::KeyboardModifiers modifiers = e->modifiers();
	if (!modifiers || (((modifiers & Qt::KeypadModifier) && e->key() == Qt::Key_Enter))) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("keyPress: %1 %2").arg(modifiers).arg(e->key()));
		switch (e->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			{
				QList<QPushButton*> ls = qFindChildren<QPushButton*>(this);
				for (int i = 0; i < ls.size(); i++) {
					QPushButton *pb = ls.at(i);
					if (pb->isDefault() && pb->isVisible()) {
						DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("keyPress: found default button"));
						if (pb->isEnabled()) {
							DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("keyPress: clicked!"));
							pb->click();
						}
						return;
					}
				}
			}
			break;
		default:
			e->ignore();
		}
		return;
	}

	QFrame::keyPressEvent(e);
}

void Page::setVisible(bool visible)
{
	QFrame::setVisible(visible);
	if (visible) {
		QWidget *next = qFindChild<QWidget*>(this, QString::fromUtf8("next"));
		if (next) {
			next->setFocus();
		}
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

void Page::registerPages(CustomUILoader *loader)
{
#define INTERNAL_REGISTER_PAGE(page) loader->registerFactory(QString::fromUtf8(#page), createPage<page>)

	INTERNAL_REGISTER_PAGE(FirmwarePage);
	INTERNAL_REGISTER_PAGE(IntroPage);
	INTERNAL_REGISTER_PAGE(DisclaimerPage);
	INTERNAL_REGISTER_PAGE(PreSignInPage);
	INTERNAL_REGISTER_PAGE(SignInPage);
	INTERNAL_REGISTER_PAGE(CreateAccountPage);
	INTERNAL_REGISTER_PAGE(DonePage);
	INTERNAL_REGISTER_PAGE(ManagePage);
	INTERNAL_REGISTER_PAGE(SettingsPage);
}

void Page::navigateTo(const char *pageName)
{
	app()->requestNavigateTo(QString::fromUtf8(pageName));
}

void Page::gotoMainPage()
{
	navigateTo("manage");
}

bool Page::prepare()
{
	QPushButton *abortBtn = qFindChild<QPushButton*>(this, QString::fromUtf8("abort"));
	if (abortBtn) {
		abortBtn->setVisible(appData()->needShowAbortButton());
	}
	return prepareImpl();
}

FirmwarePage::FirmwarePage(QWidget *parent)
: Page(parent)
{
	connect(this, SIGNAL(internalCheck()), SLOT(check()), Qt::QueuedConnection);
}

FirmwarePage::~FirmwarePage()
{
}

bool FirmwarePage::prepareImpl()
{
	m_error1 = qFindChild<QLabel*>(this, QString::fromUtf8("error1"));
	m_tryagain = qFindChild<QPushButton*>(this, QString::fromUtf8("tryagain"));

	if (!m_error1 || !m_tryagain) {
		return false;
	}

	m_error1->setVisible(false);
	m_tryagain->setVisible(false);
	return true;
}

bool FirmwarePage::loadImpl()
{
	check();
	return true;
}

void FirmwarePage::quit()
{
	app()->quit();
}

void FirmwarePage::check()
{
	AsyncOp *op = CoreApi::beginCheckRouterStatus(appData()->m_routerUsername, appData()->m_routerPassword, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("device.router.init"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		m_error1->setVisible(false);
		appData()->m_soapAction = op->property("varSoapAction").toString();
		if (appData()->needSetup()) {
			navigateTo("intro");
		} else {
			navigateTo("manage");
		}
		break;
	case RouterStatus_NoNetwork:
		m_error1->setText(markupLink(tr("firmware.error.internet")));
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		break;
	case RouterStatus_NoRouter:
		m_error1->setText(markupLink(tr("firmware.error.netgear")));
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		break;
	case RouterStatus_ParentalControlNotEnabled:
		m_error1->setText(markupLink(tr("firmware.error.support")));
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		break;
	case WTFStatus_AuthenticationFailed:
		{
			m_error1->setVisible(false);
			RouterLoginDialog dlg(appData()->m_routerUsername, appData()->m_routerPassword, this);
			if (QDialog::Accepted == dlg.exec()) {
				appData()->m_routerUsername = dlg.username();
				appData()->m_routerPassword = dlg.password();
				emit internalCheck();
			} else {
				m_error1->setText(markupLink(tr("firmware.error.netgear")));
				m_error1->setVisible(true);
				m_tryagain->setVisible(true);
				m_tryagain->setFocus();
			}
		}
		break;
	default:
		m_error1->setText(markupLink(tr("firmware.error.internet")));
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		break;
	}
}

IntroPage::IntroPage(QWidget *parent)
: Page(parent)
{
}

IntroPage::~IntroPage()
{
}

bool IntroPage::prepareImpl()
{
	QLabel *intro2 = qFindChild<QLabel*>(this, QString::fromUtf8("intro2"));

	if (!intro2) {
		return false;
	}

	QString introText = tr("intro.text.2");
	introText.append(QString::fromUtf8(" <a href=\"http://netgear.opendns.com/support/manage/windows\">"));
	introText.append(tr("intro.learnmore"));
	introText.append(QString::fromUtf8("</a>"));
	intro2->setText(introText);

	return true;
}

void IntroPage::nextPage()
{
	navigateTo("disclaimer");
}

DisclaimerPage::DisclaimerPage(QWidget *parent)
: Page(parent)
{
}

DisclaimerPage::~DisclaimerPage()
{
}

void DisclaimerPage::prevPage()
{
	navigateTo("intro");
}

void DisclaimerPage::nextPage()
{
	navigateTo("presignin");
}

PreSignInPage::PreSignInPage(QWidget *parent)
: Page(parent)
{
}

PreSignInPage::~PreSignInPage()
{
}

bool PreSignInPage::prepareImpl()
{
	m_existing = qFindChild<QRadioButton*>(this, QString::fromUtf8("existing"));
	if (!m_existing) {
		return false;
	}

	m_existing->setChecked(true);
	return true;
}

void PreSignInPage::prevPage()
{
	navigateTo("disclaimer");
}

void PreSignInPage::nextPage()
{
	if (m_existing->isChecked()) {
		navigateTo("signin");
	} else {
		navigateTo("createaccount");
	}
}

SignInPage::SignInPage(QWidget *parent)
: Page(parent)
{
	connect(this, SIGNAL(internalRefresh()), SLOT(nextPage()), Qt::QueuedConnection);
}

SignInPage::~SignInPage()
{
}

bool SignInPage::prepareImpl()
{
	m_usernameEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("username"));
	m_passwordEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("password"));
	m_errorLabel = qFindChild<QLabel*>(this, QString::fromUtf8("error"));
	m_forgotLabel = qFindChild<QLabel*>(this, QString::fromUtf8("forgot"));
	m_nextButton = qFindChild<QPushButton*>(this, QString::fromUtf8("next"));

	if (!m_usernameEdit || !m_passwordEdit || !m_errorLabel || !m_forgotLabel || !m_nextButton) {
		return false;
	}

	m_errorLabel->setVisible(false);
	QString forgotText;
	forgotText.append(QString::fromUtf8("<a href=\"http://www.opendns.com/dashboard/signin/\">"));
	forgotText.append(tr("signin.forgot"));
	forgotText.append(QString::fromUtf8("</a>"));
	m_forgotLabel->setText(forgotText);

	m_nextButton->setDisabled(true);

	m_usernameEdit->setText(appData()->m_username);

	connect(m_usernameEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_passwordEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	return true;
}

void SignInPage::prevPage()
{
	navigateTo("presignin");
}

void SignInPage::nextPage()
{
	//m_errorLabel->setVisible(false);

	AsyncOp *op = CoreApi::beginSignIn(m_usernameEdit->text(), m_passwordEdit->text(), appData()->m_routerUsername, appData()->m_routerPassword, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("signin.status"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		{
			m_errorLabel->setVisible(false);
			AppData *ad = appData();
			ad->m_username = m_usernameEdit->text();
			ad->m_password = m_passwordEdit->text();
			ad->m_userToken = op->property("varToken").toString();
			ad->m_deviceId = op->property("varDeviceID").toString();
			navigateTo("settings");
		}
		break;
	case WebApiStatus_RouterAuthenticationFailed:
		{
			RouterLoginDialog dlg(appData()->m_routerUsername, appData()->m_routerPassword, this);
			if (QDialog::Accepted == dlg.exec()) {
				appData()->m_routerUsername = dlg.username();
				appData()->m_routerPassword = dlg.password();
				emit internalRefresh();
			} else {
				m_errorLabel->setText(markupLink(tr("signin.error.failed")));
				m_errorLabel->setVisible(true);
				m_nextButton->setText(tr("tryagain"));
			}
		}
		break;
	case WebApiStatus_DeviceIdNotMine:
		m_errorLabel->setText(markupLink(tr("device.error.device_key")));
		m_errorLabel->setVisible(true);
		m_nextButton->setText(tr("tryagain"));
		break;
	case WTFStatus_AuthenticationFailed:
		m_errorLabel->setText(markupLink(tr("signin.error.signin")));
		m_errorLabel->setVisible(true);
		m_nextButton->setText(tr("tryagain"));
		break;
	default:
		m_errorLabel->setText(markupLink(tr("signin.error.failed")));
		m_errorLabel->setVisible(true);
		m_nextButton->setText(tr("tryagain"));
		break;
	}
}

void SignInPage::updateUI()
{
	m_nextButton->setDisabled(m_usernameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty());
}

CreateAccountPage::CreateAccountPage(QWidget *parent)
: Page(parent)
, m_usernameRE(QString::fromUtf8("^[a-z0-9][a-z0-9\\.\\-\\_]*[a-z0-9]$"), Qt::CaseInsensitive)
{
}

CreateAccountPage::~CreateAccountPage()
{
}

bool CreateAccountPage::prepareImpl()
{
	m_usernameEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("username"));
	m_passwordEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("password"));
	m_passwordConfirmEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("passwordConfirm"));
	m_emailEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("email"));
	m_emailConfirmEdit = qFindChild<QLineEdit*>(this, QString::fromUtf8("emailConfirm"));
	m_checkBtn = qFindChild<QPushButton*>(this, QString::fromUtf8("check"));
	m_nextBtn = qFindChild<QPushButton*>(this, QString::fromUtf8("next"));
	m_errorPrompt = qFindChild<QLabel*>(this, QString::fromUtf8("error"));
	m_usernamePrompt = qFindChild<QLabel*>(this, QString::fromUtf8("usernamePrompt"));
	m_passwordConfirmPrompt = qFindChild<QLabel*>(this, QString::fromUtf8("passwordConfirmPrompt"));
	m_emailPrompt = qFindChild<QLabel*>(this, QString::fromUtf8("emailPrompt"));
	m_emailConfirmPrompt = qFindChild<QLabel*>(this, QString::fromUtf8("emailConfirmPrompt"));

	if (!m_usernameEdit || !m_passwordEdit || !m_passwordConfirmEdit || !m_emailEdit || !m_emailConfirmEdit
		|| !m_checkBtn || !m_nextBtn || !m_errorPrompt || !m_usernamePrompt
		|| !m_passwordConfirmPrompt || !m_emailPrompt || !m_emailConfirmPrompt)
	{
		return false;
	}

	m_errorPrompt->setVisible(false);
	m_usernamePrompt->setVisible(false);
	m_passwordConfirmPrompt->setVisible(false);
	m_emailPrompt->setVisible(false);
	m_emailConfirmPrompt->setVisible(false);

	m_usernameEdit->setFocus();
	m_nextBtn->setDisabled(true);
	m_checkBtn->setDisabled(true);

	connect(m_usernameEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_passwordEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_passwordConfirmEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_emailEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_emailConfirmEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));

	return true;
}

void CreateAccountPage::prevPage()
{
	navigateTo("presignin");
}

void CreateAccountPage::nextPage()
{
	if (!validateInput()) {
		return;
	}

	AsyncOp *op = WebApi::beginCreateAccount(m_usernameEdit->text(), m_passwordEdit->text(), m_emailEdit->text(), app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("createaccount.status"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		navigateTo("signin");
		break;
	case WTFStatus_Failed:
		{
			QLineEdit *focus = NULL;
			int errorCode = op->property("varErrorCode").toInt();
			switch (errorCode) {
			case 3001:
				m_usernamePrompt->setText(markupLink(tr("createaccount.check.unavailable")));
				updateStyleClass(m_usernamePrompt, QString::fromUtf8("error"));
				m_usernamePrompt->setVisible(true);
				focus = m_usernameEdit;
				break;
			case 3006:
				m_errorPrompt->setText(markupLink(tr("createaccount.error.username")));
				m_errorPrompt->setVisible(true);
				focus = m_usernameEdit;
				break;
			case 3007:
				m_errorPrompt->setText(markupLink(tr("createaccount.error.password")));
				m_errorPrompt->setVisible(true);
				focus = m_passwordEdit;
				break;
			case 3004:
				m_emailPrompt->setText(markupLink(tr("createaccount.email.invalid")));
				m_emailPrompt->setVisible(true);
				focus = m_emailEdit;
				break;
			case 3005:
				m_emailPrompt->setText(markupLink(tr("createaccount.email.taken")));
				m_emailPrompt->setVisible(true);
				focus = m_emailEdit;
				break;
			default:
				m_errorPrompt->setText(markupLink(tr("createaccount.error")));
				m_errorPrompt->setVisible(true);
				break;
			}
			m_nextBtn->setText(tr("tryagain"));

			if (focus) {
				focus->selectAll();
				focus->setFocus();
				m_nextBtn->setDefault(true);
				m_nextBtn->setAutoDefault(true);
			}
		}
		break;
	default:
		m_errorPrompt->setText(markupLink(tr("createaccount.error")));
		m_errorPrompt->setVisible(true);
		m_nextBtn->setText(tr("tryagain"));
		break;
	}
}

void CreateAccountPage::checkAvailable()
{
	AsyncOp *op = WebApi::beginCheckNameAvailable(m_usernameEdit->text(), app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("general.status.commmicating"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		if (op->property("varAvailable").toBool()) {
			m_usernamePrompt->setText(markupLink(tr("createaccount.check.available")));
			updateStyleClass(m_usernamePrompt, QString::fromUtf8("success"));
			m_usernamePrompt->setVisible(true);
		} else {
			m_usernamePrompt->setText(markupLink(tr("createaccount.check.unavailable")));
			updateStyleClass(m_usernamePrompt, QString::fromUtf8("error"));
			m_usernamePrompt->setVisible(true);
		}
		break;
	default:
		break;
	}
}

void CreateAccountPage::updateUI()
{
	QString username = m_usernameEdit->text();
	QString password = m_passwordEdit->text();
	QString passwordConfirm = m_passwordConfirmEdit->text();
	QString email = m_emailEdit->text();
	QString emailConfirm = m_emailConfirmEdit->text();

	if (username.isEmpty() || password.isEmpty() || passwordConfirm.isEmpty() || email.isEmpty() || emailConfirm.isEmpty()) {
		m_nextBtn->setDisabled(true);
	} else {
		m_nextBtn->setDisabled(false);
	}

	m_checkBtn->setDisabled(!m_usernameRE.exactMatch(username));
}

bool CreateAccountPage::validateInput()
{
	QString username = m_usernameEdit->text();
	QString password = m_passwordEdit->text();
	QString passwordConfirm = m_passwordConfirmEdit->text();
	QString email = m_emailEdit->text();
	QString emailConfirm = m_emailConfirmEdit->text();

	bool u = !m_usernameRE.exactMatch(username);
	bool p1 = password.length() < 6 || password.length() > 20;
	bool p2 = password != passwordConfirm;

	bool ok1 = false;

	QLineEdit *focus = NULL;

	if (!u && !p1 && !p2) {
		ok1 = true;
	} else if (!u && !p1 && p2) {
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_passwordConfirmEdit;
	} else if (!u && p1 && !p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		focus = m_passwordEdit;
	} else if (!u && p1 && p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_passwordEdit;
	} else if (u && !p1 && !p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.username")));
		m_errorPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && !p1 && p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.username")));
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && p1 && !p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.username") + QString::fromUtf8(" ") + tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && p1 && p2) {
		m_errorPrompt->setText(markupLink(tr("createaccount.error.username") + QString::fromUtf8(" ") + tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_usernameEdit;
    }

	bool ok2 = false;

	if (email.indexOf(QChar::fromAscii('@')) < 0) {
		m_emailPrompt->setText(markupLink(tr("createaccount.email.invalid")));
		m_emailPrompt->setVisible(true);
		if (!focus) focus = m_emailEdit;
	} else {
		m_emailPrompt->setVisible(false);
		if (email != emailConfirm) {
			m_emailConfirmPrompt->setText(markupLink(tr("createaccount.email.confirm")));
			m_emailConfirmPrompt->setVisible(true);
			if (!focus) focus = m_emailConfirmEdit;
		} else {
			ok2 = true;
		}
	}

	if (ok1) {
		m_errorPrompt->setVisible(false);
		m_usernamePrompt->setVisible(false);
		m_passwordConfirmPrompt->setVisible(false);
	}

	if (ok2) {
		m_emailPrompt->setVisible(false);
		m_emailConfirmPrompt->setVisible(false);
	}

	if (focus) {
		focus->selectAll();
		focus->setFocus();
		m_nextBtn->setDefault(true);
		m_nextBtn->setAutoDefault(true);
	}

	return ok1 && ok2;
}

DonePage::DonePage(QWidget *parent)
: Page(parent)
{
}

DonePage::~DonePage()
{
}

ManagePage::ManagePage(QWidget *parent)
: Page(parent)
{
}

ManagePage::~ManagePage()
{
}

bool ManagePage::prepareImpl()
{
	appData()->m_inChangeBasicSettings = false;

	m_errorLabel = qFindChild<QLabel*>(this, QString::fromUtf8("error"));
	m_bundle = qFindChild<QLabel*>(this, QString::fromUtf8("bundle"));
	m_bundleDetail = qFindChild<QLabel*>(this, QString::fromUtf8("bundleDetail"));
	m_account = qFindChild<QLabel*>(this, QString::fromUtf8("account"));
	m_lpcStatus = qFindChild<QLabel*>(this, QString::fromUtf8("lpcStatus"));
	m_toggleButton = qFindChild<QPushButton*>(this, QString::fromUtf8("toggleButton"));
	m_changeButton = qFindChild<QPushButton*>(this, QString::fromUtf8("changeButton"));
	m_settingsButton = qFindChild<QPushButton*>(this, QString::fromUtf8("settingsButton"));

	if (!m_errorLabel || !m_bundle || !m_bundleDetail || !m_account || !m_lpcStatus || !m_toggleButton || !m_changeButton || !m_settingsButton) {
		return false;
	}

	m_errorLabel->setVisible(false);

	return true;
}

bool ManagePage::loadImpl()
{
	AsyncOp *op = CoreApi::beginQuerySettings(appData()->m_userToken, appData()->m_deviceId, appData()->m_routerUsername, appData()->m_routerPassword, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("manage.status.checking"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		{
			m_errorLabel->setVisible(false);
			QByteArray bundle = op->property("varBundle").toString().toUtf8().toLower();
			QByteArray lbl;
			if (bundle == "custom") {
				lbl = "manage.settings.bundle.custom.label";
			} else {
				lbl = "settings.bundle." + bundle;
			}
			QByteArray lblDetail = QByteArray("manage.settings.bundle.") + bundle;
			m_bundle->setText(tr(lbl));
			m_bundleDetail->setText(tr(lblDetail));
			updateStyleClass(m_bundle, QString::fromUtf8("settings-bundle-%1").arg(QString::fromUtf8(bundle)));

			updateLPCStatus(op->property("varParentalControlStatus").toBool());

			m_account->setText(appData()->m_username);
		}
		break;
	case WebApiStatus_DeviceIdNotMine:
		// TODO:
		m_errorLabel->setVisible(false);
		QMessageBox::information(this, tr("manage.title"), tr("device.error.device_id_not_mine"));
		navigateTo("intro");
		break;
	default:
		m_errorLabel->setVisible(true);
		m_errorLabel->setText(markupLink(tr("device.error.netgear")));
		break;
	}

	enableUI(code == WTFStatus_NoError);

	return true;
}

void ManagePage::enableUI(bool enable)
{
	m_toggleButton->setDisabled(!enable);
	m_changeButton->setDisabled(!enable);
	m_settingsButton->setDisabled(!enable);
}

void ManagePage::updateLPCStatus(bool lpcEnabled)
{
	if (lpcEnabled) {
		m_lpcStatus->setText(tr("manage.parentalcontrols.enabled"));
		updateStyleClass(m_lpcStatus, QString::fromUtf8("success"));
		m_toggleButton->setText(tr("manage.toggle.enabled"));
	} else {
		m_lpcStatus->setText(tr("manage.parentalcontrols.disabled"));
		updateStyleClass(m_lpcStatus, QString::fromUtf8("error"));
		m_toggleButton->setText(tr("manage.toggle.disabled"));
	}

	m_lpcEnabled = lpcEnabled;
}

void ManagePage::toggleLPC()
{
	AsyncOp *op = CoreApi::beginToggleParentalControl(!m_lpcEnabled, appData()->m_deviceId, appData()->m_soapAction, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr(m_lpcEnabled ? "manage.status.disabling" : "manage.status.enabling"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		{
			updateLPCStatus(op->property("varParentalControlStatus").toBool());
		}
		break;
	default:
		m_errorLabel->setVisible(true);
		m_errorLabel->setText(markupLink(tr("device.error.netgear")));
		break;
	}
}

void ManagePage::changeSettings()
{
	appData()->m_inChangeBasicSettings = true;
	navigateTo("signin");
}

void ManagePage::changeCustomSettings()
{
	AsyncOp *op = WebApi::beginAccountRelay(appData()->m_userToken, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("general.status.commmicating"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		{
			QString relayToken = op->property("varRelayToken").toString();
			QString apiKey = op->property("varApiKey").toString();
			QString baseUrl = op->property("varBaseUrl").toString();
			QString url = QString::fromUtf8("%1?device_id=%2&api_key=%3&relay_token=%4").arg(baseUrl).arg(appData()->m_deviceId).arg(apiKey).arg(relayToken);
			QDesktopServices::openUrl(QUrl(url));
		}
		break;
	default:
		break;
	}
}

SettingsPage::SettingsPage(QWidget *parent)
: Page(parent), m_currentLevel(-1), m_loadOk(false)
{
}

SettingsPage::~SettingsPage()
{
}

bool SettingsPage::prepareImpl()
{
	for (int i = 0; i < 5; i++) {
		m_levels[i] = qFindChild<QRadioButton*>(this, QString::fromUtf8("level%1").arg(i+1));
		if (!m_levels[i]) {
			return false;
		}
	}

	m_errorPrompt = qFindChild<QLabel*>(this, QString::fromUtf8("error"));
	m_nextBtn = qFindChild<QPushButton*>(this, QString::fromUtf8("next"));

	if (!m_errorPrompt || !m_nextBtn) {
		return false;
	}

	m_errorPrompt->setVisible(false);
	return true;
}

bool SettingsPage::loadImpl()
{
	loadValues();
	return true;
}

static const char *BUNDLE_NAMES[] = {
	"High", "Moderate", "Low", "Minimal", "None"
};

void SettingsPage::loadValues()
{
	AsyncOp *op = WebApi::beginGetFilters(appData()->m_userToken, appData()->m_deviceId, app()->networkAccessManager());
	bool waitOk = app()->wait(op, tr("settings.status.get"));
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}

	switch (code) {
	case WTFStatus_NoError:
		{
			m_errorPrompt->setVisible(false);
			QByteArray bundle = op->property("varBundle").toString().toLower().toUtf8();
			int index;
			if (bundle == "high") {
				index = 0;
			} else if (bundle == "moderate") {
				index = 1;
			} else if (bundle == "low") {
				index = 2;
			} else if (bundle == "minimal") {
				index = 3;
			} else {
				index = 4;
			}
			m_levels[index]->setChecked(true);
			m_loadOk = true;
			m_currentLevel = index;
			m_nextBtn->setText(tr("next"));
		}
		break;
	default:
		m_errorPrompt->setVisible(true);
		m_loadOk = false;
		m_nextBtn->setText(tr("tryagain"));
		break;
	}
}

void SettingsPage::prevPage()
{
	navigateTo("signin");
}

void SettingsPage::nextPage()
{
	if (!m_loadOk) {
		loadValues();
	} else {
		int sel = -1;
		for (int i = 0; i < 5; i++) {
			if (m_levels[i]->isChecked()) {
				sel = i;
				break;
			}
		}

		if (sel >= 0 && sel != m_currentLevel) {
			AsyncOp *op = CoreApi::beginSetFiltersAndEnableParentalControl(appData()->m_soapAction, appData()->m_userToken, appData()->m_deviceId, QString::fromUtf8(BUNDLE_NAMES[sel]), app()->networkAccessManager());
			bool waitOk = app()->wait(op, tr("settings.status.set"));
			op->deleteLater();
			int code;
			if (waitOk) {
				code = op->result().toInt();
			} else {
				// aborted
				code = WTFStatus_Aborted;
			}

			switch (code) {
			case WTFStatus_NoError:
				m_errorPrompt->setVisible(false);
				if (appData()->m_inChangeBasicSettings) {
					gotoMainPage();
				} else {
					navigateTo("done");
				}
				break;
			default:
				m_errorPrompt->setVisible(true);
				m_nextBtn->setText(tr("tryagain"));
				break;
			}
		} else {
			navigateTo("done");
		}
	}
}


#include "AsyncOp.h"
#include "coreapi.h"
#include "common.h"
#include "logger.h"
#include "pages.h"
#include "ext.h"
#include "LPCMainFrame.h"
#include "webapi.h"
#include <QUrl>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <qdebug.h>

QString m_LocalMACAddr; // add by qi
QString BYPASSUSERNAME;  //yankai add
QString SERIALNUM;
bool ISLOGINBYPASS = false;   //yankai add
template <class T>
QWidget *createPage(const QString& className, QWidget *parent, const QString& name)
{
	Q_UNUSED(className)
	T *w = new T(parent);
        w->setObjectName(name);qDebug()<<"**createPage--className:"<<className<<"setObjectName:"<<name;
	return w;
}

#define INTERNAL_REGISTER_PAGE(page) loader->registerFactory(QString::fromUtf8(#page), createPage<page>)

void LPCRegisterPages(CustomUILoader *loader)
{
	INTERNAL_REGISTER_PAGE(FirmwarePage);
	INTERNAL_REGISTER_PAGE(IntroPage);
	INTERNAL_REGISTER_PAGE(DisclaimerPage);
	INTERNAL_REGISTER_PAGE(PreSignInPage);
	INTERNAL_REGISTER_PAGE(SignInPage);
	INTERNAL_REGISTER_PAGE(CreateAccountPage);
	INTERNAL_REGISTER_PAGE(DonePage);
	INTERNAL_REGISTER_PAGE(ManagePage);
        INTERNAL_REGISTER_PAGE(BypassUserList); //20121129 yankai add
        //INTERNAL_REGISTER_PAGE(BypassUserLogin); //20121129 yankai add
	INTERNAL_REGISTER_PAGE(SettingsPage);

}

RouterLoginPopup::RouterLoginPopup(const QString& username, const QString& password, const QObject *receiver, const char *method)
: LPCNavPopup(NULL)
{
	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

	m_promptLabel = new QLabel();
	// force word wrap
	m_promptLabel->setWordWrap(true);
	layout0->addWidget(m_promptLabel);

	QFormLayout *layout1 = new QFormLayout();
	layout0->addLayout(layout1);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	layout0->addWidget(buttonBox);

	m_usernameEdit = new QLineEdit(username);
	layout1->addRow(new QLabel(), m_usernameEdit);
	m_usernameLabel = qobject_cast<QLabel*>(layout1->labelForField(m_usernameEdit));

	m_passwordEdit = new QLineEdit(password);
	m_passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
	layout1->addRow(new QLabel(), m_passwordEdit);
	m_passwordLabel = qobject_cast<QLabel*>(layout1->labelForField(m_passwordEdit));

	connect(m_usernameEdit, SIGNAL(returnPressed()), m_passwordEdit, SLOT(setFocus()));
	connect(m_passwordEdit, SIGNAL(returnPressed()), SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	connect(this, SIGNAL(internalClose(RouterLoginPopup*,bool)), receiver, method);
}

RouterLoginPopup::~RouterLoginPopup()
{
}

void RouterLoginPopup::retranslateUI()
{
	setCaption(ttt("routerlogin.title"));
	m_promptLabel->setText(ttt("routerlogin.prompt"));
	m_usernameLabel->setText(ttt("routerlogin.username"));
	m_passwordLabel->setText(ttt("routerlogin.password"));
}

void RouterLoginPopup::onPopup()
{
	m_usernameEdit->setFocus();
}

void RouterLoginPopup::accept()
{
	m_username = m_usernameEdit->text();
	m_password = m_passwordEdit->text();
	emit internalClose(this, true);
	pop();
}

void RouterLoginPopup::reject()
{
	emit internalClose(this, false);
	pop();
}

MessagePopup::MessagePopup(const char *caption, const char *content, const QObject *receiver, const char *method)
: LPCNavPopup(NULL), m_caption(caption), m_content(content)
{
	QVBoxLayout *layout0 = new QVBoxLayout();
	setLayout(layout0);

	m_contentLabel = new QLabel();
	// force word wrap
	m_contentLabel->setWordWrap(true);
	layout0->addWidget(m_contentLabel);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	layout0->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(this, SIGNAL(internalClose(MessagePopup*,bool)), receiver, method);

}

MessagePopup::~MessagePopup()
{
}

void MessagePopup::retranslateUI()
{
	m_contentLabel->setText(ttt(m_content.data()));
	setCaption(ttt(m_caption.data()));
}

void MessagePopup::accept()
{
	emit internalClose(this, true);
	pop();
}

FirmwarePage::FirmwarePage(QWidget *parent)
: LPCNavPage(parent)
{
	connect(this, SIGNAL(internalCheck()), SLOT(check()), Qt::QueuedConnection);
}

FirmwarePage::~FirmwarePage()
{
}

bool FirmwarePage::prepareImpl()
{qDebug()<<"FirmwarePage::prepareImpl()";
    m_error1 = this->findChild<QLabel*>(QString::fromUtf8("error1"));
    m_tryagain = this->findChild<QPushButton*>(QString::fromUtf8("tryagain"));

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

void FirmwarePage::check()
{
	reportStatus(LPCSTATUS_CHECKING);
	coreApi()->setRouterUsername(appData()->m_routerUsername);
	coreApi()->setRouterPassword(appData()->m_routerPassword);
	AsyncOp *op = coreApi()->beginCheckRouterStatus(appData()->m_routerUsername, appData()->m_routerPassword);
	app()->wait(op, this, SLOT(aopCheck(AsyncOp*, bool)), "device.router.init");
}

void FirmwarePage::aopCheck(AsyncOp *op, bool waitOk)
{
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
		appData()->m_model = op->property("varModel").toString();
		if (appData()->needSetup(op->property("varDeviceID").toString())) {
			navigateTo("intro");
		} else {
			gotoMainPage();
		}
		break;
	case RouterStatus_NoNetwork:
		setLPCText(m_error1, "firmware.error.internet", true);
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		reportStatus(LPCSTATUS_ERROR);
		break;
	case RouterStatus_NoRouter:
		setLPCText(m_error1, "firmware.error.netgear", true);
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		reportStatus(LPCSTATUS_ERROR);
		break;
	case RouterStatus_ParentalControlNotEnabled:
		setLPCText(m_error1, "firmware.error.support", true);
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		reportStatus(LPCSTATUS_ERROR);
		break;
	case WTFStatus_AuthenticationFailed:
		{
			m_error1->setVisible(false);
			app()->updateAppDataRouterAuth();
			RouterLoginPopup *popup = new RouterLoginPopup(appData()->m_routerUsername, appData()->m_routerPassword, this, SLOT(authCheck(RouterLoginPopup*,bool)));
			app()->push(popup);
		}
		break;
	default:
		setLPCText(m_error1, "firmware.error.internet", true);
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		reportStatus(LPCSTATUS_ERROR);
		break;
	}
}

void FirmwarePage::authCheck(RouterLoginPopup *popup, bool accepted)
{
	if (accepted) {
		appData()->m_routerUsername = popup->username();
		appData()->m_routerPassword = popup->password();
		emit internalCheck();
	} else {
		setLPCText(m_error1, "firmware.error.netgear", true);
		m_error1->setVisible(true);
		m_tryagain->setVisible(true);
		m_tryagain->setFocus();
		reportStatus(LPCSTATUS_ERROR);
	}
}

IntroPage::IntroPage(QWidget *parent)
: LPCNavPage(parent)
{
}

IntroPage::~IntroPage()
{
}

bool IntroPage::prepareImpl()
{qDebug()<<"IntroPage::prepareImpl()";
    m_introPic = this->findChild<QLabel*>(QString::fromUtf8("introPicture"));

	if (!m_introPic) {
		return false;
	}

	const QString& model = appData()->m_model;
	if (model.startsWith(QString::fromUtf8("CG")) || model.startsWith(QString::fromUtf8("DG"))) {
		m_introPic->setPixmap(QPixmap(QString::fromUtf8(":/LPCModule/images/netgear_users_small_cg_dn.png")));
	}

	return true;
}

void IntroPage::retranslateUI()
{
	LPCNavPage::retranslateUI();

    QLabel *intro2 = this->findChild<QLabel*>(QString::fromUtf8("intro2"));
	if (intro2) {
		QString introText = ttt("intro.text.2");
		introText.append(QString::fromUtf8(" <a href=\"http://netgear.opendns.com/support/manage/windows\">"));
		introText.append(ttt("intro.learnmore"));
		introText.append(QString::fromUtf8("</a>"));
		intro2->setText(introText);
	}
}

void IntroPage::nextPage()
{
	navigateTo("disclaimer");
}

DisclaimerPage::DisclaimerPage(QWidget *parent)
: LPCNavPage(parent)
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
: LPCNavPage(parent)
{
}

PreSignInPage::~PreSignInPage()
{
}

bool PreSignInPage::prepareImpl()
{qDebug()<<"PreSignInPage::prepareImpl()";
    m_existing = this->findChild<QRadioButton*>(QString::fromUtf8("existing"));
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
: LPCNavPage(parent)
{

	connect(this, SIGNAL(internalRefresh()), SLOT(nextPage()), Qt::QueuedConnection);
}

SignInPage::~SignInPage()
{
}

void SignInPage::retranslateUI()
{
	LPCNavPage::retranslateUI();

    QLabel *forgotLabel = this->findChild<QLabel*>(QString::fromUtf8("forgot"));
	if (forgotLabel) {
		QString forgotText;
		forgotText.append(QString::fromUtf8("<a href=\"http://www.opendns.com/dashboard/signin/\">"));
		forgotText.append(ttt("signin.forgot"));
		forgotText.append(QString::fromUtf8("</a>"));
		forgotLabel->setText(forgotText);
	}
}

bool SignInPage::prepareImpl()
{qDebug()<<"SignInPage::prepareImpl()";
    m_usernameEdit = this->findChild<QLineEdit*>(QString::fromUtf8("username"));
    m_passwordEdit = this->findChild<QLineEdit*>(QString::fromUtf8("password"));
    m_errorLabel = this->findChild<QLabel*>(QString::fromUtf8("error"));
    m_forgotLabel = this->findChild<QLabel*>(QString::fromUtf8("forgot"));
    m_nextButton = this->findChild<QPushButton*>(QString::fromUtf8("next"));

	if (!m_usernameEdit || !m_passwordEdit || !m_errorLabel || !m_forgotLabel || !m_nextButton) {
		return false;
	}

	m_errorLabel->setVisible(false);
	m_nextButton->setDisabled(true);
	m_usernameEdit->setText(appData()->m_username);

	connect(m_usernameEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_usernameEdit, SIGNAL(returnPressed()), m_passwordEdit, SLOT(setFocus()));
	connect(m_passwordEdit, SIGNAL(textChanged(const QString&)), SLOT(updateUI()));
	connect(m_passwordEdit, SIGNAL(returnPressed()), m_nextButton, SLOT(click()));
	return true;
}

void SignInPage::prevPage()
{
	navigateTo("presignin");
}

void SignInPage::nextPage()
{
	coreApi()->setRouterUsername(appData()->m_routerUsername);
	coreApi()->setRouterPassword(appData()->m_routerPassword);
	AsyncOp *op = coreApi()->beginSignIn(m_usernameEdit->text(), m_passwordEdit->text(), appData()->m_routerUsername, appData()->m_routerPassword);
	app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "signin.status");
}

void SignInPage::aopCheck(AsyncOp *op, bool waitOk)
{
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
                        //kai.yan
                        routerApi()->beginGetModelName();
			navigateTo("settings");
		}
		break;
	case WebApiStatus_RouterAuthenticationFailed:
		{
			app()->updateAppDataRouterAuth();
			RouterLoginPopup *popup = new RouterLoginPopup(appData()->m_routerUsername, appData()->m_routerPassword, this, SLOT(authCheck(RouterLoginPopup*,bool)));
			app()->push(popup);
		}
		break;
	case WebApiStatus_DeviceIdNotMine:
		setLPCText(m_errorLabel, "device.error.device_key", true);
		m_errorLabel->setVisible(true);
		setLPCText(m_nextButton, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		break;
	case WTFStatus_AuthenticationFailed:
		setLPCText(m_errorLabel, "signin.error.signin", true);
		m_errorLabel->setVisible(true);
		setLPCText(m_nextButton, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		break;
	default:
		setLPCText(m_errorLabel, "signin.error.failed", true);
		m_errorLabel->setVisible(true);
		setLPCText(m_nextButton, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		break;
	}
}

void SignInPage::authCheck(RouterLoginPopup *popup, bool accepted)
{
	if (accepted) {
		appData()->m_routerUsername = popup->username();
		appData()->m_routerPassword = popup->password();
		emit internalRefresh();
	} else {
		setLPCText(m_errorLabel, "signin.error.failed", true);
		m_errorLabel->setVisible(true);
		setLPCText(m_nextButton, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
	}
}

void SignInPage::updateUI()
{
	m_nextButton->setDisabled(m_usernameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty());
}

CreateAccountPage::CreateAccountPage(QWidget *parent)
: LPCNavPage(parent)
, m_usernameRE(QString::fromUtf8("^[a-z0-9][a-z0-9\\.\\-\\_]*[a-z0-9]$"), Qt::CaseInsensitive)
{
}

CreateAccountPage::~CreateAccountPage()
{
}

bool CreateAccountPage::prepareImpl()
{qDebug()<<"CreateAccountPage::prepareImpl";
    m_usernameEdit = this->findChild<QLineEdit*>(QString::fromUtf8("username"));
    m_passwordEdit = this->findChild<QLineEdit*>(QString::fromUtf8("password"));
    m_passwordConfirmEdit = this->findChild<QLineEdit*>(QString::fromUtf8("passwordConfirm"));
    m_emailEdit = this->findChild<QLineEdit*>(QString::fromUtf8("email"));
    m_emailConfirmEdit = this->findChild<QLineEdit*>(QString::fromUtf8("emailConfirm"));
    m_checkBtn = this->findChild<QPushButton*>(QString::fromUtf8("check"));
    m_nextBtn = this->findChild<QPushButton*>(QString::fromUtf8("next"));
    m_errorPrompt = this->findChild<QLabel*>(QString::fromUtf8("error"));
    m_usernamePrompt = this->findChild<QLabel*>(QString::fromUtf8("usernamePrompt"));
    m_passwordConfirmPrompt = this->findChild<QLabel*>(QString::fromUtf8("passwordConfirmPrompt"));
    m_emailPrompt = this->findChild<QLabel*>(QString::fromUtf8("emailPrompt"));
    m_emailConfirmPrompt = this->findChild<QLabel*>(QString::fromUtf8("emailConfirmPrompt"));

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

	AsyncOp *op = webApi()->beginCreateAccount(m_usernameEdit->text(), m_passwordEdit->text(), m_emailEdit->text());
	app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "createaccount.status");
}

void CreateAccountPage::aopCheck(AsyncOp *op, bool waitOk)
{
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
				setLPCText(m_usernamePrompt, "createaccount.check.unavailable", true);
				//updateStyleClass(m_usernamePrompt, QString::fromUtf8("error"));
				m_usernamePrompt->setVisible(true);
				focus = m_usernameEdit;
				break;
			case 3006:
				setLPCText(m_errorPrompt, "createaccount.error.username", true);
				m_errorPrompt->setVisible(true);
				focus = m_usernameEdit;
				break;
			case 3007:
				setLPCText(m_errorPrompt, "createaccount.error.password", true);
				m_errorPrompt->setVisible(true);
				focus = m_passwordEdit;
				break;
			case 3004:
				setLPCText(m_emailPrompt, "createaccount.email.invalid", true);
				m_emailPrompt->setVisible(true);
				focus = m_emailEdit;
				break;
			case 3005:
				setLPCText(m_emailPrompt, "createaccount.email.taken", true);
				m_emailPrompt->setVisible(true);
				focus = m_emailEdit;
				break;
			default:
				setLPCText(m_errorPrompt, "createaccount.error", true);
				m_errorPrompt->setVisible(true);
				break;
			}
			setLPCText(m_nextBtn, "tryagain");

			if (focus) {
				focus->selectAll();
				focus->setFocus();
				m_nextBtn->setDefault(true);
				m_nextBtn->setAutoDefault(true);
			}
		}
		break;
	default:
		setLPCText(m_errorPrompt, "createaccount.error", true);
		m_errorPrompt->setVisible(true);
		setLPCText(m_nextBtn, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		break;
	}
}

void CreateAccountPage::checkAvailable()
{
	AsyncOp *op = webApi()->beginCheckNameAvailable(m_usernameEdit->text());
	app()->wait(op, this, SLOT(accountCheck(AsyncOp*,bool)), "general.status.commmicating");
}

void CreateAccountPage::accountCheck(AsyncOp *op, bool waitOk)
{
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
			setLPCText(m_usernamePrompt, "createaccount.check.available", true);
			//updateStyleClass(m_usernamePrompt, QString::fromUtf8("success"));
			m_usernamePrompt->setVisible(true);
		} else {
			setLPCText(m_usernamePrompt, "createaccount.check.unavailable", true);
			//updateStyleClass(m_usernamePrompt, QString::fromUtf8("error"));
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
		setLPCText(m_errorPrompt, "createaccount.error.password", true);
		m_errorPrompt->setVisible(true);
		focus = m_passwordEdit;
	} else if (!u && p1 && p2) {
		setLPCText(m_errorPrompt, "createaccount.error.password", true);
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_passwordEdit;
	} else if (u && !p1 && !p2) {
		setLPCText(m_errorPrompt, "createaccount.error.username", true);
		m_errorPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && !p1 && p2) {
		setLPCText(m_errorPrompt, "createaccount.error.username", true);
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && p1 && !p2) {
		setLPCText(m_errorPrompt, "createaccount.error.username", true);
		//m_errorPrompt->setText(markupLink(tr("createaccount.error.username") + QString::fromUtf8(" ") + tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		focus = m_usernameEdit;
	} else if (u && p1 && p2) {
		setLPCText(m_errorPrompt, "createaccount.error.username", true);
		//m_errorPrompt->setText(markupLink(tr("createaccount.error.username") + QString::fromUtf8(" ") + tr("createaccount.error.password")));
		m_errorPrompt->setVisible(true);
		m_passwordConfirmPrompt->setVisible(true);
		focus = m_usernameEdit;
    }

	bool ok2 = false;

    if (email.indexOf(QChar::fromLatin1('@')) < 0) {
		setLPCText(m_emailPrompt, "createaccount.email.invalid", true);
		m_emailPrompt->setVisible(true);
		if (!focus) focus = m_emailEdit;
	} else {
		m_emailPrompt->setVisible(false);
		if (email != emailConfirm) {
			setLPCText(m_emailConfirmPrompt, "createaccount.email.confirm", true);
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
: LPCNavPage(parent)
{
}

DonePage::~DonePage()
{
}

ManagePage::ManagePage(QWidget *parent)
: LPCNavPage(parent), m_lastStatus(LPCSTATUS_UNKNOWN)
{
    qDebug()<<"ManagePage::ManagePage(QWidget *parent)";
    //add by qicheng.ai
    m_LocalMACAddr = "";
    QHostInfo::lookupHost("routerlogin.com", this, SLOT(GetLookupData(QHostInfo)));
}


void ManagePage::GetLookupData(const QHostInfo &Host)
{
    if(Host.addresses().size() > 0)
    {
        QList<QNetworkInterface> ilist = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface interface,ilist)
        {
            if(interface.isValid())
            {
                QList<QNetworkAddressEntry> entrylist = interface.addressEntries();
                foreach(QNetworkAddressEntry entry, entrylist)
                {
                    QString getewayaddr = Host.addresses().at(0).toString();
                    if(getewayaddr != "")
                    {
                        QString hostaddr = entry.ip().toString();
                        QString getewayaddrsp = getewayaddr.left(getewayaddr.lastIndexOf("."));
                        QString hostaddrsp = hostaddr.left(hostaddr.lastIndexOf("."));
                        if(getewayaddrsp == hostaddrsp)
                        {
                            m_LocalMACAddr = interface.hardwareAddress().toUpper().replace(':',"");
                            qDebug()<<"ManagePage::GetLookupData(const QHostInfo &Host):"<<m_LocalMACAddr;
                        }
                    }

                }
            }

        }
    }
}

ManagePage::~ManagePage()
{
}

bool ManagePage::prepareImpl()
{qDebug()<<"ManagePage::prepareImpl()";
	appData()->m_inChangeBasicSettings = false;

    m_errorLabel = this->findChild<QLabel*>(QString::fromUtf8("error"));
    m_bundle = this->findChild<QLabel*>(QString::fromUtf8("bundle"));
    m_bundleDetail = this->findChild<QLabel*>(QString::fromUtf8("bundleDetail"));
    m_account = this->findChild<QLabel*>(QString::fromUtf8("account"));
    m_lpcStatus = this->findChild<QLabel*>(QString::fromUtf8("lpcStatus"));
    m_toggleButton = this->findChild<QPushButton*>(QString::fromUtf8("toggleButton"));
    m_changeButton = this->findChild<QPushButton*>(QString::fromUtf8("changeButton"));
    m_settingsButton = this->findChild<QPushButton*>(QString::fromUtf8("settingsButton"));
        m_bypassAccount = this->findChild<QLabel*>(QString::fromUtf8("bypass_account"));     //yankai add
        m_bypassButton = this->findChild<QPushButton*>(QString::fromUtf8("bypassButton"));
        if (!m_errorLabel || !m_bundle || !m_bundleDetail || !m_account || !m_lpcStatus || !m_toggleButton || !m_changeButton || !m_settingsButton || !m_bypassAccount || !m_bypassButton) {
                return false;
        }

	m_errorLabel->setVisible(false);

        //kai.yan add
        AsyncOp *op = routerApi()->beginGetModelName();
        app()->wait(op, this, SLOT(aopSerialNum(AsyncOp*,bool)), "manage.status.checking");
        op = coreApi()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
        app()->wait(op, this, SLOT(aopGetDeviceID(AsyncOp*,bool)), "manage.status.checking");
        return true;
}

//kai.yan add
void ManagePage::aopSerialNum(AsyncOp *op, bool waitOk)
{
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
                    //kai.yan add

                    BYPASSUSERNAME = m_outfile.readFile(SERIALNUM);
                    qDebug()<<"BYPASSUSERNAME:===="<<BYPASSUSERNAME;
                    if(BYPASSUSERNAME == "")
                    {
                        ISLOGINBYPASS = false;
                    }else
                    {
                        ISLOGINBYPASS = true;
                    }
             }
    }
}

bool ManagePage::loadImpl()
{   qDebug()<<"******ManagePage::loadImpl()*******";
	coreApi()->setRouterUsername(appData()->m_routerUsername);
	coreApi()->setRouterPassword(appData()->m_routerPassword);
	AsyncOp *op = coreApi()->beginQuerySettings(appData()->m_userToken, appData()->m_deviceId, appData()->m_routerUsername, appData()->m_routerPassword);
	app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "manage.status.checking");
	return true;
}

void ManagePage::aopCheck(AsyncOp *op, bool waitOk)
{
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
			QByteArray bundle;
			const char* typeName = op->property("varBundle").typeName();
			QByteArray lbl;
			if(typeName == QString("QStringList").toLatin1())
			{
				lbl = "manage.settings.bundle.custom.label";
				m_lastStatus = LPCSTATUS_UNKNOWN;
			}
			 else {
				bundle = op->property("varBundle").toString().toUtf8().toLower();
				lbl = "settings.bundle." + bundle;
			}

			bool lpcEnabled = op->property("varParentalControlStatus").toBool();

			if (bundle == "high") {
				m_lastStatus = LPCSTATUS_LEVEL_HIGH;
			} else if (bundle == "moderate") {
				m_lastStatus = LPCSTATUS_LEVEL_MODERATE;
			} else if (bundle == "low") {
				m_lastStatus = LPCSTATUS_LEVEL_LOW;
			} else if (bundle == "minimal") {
				m_lastStatus = LPCSTATUS_LEVEL_MINIMAL;
			} else if (bundle == "none") {
				m_lastStatus = LPCSTATUS_LEVEL_NONE;
			}

			if (lpcEnabled) {
				reportStatus(m_lastStatus);
			} else {
				reportStatus(LPCSTATUS_DISABLED);
			}

			QByteArray lblDetail = QByteArray("manage.settings.bundle.") + bundle;
			setLPCText(m_bundle, lbl);
			setLPCText(m_bundleDetail, lblDetail);
			//updateStyleClass(m_bundle, QString::fromUtf8("settings-bundle-%1").arg(QString::fromUtf8(bundle)));

			updateLPCStatus(lpcEnabled);

			m_account->setText(appData()->m_username);

			if(ISLOGINBYPASS)
			{
				m_bypassAccount->setText(BYPASSUSERNAME);  //yankai add
				setLPCText(m_bypassButton, "magage.bypass.logout");
			}
		}
		break;
	case WebApiStatus_DeviceIdNotMine:
		{
			// TODO:
			m_errorLabel->setVisible(false);
			//MessagePopup *popup = new MessagePopup("general.wait.title", "device.error.device_id_not_mine", this, SLOT(devicePrompt(MessagePopup*,bool)));
			//app()->push(popup);
			navigateTo("intro");
		}
		break;
	default:
		if (op->property("varErrorCode").toInt() == 4003) {
			DBGLOG(LOG_INFO, 2, QString::fromUtf8("Jump for 4003!"));
			// Unknown device ???
			m_errorLabel->setVisible(false);
			navigateTo("intro");
		} else {
			m_errorLabel->setVisible(true);
			setLPCText(m_errorLabel, "device.error.netgear", true);
			reportStatus(LPCSTATUS_ERROR);
          //  navigateTo("intro"); //kai.yan timeout for reflesh
		}
		break;
	}

	enableUI(code == WTFStatus_NoError);
}

void ManagePage::devicePrompt(MessagePopup *popup, bool accepted)
{
	navigateTo("intro");
}

void ManagePage::enableUI(bool enable)
{
	m_toggleButton->setDisabled(!enable);
	m_changeButton->setDisabled(!enable);
	m_settingsButton->setDisabled(!enable);
        m_bypassButton->setDisabled(!enable);

}

void ManagePage::updateLPCStatus(bool lpcEnabled)
{
	if (lpcEnabled) {
		setLPCText(m_lpcStatus, "manage.parentalcontrols.enabled");
		//updateStyleClass(m_lpcStatus, QString::fromUtf8("success"));
		setLPCText(m_toggleButton, "manage.toggle.enabled");
	} else {
		setLPCText(m_lpcStatus, "manage.parentalcontrols.disabled");
		//updateStyleClass(m_lpcStatus, QString::fromUtf8("error"));
		setLPCText(m_toggleButton, "manage.toggle.disabled");
	}

	m_lpcEnabled = lpcEnabled;
}

void ManagePage::toggleLPC()
{
	AsyncOp *op = coreApi()->beginToggleParentalControl(!m_lpcEnabled, appData()->m_deviceId);
	app()->reportLPCEnableDisable(!m_lpcEnabled);
	app()->wait(op, this, SLOT(aopToggle(AsyncOp*,bool)), m_lpcEnabled ? "manage.status.disabling" : "manage.status.enabling");
}

void ManagePage::aopToggle(AsyncOp *op, bool waitOk)
{
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
			bool lpcEnabled = op->property("varParentalControlStatus").toBool();
			if (lpcEnabled) {
				reportStatus(m_lastStatus);
			} else {
				reportStatus(LPCSTATUS_DISABLED);
			}
			updateLPCStatus(lpcEnabled);
		}
		break;
	default:
		m_errorLabel->setVisible(true);
		setLPCText(m_errorLabel, "device.error.netgear", true);
		//MessagePopup *popup = new MessagePopup("general.wait.title", "device.error.device_id_not_mine", this, SLOT(devicePrompt(MessagePopup*,bool)));
		//app()->push(popup);
		navigateTo("intro");
		break;
	}
}

void ManagePage::changeSettings()
{
        app()->reportLPCBasicSettingClick();
        appData()->m_inChangeBasicSettings = true;
        navigateTo("signin");
}

void ManagePage::changeCustomSettings()
{
    app()->reportLPCCustomSettingClick();
    AsyncOp *op = webApi()->beginAccountRelay(appData()->m_userToken);
    app()->wait(op, this, SLOT(aopCustom(AsyncOp*,bool)), "general.status.commmicating");
}

//20121128 yankai add
void ManagePage::enterBypass()
{
    qDebug()<<"**enterBypass**";

   navigateTo("bypassAccountList");
}
//yankai end
void ManagePage::aopGetDeviceID(AsyncOp *op, bool waitOk)
{
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
			// op->setProperty("varDeviceID",);
			qDebug()<<"varDeviceID:"<<op->property("varDeviceID").toString();
			appData()->m_deviceId = op->property("varDeviceID").toString();
			// navigateTo("bypassAccountList");
			break;
		}
	case  WebApiStatus_RouterAuthenticationFailed:
		coreApi()->beginAuthenticateAndGetDeviceID(appData()->m_routerUsername,appData()->m_routerPassword);
		break;

    }
}
void ManagePage::aopCustom(AsyncOp *op, bool waitOk)
{
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
			app()->reportLPCLevel(5);
			QString relayToken = op->property("varRelayToken").toString();
			QString apiKey = op->property("varApiKey").toString();
			QString baseUrl = op->property("varBaseUrl").toString();
			QString url = QString::fromUtf8("%1?device_id=%2&api_key=%3&relay_token=%4").arg(baseUrl).arg(appData()->m_deviceId).arg(apiKey).arg(relayToken);
                        qDebug()<<"WTFStatus_NoError:"<<WTFStatus_NoError<<" url:"<<url;
                        QDesktopServices::openUrl(QUrl(url));
		}
		break;
	default:
		{
			app()->reportLPCLevel(5);
                        QString url = QString::fromUtf8("http://netgear.opendns.com/sign_in.php");
                        QDesktopServices::openUrl(QUrl(url));
		}
		break;
	}
}

SettingsPage::SettingsPage(QWidget *parent)
: LPCNavPage(parent), m_currentLevel(-1), m_loadOk(false)
{
}

SettingsPage::~SettingsPage()
{
}

bool SettingsPage::prepareImpl()
{qDebug()<<"SettingsPage::prepareImpl()";
	for (int i = 0; i < 6; i++) {
        m_levels[i] = this->findChild<QRadioButton*>(QString::fromUtf8("level%1").arg(i+1));
		connect(m_levels[i],SIGNAL(clicked()),this,SLOT(radioChanged()));
		if (!m_levels[i]) {
			return false;
		}
	}
    m_errorPrompt = this->findChild<QLabel*>(QString::fromUtf8("error"));
    m_nextBtn = this->findChild<QPushButton*>(QString::fromUtf8("next"));

	m_frame_custom = this->findChild<QFrame*>(QString::fromUtf8("frame_custom"));
	if (!m_frame_custom)
	{
		return false;
	}
	m_frame_custom->setVisible(false);
	m_customList = m_frame_custom->findChildren<QCheckBox*>();

	if (!m_errorPrompt || !m_nextBtn) {
		return false;
	}

	m_errorPrompt->setVisible(false);
	return true;
}

bool SettingsPage::loadImpl()
{   qDebug()<<"*******SettingsPage::loadImpl()******";
	loadValues();
	return true;
}

static const char *BUNDLE_NAMES[] = {
	"High", "Moderate", "Low", "Minimal", "None", "Custom"
};
static const char *CUSTOM_TYPE[] =
{
	"Academic Fraud", "Adult Themes", "Adware",
	"Alcohol", "Anime/Manga/Webcomic", "Auctions",
	"Automotive", "Blogs", "Business Services",
	"Chat", "Classifieds", "Dating",
	"Drugs", "Ecommerce/Shopping", "Educational Institutions",
	"File Storage", "Financial Institutions","Forums/Message boards",
	"Gambling", "Games", "German Youth Protection", 
	"Government", "Hate/Discrimination", "Health and Fitness",
	"Humor", "Instant Messaging", "Jobs/Employment",
	"Lingerie/Bikini", "Movies", "Music", 
	"News/Media", "Non-Profits", "Nudity",
	"P2P/File sharing", "Parked Domains", "Photo Sharing",
	"Podcasts", "Politics", "Pornography", 
	"Portals", "Proxy/Anonymizer", "Radio",
	"Religious", "Research/Reference", "Search Engines",
	"Sexuality", "Social Networking", "Software/Technology",
	"Sports", "Tasteless", "Television", 
	"Tobacco", "Travel", "Typo Squatting",
	"Video Sharing", "Visual Search Engines", "Weapons",
	"Web Spam", "Webmail", "Phishing Protection"
};

void SettingsPage::loadValues()
{
       qDebug()<<"SettingsPage::loadValues():m_userToken:"<<appData()->m_userToken<<" m_deviceId:"<<appData()->m_deviceId;
        AsyncOp *op = webApi()->beginGetFilters(appData()->m_userToken, appData()->m_deviceId);
	app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "settings.status.get");
}

void SettingsPage::aopCheck(AsyncOp *op, bool waitOk)
{
	op->deleteLater();
	int code;
	if (waitOk) {
		code = op->result().toInt();
	} else {
		// aborted
		code = WTFStatus_Aborted;
	}
        qDebug()<<"***SettingsPage::aopCheck:waitOk"<<waitOk<<"code"<<code;
	switch (code) {
	case WTFStatus_NoError:
		{
			m_errorPrompt->setVisible(false);
			QVariant tempBundle = op->property("varBundle");
			const char *typeName = tempBundle.typeName();
			QString bundle;
			QStringList bundleList;
			if(typeName == QString("QStringList").toLatin1())
			{
				bundleList = tempBundle.toStringList();
			}else
			{
				bundle = tempBundle.toString().toLower();
			}
			
			int index;
			if (bundle == "high") {
				index = 0;
			} else if (bundle == "moderate") {
				index = 1;
			} else if (bundle == "low") {
				index = 2;
			} else if (bundle == "minimal") {
				index = 3;
			} else if(bundle == "none"){
				index = 4;
			}else
			{
				index = 5;
				m_frame_custom->setVisible(true);
				if(bundleList.count()>0)
				{
					for(int i=0;i<m_customList.count();i++)
					{
						for(int j = 0; j < bundleList.count(); j++)
						{
							QString temp= bundleList[j].remove("\\");
							if(QString::fromUtf8(CUSTOM_TYPE[i]).compare(temp,Qt::CaseInsensitive)==0)
							{
								m_customList[i]->setChecked(true);
							}
						}
					}
					
				}
			}
			m_levels[index]->setChecked(true);
			m_loadOk = true;
			m_currentLevel = index;
			setLPCText(m_nextBtn, "next");
		}
		break;
	default:
		m_errorPrompt->setVisible(true);
		m_loadOk = false;
		setLPCText(m_nextBtn, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
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
		for (int i = 0; i < 6; i++) {
			if (m_levels[i]->isChecked()) {
				sel = i;
				break;
			}
		}

		QStringList bundleList; 
		QStringList templsit;
		if (sel >= 0 && sel != m_currentLevel || sel ==5 ) {
			m_currentLevel = sel;
			////////////////////////////////////////kai.yan add custom filter//////////////////////////////////
			QString bundle = "";
			if(sel == 5)
			{
				for (int i= 0; i<m_customList.count();i++)
				{
					if(m_customList[i]->isChecked())
					{
						templsit << QString::fromUtf8(CUSTOM_TYPE[i]);
					}
				}
				for(int i = 0; i<templsit.count();i++)
				{
					if(i != (templsit.count()-1))
					{
						bundle += templsit[i] +",";
					}else
					{
						bundle += templsit[i];
					}
				}
				if(bundle =="")
				{
					bundleList <<"bundle"<<QString::fromUtf8("None");
				}else
				{
					bundleList<<"categories"<<bundle;
				}
			}else
			{
				bundle = QString::fromUtf8(BUNDLE_NAMES[sel]);
				bundleList <<"bundle"<<bundle;
			}
			
			/////////////////////////////////////////////////// end ///////////////////////
			AsyncOp *op = coreApi()->beginSetFiltersAndEnableParentalControl(appData()->m_userToken, appData()->m_deviceId, bundleList);
			app()->wait(op, this, SLOT(aopChange(AsyncOp*,bool)), "settings.status.set");
		}
		else {
			navigateTo("done");
		}
	}
}

void SettingsPage::aopChange(AsyncOp *op, bool waitOk)
{
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
		app()->reportLPCLevel(m_currentLevel);
		m_errorPrompt->setVisible(false);
		if (appData()->m_inChangeBasicSettings) {
			gotoMainPage();
		} else {
			navigateTo("done");
		}
		break;
	default:
		m_errorPrompt->setVisible(true);
		setLPCText(m_nextBtn, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		break;
	}
}

void SettingsPage::radioChanged()
{
	if(sender() == m_levels[5])
	{
		m_frame_custom->setVisible(true);
	}else
	{
		m_frame_custom->setVisible(false);
	}
}

//20121129 yankai add
BypassUserList::BypassUserList(QWidget *parent)
    :LPCNavPage(parent)
{

}
BypassUserList::~BypassUserList()
{
}
bool BypassUserList::prepareImpl()
{
    qDebug()<<"****BypassUserList::prepareImpl()";
    bypassUserList = this->findChild<QListWidget*>(QString::fromUtf8("bypassUserlistWidget"));
    stackedWidget = this->findChild<QStackedWidget*>(QString::fromUtf8("stackedWidget"));
    bypassMsg = this->findChild<QLabel*>(QString::fromUtf8("bypassMsg"));
    m_error = this->findChild<QLabel*>(QString::fromUtf8("error"));
    bypassUserName = this->findChild<QLineEdit*>(QString::fromUtf8("bypassUserName"));
    bypassPassword = this->findChild<QLineEdit*>(QString::fromUtf8("bypassPassword"));
    m_errorLabel = this->findChild<QLabel*>(QString::fromUtf8("error_Login"));
    m_bypassLogin = this->findChild<QPushButton*>(QString::fromUtf8("bypassLogin"));
	lab_user_login = this->findChild<QLabel*>(QString::fromUtf8("bypass_login"));
	m_btnReflesh = this->findChild<QPushButton*>(QString::fromUtf8("bypassReflesh"));

    if(!bypassUserList || !stackedWidget || !bypassMsg || !m_error || !bypassUserName || !bypassPassword || !m_errorLabel || !m_bypassLogin || !lab_user_login || !m_btnReflesh)
    {
        return false;
    }
    if(!ISLOGINBYPASS)
    {
        stackedWidget->setCurrentIndex(0);
    }else
    {
        stackedWidget->setCurrentIndex(1);
      //  QString msg = ttt(QString("bypass.login.msg").toLatin1().data());
        //bypassMsg->setText(msg + BYPASSUSERNAME);
		lab_user_login->setText(BYPASSUSERNAME);
    }
    m_error->setVisible(false);
    AsyncOp *op = coreApi()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
    app()->wait(op, this, SLOT(aopGetDeviceID(AsyncOp*,bool)), "bypass.status.userlist");
    connect(bypassPassword, SIGNAL(returnPressed()), m_bypassLogin, SLOT(click()));
    return true;
}
bool BypassUserList::loadImpl()
{
    //kai.yan for get Serial number--
    AsyncOp *op = routerApi()->beginGetModelName();
    app()->wait(op, this, SLOT(aopSerialNum(AsyncOp*,bool)), "bypass.status.userlist");
    //---
    aopGetBypassUers();
    return true;
}
void BypassUserList::gotoManagePage()
{
    navigateTo("manage");
}


void BypassUserList::aopGetDeviceID(AsyncOp *op, bool waitOk)
{
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
			m_error->setVisible(false);
            qDebug()<<"varDeviceID:"<<op->property("varDeviceID").toString();
            appData()->m_deviceId = op->property("varDeviceID").toString();
            aopGetBypassUers();
			break;
        }
	case  WebApiStatus_RouterAuthenticationFailed:
		coreApi()->beginAuthenticateAndGetDeviceID(appData()->m_routerUsername,appData()->m_routerPassword);
		break;
	default:
	//	m_error->setVisible(true);
	//	m_error->setText("Get the device ID failed!");
		reportStatus(LPCSTATUS_ERROR);
		break;
    }
}
void BypassUserList::aopSerialNum(AsyncOp *op, bool waitOk)
{
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
               qDebug()<<"aopSerialNum:"<<SERIALNUM;
             }
    }
}
void BypassUserList::aopGetBypassUers()
{
    AsyncOp *op = webApi()->beginGetUsersForDeviceId(appData()->m_deviceId);
    app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "bypass.status.userlist");
}
void BypassUserList::aopCheck(AsyncOp *op, bool waitOk)
{
    op->deleteLater();
    int code;
    if (waitOk) {
            code = op->result().toInt();
    } else {
            // aborted
            code = WTFStatus_Aborted;
    }
    qDebug()<<"***code***:"<<code;
    switch (code) {
    case WTFStatus_NoError:
        {
            m_error->setVisible(false);
                bypassUserList->clear();
                QStringList varUserName = op->property("varList").toStringList();
                foreach(QString item,varUserName)
                {
                    QListWidgetItem *widgetItem = new QListWidgetItem();
                    widgetItem->setText(item);
                    widgetItem->setSizeHint(QSize(50,20));
                    widgetItem->setTextAlignment(Qt::AlignHCenter);
                    bypassUserList->addItem(widgetItem);
                    qDebug()<<"*****varUserName****:"<<item;
                }

//            }
            break;
        }
	case WTFStatus_Failed:
		{
			 m_error->setVisible(true);
			 int errorCode = op->property("varErrorCode").toInt();
			 if(errorCode == 4012)
			 {
				 setLPCText(m_error,"bypass.error.nouser",true);
			 }
			break;
		}
    default:
            m_error->setVisible(true);
            setLPCText(m_btnReflesh, "tryagain");
            reportStatus(LPCSTATUS_ERROR);
            break;
    }
}
void BypassUserList::bypassLogin(QListWidgetItem *item)
{

    if(item!=NULL)
    {
         BYPASSUSERNAME = item->text();
         qDebug()<<"BypassUserList::bypassLogin";
         stackedWidget->setCurrentIndex(2);
         bypassUserName->setText(BYPASSUSERNAME);
         m_errorLabel->setVisible(false);
         m_bypassLogin->setDisabled(true);
         connect(bypassPassword, SIGNAL(textChanged(QString)),this, SLOT(updateUI()));
    }

}
void BypassUserList::bypassUserReflesh()
{
    AsyncOp *op = coreApi()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
    app()->wait(op, this, SLOT(aopGetDeviceID(AsyncOp*,bool)), "general.status.commmicating");

}
void BypassUserList::bypassLogout()
{
    QString mac = m_LocalMACAddr;
    qDebug()<<"BypassUserList::bypassLogout():"<<mac;
    AsyncOp *op = coreApi()->beginWrappedDeleteMacAddress(mac);
    app()->wait(op, this, SLOT(logout_callback(AsyncOp*,bool)), "bypass.status.logout");
}

void BypassUserList::logout_callback(AsyncOp *op, bool waitOk)
{
    op->deleteLater();
    int code;
    if (waitOk) {
            code = op->result().toInt();
    } else {
            // aborted
            code = WTFStatus_Aborted;
    }
    qDebug()<<"----bypassLogout::logout_callback()--:"<<code;
    switch (code)
    {
        case WTFStatus_NoError:
        {
            BYPASSUSERNAME = "";
            //----kai.yan add
            m_outfile.writeFile(SERIALNUM,BYPASSUSERNAME);
            //--------
            ISLOGINBYPASS = false;
            navigateTo("manage");
        }
        break;
		case WebApiStatus_RouterAuthenticationFailed:
			 op = coreApi()->beginAuthenticateAndDeleteMacAddress(appData()->m_routerUsername, appData()->m_routerPassword, m_LocalMACAddr);
			 app()->wait(op, this, SLOT(logout_callback(AsyncOp*,bool)), "bypass.status.logout");
			break;
    default:
        int errorCode = op->property("varResponseCode").toInt();
        qDebug()<<"errorCode:"<<errorCode;
        if(errorCode == 001)
        {
            m_outfile.writeFile(SERIALNUM,"");
            ISLOGINBYPASS = false;
            navigateTo("manage");
        }
        break;
    }

}

void BypassUserList::bypassLoginin()
{

    qDebug()<<"------BypassUserList::bypassLoginin()----"<<appData()->m_deviceId<<"bypassUserName:"<<bypassUserName->text()<<" bypassPassword:"<<bypassPassword->text();
    AsyncOp *op = webApi()->beginGetDeviceChild(appData()->m_deviceId,bypassUserName->text(), bypassPassword->text());
    app()->wait(op, this, SLOT(aopCheck_Login(AsyncOp*,bool)), "bypass.status.login");

}
void BypassUserList::bypassCancel()
{
   // navigateTo("bypassAccountList");
   stackedWidget->setCurrentIndex(0);
}
void BypassUserList::aopCheck_Login(AsyncOp *op, bool waitOk)
{
    op->deleteLater();
    int code;
    if (waitOk) {
            code = op->result().toInt();
    } else {
            // aborted
            code = WTFStatus_Aborted;
    }
    qDebug()<<"----BypassUserLogin::aopCheck()--:"<<code;
    switch (code) {
    case WTFStatus_NoError:
        {
            m_errorLabel->setVisible(false);
            qDebug()<<"----varChildDeviceId---:"<<op->property("varChildDeviceId").toString();

            //----------add by qicheng.ai
            QString mac = m_LocalMACAddr;
            QString divceID = op->property("varChildDeviceId").toString();
            op = coreApi()->beginWrappedSetDNSMasqDeviceID(mac,divceID);
            //---------

            //----kai.yan add
            m_outfile.writeFile(SERIALNUM,BYPASSUSERNAME);
            //--------
            ISLOGINBYPASS = true;
            navigateTo("manage");
        }
            break;
	case WTFStatus_AuthenticationFailed:
        setLPCText(m_errorLabel, "bypass.error.password");
		m_errorLabel->setVisible(true);
		setLPCText(m_bypassLogin, "tryagain");
		reportStatus(LPCSTATUS_ERROR);
		ISLOGINBYPASS = false;
		break;
    default:
            ISLOGINBYPASS = false;
            setLPCText(m_errorLabel, "bypass.error.login");
            m_errorLabel->setVisible(true);
            int errorCode = op->property("varResponseCode").toInt();
            qDebug()<<"errorCode:"<<errorCode;
            if(errorCode == 2003)
            {
                navigateTo("intro");
            }
            break;
        }
}
void BypassUserList::updateUI()
{
    qDebug()<<"BypassUserLogin::updateUI()";
    m_bypassLogin->setDisabled(bypassPassword->text().isEmpty());
}

//BypassUserLogin::BypassUserLogin(QWidget *parent)
//         :LPCNavPage(parent)
//{
//}
// BypassUserLogin::~BypassUserLogin()
//{
//     if(outfile != NULL)
//     {
//         delete outfile;
//         outfile = NULL;
//     }
// }
//bool BypassUserLogin::loadImpl()
//{
//    qDebug()<<"BypassUserLogin::loadImpl()";

//    return true;
//}
//bool BypassUserLogin::prepareImpl()
//{
//    bypassUserName = qFindChild<QLineEdit*>(this, QString::fromUtf8("bypassUserName"));
//    bypassPassword = qFindChild<QLineEdit*>(this, QString::fromUtf8("bypassPassword"));
//    m_errorLabel = qFindChild<QLabel*>(this,QString::fromUtf8("error"));
//    m_bypassLogin = qFindChild<QPushButton*>(this,QString::fromUtf8("bypassLogin"));
//    if(!bypassUserName || !bypassPassword || !m_errorLabel || !m_bypassLogin)
//    {
//        return false;
//    }
//    qDebug()<<"BypassUserLogin::prepareImpl()";
//    bypassUserName->setText(BYPASSUSERNAME);
//    m_errorLabel->setVisible(false);
////    m_bypassLogin->setDisabled(true);
////    connect(bypassPassword, SIGNAL(textChanged(QString)),this, SLOT(updateUI()));
//    return true;
//}

//void BypassUserLogin::bypassLoginin()
//{

//    qDebug()<<"------BypassUserLogin::bypassLoginin()----"<<appData()->m_deviceId<<"bypassUserName:"<<bypassUserName->text()<<" bypassPassword:"<<bypassPassword->text();
//    AsyncOp *op = webApi()->beginGetDeviceChild(appData()->m_deviceId,bypassUserName->text(), bypassPassword->text());
//    app()->wait(op, this, SLOT(aopCheck(AsyncOp*,bool)), "bypass.status");

//}
//void BypassUserLogin::bypassCancel()
//{
//    navigateTo("bypassAccountList");
//}
//void BypassUserLogin::aopCheck(AsyncOp *op, bool waitOk)
//{
//    op->deleteLater();
//    int code;
//    if (waitOk) {
//            code = op->result().toInt();
//    } else {
//            // aborted
//            code = WTFStatus_Aborted;
//    }
//    qDebug()<<"----BypassUserLogin::aopCheck()--:"<<code;
//    switch (code) {
//    case WTFStatus_NoError:
//        {
//            m_errorLabel->setVisible(false);
//            qDebug()<<"----varChildDeviceId---:"<<op->property("varChildDeviceId").toString();

//            //----------add by qicheng.ai
//            QString mac = m_LocalMACAddr;
//            QString divceID = op->property("varChildDeviceId").toString();
//            op = coreApi()->beginWrappedSetDNSMasqDeviceID(mac,divceID);
//            //---------

//            //----kai.yan add

//            outfile = new Outfile;
//            outfile->writeFile(SERIALNUM,BYPASSUSERNAME);

//         //   qDebug()<<"varSerialNumber:\n"<<op->property("varSerialNumber").toString();
//            //---------

////             qDebug()<<"verModelName:\n"<<op->property("verModelName").toString();
//            //--------
//            ISLOGINBYPASS = true;
//            navigateTo("manage");
//        }
//            break;
//    default:
//            BYPASSUSERNAME = "";
//            //----kai.yan add
//            outfile = new Outfile;
//            outfile->writeFile(SERIALNUM,BYPASSUSERNAME);
//            //--------
//            ISLOGINBYPASS = false;
//            m_errorLabel->setText("You enter the wrong password.");
//            m_errorLabel->setVisible(true);
//            break;
//        }
//}
//void BypassUserLogin::updateUI()
//{
//    qDebug()<<"BypassUserLogin::updateUI()";
//    m_bypassLogin->setDisabled(bypassPassword->text().isEmpty());
//}
//end

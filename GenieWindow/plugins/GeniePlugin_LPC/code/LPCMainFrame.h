#ifndef __LPCMainFrame_h__
#define __LPCMainFrame_h__

#include "AsyncOp.h"
#include "ext.h"
#include "coreapi.h"
#include <QByteArray>
#include <QPointer>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QtNetwork/QNetworkAccessManager>

enum LPCStatus {
	LPCSTATUS_UNKNOWN,
	LPCSTATUS_CHECKING,
	LPCSTATUS_ERROR,
	LPCSTATUS_DISABLED,
	LPCSTATUS_LEVEL_HIGH,
	LPCSTATUS_LEVEL_MODERATE,
	LPCSTATUS_LEVEL_LOW,
	LPCSTATUS_LEVEL_MINIMAL,
	LPCSTATUS_LEVEL_NONE,
};

class WTFHelper
{
public:
	virtual QByteArray currentLanguageCode() = 0;
	virtual void reportLPCStatus(int status) = 0;
	virtual void changeLanguage() = 0;
	virtual QString translateText(const char *code) = 0;
	virtual void queryRouterAuth(QString& username, QString& password) = 0;
	virtual void reportLPCEnableDisable(bool enable) {}
	virtual void reportLPCLevel(int level) {}
	virtual void reportLPCBasicSettingClick() {}
	virtual void reportLPCCustomSettingClick() {}
	virtual QString querySmartNetworkBaseUrl() { return QLatin1String("https://appgenie.netgear.com"); }
};

class LPCNavPage;
class LPCNavPopup;

class AppData
{
public:
	QString m_username;
	QString m_password;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_deviceId;
	QString m_userToken;
	QString m_model;
	bool m_inChangeBasicSettings;

	AppData();
	~AppData();
	bool needSetup(const QString& deviceId) const;
	bool needShowAbortButton() const;
	void load();
	void save();
	void reset();
};

class LPCMainFrame
	: public QFrame
{
	Q_OBJECT

public:
	LPCMainFrame(WTFHelper *helper, QWidget *parent);
	virtual ~LPCMainFrame();

	void detachEnv();

	void onActivate(bool active);
	void push(LPCNavPopup *popup);
	void pop(LPCNavPopup *popup);
	void jumpSmart(const QString& cpid, const QString& smartNetworkUsername, const QString& smartNetworkPassword, const QString& routerUsername, const QString& routerPassword);

	void requestNavigateTo(const QString& pageName, bool force);

	AppData *appData() const { return m_appData; }
	void updateAppDataRouterAuth();
	QNetworkAccessManager *networkAccessManager() const { return m_nam; }
	void wait(AsyncOp *op, const QObject *receiver, const char *method, const QString& promptText = QString());
	void wait(AsyncOp *op, const QObject *receiver, const char *method, const char *promptId);

	QString ttt(const char *sourceText, const char *disambiguation = 0, int n = -1);

	virtual void reportStatus(int status);
	virtual void reportLPCEnableDisable(bool enable);
	virtual void reportLPCLevel(int level);
	virtual void reportLPCBasicSettingClick();
	virtual void reportLPCCustomSettingClick();

	CoreApi *coreApi() const;

signals:
	void internalRequestNavigateTo(const QString& pageName, bool force);

public slots:
	void navigateTo(const QString& pageName, bool force = false);

protected:
	virtual void changeEvent(QEvent *e);

private:
	void changeLanguage();
	QWidget *loadUI(const QString& name, QWidget *parentWidget);

private:
	int m_lpcStatus;
	AppData *m_appData;
	QNetworkAccessManager *m_nam;
	SimpleTranslator *m_translator;
	CustomUILoader *m_uiLoader;
	WTFHelper *m_helper;
	QStackedLayout *m_rootLayout;
	QStackedLayout *m_navPane;
	QPointer<LPCNavPage> m_activePage;
	QString m_activePageName;

	bool m_bFirstActivate;
	CoreApi *m_coreApi;
};

class LPCPopupHost
	: public QFrame
{
	Q_OBJECT

public:
	LPCPopupHost(LPCNavPopup *child, QWidget *parent);
	virtual ~LPCPopupHost();

	void retranslateUI();

private:
	LPCNavPopup *m_child;
};

class LPCPopupFrame
	: public QFrame
{
	Q_OBJECT

public:
	LPCPopupFrame(LPCNavPopup *content, QWidget *parent);
	virtual ~LPCPopupFrame();

	void changeCaption(const QString& caption);

protected:
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

private:
	QLabel *m_title;
	bool m_tracking;
	int m_dx;
	int m_dy;
};

#include "LPCNavPopup.h"

class InternalWaitPopup
	: public LPCNavPopup
{
	Q_OBJECT

public:
	InternalWaitPopup(AsyncOp *op, const QObject *receiver, const char *method, const QString& promptText, QWidget *parent);
	InternalWaitPopup(AsyncOp *op, const QObject *receiver, const char *method, const char *promptId, QWidget *parent);
	virtual ~InternalWaitPopup();

	virtual void retranslateUI();

signals:
	void internalClose(AsyncOp *op, bool waitOk);

private slots:
	void onAsyncOpFinished();
	void onAbortButtonClicked();

private:
	void initCommon();

private:
	AsyncOp *m_op;
	QPushButton *m_btnAbort;
	QByteArray m_promptId;
	QLabel *m_promptLabel;
};

#endif // __LPCMainFrame_h__

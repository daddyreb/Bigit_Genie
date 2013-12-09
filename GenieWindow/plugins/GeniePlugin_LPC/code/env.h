#ifndef __env_h__
#define __env_h__

#include "LPCMainFrame.h"
#include "LPCNavPopup.h"
#include <QtGui/QApplication>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

class HomeApp
	: public QApplication
{
	Q_OBJECT
	
public:
	HomeApp(int& argc, char **argv);
	virtual ~HomeApp();

	int run();

private:
};

class HomeWindow
	: public QWidget
	, public WTFHelper
{
	Q_OBJECT

public:
	HomeWindow();
	virtual ~HomeWindow();

	virtual QByteArray currentLanguageCode();
	virtual void reportLPCStatus(int status);
	virtual void changeLanguage();
	virtual QString translateText(const char *code);
	virtual void queryRouterAuth(QString& username, QString& password);

private slots:
	void onSwitchTab();
	void onBigButtonClicked();
	void onLanguageAction();
	void onPopup1();
	void onPopup2();
	void onNav();

private:
	QTabWidget *m_tab;
	LPCMainFrame *m_lpcFrame;
	QByteArray m_langCode;
	SimpleTranslator *m_translator;
};

class TestPopupA
	: public LPCNavPopup
{
	Q_OBJECT

public:
	TestPopupA(QWidget *parent);
	virtual ~TestPopupA();
};

class TestPopupB
	: public LPCNavPopup
{
	Q_OBJECT

public:
	TestPopupB(QWidget *parent);
	virtual ~TestPopupB();

private slots:
	void test();
};

#endif // __env_h__

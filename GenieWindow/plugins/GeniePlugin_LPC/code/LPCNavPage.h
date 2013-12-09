#ifndef __LPCNavPage_h__
#define __LPCNavPage_h__

#include <QFrame>

class AppData;
class LPCMainFrame;
class CoreApi;
class WebApi;
class RouterApi;

class LPCNavPage
	: public QFrame
{
	Q_OBJECT

public:
	LPCNavPage(QWidget *parent);
	virtual ~LPCNavPage();

	void init(LPCMainFrame *mainFrame);

	bool prepare();
	bool load() { return loadImpl(); }
	void unload() { unloadImpl(); }

	virtual void retranslateUI();

	QString ttt(const char *sourceText, const char *disambiguation = 0, int n = -1);

	LPCMainFrame *app() const { return m_mainFrame; }
	AppData *appData() const;

	CoreApi *coreApi() const;
	WebApi *webApi() const;
	RouterApi *routerApi() const;

	void setLPCText(QWidget *widget, const char *text, bool markup = false);

	void reportStatus(int status);

public slots:
	void gotoMainPage();

protected:
	virtual bool prepareImpl() { return true; }
	virtual bool loadImpl() { return true; }
	virtual void unloadImpl() {}

	void navigateTo(const char *pageName, bool force = true);

private:
	void updateTextUI(QWidget *widget);
	void updateWidgetText(QWidget *widget, const QString& text);
	void updateTextUIWork(QWidget *widget);

private:
	LPCMainFrame *m_mainFrame;
};

#endif // __LPCNavPage_h__

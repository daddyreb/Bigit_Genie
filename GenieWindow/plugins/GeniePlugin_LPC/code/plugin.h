#ifndef __plugin_h__
#define __plugin_h__

#include "LPCMainFrame.h"
#include <QtCore/QPointer>
#include <QtCore/QMap>
#include <QtCore/QTimer>

#include "genie2_interface.h"

class LPCPlugin
	: public QObject
        , public IPluginInf2
	, public WTFHelper
{
	Q_OBJECT
        Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_LPC.json")

public:
	LPCPlugin();
	virtual ~LPCPlugin();

        virtual QVariant command(int cmd,const QVariant &paras);

	virtual QByteArray currentLanguageCode();
	virtual void reportLPCStatus(int status);
	virtual void changeLanguage();
	virtual QString translateText(const char *code);
	virtual void queryRouterAuth(QString& username, QString& password);
	virtual void reportLPCEnableDisable(bool enable);
	virtual void reportLPCLevel(int level);
	virtual void reportLPCBasicSettingClick();
	virtual void reportLPCCustomSettingClick();
	virtual QString querySmartNetworkBaseUrl();

private slots:
	void restartSlot();

private:
	void restart();

private:
	QPointer<LPCMainFrame> m_mainFrame;
	QMap<QByteArray, int> m_i18nMap;
	QUuid m_uuid;
	QTimer m_jumpTimer;
	bool m_pendingRestart;
};

#endif // __plugin_h__

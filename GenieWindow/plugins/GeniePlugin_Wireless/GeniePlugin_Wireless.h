#ifndef GENIEPLUGIN_WIRELESS_H
#define GENIEPLUGIN_WIRELESS_H

#include <QObject>
#include "IPluginInf2.h"
#include "language_Wireless.h"


typedef enum WIRELESS_STATE_tag
{
    WIRELESS_CONNECTED,
    WIRELESS_NOTCONNECTED,
    WIRELESS_NOTENABLED
}WIRELESS_STATE;

class QGenieWirelessTabWidget;
class GenieReconnectThread;

class GeniePlugin_Wireless : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Wireless.json")
public:
            GeniePlugin_Wireless();
    virtual ~GeniePlugin_Wireless();
private:
    QGenieWirelessTabWidget         *m_widget;
#ifdef Q_OS_MACX
	GenieReconnectThread *mReconnectThread;
#endif
protected:
    static IPluginInf2              *m_single_instance;
public:
    virtual QVariant command(int cmd,const QVariant &paras=QVariant());
public:
    static IPluginInf2* single_instance();
    static QString translateText(int idx);
//    static void function_usage_statistics(const QString &fun , int value = -1);
    static void output_log(const QString &log);
    static QString m_s_ssid;
protected:
#ifdef Q_OS_MACX
    void reconnect_airport(const QString &ssid,const QString &pwd);
#endif

private:
    QString m_routerModel;
    bool m_rcfgBusy;
};

#endif // GENIEPLUGIN_WIRELESS_H

#ifndef GENIEPLUGIN_INTERNET_H
#define GENIEPLUGIN_INTERNET_H

#include <QObject>
#include "IPluginInf2.h"
#include "QGenieNetworkDiagnose.h"
#include "language_Internet.h"


#ifdef GENIE_ENABLE_NET_MONITOR
#include "QGenieNetMonitor.h"
#endif // GENIE_ENABLE_NET_MONITOR

typedef enum INTERNET_STATE_tag
{
    INTERNET_CONNECTED,
    INTERNET_DISCONNECTED,
    INTERNET_OVERFLOW
}INTERNET_STATE;

class GeniePlugin_Internet : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Internet.json")
public:
    GeniePlugin_Internet();
    virtual ~GeniePlugin_Internet(){}
protected:
    QGenieNetworkDiagnose                   *net_diagnose;
    static    IPluginInf2                   *m_single_instance;
public:
    virtual QVariant command(int cmd,const QVariant &paras=QVariant());
 public:
     static IPluginInf2 *single_instance();
     static QString translateText(int idx);
     static void function_usage_statistics(const QString &fun);
     static void output_log(const QString &log);
     //static QString infomanager_getinfo(int what,int para=0);

#ifdef GENIE_ENABLE_NET_MONITOR
public:
	static NetMonitor *getNetMonitor();
private slots:
	void checkNetMonitor();
	void forceRestartNetMonitor();
private:
	NetMonitor m_netMonitor;
	QTimer m_nmTestTimer;
	QTimer m_nmForceTimer;
#endif // GENIE_ENABLE_NET_MONITOR
};

#endif // GENIEPLUGIN_INTERNET_H

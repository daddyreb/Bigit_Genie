#ifndef GeniePlugin_WSetupPlugin_H
#define GeniePlugin_WSetupPlugin_H

#include <QObject>
#include "IPluginInf2.h"
#include "language_WirelessExport.h"


class QGenieWirelessSetupWizard;

class GeniePlugin_WSetupPlugin : public QObject,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_WirelessExport.json")
public:
    GeniePlugin_WSetupPlugin();
    virtual ~GeniePlugin_WSetupPlugin(){}
public:
    virtual QVariant command(int cmd,const QVariant &paras=QVariant());
public:
   //for wlan export
   static QString           m_s_ssid;
   static QString           m_s_pwd;
   static QString           m_s_auth;
   static QString           m_s_cipher;
public:
   static QString translateText(int idx);
   static void fun_statistics(const QString & fun);
   static void output_log(const QString &log);

private:
   QGenieWirelessSetupWizard *m_mainExportWizard;
   QGenieWirelessSetupWizard *m_guestExportWizard;
};

#endif // GeniePlugin_WSetupPlugin_H

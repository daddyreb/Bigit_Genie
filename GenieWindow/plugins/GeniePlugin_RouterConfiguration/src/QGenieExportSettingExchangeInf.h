#ifndef QGENIEEXPORTSETTINGEXCHANGEINF_H
#define QGENIEEXPORTSETTINGEXCHANGEINF_H
class IPluginInf;

class QGenieExportSettingExchangeInf
{
public:
    virtual void set_settingplugininf(IPluginInf *inf)=0;
};

Q_DECLARE_INTERFACE(QGenieExportSettingExchangeInf,"com.dragonflow.GeniePlugin.QGenieExportSettingExchangeInf/1.0")
#endif // QGENIEEXPORTSETTINGEXCHANGEINF_H

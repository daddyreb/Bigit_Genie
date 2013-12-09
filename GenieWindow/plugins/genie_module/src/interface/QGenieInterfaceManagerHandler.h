#ifndef QGENIEINTERFACEMANAGERHANDLER_H
#define QGENIEINTERFACEMANAGERHANDLER_H

class QGenieInterfaceManagerHandler:public IManagerHandler
        , public Singleton<QGenieInterfaceManagerHandler>
{
public:
    QGenieInterfaceManagerHandler();
    virtual IClickManager *getClickManager();
    virtual IInfoManager *getInfoManager();
    virtual IToyManager *getToyManager();
    virtual ITrayManager *getTrayManager();
    virtual IUiManager *getUiManager();
    virtual ISoapManager *getSoapManager();
    virtual ILanguageManager *getLanguageManager();
    virtual ILogManager *getLogManager();
    virtual ISettings *getSettings();
#ifdef GENIE_MM
    virtual IMobileManager *getMobileManager();
#endif
};

#endif // QGENIEINTERFACEMANAGERHANDLER_H

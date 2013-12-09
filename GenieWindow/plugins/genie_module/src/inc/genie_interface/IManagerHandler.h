#ifndef IMANAGERHANDLER_H
#define IMANAGERHANDLER_H

class IClickManager;
class IInfoManager;
class IToyManager;
class ITrayManager;
class IUiManager;
class ISoapManager;
class ILanguageManager;
class ILogManager;
class ISettings;
class IMobileManager;

//get the handler frame this class,all the function to control the main frame begin
//from this
class IManagerHandler
{
public:
virtual IClickManager *getClickManager()=0;
virtual IInfoManager *getInfoManager()=0;
virtual IToyManager *getToyManager()=0;
virtual ITrayManager *getTrayManager()=0;
virtual IUiManager *getUiManager()=0;
virtual ISoapManager *getSoapManager()=0;
virtual ILanguageManager *getLanguageManager()=0;
virtual ILogManager *getLogManager()=0;
virtual ISettings *getSettings()=0;
#ifdef GENIE_MM
    virtual IMobileManager *getMobileManager()=0;
#endif
};


#endif

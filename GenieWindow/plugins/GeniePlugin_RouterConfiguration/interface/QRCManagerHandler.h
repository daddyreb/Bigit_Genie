#ifndef QRCMANAGERHANDLER_H
#define QRCMANAGERHANDLER_H
#include "IManagerHandler.h"
#include "QRCUiManager.h"
#include "Singleton.h"

class QRCManagerHandler:public IManagerHandler,public Singleton<QRCManagerHandler>
{
public:
    QRCManagerHandler();
public:
    virtual IClickManager *getClickManager();
    virtual IInfoManager *getInfoManager();
    virtual IToyManager *getToyManager();
    virtual ITrayManager *getTrayManager();
    virtual IUiManager *getUiManager();
    virtual ISoapManager *getSoapManager();
    virtual ILanguageManager *getLanguageManager();
    virtual ILogManager *getLogManager();
    virtual ISettings *getSettings();
    void setBase(IManagerHandler *base);

    IManagerHandler *mBase;
    QRCUiManager mUiManager;

};

#endif // QRCMANAGERHANDLER_H

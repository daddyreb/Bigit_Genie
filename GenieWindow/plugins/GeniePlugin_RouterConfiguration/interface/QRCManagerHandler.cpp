#include <QtCore>
#include "QRCManagerHandler.h"

QRCManagerHandler::QRCManagerHandler()
    :mBase(NULL)
{
}

 IClickManager *QRCManagerHandler::getClickManager()
 {
     if(mBase)
     {
         return mBase->getClickManager();
     }
     return NULL;
 }

 IInfoManager *QRCManagerHandler::getInfoManager()
 {
     if(mBase)
     {
         return mBase->getInfoManager();
     }
     return NULL;

 }

 IToyManager *QRCManagerHandler::getToyManager()
 {
     if(mBase)
     {
         return mBase->getToyManager();
     }
     return NULL;

 }

 ITrayManager *QRCManagerHandler::getTrayManager()
 {
     if(mBase)
     {
         return mBase->getTrayManager();
     }
     return NULL;

 }

 IUiManager *QRCManagerHandler::getUiManager()
 {
//     if(mBase)
//     {
//         return mBase->getUiManager();
//     }
//     return NULL;
     return &mUiManager;

 }

 ISoapManager *QRCManagerHandler::getSoapManager()
 {
     if(mBase)
     {
         return mBase->getSoapManager();
     }
     return NULL;
 }

 ILanguageManager *QRCManagerHandler::getLanguageManager()
 {
     if(mBase)
     {
         return mBase->getLanguageManager();
     }
     return NULL;

 }

 ILogManager *QRCManagerHandler::getLogManager()
 {
    if(mBase)
    {
        return mBase->getLogManager();
    }
    return NULL;
 }

 ISettings *QRCManagerHandler::getSettings()
 {
    if(mBase)
    {
       return mBase->getSettings();
    }
    return NULL;
 }

void QRCManagerHandler::setBase( IManagerHandler *base)
{
    mBase=base;
    mUiManager.setBase(base->getUiManager());
}


#include "stable.h"

QGenieInterfaceManagerHandler::QGenieInterfaceManagerHandler()
{
}


IClickManager *QGenieInterfaceManagerHandler::getClickManager()
{

    QGenieInterfaceClickManager *manager=QGenieInterfaceClickManager::GetSingletonPtr();
    ASSERT(manager);
    return manager;
}
IInfoManager *QGenieInterfaceManagerHandler::getInfoManager()
{
    QGenieInterfaceInfoManager *manager=QGenieInterfaceInfoManager::GetSingletonPtr();
    ASSERT(manager);
    return manager;
}
IToyManager *QGenieInterfaceManagerHandler::getToyManager()
{
    QGenieInterfaceToyManager *manager=QGenieInterfaceToyManager::GetSingletonPtr();
    ASSERT(manager);
    return manager;
}

ITrayManager *QGenieInterfaceManagerHandler::getTrayManager()
{
    QGenieInterfaceTrayManager *manager=QGenieInterfaceTrayManager::GetSingletonPtr();
    ASSERT(manager);
    return manager;
}

IUiManager *QGenieInterfaceManagerHandler::getUiManager()
{
    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uimanager);
    return uimanager;
}

ISoapManager *QGenieInterfaceManagerHandler::getSoapManager()
{
    QGenieInterfaceSoapManager *soapmanager=QGenieInterfaceSoapManager::GetSingletonPtr();
    ASSERT(soapmanager);
    return soapmanager;
}

ILanguageManager *QGenieInterfaceManagerHandler::getLanguageManager()
{
    QGenieInterfaceLanguageManager *lanmanager=QGenieInterfaceLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);
    return lanmanager;

}

ILogManager *QGenieInterfaceManagerHandler::getLogManager()
{
    QGenieInterfaceLogManager *logmanager=QGenieInterfaceLogManager::GetSingletonPtr();
    ASSERT(logmanager);
    return logmanager;

}

ISettings *QGenieInterfaceManagerHandler::getSettings()
{
    QGenieSettings *settings=QGenieSettings::GetSingletonPtr();
    ASSERT(settings);
    return settings;
}

#ifdef GENIE_MM
IMobileManager *QGenieInterfaceManagerHandler::getMobileManager()
{
    QGenieInterfaceMobileManager *mmanager=QGenieInterfaceMobileManager::GetSingletonPtr();
    ASSERT(mmanager);
    return mmanager;
}
#endif


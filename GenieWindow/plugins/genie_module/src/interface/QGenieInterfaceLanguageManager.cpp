#include "stable.h"

QGenieInterfaceLanguageManager::QGenieInterfaceLanguageManager()
{
}

QString QGenieInterfaceLanguageManager::getText(int idx,QUuid uuid)
{
    QGenieLanguageManager * languagemanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(languagemanager);

    return languagemanager->getText(idx,uuid);

}

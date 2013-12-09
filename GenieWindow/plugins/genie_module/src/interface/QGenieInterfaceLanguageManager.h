#ifndef QGENIEINTERFACELANGUAGEMANAGER_H
#define QGENIEINTERFACELANGUAGEMANAGER_H


#define DEFINE_LANGUAGE_MANAGER     QGenieInterfaceLanguageManager *__lanmanager_jfdahkldafd=QGenieInterfaceLanguageManager::GetSingletonPtr();\
            ASSERT(__lanmanager_jfdahkldafd);\
                    const QUuid _uuidllkkjk(FRAME_UUID);
#define SET_TEXT(what,textid) do{(what)->setText(__lanmanager_jfdahkldafd->getText(textid,_uuidllkkjk));}while(0)
#define SET_TITLE(what,textid) do{(what)->setWindowTitle(__lanmanager_jfdahkldafd->getText(textid,_uuidllkkjk));}while(0)
#define SET_LABEL_TEXT(what,textid,uuid) do{(what)->setText(__lanmanager_jfdahkldafd->getText(textid,uuid));}while(0)
#define GET_TEXT(textid) (__lanmanager_jfdahkldafd->getText(textid,_uuidllkkjk))

class QGenieInterfaceLanguageManager:public ILanguageManager,public Singleton<QGenieInterfaceLanguageManager>
{
public:
    QGenieInterfaceLanguageManager();
    QString getText(int idx,QUuid uuid);
};

#endif // QGENIEINTERFACELANGUAGEMANAGER_H

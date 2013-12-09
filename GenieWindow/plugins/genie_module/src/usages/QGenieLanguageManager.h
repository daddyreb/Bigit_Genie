#ifndef __QLANGUAGEMANAGER__
#define __QLANGUAGEMANAGER__


enum E_LANGUAGE_TYPE{
    ELT_ENGLISH=0,
    ELT_GERMANY,
    ELT_SWEDISH,
    ELT_FRENCH,
    ELT_DUTCH,
    ELT_ITALIAN,
    ELT_SPANISH,
    ELT_DANISH,
    ELT_FINNISH,
    ELT_NORWEGIAN,
    ELT_GREEK,
    ELT_PORTUGUESE,
    ELT_RUSSIAN,
    ELT_POLISH,
    ELT_HUNGARIAN,
    ELT_CZECH,
    ELT_SLOVENIAN,
    ELT_SLOVAK,
    ELT_ROMANIAN,
   // ELT_BULGARIAN,
  //  ELT_CROATIAN,
    ELT_CHINESE,
    ELT_KOREAN,
   // ELT_JAPANESE,
    ELT_ARABIC,
    ELT_NUM,
    ELT_ERROR
};
#include "stable.h"

class QGenieInerTextManager;


class QGenieLanguageManager:public QObject,public Singleton<QGenieLanguageManager>
{
    Q_OBJECT
public:
    QGenieLanguageManager();
    ~QGenieLanguageManager();
    void setLanguage(E_LANGUAGE_TYPE type,bool withmessage=true);
   QTranslator *mTranslator;
    QMap<int, QString> mStrMap;
    void dumpMap(const QMap<int ,QString> & map);
    void initDefaultMap();

    void prepareMap(QString filename,QMap<int,QString> & result,int lantype);
    QString getText(int idx,QUuid uuid);
    E_LANGUAGE_TYPE mCurrentType;
    QMap<QUuid,E_LANGUAGE_TYPE> mTypeCache;
    void unloadPi(IPluginInf *pi);
    QGenieInerTextManager *mInnerTextManager;
    QString getTypeStr();
    void initLanguageSetting_step1();
    void initLanguageSetting_step2();
protected:
    int getSysLanIdx() const;
    void setLayoutDirection(int type);

protected slots:
    void slot_setLanguage(int idx);
    void slot_setLayoutDirWithCurType();
};

#endif

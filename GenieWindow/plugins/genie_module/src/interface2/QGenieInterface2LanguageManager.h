#ifndef QGENIEINTERFACE2LANGUAGEMANAGER_H
#define QGENIEINTERFACE2LANGUAGEMANAGER_H
#include <QObject>
#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include "Singleton.h"


enum WIDGET_TYPE
{
    WT_LABEL
    ,WT_EDITOR
    ,WT_BUTTON
    ,WT_CHECKBOX
    ,WT_RADIOBUT
};

class _tTranslateDiscription
{
    void *w;
    short type;
    short uuid;
    int lan_idx;
    QString mode;

    friend class QGenieInterface2LanguageManager;
};

class QGenieInterface2LanguageManager:public QObject,public Singleton<QGenieInterface2LanguageManager>
{
    Q_OBJECT
public:
    QGenieInterface2LanguageManager();
    ~QGenieInterface2LanguageManager();
    inline int currentLan()
    {
        return mLan;
    }
    const char *currentLanShortStr();

    void translateAll();

    QString getText(int uuid,int lan_idx);

    QVariant processCmd(int cmd ,int uuid,const QVariant &para);
    void setTranslateData(void *w,int type,int uuid,int lan_idx,const QString &mode=QString());
protected:
    void loadMap(const QString &fname,QMap<int,QString> &map);
    void setLayoutDirection();
    void translateItem(_tTranslateDiscription *item);
    int mLan;
    QMap<int,QMap<int,QString> > mCache;

    QMap<void *,_tTranslateDiscription *> mLabelTranslateData;
public slots:
    void slot_WidgetDestroyd(QObject*);
    void slot_setLanguage(int i);
};

#endif // QGENIEINTERFACE2LANGUAGEMANAGER_H

#ifndef QGENIEBIGBUTTONMANAGER_H
#define QGENIEBIGBUTTONMANAGER_H
#ifndef IN_STABLE_H
#include <QObject>
#include "Singleton.h"
#include "QGenieButtonBig.h"
#include <QList>
#include <QTime>
#endif
#define BBT_NUM 6


class QGenieButtonBig;
class QGeniePagePluginProxy;
class QGenieButtonStackViewController;

class QGenieBigButtonManager:public QObject,public Singleton<QGenieBigButtonManager>

{
    Q_OBJECT

public:
    QGenieBigButtonManager();
    void registBigButton(QGenieButtonBig *b);
    void realRegistBigButton();;
    void setType(int idx,QString s);
//    void loadStyleSheet();
    QGenieButtonBig *getBigButton(int idx);
    void setLink(int idx,QGeniePagePluginProxy *w,QGenieButtonStackViewController *leftbutton);
//    void updateAllState(bool force=false);
//    //force update state when home page up

//    void updateState(int idx);
//    void updateState(QGenieButtonBig *but,bool force=false);

protected:
    QGenieButtonBig *mBigButtons[BBT_NUM];

    //because when register buttons in button's constructor,the property have not be set by frame then,
    //so we delay the register process here
    QList<QGenieButtonBig *> mBigButtonsCache;
    QTime mTime;


protected slots:
//    void slot_SetState(int idx,int state);
//    void slot_updateAllState();
    friend class QGenieButtonBig;
};

#endif // QGenieBigButtonManager_H

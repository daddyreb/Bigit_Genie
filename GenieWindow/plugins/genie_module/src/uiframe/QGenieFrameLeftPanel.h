#ifndef QGENIEFRAMELEFTPANEL_H
#define QGENIEFRAMELEFTPANEL_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QFrame>
#include "QGenieButtonStackViewController.h"
#include "Singleton.h"
#include <QEvent>
#include <QHash>
#ifdef ENABLE_MULTI_LEFT_BUTTON
#include <QMap>
#endif
#include "ui_QGenieFrameLeftPanel.h"
#endif


class plug_inf;


//class QGenieDynamicLeftPanel;
class QPushButton;
class QGenieButtonStackViewController;
class QGenieFrameLeftPanel:public QFrame,public Singleton<QGenieFrameLeftPanel>
{
    Q_OBJECT
public:
    QGenieFrameLeftPanel(QWidget *parent=NULL);
    void addButton(QGenieButtonStackViewController *bt,QWidget *w);
#ifdef ENABLE_MULTI_LEFT_BUTTON
    QList<QGenieButtonStackViewController *> createButton(plug_inf *p,QString text,QWidget *linkW);
    void updateCachedLeftButtonUi();
#else
    QGenieButtonStackViewController *createButton(QString text,QWidget *linkW);
#endif

#ifdef UNLOAD_PI
    void unloadPi(IPluginInf *pi);
#endif
    void changeStackIdx(int idx);
protected:
    void changeEvent(QEvent* event);
    QHash<QWidget *,QPushButton *> mButtonHash;
    int mPageNum;
#ifdef ENABLE_MULTI_LEFT_BUTTON
    QMap<int,QGenieButtonStackViewController *>         mPrioLeftButMap;
    QMap<QGenieButtonStackViewController *,QWidget *>   mLeftButlinkWMap;
#endif

private:
    Ui::QGenieFrameLeftPanel mUi;
protected slots:
    void slot_ShowLast();
    void slot_ShowNext();
};

#endif // QGENIEFRAMELEFTPANEL_H

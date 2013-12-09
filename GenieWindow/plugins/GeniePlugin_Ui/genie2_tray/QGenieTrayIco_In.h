#ifndef QGENIETRAYICO_IN_H
#define QGENIETRAYICO_IN_H
class QGenieSelfTrayMessage;
class QAction;

#include <QObject>
#include <QtGui>
#include <QCheckBox>
#include <QtWidgets>
#include "QGenieTrayManager.h"
#include "IGenieTrayIco.h"

class QGenieDialogExit;
class QGenieSelfTrayMessage;
class QGenieTrayManager;


class QGenieTrayIco_In:public QObject,public IGenieTrayIco
{
    Q_OBJECT
public:
    QGenieTrayIco_In(QGenieTrayManager*manager);
    ~QGenieTrayIco_In();

    void uiCommand(int cmd,const QString &para=QString());


//    void slot_ButtonClicked();
//    void slot_ActChecked(bool);
//    void slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason);

//    void startup();
protected:
    QMenu * _createMenu(const QDomElement &domelem);
    QMenu * findMenu(const QString &objname);
    QAction *findAction(const QString &objname);
    void createTrayMenu(const QString &uiFName);
    void translateUi(const QString &paras);
    void setActCheckState(const QString &paras);
protected:
    QList<QMenu *> mMenuCache;
    QList<QAction *> mActCache;
    QGenieSelfTrayMessage *mSelfTrayMessage;
    QMenu *mTrayMenu;
    QSystemTrayIcon *mTrayIcon;
    QGenieTrayManager* mTrayManager;
protected slots:
    void slot_ActionTrigged(QAction *act);
    void slot_AboutToShow();
//    void slot_EmitInitMessage();
    void slot_TrayActivated(QSystemTrayIcon::ActivationReason);
    friend class QGenieTrayManager;
};

#endif // QGENIETRAYICO_IN_H

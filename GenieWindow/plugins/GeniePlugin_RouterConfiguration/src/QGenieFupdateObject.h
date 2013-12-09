#ifndef QGENIEFUPDATEOBJECT_H
#define QGENIEFUPDATEOBJECT_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class QPiObject;

class QGenieFupdateObject : public QObject,public Singleton<QGenieFupdateObject>
{
    Q_OBJECT
public:
    explicit QGenieFupdateObject(QObject *parent = 0);
    ~QGenieFupdateObject();
    QWidget *getUi();
    void whenLogin();
    void changePage(const QString &name);
    void translateUi();
    void wantRefreshData();
    void tryRefreshData();
    void tryUpdateFirware();//added by hu
protected:

    void setVersionStr();
    void setFinishPageStr(int lan1,int lan2,int lan3=-1);
    void setUpdatePageStr(int lan1,int lan2);//hu
    void setRestartPageStr(int lan1,int lan2);//hu



signals:

public slots:
    void slot_ButClicked();
    void slot_DataReturned(int session, const QStringList &lst);
    void slot_DataReturned2(int session, const QStringList &lst);//added by hu
    void slot_Timeout();
	void slot_UpdateFinish();
protected:
    QWidget *mW;
    QStackedWidget *mStack;
    QPiObject *mPi;
    QPiObject *mPi2;//hu
    QPiObject *mPi3;//hu

    QPushButton *but_l;
    QPushButton *but_r;
    QPushButton *but_finish;
    QPushButton *but_restart;

    QString mCurV;
    QString mNewV;
    QTimer mTimer;
    int mRetryTimes;

    int mLanFinish1;
    int mLanFinish2;
    int mLanFinish3;
    int mSession;
//    int mBehindSession;

};

#endif // QGENIEFUPDATEOBJECT_H

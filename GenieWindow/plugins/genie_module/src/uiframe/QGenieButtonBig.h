#ifndef QGENIEBUTTONBIG_H
#define QGENIEBUTTONBIG_H
#ifndef IN_STABLE_H
#include "QGenieButtonStackViewController.h"
#include <QVector>
#include "ui_QGenieButtonBig.h"
#include "IPluginInf.h"
#endif

class QGenieButtonStackViewController;


enum BIG_BUTTON_STATUS {
    BBS_GOOD,
    BBS_WARRING,
    BBS_ERROR,
    BBS_WAITING,
    BBS_USER,
    BBS_USER2,
    BBS_NUM
};

enum USER_TEXT_CACHE
{
    UTC_GOOD_LEFT=0,
    UTC_GOOD_RIGHT,
    UTC_WARRING_LEFT,
    UTC_WARRING_RIGHT,
    UTC_ERROR_LEFT,
    UTC_ERROR_RIGHT,
    UTC_WAITING_LEFT,
    UTC_WAITING_RIGHT,
    UTC_USER_LEFT,
    UTC_USER_RIGHT,
    UTC_TEXT,
    UTC_NUM
};

//enum STATE_UPDATE_POLICY
//{
//    SUP_NO_UPDATE,
//    SUP_BY_FRAME,
//    SUP_BY_PLUGIN
//};


class QGenieButtonBig : public QGenieButtonStackViewController
{
    Q_OBJECT
public:
    QGenieButtonBig(QWidget *parent=NULL);
  //  void setState(BIG_BUTTON_STATUS st);
    void setSubState(ENUM_BIGBT_SUB_STATE st);
    void setBackGroundIco(const QString &ico);
    void setType(const QString & tp);
//    void loadStyleSheet();
    void parseConfigString(const QString &s);
    virtual void click();
//    STATE_UPDATE_POLICY mUpdatePolicy;
    void setUserStyle(const QString &s);
    void setBKG(const QString &s);
protected:
    Ui::QGenieButtonBig mUi;
    void changeText();
//	BIG_BUTTON_STATUS mState;
//	QString mType;
//     int mUpdateFrequency;
//     int mUpdateLastTime;
//     bool canUpdateStateNow(int msec) const;
 //    int mTitleIdx;

protected:
    void changeEvent(QEvent* event);
    friend class QGenieBigButtonManager;
    int mLanIdx;
//    QVector<int> mUserTextCache;
//    QVector<bool> mUserTextFlag;
  //  friend class QGeniePluginProxyManager;
private slots:
    void slot_NotifySubState();
//    void on_sub_but_error_clicked();
};

#endif // QGENIEBUTTONBIG_H

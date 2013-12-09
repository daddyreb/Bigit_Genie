#ifndef QGENIERCPAGEGUESSACCESS_H
#define QGENIERCPAGEGUESSACCESS_H

#include <QWidget>
#include <QTimer>
#include "Singleton.h"
#include "qtsoap.h"
#include <QtCore>
#include <QtGui>
#include "soapclient.h"

//class QThreadIsNetgear;
class WidgetTestUi;

namespace Ui {
    class QGenieRCPageGuessAccess;
}

class QGenieRCPageGuessAccess : public QWidget ,public ISoapClient,public Singleton<QGenieRCPageGuessAccess>
{
    Q_OBJECT
public:
    enum PENDING_REQUEST
    {
        PR_ENABLE=1<<0,
        PR_SETTING=1<<1
    };

    QGenieRCPageGuessAccess(QWidget *parent = 0);
    ~QGenieRCPageGuessAccess();
    void refresh(int delay=0);
    void startUiRefresh();
    void stopUiRefresh(bool haveError);
    void showError(int lanidx );
    void hideError();
    virtual void parseSoapResponse(int cmd,QtSoapMessage &message);
    virtual void whenSoapError(int cmd,const QString& reson );
    void askWhenGaccTimeoutThenSoapReturn();
    inline bool isRefreshing()
    {
        return  mRefreshTimer.isActive();
    }
//

    void notifyMac(QString mac);
    QString mMac;
    void refreshWhenChangeIdxJustAfterLogin();
    QRadioButton *mEnabledSaved;
    QRadioButton *mWpaCur;
    QString mSsidSaved;
    QString mKeySaved;

    bool mJustLogin;

    QTimer mTimerToDisable;

protected:
    void changeEvent(QEvent *e);
public:
    void clearText();
    void whenLogin();
    Ui::QGenieRCPageGuessAccess *ui;
    QTimer mRefreshTimer;
    QTimer mRefreshDelay;
    QTimer mApplyDelay;
    int mErrorIdx;
    int mEnableIdx;

    void apply(int delay);
    void setGuestAccessPara();
    void startSettingMode();
    void endSettingMode();
    void setUiEnableState(bool enable);
protected slots:
    void slot_timeout();
    void slot_refresh();
    void on_but_gacc_apply_clicked();
    void slot_SetApplyButtonState();
    void on_rdt_enable_clicked(bool ck);
    void on_rdt_disable_clicked(bool ck);
    void on_but_gacc_modify_clicked();
    void on_but_set_gacc_cancel_clicked();
    void on_but_info_gacc_cancel_clicked();
    void slot_timeoutDisable ();
    void slot_refreshDelayTimeout();
    void slot_applyDelayTimeout();
    friend class WidgetTestUi;
	friend class QGenieRCLogic;

};

#endif // QGENIERCPAGEGUESSACCESS_H

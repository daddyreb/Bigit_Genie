#ifndef SMARTWIRELESSSETTINGWIZARD_H
#define SMARTWIRELESSSETTINGWIZARD_H

#include <QDialog>
#include "mainpipedefines.h"

namespace Ui {
class SmartWirelessSettingWizard;
}

class EnterWirelessSettingPage;
class WirelessSettingCopyRouterSettingPage;
class QEvent;
class QMouseEvent;

class SmartWirelessSettingWizard : public QDialog
{
    Q_OBJECT
private:
    enum{
        SWSW_ENTER_PAGE
        ,SWSW_COPY_PAGE
    };

public:
    explicit SmartWirelessSettingWizard(QWidget *parent = 0);
    ~SmartWirelessSettingWizard();

public:
    int begin(const QString &model, const QString &ip, const WLANInfo &wlanInfo);
    void end(bool accept = false);
    bool running() const;

    void notifySearchRouterResult(bool foundRouter, const WLANInfo *wlanInfo);

signals:
    void requestSearchRouter();
    void requestApplySetting(const QString &ip, const WLANInfo *wlanInfo);
    void cancelWizard();

private slots:
    void onSearchRouterClicked();
    void onApplySettingProxy(const WLANInfo *wlanInfo);

private:
    void setDeviceWlanInfo(const QString &model, const QString &ip, const WLANInfo &wlanInfo);
    void centerShow();
    void switch2Page(int pageCode);

protected:
    void changeEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    void retranslateUi();

private:
    Ui::SmartWirelessSettingWizard *ui;
    EnterWirelessSettingPage *m_enterPage;
    WirelessSettingCopyRouterSettingPage *m_copyPage;

private:
    bool m_wizardRunning;
    QString m_devModel;
    QString m_devIp;
    WLANInfo m_wlanInfo;

    QPoint m_pos;
    bool m_canMove;
};

#endif // SMARTWIRELESSSETTINGWIZARD_H

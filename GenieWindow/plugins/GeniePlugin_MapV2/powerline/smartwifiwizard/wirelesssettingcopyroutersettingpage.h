#ifndef WIRELESSSETTINGCOPYROUTERSETTINGPAGE_H
#define WIRELESSSETTINGCOPYROUTERSETTINGPAGE_H

#include <QWidget>
#include "mainpipedefines.h"

namespace Ui {
class WirelessSettingCopyRouterSettingPage;
}

class QEvent;


class WirelessSettingCopyRouterSettingPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit WirelessSettingCopyRouterSettingPage(QWidget *parent = 0);
    ~WirelessSettingCopyRouterSettingPage();
    
public:
    void initWlanInfo(const QString &model, const WLANInfo & wlanInfo);
    void initRouterSettings(const WLANInfo &wlanInfo);

signals:
    void cancelClicked();
    void requestApplySetting(const WLANInfo *wlanInfo);

private slots:
    void onCopyRouterSettings();
    void onSettingValidityUpdate(bool valid);
    void onApplySetting();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private:
    Ui::WirelessSettingCopyRouterSettingPage *ui;

private:
    WLANInfo m_routerSettings;
    QString m_devModel;
};

#endif // WIRELESSSETTINGCOPYROUTERSETTINGPAGE_H

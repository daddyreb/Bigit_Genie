#ifndef ENTERWIRELESSSETTINGPAGE_H
#define ENTERWIRELESSSETTINGPAGE_H

#include <QWidget>
#include "mainpipedefines.h"

namespace Ui {
class EnterWirelessSettingPage;
}

class QEvent;

class EnterWirelessSettingPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit EnterWirelessSettingPage(QWidget *parent = 0);
    ~EnterWirelessSettingPage();
    
public:
    void initWlanInfo(const QString &model, const WLANInfo & wlanInfo);
    void ControlSearchRouterUi(bool startOrStop);

signals:
    void cancelClicked();
    void searchRouterClicked();
    void requestApplySetting(const WLANInfo *wlanInfo);

private slots:
    void onSettingValidityUpdate(bool valid);
    void onApplySetting();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private:
    Ui::EnterWirelessSettingPage *ui;
    QString m_devModel;
};

#endif // ENTERWIRELESSSETTINGPAGE_H

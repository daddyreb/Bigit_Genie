#ifndef WIRELESSSETTINGREADONLYPANNEL_H
#define WIRELESSSETTINGREADONLYPANNEL_H

#include <QWidget>
#include "mainpipedefines.h"

namespace Ui {
class WirelessSettingReadonlyPannel;
}

class QEvent;

class WirelessSettingReadonlyPannel : public QWidget
{
    Q_OBJECT
    
public:
    explicit WirelessSettingReadonlyPannel(QWidget *parent = 0);
    ~WirelessSettingReadonlyPannel();

public:
    void initRouterSettings(const WLANInfo &wlanInfo);

signals:
    void copyRouterSettings();

private:
    void updateUiElements();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private slots:
    void btnClicked();

private:
    Ui::WirelessSettingReadonlyPannel *ui;
};

#endif // WIRELESSSETTINGREADONLYPANNEL_H

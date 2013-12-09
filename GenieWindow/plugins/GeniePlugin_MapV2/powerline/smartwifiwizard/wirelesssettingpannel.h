#ifndef WIRELESSSETTINGPANNEL_H
#define WIRELESSSETTINGPANNEL_H

#include <QWidget>
#include "mainpipedefines.h"

namespace Ui {
class WirelessSettingPannel;
}

class QEvent;
class QRegExpValidator;

class WirelessSettingPannel : public QWidget
{
    Q_OBJECT
    
public:
    explicit WirelessSettingPannel(QWidget *parent = 0);
    ~WirelessSettingPannel();

public:
    void initWlanInfo(const WLANInfo & wlanInfo);
    bool isSettingValid() const;
    void retriveSetting(WLANInfo &wlanInfo) const;

signals:
    void settingValidityUpdate(bool valid);

private:
    void processSecurityTypeChanged(const QString &text);
    void processwepAuthTypeChanged(const QString &text);
    void processwepEncryptionStrengthChanged(const QString &text);
    void generateWEPKeys();
    QString wepKeyGenerater(const QString &passphrase, int keyLen, int delta);
    void updateUiElements();
    void updateWepKeyValidator();

private slots:
    void onCBCurrentIndexChanged(const QString &text);
    void onLETextChanged(const QString &text);
    void onRBToggled(bool checked);
    void btnClicked();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private:
    Ui::WirelessSettingPannel *ui;
    QRegExpValidator *m_wepKeyValidator;
};

#endif // WIRELESSSETTINGPANNEL_H

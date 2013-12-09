#include "wirelesssettingreadonlypannel.h"
#include "ui_wirelesssettingreadonlypannel.h"
#include <QMap>
#include <QEvent>
#include "language_i18n.h"

WirelessSettingReadonlyPannel::WirelessSettingReadonlyPannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WirelessSettingReadonlyPannel)
{
    ui->setupUi(this);

    connect(ui->m_copySettingBtn, SIGNAL(clicked()),
            this , SIGNAL(copyRouterSettings()));

    //ui->m_secTypeLB->setText("WEP");

    retranslateUi();
}

WirelessSettingReadonlyPannel::~WirelessSettingReadonlyPannel()
{
    delete ui;
}

void WirelessSettingReadonlyPannel::btnClicked()
{
//    if(0 == ui->m_secTypeLB->text().compare("WEP", Qt::CaseInsensitive)){
//        ui->m_secTypeLB->setText("None");
//    }else{
//        ui->m_secTypeLB->setText("WEP");
//    }

//    updateUiElements();
}

void WirelessSettingReadonlyPannel::updateUiElements()
{
    bool secWEP = (0 == ui->m_secTypeLB->text().compare("WEP", Qt::CaseInsensitive));

    ui->m_authTypeLB->setVisible(secWEP);
    ui->m_authTypeTagLB->setVisible(secWEP);
    ui->m_encryptionLenLB->setVisible(secWEP);
    ui->m_encryptionLenTagLB->setVisible(secWEP);
    ui->m_keyLB->setVisible(secWEP);
    ui->m_keyTagLB->setVisible(secWEP);
}

void WirelessSettingReadonlyPannel::initRouterSettings(const WLANInfo &wlanInfo)
{
    QMap<QString, QString> secvalue2SecStringMap;
    secvalue2SecStringMap["NONE"] = "None";
    secvalue2SecStringMap["WEP"] = "WEP";
    secvalue2SecStringMap["WPA-PSK"] = "WPA-PSK[TKIP]";
    secvalue2SecStringMap["WPA2-PSK"] = "WPA2-PSK[AES]";
    secvalue2SecStringMap["WPA-PSK/WPA2-PSK"] = "WPA-PSK[TKIP]+WPA2-PSK[AES]";

    ui->m_ssidLB->setText(wlanInfo.m_ssid);
    ui->m_secTypeLB->setText(secvalue2SecStringMap.value(wlanInfo.m_basicEncryptionModes.toUpper()));
    ui->m_passphraseLB->setText(wlanInfo.m_wlanPassphrase);

    ui->m_authTypeLB->setText(wlanInfo.m_wepAuthType);
    ui->m_encryptionLenLB->setText((wlanInfo.m_wepKey.length() == 10) ? "64 bits" : "128 bits");
    ui->m_keyLB->setText(wlanInfo.m_wepKey);

    updateUiElements();
}

void WirelessSettingReadonlyPannel::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WirelessSettingReadonlyPannel::retranslateUi()
{
    I18N_TEXT(ui->m_ssidTagLB, Text, L_PLC_WIFI_WIZARD_SSID);
    I18N_TEXT(ui->m_secTypeTagLB, Text, L_PLC_WIFI_WIZARD_SECURITY_TYPE);
    I18N_TEXT(ui->m_authTypeTagLB, Text, L_PLC_WIFI_WIZARD_AUTHENTICATION_TYPE);
    I18N_TEXT(ui->m_encryptionLenTagLB, Text, L_PLC_WIFI_WIZARD_ENCRYPTION_STRENGTH);
    I18N_TEXT(ui->m_keyTagLB, Text, L_PLC_WIFI_WIZARD_WEP_KEY1);
    I18N_TEXT(ui->m_passphraseTagLB,Text, L_PLC_WIFI_WIZARD_PASSPHRASE);
    I18N_TEXT(ui->m_copySettingBtn, Text, L_PLC_WIFI_WIZARD_COPY_ROUTERSETTING_BTN_TEXT);

    ui->m_authTypeTagLB->setText(ui->m_authTypeTagLB->text() + ":");
    ui->m_encryptionLenTagLB->setText(ui->m_encryptionLenTagLB->text() + ":");

    QString tmpText = ui->m_keyTagLB->text().replace("1","");
    tmpText = tmpText.trimmed() + ":";
    ui->m_keyTagLB->setText(tmpText);
    ui->m_passphraseTagLB->setText(ui->m_passphraseTagLB->text() + ":");
}

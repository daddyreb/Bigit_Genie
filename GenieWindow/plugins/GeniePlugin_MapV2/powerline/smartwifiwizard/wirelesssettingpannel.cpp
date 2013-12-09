#include "wirelesssettingpannel.h"
#include "ui_wirelesssettingpannel.h"

#include <QEvent>
#include <QtGlobal>
#include <QRegExpValidator>
#include "language_i18n.h"

#define WEP_KEY_VALIDATOR_REGEXP "([a-f]|[A-F]|[0-9]){%1}"

WirelessSettingPannel::WirelessSettingPannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WirelessSettingPannel)
  ,m_wepKeyValidator(0)
{
    ui->setupUi(this);
    connect(ui->m_wepkeyGenerateBtn, SIGNAL(clicked()), SLOT(btnClicked()));

    const char *basicEncryptions[] = {
        "None"
        ,"WEP"
        ,"WPA-PSK"
        ,"WPA2-PSK"
        ,"WPA-PSK/WPA2-PSK"
    };

    for(size_t idx = 0 ; idx < (sizeof(basicEncryptions) / sizeof(basicEncryptions[0])) ; ++idx){
        ui->m_securityCB->setItemData(idx, QString(basicEncryptions[idx]));
    }

    ui->m_wirelessModePromptPane->setVisible(ui->m_securityCB->currentText().contains("WPA-PSK"));
    updateWepKeyValidator();

    retranslateUi();
}

WirelessSettingPannel::~WirelessSettingPannel()
{
    delete ui;
}

void WirelessSettingPannel::onCBCurrentIndexChanged(const QString &text)
{
    QComboBox *signalSrc = qobject_cast<QComboBox *>(sender());

    if(signalSrc == ui->m_securityCB){
        processSecurityTypeChanged(text);
    }else if(signalSrc == ui->m_wepAuthTypeCB){
        processwepAuthTypeChanged(text);
    }else if(signalSrc == ui->m_wepEncryptionLenCB){
        processwepEncryptionStrengthChanged(text);
    }

    updateUiElements();

}

void WirelessSettingPannel::processSecurityTypeChanged(const QString &text)
{
    if(0 == text.compare("None", Qt::CaseInsensitive)){
        ui->m_passPhraseLB->setVisible(false);
        ui->m_passPhraseLE->setVisible(false);
        ui->m_wepSecurityPane->setVisible(false);
    }else if(0 == text.compare("WEP", Qt::CaseInsensitive)){
        ui->m_passPhraseLB->setVisible(false);
        ui->m_passPhraseLE->setVisible(false);
        ui->m_wepSecurityPane->setVisible(true);
    }else{
        ui->m_passPhraseLB->setVisible(true);
        ui->m_passPhraseLE->setVisible(true);
        ui->m_wepSecurityPane->setVisible(false);

    }

    ui->m_wirelessModePromptPane->setVisible(text.contains("WPA-PSK"));
}

void WirelessSettingPannel::processwepAuthTypeChanged(const QString &text)
{

}

void WirelessSettingPannel::updateWepKeyValidator()
{
    if(0 == m_wepKeyValidator){
        m_wepKeyValidator = new QRegExpValidator(this);
    }

    int lenLimit = (ui->m_wepEncryptionLenCB->currentText().contains("64") ? 10 : 26);

    QString regStr(WEP_KEY_VALIDATOR_REGEXP);
    m_wepKeyValidator->setRegExp(QRegExp(regStr.arg(lenLimit)));

    ui->m_wepkey1LE->setValidator(m_wepKeyValidator);
    ui->m_wepkey2LE->setValidator(m_wepKeyValidator);
    ui->m_wepkey3LE->setValidator(m_wepKeyValidator);
    ui->m_wepkey4LE->setValidator(m_wepKeyValidator);

    ui->m_wepkey1LE->setMaxLength(lenLimit);
    ui->m_wepkey2LE->setMaxLength(lenLimit);
    ui->m_wepkey3LE->setMaxLength(lenLimit);
    ui->m_wepkey4LE->setMaxLength(lenLimit);
}

void WirelessSettingPannel::processwepEncryptionStrengthChanged(const QString &text)
{
    Q_UNUSED(text);

    updateWepKeyValidator();

    ui->m_wepkey1LE->clear();
    ui->m_wepkey2LE->clear();
    ui->m_wepkey3LE->clear();
    ui->m_wepkey4LE->clear();
}

void WirelessSettingPannel::initWlanInfo(const WLANInfo & wlanInfo)
{
    ui->m_ssidLE->setText(wlanInfo.m_ssid);

    const int secItemCount = ui->m_securityCB->count();
    ui->m_securityCB->setCurrentIndex(-1);
    for(int idx = 0 ; idx < secItemCount ; ++idx){
        if(0 == wlanInfo.m_basicEncryptionModes.compare(ui->m_securityCB->itemData(idx).toString(), Qt::CaseInsensitive)){
            ui->m_securityCB->setCurrentIndex(idx);
            break;
        }
    }

    if(0 == wlanInfo.m_basicEncryptionModes.compare("WEP", Qt::CaseInsensitive)){
        ui->m_wepPassphraseLE->setText(wlanInfo.m_wlanPassphrase);
        ui->m_passPhraseLE->clear();

        const int authCount = ui->m_wepAuthTypeCB->count();
        for(int ikx = 0 ; ikx < authCount ; ++ikx){
            if(0 == ui->m_wepAuthTypeCB->itemText(ikx).compare(wlanInfo.m_wepAuthType, Qt::CaseInsensitive)){
                ui->m_wepAuthTypeCB->setCurrentIndex(ikx);
                break;
            }
        }

        const int encryptionLenCount = ui->m_wepEncryptionLenCB->count();
        for(int index = 0 ; index < encryptionLenCount ; ++index){
            if(ui->m_wepEncryptionLenCB->itemText(index).contains((wlanInfo.m_wepKey.length() == 10) ? "64" : "128")){
                ui->m_wepEncryptionLenCB->setCurrentIndex(index);
                break;
            }
        }

        ui->m_wepkey1LE->setText(wlanInfo.m_wepKey);
        ui->m_wepkey2LE->setText(wlanInfo.m_wepKey);
        ui->m_wepkey3LE->setText(wlanInfo.m_wepKey);
        ui->m_wepkey4LE->setText(wlanInfo.m_wepKey);

    }else{

        ui->m_passPhraseLE->setText(wlanInfo.m_wlanPassphrase);
        ui->m_wepPassphraseLE->clear();

        ui->m_wepAuthTypeCB->setCurrentIndex(0);
        ui->m_wepEncryptionLenCB->setCurrentIndex(0);

        ui->m_wepkey1LE->clear();
        ui->m_wepkey2LE->clear();
        ui->m_wepkey3LE->clear();
        ui->m_wepkey4LE->clear();
    }

    ui->m_wepkey1RB->setChecked(true);

    processSecurityTypeChanged(ui->m_securityCB->currentText());

    updateUiElements();
}

bool WirelessSettingPannel::isSettingValid() const
{
    bool settingValid = (!ui->m_ssidLE->text().isEmpty() && ui->m_ssidLE->text().length() <= 32);

    settingValid = (settingValid && (ui->m_securityCB->currentIndex() >= 0));

    QString securityType = ui->m_securityCB->itemData(ui->m_securityCB->currentIndex()).toString();

    if(0 == securityType.compare("WEP", Qt::CaseInsensitive)){
        //settingValid = (settingValid && !ui->m_wepPassphraseLE->text().isEmpty() && ui->m_wepPassphraseLE->text().length() <= 32);

        settingValid = (settingValid && (ui->m_wepAuthTypeCB->currentIndex() >= 0));

        int keyLen = 0;
        if(ui->m_wepEncryptionLenCB->currentText().contains("64")){
            keyLen = 10;
        }else if(ui->m_wepEncryptionLenCB->currentText().contains("128")){
            keyLen = 26;
        }else{
            settingValid = false;
        }

        int selectedKeyLen = 0;
        if(ui->m_wepkey1RB->isChecked()){
            selectedKeyLen = ui->m_wepkey1LE->text().length();
        }else if(ui->m_wepkey2RB->isChecked()){
            selectedKeyLen = ui->m_wepkey2LE->text().length();
        }else if(ui->m_wepkey3RB->isChecked()){
            selectedKeyLen = ui->m_wepkey3LE->text().length();
        }else{
            selectedKeyLen = ui->m_wepkey4LE->text().length();
        }

        settingValid = (settingValid && (selectedKeyLen == keyLen));

    }else if(securityType.compare("None", Qt::CaseInsensitive)){
        settingValid = (settingValid && (ui->m_passPhraseLE->text().length() >= 8) && (ui->m_passPhraseLE->text().length() <= 64));
    }

    return settingValid;
}

void WirelessSettingPannel::retriveSetting(WLANInfo &wlanInfo) const
{
    wlanInfo.m_ssid = ui->m_ssidLE->text();
    wlanInfo.m_basicEncryptionModes = ui->m_securityCB->itemData(ui->m_securityCB->currentIndex()).toString();

    if(0 == wlanInfo.m_basicEncryptionModes.compare("WEP", Qt::CaseInsensitive)){
        wlanInfo.m_wepAuthType = ui->m_wepAuthTypeCB->currentText();
        if(ui->m_wepkey1RB->isChecked()){
            wlanInfo.m_wepKey = ui->m_wepkey1LE->text();
        }else if(ui->m_wepkey2RB->isChecked()){
            wlanInfo.m_wepKey = ui->m_wepkey2LE->text();
        }else if(ui->m_wepkey3RB->isChecked()){
            wlanInfo.m_wepKey = ui->m_wepkey3LE->text();
        }else if(ui->m_wepkey4RB->isChecked()){
            wlanInfo.m_wepKey = ui->m_wepkey4LE->text();
        }

        wlanInfo.m_wlanPassphrase = ui->m_wepPassphraseLE->text();
    }else if(wlanInfo.m_basicEncryptionModes.compare("None", Qt::CaseInsensitive)){
        wlanInfo.m_wlanPassphrase = ui->m_passPhraseLE->text();
        wlanInfo.m_wpaEncryptionModes = wlanInfo.m_basicEncryptionModes;
    }else{
        wlanInfo.m_wlanPassphrase.clear();
    }
}

void WirelessSettingPannel::onLETextChanged(const QString &text)
{
    QLineEdit *signalSrc = qobject_cast<QLineEdit *>(sender());

    if(signalSrc == ui->m_wepkey1LE ||
            signalSrc == ui->m_wepkey2LE ||
            signalSrc == ui->m_wepkey3LE ||
            signalSrc == ui->m_wepkey4LE){
        signalSrc->setText(text.toUpper());
    }

    updateUiElements();
}

void WirelessSettingPannel::onRBToggled(bool checked)
{
    Q_UNUSED(checked);

    updateUiElements();
}

void WirelessSettingPannel::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WirelessSettingPannel::retranslateUi()
{
    I18N_TEXT(ui->m_ssidTagLB, Text, L_PLC_WIFI_WIZARD_SSID);
    I18N_TEXT(ui->m_secTypeTagLB, Text, L_PLC_WIFI_WIZARD_SECURITY_TYPE);
    I18N_TEXT(ui->m_passPhraseLB, Text, L_PLC_WIFI_WIZARD_PASSPHRASE);
    I18N_TEXT(ui->m_wepEncryptionTitleLB, Text, L_PLC_WIFI_WIZARD_SRCURITY_ENCRYPTION_WEP);
    I18N_TEXT(ui->m_authTypeTagLB, Text, L_PLC_WIFI_WIZARD_AUTHENTICATION_TYPE);
    I18N_TEXT(ui->m_encryptionStrengthTagLB, Text, L_PLC_WIFI_WIZARD_ENCRYPTION_STRENGTH);
    I18N_TEXT(ui->m_wepEncryptipnKeyTagLB, Text, L_PLC_WIFI_WIZARD_SRCURITY_ENCRYPTION_WEP_KEY);
    I18N_TEXT(ui->m_wepPassphraseTagLB, Text, L_PLC_WIFI_WIZARD_PASSPHRASE);
    I18N_TEXT(ui->m_wepkeyGenerateBtn, Text, L_PLC_WIFI_WIZARD_GENERATE);
    I18N_TEXT(ui->m_wepkey1RB, Text, L_PLC_WIFI_WIZARD_WEP_KEY1);
    I18N_TEXT(ui->m_wepkey2RB, Text, L_PLC_WIFI_WIZARD_WEP_KEY2);
    I18N_TEXT(ui->m_wepkey3RB, Text, L_PLC_WIFI_WIZARD_WEP_KEY3);
    I18N_TEXT(ui->m_wepkey4RB, Text, L_PLC_WIFI_WIZARD_WEP_KEY4);

    I18N_TEXT(ui->m_wirelessModePromptLB,Text, L_PLC_WIFI_WIZARD_WIRELESSMODE_PROMPT);

    ui->m_authTypeTagLB->setText(ui->m_authTypeTagLB->text() + ":");
    ui->m_encryptionStrengthTagLB->setText(ui->m_encryptionStrengthTagLB->text() + ":");
    ui->m_wepPassphraseTagLB->setText(ui->m_wepPassphraseTagLB->text() + ":");
    ui->m_passPhraseLB->setText(ui->m_passPhraseLB->text() + ":");

}

void WirelessSettingPannel::btnClicked()
{
    QPushButton *srcBtn = qobject_cast<QPushButton *>(sender());

    if(srcBtn == ui->m_wepkeyGenerateBtn){
        generateWEPKeys();
    }
}

QString WirelessSettingPannel::wepKeyGenerater(const QString &passphrase, int keyLen, int delta)
{
    QString key;
    int idx = 0 ;
    int seed = 0;
    unsigned char randByte = 0;

    const int count = passphrase.length();

    do{

        if(idx < count){
            if(idx == (keyLen / 2 - 1)){
                seed = 0;
                for(int k = idx ; k < count ; ++k){
                    seed += (passphrase[k].unicode() + (k + 1) * delta);
                }
            }else{
                seed = (passphrase[idx].unicode() + (idx + 1) * delta);
            }
        }else{
            seed = (passphrase[passphrase.length() - 1].unicode() + (idx + 1) * delta);
        }

        qsrand(seed);

        randByte = (unsigned char)qrand();
        char buffer[16];
        sprintf(buffer, "%02X", randByte);
        key += buffer;

    }while(++idx < (keyLen / 2));

    return key;
}

void WirelessSettingPannel::generateWEPKeys()
{
    const int keyLen = (ui->m_wepEncryptionLenCB->currentText().contains("128") ? 26 : 10);
    QString wepPassphrase = ui->m_wepPassphraseLE->text();
    if(ui->m_wepEncryptionLenCB->currentIndex() < 0 || wepPassphrase.isEmpty()){
        return;
    }


    ui->m_wepkey1LE->setText(wepKeyGenerater(wepPassphrase, keyLen, 3));
    ui->m_wepkey2LE->setText(wepKeyGenerater(wepPassphrase, keyLen, 5));
    ui->m_wepkey3LE->setText(wepKeyGenerater(wepPassphrase, keyLen, 7));
    ui->m_wepkey4LE->setText(wepKeyGenerater(wepPassphrase, keyLen, 13));
}

void WirelessSettingPannel::updateUiElements()
{
    emit settingValidityUpdate(isSettingValid());

    ui->m_wepkeyGenerateBtn->setDisabled(ui->m_wepPassphraseLE->text().isEmpty() || (ui->m_wepEncryptionLenCB->currentIndex() < 0));
}

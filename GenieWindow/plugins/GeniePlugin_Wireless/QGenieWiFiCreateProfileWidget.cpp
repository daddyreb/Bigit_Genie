#include "QGenieWiFiCreateProfileWidget.h"
#include "ui_QGenieWiFiCreateProfileWidget.h"
#include "GeniePlugin_Wireless.h"
#ifdef Q_OS_WIN
#include "QGenieWiFiProfileManagerController.h"
#elif defined Q_OS_MACX
#include "QGenieWiFiUtilityControllerForMacx.h"
#endif

#include <QtDebug>

#ifdef Q_OS_WIN
//modify by caiyujun for bug 1920
//bug 1920:remove WPA and WPA2 for adding a new network profile
//static char * g_authsUi[]  = {"open","shared","WPA","WPA-PSK","WPA2","WPA2-PSK"};
//static char * g_auths[]  = {"open","shared","WPA","WPAPSK","WPA2","WPA2PSK"};
static char * g_authsUi[]  = {"open","shared","WPA-PSK","WPA2-PSK"};
static char * g_auths[]  = {"open","shared","WPAPSK","WPA2PSK"};
//modify end
#elif defined Q_OS_MACX
static char *g_securitysUi[] = {"None","WEP","WPA Personal","WPA2 Personal"};
static char *g_securitys[] = {"OPEN","WEP","WPA","WPA2"};
#endif

QGenieWiFiCreateProfileWidget::QGenieWiFiCreateProfileWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::QGenieWiFiCreateProfileWidget)
{
    ui->setupUi(this);

    ui->m_ssidLE->setMaxLength(32);
#ifdef Q_OS_MACX
    ui->m_netKeyLE->setMaxLength(64);
    ui->m_noneBroadcastCB->setVisible(false);
    ui->m_encryLB->setVisible(false);
    ui->m_encryList->setVisible(false);
#else
    ui->m_netKeyLE->setMaxLength(64);
#endif

    connect(ui->m_okBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->m_cancelBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(ui->m_authList,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChanged(int)));
    connect(ui->m_encryList,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChanged(int)));
    connect(ui->m_ssidLE,SIGNAL(textChanged(QString)),this,SLOT(textChanged(QString)));
    connect(ui->m_netKeyLE,SIGNAL(textChanged(QString)),this,SLOT(textChanged(QString)));

    connect(ui->m_clearTextCB,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));

#ifdef Q_OS_WIN
    initAuthList();
#elif defined Q_OS_MACX
    initSecurityList();
#endif

    retranslateUi();
}

QGenieWiFiCreateProfileWidget::~QGenieWiFiCreateProfileWidget()
{
    delete ui;
}

void QGenieWiFiCreateProfileWidget::btnClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(this->sender());

    if(btn == ui->m_okBtn)
    {
        createProfile();
    }
    else if(btn == ui->m_cancelBtn)
    {
        emit addProfileCompleted(false);
    }
}

#ifdef Q_OS_WIN
void QGenieWiFiCreateProfileWidget::initAuthList()
{
    QStringList authList;
    for(int i = 0 ; i < int(sizeof(g_authsUi)/sizeof(g_authsUi[0])) ; ++i)
    {
        authList.append(g_authsUi[i]);
    }

    ui->m_authList->addItems(authList);

    for(int j = 0 ; j < ui->m_authList->count() ; ++j)
    {
        ui->m_authList->setItemData(j,QString(g_auths[j]));
    }

    ui->m_authList->setCurrentIndex(0);
}
#elif defined Q_OS_MACX
void QGenieWiFiCreateProfileWidget::initSecurityList()
{
    QStringList secList;
    for(int i = 0 ; i < int(sizeof(g_securitysUi)/sizeof(g_securitysUi[0])) ; ++i)
    {
        secList.append(g_securitysUi[i]);
    }

    ui->m_authList->addItems(secList);

    for(int j = 0 ; j < ui->m_authList->count() ; ++j)
    {
        ui->m_authList->setItemData(j,QString(g_securitys[j]));
    }

    ui->m_authList->setCurrentIndex(0);
}
#endif

void QGenieWiFiCreateProfileWidget::currentIndexChanged(int index)
{
    QComboBox *cb = qobject_cast<QComboBox *>(this->sender());

#ifdef Q_OS_WIN
    if(cb == ui->m_authList)
    {
        QStringList encrys;

        if(index > 1)
        {
            encrys << "AES" << "TKIP";
        }
        else
        {
            encrys << "WEP" << "none";
        }

        ui->m_encryList->clear();
        ui->m_encryList->addItems(encrys);
        ui->m_encryList->setCurrentIndex(0);
    }
#endif

    if((cb == ui->m_authList) || (cb == ui->m_encryList))
    {
        ui->m_netKeyLE->clear();
    }

    updateUiElements();
}

void QGenieWiFiCreateProfileWidget::updateUiElements()
{
    bool benable = false;
#ifdef Q_OS_WIN
    benable = (((ui->m_authList->currentText() == "open") && (ui->m_encryList->currentText() == "none")) ||
               (ui->m_authList->currentText() == "WPA") ||
               (ui->m_authList->currentText() == "WPA2"));
    benable = !benable;
#elif defined Q_OS_MACX
    benable = (ui->m_authList->currentText() != g_securitysUi[0]);
#endif

    ui->m_clearTextCB->setEnabled(benable);
    ui->m_netKeyLE->setEnabled(benable);

    ui->m_okBtn->setEnabled(isSsidValid() && isNetworkKeyValid());
}

bool QGenieWiFiCreateProfileWidget::isSsidValid()
{
    QString ssid = ui->m_ssidLE->text();

    return ((ssid.length() > 0) && ((ssid.length() <= 32)));
}

bool QGenieWiFiCreateProfileWidget::isNetworkKeyValid()
{
    QString networkKey = ui->m_netKeyLE->text();

#ifdef Q_OS_WIN
    if(((ui->m_authList->currentText() == "open") && (ui->m_encryList->currentText() == "none")) ||
       (ui->m_authList->currentText() == "WPA") ||
       (ui->m_authList->currentText() == "WPA2"))
    {
        return networkKey.isEmpty();
    }
    else if((ui->m_authList->currentText() == "WPA-PSK") ||
            (ui->m_authList->currentText() == "WPA2-PSK"))
    {
        return ((networkKey.length() >= 8) && (networkKey.length() <= 64));
    }
    else
    {
        return ((networkKey.length() == 5) ||
                (networkKey.length() == 13) ||
                (networkKey.length() == 10) ||
                (networkKey.length() == 26) );
    }
#elif defined Q_OS_MACX
    if(ui->m_authList->currentText() == QString(g_securitysUi[1]))
    {
        return ((networkKey.length() == 5) ||
                (networkKey.length() == 13) ||
                (networkKey.length() == 10) ||
                (networkKey.length() == 26) );
    }
    else if(ui->m_authList->currentText() == QString(g_securitysUi[2]) ||
            ui->m_authList->currentText() == QString(g_securitysUi[3]) )
    {
        return ((networkKey.length() >= 8) && (networkKey.length() <= 64));
    }
    else
    {
        return networkKey.isEmpty();
    }
#endif
}

void QGenieWiFiCreateProfileWidget::textChanged(const QString &text)
{
    updateUiElements();
}

void QGenieWiFiCreateProfileWidget::createProfile()
{
    bool    bret    = false;

    QString ssid    = ui->m_ssidLE->text();
    QString key     = ui->m_netKeyLE->text();

#ifdef Q_OS_WIN
    QString auth = ui->m_authList->itemData(ui->m_authList->currentIndex()).toString();
    QString cipher = ui->m_encryList->currentText();
    bool noneB = ui->m_noneBroadcastCB->isChecked();
    bret = g_WifiProfileManager.addProfile(ssid,auth,cipher,key,noneB);

    qDebug() << "create profile :ssid = " << ssid << " auth = " << auth
            << " cipher = " << cipher << " key = " << key
            << " noneBroadcast = " << noneB << "result = " << bret;

#elif defined Q_OS_MACX
    QString sectype = ui->m_authList->itemData(ui->m_authList->currentIndex()).toString();
    bret = g_WifiProfileManager.addProfile(ssid,sectype,key);

    qDebug() << "create profile :ssid = " << ssid << " security = " << sectype
            << " key = " << key << "result = " << bret;
#endif

    emit addProfileCompleted(bret);
}

void QGenieWiFiCreateProfileWidget::stateChanged(int state)
{
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

    if(cb == ui->m_clearTextCB)
    {
        ui->m_netKeyLE->setEchoMode((state == Qt::Checked)?QLineEdit::Normal:QLineEdit::Password);
    }
}

void QGenieWiFiCreateProfileWidget::resetUi()
{
    ui->m_ssidLE->clear();
    ui->m_authList->setCurrentIndex(0);
    ui->m_clearTextCB->setChecked(false);
    ui->m_noneBroadcastCB->setChecked(false);
    ui->m_netKeyLE->clear();
}

void QGenieWiFiCreateProfileWidget::retranslateUi()
{
    ui->m_ssidLabel->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_SSID_NAME)/*"SSID:"*/);
#ifdef Q_OS_WIN
    ui->m_authLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_AUTHENTICATION_NAME)/*"Authentication:"*/);
#elif defined Q_OS_MACX
    ui->m_authLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_SECURITY_NAME)/*"Security:"*/);
#endif
    ui->m_encryLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_ENCRYPTION_NAME)/*"Encryption:"*/);
    ui->m_netKeyLB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_NETWORKKEY_NAME)/*"Network key:"*/);
    ui->m_clearTextCB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PLAINTEXT_CB_TEXT));
    ui->m_noneBroadcastCB->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCPW_NONEBROADCASTNETWORK)/*"None broadcast network"*/);

    ui->m_okBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_OK_TEXT));
    ui->m_cancelBtn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_CANCEL_BTN_TEXT));
}

void QGenieWiFiCreateProfileWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
        retranslateUi();
    QWidget::changeEvent(event);
}

#include "QGenieAvailableNetworkItem.h"
#include "GeniePlugin_Wireless.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPixmap>
#include <QFont>

QGenieAvailableNetworkItem::QGenieAvailableNetworkItem(QWidget *parent)
    : QFrame(parent),security_enabled(false),signal_strength(0),cur_connectstate(0)
{
    //this->setAutoFillBackground(true);

    wireless_image_lb                 = new QLabel(this);
    ssid_text_lb                      = new QLabel(this);
    channel_text_lb                   = new QLabel(this);
    securityenabled_image_lb          = new QLabel(this);
    securityprompt_text_lb            = new QLabel(this);
    signal_image_lb                   = new QLabel(this);
    connectprompt_text_lb             = new QLabel(this);
    connectstate_text_lb              = new QLabel(this);

    ssid_text_lb->setObjectName(QString("NetworkItem_SSID"));
    connectstate_text_lb->setObjectName(QString("NetworkItem_ConnectState"));

//#ifdef Q_OS_WIN
    channel_text_lb->setVisible(false);
//#endif

    connectprompt_text_lb->setVisible(false);

    QFont font;
    font.setBold(true);
    ssid_text_lb->setFont(font);
    channel_text_lb->setFont(font);
    //ssid_text_lb->setWordWrap(true);

    securityenabled_image_lb->setPixmap(QPixmap(":/image/Sec_Enabled.png"));

    //securityprompt_text_lb->setWordWrap(true);
    connectprompt_text_lb->setWordWrap(true);

    //initialize layout
    QHBoxLayout *sec_signal_layout = new QHBoxLayout();
    sec_signal_layout->setMargin(0);
    sec_signal_layout->setSpacing(2);
    sec_signal_layout->addWidget(securityenabled_image_lb);
    sec_signal_layout->addWidget(securityprompt_text_lb);
    QSpacerItem  *sec_signal_item = new QSpacerItem(0,0,QSizePolicy::MinimumExpanding);
    sec_signal_layout->addItem(sec_signal_item);
    sec_signal_layout->addWidget(channel_text_lb);
    sec_signal_layout->addWidget(signal_image_lb);

    QVBoxLayout  *ver_layout = new QVBoxLayout();
    ver_layout->setMargin(0);
    ver_layout->setSpacing(2);
    ver_layout->addLayout(sec_signal_layout);
    ver_layout->addWidget(connectprompt_text_lb);

    QHBoxLayout     *hor_layout3 = new QHBoxLayout();
    hor_layout3->setMargin(2);
    QSpacerItem         *spitem3    = new QSpacerItem(0,0,QSizePolicy::Expanding);
    hor_layout3->addWidget(ssid_text_lb);
    hor_layout3->addItem(spitem3);
    hor_layout3->addWidget(connectstate_text_lb);
    //hor_layout3->addWidget(channel_text_lb);

    QVBoxLayout *verlayout = new QVBoxLayout();
    verlayout->setMargin(0);
    verlayout->setSpacing(0);
    verlayout->addLayout(hor_layout3);
    verlayout->addLayout(ver_layout);


    QHBoxLayout    *hor_layout = new QHBoxLayout(this);
    hor_layout->setMargin(4);
    hor_layout->setSpacing(0);
    hor_layout->addWidget(wireless_image_lb);
    hor_layout->addLayout(verlayout);
    //layout end

    retranslateUi();
}

void QGenieAvailableNetworkItem::retranslateUi()
{
    channel_text_lb->setText(/*tr("Channel: %1").arg(channel)*/
            GeniePlugin_Wireless::translateText(L_WIRELESS_ANI_CHANNEL_LB_TEXT).arg(channel));
    if(1 == cur_connectstate)
    {
        connectstate_text_lb->setText(/*tr("Connected")*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_ANI_CONNECTSTATE_CONNECTED));
    }
    else if(0 == cur_connectstate)
    {
        connectstate_text_lb->setText(tr(""));
    }
    else
    {
        connectstate_text_lb->setText(
                GeniePlugin_Wireless::translateText(L_WIRELESS_ANI_CONNECTSTATE_LIMITEDCONNECT));
    }

    if(this->security_enabled)
    {
        QString str_auth_present(this->str_auth_algorithm);
        if(str_auth_present=="WPAPSK")
        {
            str_auth_present = "WPA-PSK";
        }
        else if(str_auth_present=="WPA2PSK")
        {
            str_auth_present = "WPA2-PSK";
        }

        securityprompt_text_lb->setText(/*tr(texts_to_tr[0])*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_ANI_SECPROMPT_ENABLE_SEC)
                +QString("(%1)").arg(str_auth_present));
        //connectprompt_text_lb->setText(tr(texts_to_tr[2]));
    }
    else
    {
        securityprompt_text_lb->setText(/*tr(texts_to_tr[1])*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_ANI_SECPROMPT_DISABLE_SEC));
        //connectprompt_text_lb->setText(tr(texts_to_tr[3]));
    }
}

void QGenieAvailableNetworkItem::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QFrame::changeEvent(event);
}

void QGenieAvailableNetworkItem::set_ssid(const QString &ssid)
{
    ssid_text_lb->setText(ssid);
}

QString QGenieAvailableNetworkItem::get_ssid()
{
   return ssid_text_lb->text();
}

void QGenieAvailableNetworkItem::set_auth_algorithm(const QString &auth)
{
    this->str_auth_algorithm = auth;
    retranslateUi();
}

QString QGenieAvailableNetworkItem::get_auth_algorithm()
{
    return str_auth_algorithm;
}

void QGenieAvailableNetworkItem::set_encry_algorithm(const QString &encry)
{
    str_encry_algorithm = encry;
}

QString QGenieAvailableNetworkItem::get_encry_algorithm()
{
    return str_encry_algorithm;
}

void QGenieAvailableNetworkItem::set_sec_enabled(bool enabled)
{
    this->security_enabled = enabled;
    if(enabled)
        this->securityenabled_image_lb->setVisible(true);
    else
        this->securityenabled_image_lb->setVisible(false);

    retranslateUi();
}

bool QGenieAvailableNetworkItem::get_sec_enabled()
{
    return security_enabled;
}

void QGenieAvailableNetworkItem::set_bss_type(const QString &bss_type)
{
#ifdef Q_OS_WIN
    if(   QString("infrastructure") != bss_type
       && QString("independent") != bss_type
       && QString("any") != bss_type)
        return;
#elif defined Q_OS_MACX
    if(   QString("1") != bss_type
       && QString("0") != bss_type)
        return;
#endif

    str_bss_type = bss_type;

    if (
#ifdef Q_OS_WIN
         QString("independent")==str_bss_type
#elif defined Q_OS_MACX
         QString("1")==str_bss_type
#endif
       )
        wireless_image_lb->setPixmap(QPixmap(":/image/A_WNetwork.png"));
    else
        wireless_image_lb->setPixmap(QPixmap(":/image/I_WNetwork.png"));
}

void QGenieAvailableNetworkItem::set_signal_strength(int strength)
{
    if (strength<0)
        strength = 0;
#ifdef Q_OS_WIN
    int index =(strength==0?0: (strength-1)/20 +1);
#elif defined Q_OS_MACX
    int index = strength;
    if(index > 5)
    {
        index = 5;
    }
#endif
    signal_image_lb->setPixmap(QPixmap(QString(":/image/W_Signal_%1.png").arg(index)));
    //signal_image_lb->setToolTip(QString("%1%").arg(strength));
}

void QGenieAvailableNetworkItem::set_channel(int ch)
{
    this->channel = ch;
    retranslateUi();
}

void QGenieAvailableNetworkItem::set_hasprofile(bool hasprofile)
{
    bhasprofile = hasprofile;
}

bool QGenieAvailableNetworkItem::get_hasprofile()
{
    return bhasprofile;
}

void QGenieAvailableNetworkItem::set_profile(const QString &profile)
{
    str_profilename = profile;
}

QString QGenieAvailableNetworkItem::get_profile()
{
    return str_profilename;
}

void QGenieAvailableNetworkItem::set_selected(bool sel)
{
    connectprompt_text_lb->setVisible(sel);
}

void QGenieAvailableNetworkItem::set_curconnected(int connectstate)
{
    cur_connectstate = connectstate;
    retranslateUi();
}

int QGenieAvailableNetworkItem::get_curconnected()
{
    return cur_connectstate;
}

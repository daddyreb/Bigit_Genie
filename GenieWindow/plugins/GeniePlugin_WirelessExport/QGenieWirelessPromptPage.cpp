#include "QGenieWirelessPromptPage.h"
#include "GeniePlugin_WSetupPlugin.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>


QGenieWirelessPromptPage::QGenieWirelessPromptPage(QWidget *parent) :
        QFrame(parent),m_bfrom_router(false),m_networkprompt_lang_idx(-1)
{
    m_strssid.clear();
    m_strpwd.clear();

    welcome_image_lb  = new QLabel(this);
    network_prompt_lb = new QLabel(this);
    ssid_lb           = new QLabel(this);
    prev_btn          = new QPushButton(this);
    next_btn          = new QPushButton(this);
    cancel_btn        = new QPushButton(this);
    progress_panel    = new QGenieProgressPanel(this);

    network_prompt_lb->setWordWrap(true);
    network_prompt_lb->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);
    network_prompt_lb->setAlignment(Qt::AlignCenter);

    ssid_lb->setObjectName(QString("ssid_lb"));
    ssid_lb->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);
    ssid_lb->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    ssid_lb->setStyleSheet(QString::fromUtf8("#ssid_lb{font-weight:bold;}"));
    ssid_lb->setVisible(false);
    welcome_image_lb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    progress_panel->setVisible(false);

    welcome_image_lb->setPixmap(QPixmap(":/image/Welcome.png"));
    QHBoxLayout  *btns_layout =new QHBoxLayout();
    //QSpacerItem     *opt_cancel_sp = new QSpacerItem(10,0,QSizePolicy::Fixed);

    btns_layout->setSpacing(20);

    btns_layout->addStretch(1);
    btns_layout->addWidget(prev_btn);
    btns_layout->addWidget(next_btn);
    //    btns_layout->addItem(opt_cancel_sp);
    btns_layout->addWidget(cancel_btn);


    QFrame *ver_line = new QFrame(this);
    //    ver_line->setGeometry(QRect(143, 70, 10, 210));
    ver_line->setFixedHeight(207);
    ver_line->setFrameShape(QFrame::VLine);
    ver_line->setFrameShadow(QFrame::Sunken);
    ver_line->setObjectName("Splitline1");

    QSpacerItem    *spitem0 = new QSpacerItem(0,60,QSizePolicy::Minimum,QSizePolicy::Fixed);
    QSpacerItem    *spitem1 = new QSpacerItem(0,60,QSizePolicy::Minimum,QSizePolicy::Fixed);
    QVBoxLayout   *prompt_waitting_layout = new QVBoxLayout();
    prompt_waitting_layout->addItem(spitem0);
    prompt_waitting_layout->addWidget(progress_panel);
    prompt_waitting_layout->addWidget(network_prompt_lb);
    prompt_waitting_layout->addWidget(ssid_lb);
    prompt_waitting_layout->addItem(spitem1);



    //QWidget   *left_panel = new QWidget(this);
    //    right_panel->setMinimumWidth(400);
    //QSpacerItem    *spitem = new QSpacerItem(20,0,QSizePolicy::Fixed,QSizePolicy::Minimum);
    //QSpacerItem    *spitem_1 = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);
    QHBoxLayout  *image_prompt_layout = new QHBoxLayout();
    image_prompt_layout->setSpacing(10);
    //    image_prompt_layout->addStretch(1);
    //image_prompt_layout->addItem(spitem);
    image_prompt_layout->addWidget(welcome_image_lb);
    image_prompt_layout->addWidget(ver_line);
    //    image_prompt_layout->addWidget(network_prompt_lb);
    image_prompt_layout->addLayout(prompt_waitting_layout);
    // image_prompt_layout->addStretch(1);

    QVBoxLayout *page_layout = new QVBoxLayout(this);
    page_layout->addLayout(image_prompt_layout);
    //page_layout->addWidget(right_panel);
    page_layout->addLayout(btns_layout);


    connect(prev_btn,SIGNAL(clicked()),this,SIGNAL(wirelesspromptpage_prev_btn_clicked()));
    connect(next_btn,SIGNAL(clicked()),this,SIGNAL(wirelesspromptpage_next_btn_clicked()));
    connect(cancel_btn,SIGNAL(clicked()),this,SLOT(cancel_btn_clicked()));

    retranslateUi();
}

void QGenieWirelessPromptPage::changeEvent(QEvent *event)
{
    if(event->type()==event->LanguageChange)
    {
        retranslateUi();
    }
    QFrame::changeEvent(event);
}

void QGenieWirelessPromptPage::retranslateUi()
{
    if(L_WIRELESSEXPORT_WPP_ADDDEVICE_2NETWORK==m_networkprompt_lang_idx)
    {
        network_prompt_lb->setText(GeniePlugin_WSetupPlugin::translateText(m_networkprompt_lang_idx)
                                   /* .arg(m_strssid)*/);
        ssid_lb->setVisible(true);
        ssid_lb->setText(m_strssid);
    }
    else
    {
        ssid_lb->setVisible(false);
        network_prompt_lb->setText(GeniePlugin_WSetupPlugin::translateText(m_networkprompt_lang_idx));
    }

    prev_btn->setText(/*tr("Prev")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WPP_PREV_BTN_TEXT));
    next_btn->setText(/*tr("Next")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WPP_NEXT_BTN_TEXT));
    cancel_btn->setText(/*tr("Cancel")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WPP_CANCEL_BTN_TEXT));
}

void QGenieWirelessPromptPage::init(const QString &ssid, const QString &pwd)
{
    m_strssid.clear();
    m_strpwd.clear();
    m_strnicguid.clear();
    m_strprofilename.clear();
    m_bfrom_router = false;


    next_btn->setDisabled( m_strssid.isEmpty());

    GeniePlugin_WSetupPlugin::output_log("QGenieWirelessPromptPage init()");

    getwlaninfo_complete(!ssid.isEmpty(),ssid,
                         pwd,QString(),
                         QString());
}

void QGenieWirelessPromptPage::getwlaninfo_complete(bool bok,const QString &ssid,const QString &key
                                                    ,const QString &guid,const QString &profilename)
{
    GeniePlugin_WSetupPlugin::output_log(QString("getwlaninfo_complete:ssid=%1,pwd=%2")
                                         .arg(ssid,key));

    progress_panel->setVisible(false);
    progress_panel->stop();

    if(bok)
    {
        m_strssid           = ssid;
        m_strpwd            = key;
        m_strnicguid        = guid;
        m_strprofilename    = profilename;

        if(m_strnicguid.isEmpty())//from router
        {
            m_bfrom_router = true;
        }
        else
        {
            m_bfrom_router = false;
        }
        network_prompt_lb->setText(/*tr("Add new computer  and device to network \"%1\"")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WPP_ADDDEVICE_2NETWORK)/*.arg(m_strssid)*/);
        m_networkprompt_lang_idx = L_WIRELESSEXPORT_WPP_ADDDEVICE_2NETWORK;
        ssid_lb->setVisible(true);
        ssid_lb->setText(m_strssid);
    }
    else
    {
        network_prompt_lb->setText(/*tr("No wireless information to export.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WPP_NO_WLAN_CONFIG));
        m_networkprompt_lang_idx = L_WIRELESSEXPORT_WPP_NO_WLAN_CONFIG;
        ssid_lb->setVisible(false);
    }

    next_btn->setDisabled(!bok);
    prev_btn->setDisabled(false);
}

bool QGenieWirelessPromptPage::is_fromrouter()
{
    return m_bfrom_router;
}

const QString & QGenieWirelessPromptPage::get_ssid()
{
    return m_strssid;
}

const QString & QGenieWirelessPromptPage::get_pwd()
{
    return m_strpwd;
}

const QString & QGenieWirelessPromptPage::get_nicguid()
{
    return m_strnicguid;
}

const QString & QGenieWirelessPromptPage::get_profilename()
{
    return m_strprofilename;
}

void QGenieWirelessPromptPage::cancel_btn_clicked()
{
    progress_panel->setVisible(false);
    progress_panel->stop();

    emit wirelesspromptpage_cancel_btn_clicked();
}

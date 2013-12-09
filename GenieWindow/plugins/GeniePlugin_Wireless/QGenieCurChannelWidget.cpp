#include "QGenieCurChannelWidget.h"
#include "QGenieWirelessCommon.h"
#include "GeniePlugin_Wireless.h"
#include "genie2_interface.h"

#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern DiagnoseInterface    *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface     *wifiApi_inf;
#endif


QGenieCurChannelWidget::QGenieCurChannelWidget(QWidget *parent) :
    QWidget(parent),
    m_cur_channel(0),
    m_bchannelrefresh_ok(false),
    m_bshowautochannel(false),
    m_changechannel_enable(false)
{
    m_cur_network_lb   = new QLabel(this);
    m_cur_channel_lb   = new QLabel(this);
    m_prompt_lb        = new QLabel(this);
    m_refresh_btn      = new QPushButton(this);
    m_changechannel_btn= new QPushButton(this);

    m_progress_panel     = new QGenieProgressPanel(this);
    m_progress_panel->setAnimationDelay(100);
    m_progress_panel->setFixedSize(25,25);

    m_cur_network_lb->setObjectName(QString("WirelessTitle"));
    m_cur_channel_lb->setObjectName(QString("WirelessTitle"));

    m_changechannel_btn->setEnabled(false);
    m_prompt_lb->setVisible(false);
    m_prompt_lb->setAlignment(Qt::AlignRight);

    QHBoxLayout     *curchannel_layout = new QHBoxLayout();
    m_cur_network_lb->setWordWrap(true);
    m_cur_network_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    curchannel_layout->setSpacing(10);
    curchannel_layout->addWidget(m_cur_network_lb);
    curchannel_layout->addWidget(m_progress_panel);
    curchannel_layout->addWidget(m_refresh_btn);

    QHBoxLayout     *prompt_btn_layout = new QHBoxLayout();
    m_cur_channel_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    prompt_btn_layout->addWidget(m_cur_channel_lb);
    prompt_btn_layout->addStretch(1);
    prompt_btn_layout->addWidget(m_changechannel_btn);


    QVBoxLayout     *ver_layout  = new QVBoxLayout(this);
    ver_layout->setContentsMargins(1,1,1,1);
    ver_layout->setSpacing(3);
    ver_layout->addLayout(curchannel_layout);
    ver_layout->addLayout(prompt_btn_layout);
    ver_layout->addWidget(m_prompt_lb);

    retranslateUi();

    connect(m_refresh_btn,SIGNAL(clicked()),this,SLOT(refresh_btn_clicked()));
    connect(m_changechannel_btn,SIGNAL(clicked()),this,SLOT(changechannel_clicked()));


#ifdef Q_OS_WIN
    m_refresh_btn->setEnabled(diagnose_inf != 0);
#elif defined Q_OS_MACX
    m_refresh_btn->setEnabled(wifiApi_inf != 0);
#endif

}

void QGenieCurChannelWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QWidget::changeEvent(event);
}


void QGenieCurChannelWidget::retranslateUi()
{
    QString temp;
    if(m_bchannelrefresh_ok)
    {
        if(0 == m_cur_channel || m_cur_ssid.isEmpty())
        {
            m_cur_channel_lb->setVisible(false);
            m_cur_network_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_CHANNEL_LB_NOTNETGEARROUTER));
        }
        else
        {
            m_cur_channel_lb->setVisible(true);
            if(m_bshowautochannel)
            {
                m_cur_channel_lb->setText(
                            GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_CHANNEL_LB_CURCHANNEL).arg(QString("%1 (Auto)").arg(temp.setNum(m_cur_channel))));
            }
            else
            {
                m_cur_channel_lb->setText(
                            GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_CHANNEL_LB_CURCHANNEL).arg(temp.setNum(m_cur_channel)));
            }

            m_cur_network_lb->setText(
                        GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_CHANNEL_LB_NOTNETGEARROUTER));
        }
    }
    else
    {
        m_cur_channel_lb->setVisible(false);
#ifdef Q_OS_WIN
        m_cur_network_lb->setText(
                    GeniePlugin_Wireless::translateText(
                        (0 == diagnose_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRELESS_CCW_CHANNEL_LB_NOINFORMATION));
#elif defined Q_OS_MACX
        m_cur_network_lb->setText(
                    GeniePlugin_Wireless::translateText(
                        (0 == wifiApi_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRELESS_CCW_CHANNEL_LB_NOINFORMATION));
#endif
    }


#ifdef Q_OS_WIN
    QString prompttext = "<body style=\" font-family:'Times New Roman'; font-size:20pt; font-weight:400; font-style:normal;\">"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">%1</span></p></body>";
#elif defined Q_OS_MACX
    QString prompttext = "<body style=\" font-size:13pt; font-weight:400; font-style:normal;\">"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:13pt;\">%1</span></p></body>";
#endif

    m_prompt_lb->setText(prompttext.arg(GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_PROMPT_LB_TEXT)));

    m_refresh_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_REFRESH_BTN_TEXT));
    m_changechannel_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_CCW_CHANGECHANNEL_BTN_TEXT));
}


void QGenieCurChannelWidget::refresh_btn_clicked()
{
    begin_refresh();
}

void QGenieCurChannelWidget::changechannel_clicked()
{
    //statistics
    GENIE2_STATISTICS("W6");
    //end

    //    if(g_manager_handler)
    //    {
    //        IUiManager *ui_manager = g_manager_handler->getUiManager();
    //        if(ui_manager)
    //        {
    //            ui_manager->command(UI_CMD_CHANGE_CHANNEL,QString());
    //            GeniePlugin_Wireless::output_log(QString("changechannel_clicked:ui_manager->command:%1").arg(UI_CMD_CHANGE_CHANNEL));
    //        }
    //    }

    GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_NETCONFIG, EPCC_TURN_TO_CHANNEL, 0);
    GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,PLUGIN_UUID_NETCONFIG);

}

void QGenieCurChannelWidget::onViewCurrentPage(int curPage)
{
    m_changechannel_btn->setVisible(curPage == 0);
    m_prompt_lb->setVisible((curPage == 0) && (!m_cur_ssid.isEmpty()) && (m_cur_channel!=0) && refresh_netgearrouter());
}

void QGenieCurChannelWidget::begin_refresh()
{
    m_refresh_btn->setEnabled(false);
    m_changechannel_btn->setEnabled(false);
    m_prompt_lb->setVisible(false);
    m_progress_panel->start();
    emit refresh_request();
}

bool QGenieCurChannelWidget::refresh_netgearrouter()
{
    QVariant result = GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL);
    return !result.toString().isEmpty();
}

void QGenieCurChannelWidget::loadData(bool brefreshok, const QString &curssid, int curchannel, bool bautochannel)
{
    m_bchannelrefresh_ok = brefreshok;
    m_cur_ssid           = curssid;
    m_cur_channel        = curchannel;
    m_bshowautochannel   = bautochannel;

    retranslateUi();
    m_progress_panel->stop();
    m_refresh_btn->setEnabled(true);
    m_changechannel_btn->setEnabled(m_changechannel_enable && (!m_cur_ssid.isEmpty()) && (m_cur_channel!=0) && refresh_netgearrouter());
    m_prompt_lb->setVisible((!m_cur_ssid.isEmpty()) && (m_cur_channel!=0) && refresh_netgearrouter());
}

void QGenieCurChannelWidget::enableChangeChannel(bool enable)
{
    m_changechannel_enable = enable;
    m_changechannel_btn->setEnabled(m_changechannel_enable && (!m_cur_ssid.isEmpty()) && (m_cur_channel!=0) && refresh_netgearrouter());
}

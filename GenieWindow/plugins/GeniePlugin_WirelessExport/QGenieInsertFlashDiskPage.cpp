#include "QGenieInsertFlashDiskPage.h"
#include "GeniePlugin_WSetupPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QtDebug>

#ifdef PLATFORM_WIN32
#include "WSetupApiInterface.h"
extern WSetupApiInterface * setupapi_inf ;
#elif defined PLATFORM_MACOSX
#include "QGenieMacOSXApi.h"
#endif



QGenieInsertFlashDiskPage::QGenieInsertFlashDiskPage(QWidget *parent) :
        QFrame(parent),m_bexporting(false),m_opt_prompt_lb_lang_idx(-1),m_bdicoverflash_flag(false)
{
    m_nic_guid.clear();
    m_ssid.clear();
    m_profile_name_or_pwd.clear();

    cur_flash_disk.clear();

    opt_prompt_lb          =  new QLabel(this);
    opt_image_lb           =  new QLabel(this);
    prev_btn               = new QPushButton(this);
    next_btn               = new QPushButton(this);
    cancel_btn             = new QPushButton(this);


    opt_prompt_lb->setWordWrap(true);
    opt_prompt_lb->setAlignment(Qt::AlignCenter);
    opt_prompt_lb->setMinimumSize(420,80);

    opt_image_lb->setPixmap(QPixmap(":/image/InsertFlash.png"));

    QHBoxLayout *btns_layout = new QHBoxLayout();

    btns_layout->setSpacing(20);

    btns_layout->addStretch(1);
    btns_layout->addWidget(prev_btn);
    btns_layout->addWidget(next_btn);
    btns_layout->addWidget(cancel_btn);

    QHBoxLayout     *opt_image_layout = new QHBoxLayout();
    opt_image_layout->addStretch(1);
    opt_image_layout->addWidget(opt_image_lb);
    opt_image_layout->addStretch(1);

    QHBoxLayout     *opt_prompt_layout = new QHBoxLayout();
    opt_prompt_layout->addStretch(1);
    opt_prompt_layout->addWidget(opt_prompt_lb);
    opt_prompt_layout->addStretch(1);

    QSpacerItem  *ver_sp = new QSpacerItem(0,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
    QVBoxLayout  *page_layout = new QVBoxLayout(this);
    page_layout->addItem(ver_sp);
    page_layout->addLayout(opt_image_layout);
    page_layout->addStretch(1);
    page_layout->addLayout(opt_prompt_layout);
    page_layout->addStretch(1);
    page_layout->addLayout(btns_layout);

    connect(prev_btn,SIGNAL(clicked()),this,SIGNAL(ifdpage_prev_btn_clicked()));
    connect(next_btn,SIGNAL(clicked()),this,SIGNAL(ifdpage_next_btn_clicked()));
    connect(cancel_btn,SIGNAL(clicked()),this,SIGNAL(ifdpage_cancel_btn_clicked()));

    connect(next_btn,SIGNAL(clicked()),this,SLOT(on_next_btn_clicked()));

    connect(&m_export_thread,SIGNAL(export_completed(bool)),this,SLOT(export_completed(bool)));

    discover_flash_disk();

    connect(&discover_flash_timer,SIGNAL(timeout()),this,SLOT(discover_flash_disk()));
    discover_flash_timer.setInterval(1000);
//    discover_flash_timer.start();

    retranslateUi();
    QString qss=
    "background-color:rgb(255,255,255);"
    "border-style:solid;"
    "border-color:rgb(153,153,153);"
    "border-radius:9px;"
    "border-width:1px;";
    opt_prompt_lb->setStyleSheet(qss);

}

void QGenieInsertFlashDiskPage::changeEvent(QEvent *event)
{
    if(event->type()==event->LanguageChange)
    {
        retranslateUi();
    }
    QFrame::changeEvent(event);
}

void QGenieInsertFlashDiskPage::retranslateUi()
{
    opt_prompt_lb->setText(GeniePlugin_WSetupPlugin::translateText(m_opt_prompt_lb_lang_idx));
    prev_btn->setText(/*tr("Prev")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_PREV_BTN_TEXT));
    next_btn->setText(/*tr("Next")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_NEXT_BTN_TEXT));
    cancel_btn->setText(/*tr("Cancel")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_CANCEL_BTN_TEXT));
}

void QGenieInsertFlashDiskPage::on_next_btn_clicked()
{
    if(
#ifdef PLATFORM_WIN32
            0==setupapi_inf ||
#endif
            cur_flash_disk.isEmpty() ||
            m_nic_guid.isEmpty() || m_ssid.isEmpty() ||
            ((QString("from router") != m_nic_guid) && m_profile_name_or_pwd.isEmpty()))
        return;

    if(m_export_thread.begin_export(m_nic_guid, m_ssid,m_profile_name_or_pwd, m_auth, m_cipher, cur_flash_disk))
    {
        m_bexporting = true;
        next_btn->setEnabled(false);
        prev_btn->setEnabled(false);
        cancel_btn->setEnabled(false);
        opt_prompt_lb->setText(/*tr("Being exporting wireless configuration please wait ...")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_EXPORTING));
        m_opt_prompt_lb_lang_idx = L_WIRELESSEXPORT_FDP_OPTPROMPT_EXPORTING;
    }
    else
    {
        opt_prompt_lb->setText(/*tr("Try to export wireless configuration failed.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_TRY_EXPORT_FAILED));
        m_opt_prompt_lb_lang_idx = L_WIRELESSEXPORT_FDP_OPTPROMPT_TRY_EXPORT_FAILED;
    }
}

void QGenieInsertFlashDiskPage::discover_flash_disk()
{
#ifdef PLATFORM_WIN32
    QStringList ld_list;

    QStringList removable_list_cur;

    if(0==setupapi_inf)
        goto go_out;

    if(!setupapi_inf->get_logicaldriver_letters(ld_list))
    {
        goto go_out;
    }

    if(!cur_flash_disk.isEmpty())
    {
        if(!ld_list.contains(cur_flash_disk,Qt::CaseInsensitive))
        {
            cur_flash_disk.clear();
            m_bdicoverflash_flag = false;
        }
    }

    if(cur_flash_disk.isEmpty())
    {
        removable_list_cur.clear();
        foreach(QString disk,ld_list)
        {
            if(setupapi_inf->is_logicaldriver_removable(disk))
            {
                removable_list_cur.append(disk);
            }
        }

        if(m_bdicoverflash_flag)
        {
            if(removable_list_cur.size() > m_removable_list_prev.size())
            {
                foreach(QString removable,removable_list_cur)
                {
                    if(!m_removable_list_prev.contains(removable,Qt::CaseInsensitive))
                    {
                        cur_flash_disk = removable;
                        m_bdicoverflash_flag = false;
                        break;
                    }
                }
            }
            else if(removable_list_cur.size() < m_removable_list_prev.size())
            {
                m_removable_list_prev.clear();
                m_removable_list_prev=removable_list_cur;
            }
        }
        else
        {
            m_removable_list_prev.clear();
            m_removable_list_prev=removable_list_cur;
            m_bdicoverflash_flag = true;
        }
    }
#elif defined PLATFORM_MACOSX
    QStringList ld_list = QGenieMacOSXApi::get_volumes_list();

    if(!cur_flash_disk.isEmpty())
    {
        if(!ld_list.contains(cur_flash_disk,Qt::CaseInsensitive))
        {
            cur_flash_disk.clear();
            m_bdicoverflash_flag = false;
        }
    }

    if(cur_flash_disk.isEmpty())
    {
        if(m_bdicoverflash_flag)
        {
            if(ld_list.size() > m_removable_list_prev.size())
            {
                foreach(QString removable,ld_list)
                {
                    if(!m_removable_list_prev.contains(removable,Qt::CaseInsensitive))
                    {
                        cur_flash_disk = removable;
                        m_bdicoverflash_flag = false;
                        break;
                    }
                }
            }
            else if(ld_list.size() < m_removable_list_prev.size())
            {
                m_removable_list_prev.clear();
                m_removable_list_prev = ld_list;
            }
        }
        else
        {
            m_removable_list_prev.clear();
            m_removable_list_prev = ld_list;
            m_bdicoverflash_flag = true;
        }
    }
#endif

    go_out:
    if(cur_flash_disk.isEmpty())
    {
        opt_prompt_lb->setText(/*tr("Please insert a flash disk to the usb port and continue.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_INSERT_FLASHDISK));
        m_opt_prompt_lb_lang_idx = L_WIRELESSEXPORT_FDP_OPTPROMPT_INSERT_FLASHDISK;
    }
    else
    {
        if(!m_ssid.isEmpty() && !m_bexporting)
        {
            opt_prompt_lb->setText(/*tr("Click next to export wireless configuration.")*/
                    GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_CLICK_NEXT));
            m_opt_prompt_lb_lang_idx=L_WIRELESSEXPORT_FDP_OPTPROMPT_CLICK_NEXT ;
        }

        qDebug () << "found flash disk:" << cur_flash_disk;
    }
    next_btn->setDisabled(cur_flash_disk.isEmpty() || m_ssid.isEmpty() || m_bexporting);
}

void QGenieInsertFlashDiskPage::init(const QString &nic_guid,const QString &ssid,const QString &profile_name_or_pwd
                                     ,const QString &auth, const QString &cipher)
{

    cur_flash_disk.clear();
    m_bdicoverflash_flag = false;
    m_removable_list_prev.clear();

    m_nic_guid = nic_guid;
    m_ssid = ssid;
    m_profile_name_or_pwd  = profile_name_or_pwd;
    m_auth = auth;
    m_cipher = cipher;

    if(cur_flash_disk.isEmpty())
    {
        opt_prompt_lb->setText(/*tr("Please insert a flash disk to the usb port and continue.")*/
                GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_INSERT_FLASHDISK));
        m_opt_prompt_lb_lang_idx = L_WIRELESSEXPORT_FDP_OPTPROMPT_INSERT_FLASHDISK;
    }
    else
    {
        if(!m_ssid.isEmpty() && !m_bexporting)
        {
            opt_prompt_lb->setText(/*tr("Click next to export wireless configuration.")*/
                    GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_OPTPROMPT_CLICK_NEXT));
            m_opt_prompt_lb_lang_idx = L_WIRELESSEXPORT_FDP_OPTPROMPT_CLICK_NEXT;
        }
    }
    next_btn->setDisabled(cur_flash_disk.isEmpty() || m_ssid.isEmpty() || m_bexporting);
}

void QGenieInsertFlashDiskPage::export_completed(bool bok)
{
    emit  ifdpage_export_complete(bok);
    next_btn->setEnabled(true);
    prev_btn->setEnabled(true);
    m_bexporting = false;
    cancel_btn->setEnabled(true);
}

void QGenieInsertFlashDiskPage::on_stack_changed(int)
{
    if(this->isVisible())
    {
        discover_flash_disk();
        discover_flash_timer.start();
    }
    else
    {
        discover_flash_timer.stop();
    }
}

#include "QGenieWirelessSetupWizard.h"

#include <QPluginLoader>
#include <QApplication>
#include "genie2_interface.h"
#ifdef PLATFORM_WIN32
#include "WSetupApiInterface.h"
WSetupApiInterface            *setupapi_inf =0;
#define WSETUP_APIPLUGIN      QString("WSetupApiPlugin.dll")
#endif

QGenieWirelessSetupWizard::QGenieWirelessSetupWizard(QWidget *parent) :
        QFrame(parent)
{
#ifdef PLATFORM_WIN32
    if(0==setupapi_inf)
    {
        QString app_dir_path = qApp->applicationDirPath();
        QPluginLoader loader(app_dir_path+"/"+WSETUP_APIPLUGIN);
        setupapi_inf =qobject_cast<WSetupApiInterface*>( loader.instance());
    }
#endif

    welcome_page    = new QGenieWelcomePage(this);
    wprompt_page    = new QGenieWirelessPromptPage(this);
    ifd_page        = new QGenieInsertFlashDiskPage(this);
    result_page     = new QGenieResultPage(this);

    wizard_layout   = new QStackedLayout(this);
    wizard_layout->addWidget(welcome_page);
    wizard_layout->addWidget(wprompt_page);
    wizard_layout->addWidget(ifd_page);
    wizard_layout->addWidget(result_page);

    connect(welcome_page,SIGNAL(welcomepage_next_btn_clicked()),this,SLOT(on_welcomepage_next_btn_clicked()));
    connect(welcome_page,SIGNAL(welcomepage_cancel_btn_clicked()),this,SLOT(on_page_cancel_btn_clicked()));

    connect(wprompt_page,SIGNAL(wirelesspromptpage_prev_btn_clicked()),
            this,SLOT(on_wirelesspromptpage_prev_btn_clicked()));
    connect(wprompt_page,SIGNAL(wirelesspromptpage_next_btn_clicked()),
            this,SLOT(on_wirelesspromptpage_next_btn_clicked()));
    connect(wprompt_page,SIGNAL(wirelesspromptpage_cancel_btn_clicked()),
            this,SLOT(on_page_cancel_btn_clicked()));

    connect(ifd_page,SIGNAL(ifdpage_prev_btn_clicked()),this,SLOT(on_ifdpage_prev_btn_clicked()));
    connect(ifd_page,SIGNAL(ifdpage_export_complete(bool)),this,SLOT(on_ifdpage_export_complete(bool)));
    connect(ifd_page,SIGNAL(ifdpage_cancel_btn_clicked()),this,SLOT(on_page_cancel_btn_clicked()));

    connect(result_page,SIGNAL(resultpage_end_btn_clicked()),this,SLOT(on_resultpage_end_btn_clicked()));

    connect(wizard_layout,SIGNAL(currentChanged(int)),ifd_page,SLOT(on_stack_changed(int)));
    wizard_layout->setCurrentWidget(welcome_page);
    QList<QPushButton *> butlist=this->findChildren<QPushButton *>();
    int c=butlist.count();
    for(int i=0;i<c;i++)
    {
        butlist[i]->setAutoDefault(true);
    }
}

void QGenieWirelessSetupWizard::SetExportData(const QString &ssid, const QString &pwd,
                   const QString &auth, const QString &cipher)
{
    m_ssid = ssid;
    m_pwd = pwd;
    m_auth = auth;
    m_cipher = cipher;
}

void QGenieWirelessSetupWizard::on_welcomepage_next_btn_clicked()
{
    wizard_layout->setCurrentWidget(wprompt_page);
    wprompt_page->init(m_ssid, m_pwd);
}


void QGenieWirelessSetupWizard::on_wirelesspromptpage_prev_btn_clicked()
{
    wizard_layout->setCurrentWidget(welcome_page);
}

void QGenieWirelessSetupWizard::on_wirelesspromptpage_next_btn_clicked()
{
    if(wprompt_page->is_fromrouter())
    {
        ifd_page->init(QString("from router"),wprompt_page->get_ssid(),wprompt_page->get_pwd()
                       ,m_auth,m_cipher);
    }
    else
    {
        ifd_page->init(wprompt_page->get_nicguid(),wprompt_page->get_ssid(),wprompt_page->get_profilename()
                       ,m_auth,m_cipher);
    }
    wizard_layout->setCurrentWidget(ifd_page);
}


void QGenieWirelessSetupWizard::on_ifdpage_prev_btn_clicked()
{
    wizard_layout->setCurrentWidget(wprompt_page);
}

void QGenieWirelessSetupWizard::on_ifdpage_next_btn_clicked()
{

}

void QGenieWirelessSetupWizard::on_ifdpage_export_complete(bool bok)
{
    result_page->init(bok);
    wizard_layout->setCurrentWidget(result_page);
}

void QGenieWirelessSetupWizard::on_page_cancel_btn_clicked()
{
    wizard_layout->setCurrentWidget(welcome_page);
    GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_NETCONFIG,EPCC_WIRELESSEXPORT_RETURN,0);
}

void QGenieWirelessSetupWizard::on_resultpage_end_btn_clicked()
{
    on_page_cancel_btn_clicked();
}

void QGenieWirelessSetupWizard::init()
{

    wizard_layout->setCurrentWidget(welcome_page);
}



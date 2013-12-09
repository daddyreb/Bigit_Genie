#include "smartwirelesssettingwizard.h"
#include "ui_smartwirelesssettingwizard.h"
#include "wirelesssettingcopyroutersettingpage.h"
#include "enterwirelesssettingpage.h"
#include <QMessageBox>

#include "language_i18n.h"
#include <QEvent>
#include <QMouseEvent>

SmartWirelessSettingWizard::SmartWirelessSettingWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SmartWirelessSettingWizard)
  ,m_wizardRunning(false)
  ,m_canMove(false)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_enterPage = new EnterWirelessSettingPage(this);
    connect(m_enterPage, SIGNAL(searchRouterClicked()),
            SLOT(onSearchRouterClicked()));
    connect(m_enterPage, SIGNAL(requestApplySetting(const WLANInfo*)), SLOT(onApplySettingProxy(const WLANInfo*)));
    connect(m_enterPage, SIGNAL(cancelClicked()), this, SIGNAL(cancelWizard()));


    m_copyPage = new WirelessSettingCopyRouterSettingPage(this);
    connect(m_copyPage, SIGNAL(requestApplySetting(const WLANInfo*)), SLOT(onApplySettingProxy(const WLANInfo*)));
    connect(m_copyPage, SIGNAL(cancelClicked()), this, SIGNAL(cancelWizard()));

    m_copyPage->setVisible(false);

    ui->m_wizardScrollArea->setWidget(m_enterPage);


    setVisible(false);

    retranslateUi();
}

SmartWirelessSettingWizard::~SmartWirelessSettingWizard()
{
    delete m_enterPage;
    delete m_copyPage;

    delete ui;
}

void SmartWirelessSettingWizard::onSearchRouterClicked()
{
    m_enterPage->ControlSearchRouterUi(true);
    emit requestSearchRouter();
}

void SmartWirelessSettingWizard::onApplySettingProxy(const WLANInfo *wlanInfo)
{
    WLANInfo * pwlanInfo = const_cast<WLANInfo *>(wlanInfo);

    pwlanInfo->m_channel = m_wlanInfo.m_channel;
    pwlanInfo->m_region = m_wlanInfo.m_region;
    pwlanInfo->m_wirelessMode = m_wlanInfo.m_wirelessMode;

    emit requestApplySetting(m_devIp, wlanInfo);
}

void SmartWirelessSettingWizard::setDeviceWlanInfo(const QString &model, const QString &ip, const WLANInfo &wlanInfo)
{
    m_devModel = model;
    m_devIp = ip;
    m_wlanInfo = wlanInfo;

    ui->m_advWirelessSettingPane->setAdvanceSettingInfo(m_devModel, m_devIp);
    m_enterPage->initWlanInfo(m_devModel, wlanInfo);
    m_copyPage->initWlanInfo(m_devModel ,wlanInfo);

    switch2Page(SWSW_ENTER_PAGE);
}

void SmartWirelessSettingWizard::notifySearchRouterResult(bool foundRouter, const WLANInfo *wlanInfo)
{
    if(!m_wizardRunning){
        return;
    }

    m_enterPage->ControlSearchRouterUi(false);

    if(foundRouter){
        m_copyPage->initRouterSettings(*wlanInfo);
        switch2Page(SWSW_COPY_PAGE);
    }/*else{
        QMessageBox::information(this, "Prompt", "NETGEAR router not founded!");
    }*/
}

void SmartWirelessSettingWizard::centerShow()
{
    show();

    QWidget *pwidget = qobject_cast<QWidget *>(parent());
    if(pwidget)
    {
        QPoint ref_pt = pwidget->mapToGlobal(pwidget->pos());
        move(ref_pt.x() +(pwidget->width() - width()) / 2,
             ref_pt.y() + (pwidget->height() - height()) / 2);
    }
}

int SmartWirelessSettingWizard::begin(const QString &model, const QString &ip, const WLANInfo &wlanInfo)
{
    m_wizardRunning = true;
    setDeviceWlanInfo(model, ip, wlanInfo);
    //centerShow();
    return exec();
}

void SmartWirelessSettingWizard::end(bool accept)
{
    m_wizardRunning = false;
    if(accept){
        SmartWirelessSettingWizard::accept();
    }else{
        reject();
    }

    setVisible(false);

    m_enterPage->ControlSearchRouterUi(false);

}

bool SmartWirelessSettingWizard::running() const
{
    return m_wizardRunning;
}

void SmartWirelessSettingWizard::switch2Page(int pageCode)
{
    switch(pageCode){
    case SWSW_ENTER_PAGE:
        m_copyPage->setVisible(false);
        ui->m_wizardScrollArea->takeWidget();
        ui->m_wizardScrollArea->setWidget(m_enterPage);
        m_enterPage->show();
        break;
    case SWSW_COPY_PAGE:
        m_enterPage->setVisible(false);
        ui->m_wizardScrollArea->takeWidget();
        ui->m_wizardScrollArea->setWidget(m_copyPage);
        m_copyPage->show();
        break;
    default:
        break;
    }
}

void SmartWirelessSettingWizard::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void SmartWirelessSettingWizard::mousePressEvent(QMouseEvent *event)
{
    m_pos = event->globalPos();
    m_canMove = true;
}

void SmartWirelessSettingWizard::mouseReleaseEvent(QMouseEvent *)
{
    m_canMove = false;
}

void SmartWirelessSettingWizard::mouseMoveEvent(QMouseEvent *event)
{
    if(m_canMove && (event->buttons() & Qt::LeftButton))
    {
        move(pos().x() - (m_pos.x() - event->globalPos().x()),
             pos().y() - (m_pos.y() - event->globalPos().y()));

        m_pos = event->globalPos();
    }
}

void SmartWirelessSettingWizard::retranslateUi()
{
    I18N_TEXT(ui->label_title, Text, L_PLC_WIFI_WIZARD_TITLE);
}

#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <List>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSettings>
#include <QThread>
#include <QPainter>
#include <QPen>
#include <QDesktopWidget>
#include <QtUiTools>

#include "extendermainwindow.h"
#include "ui_mainwindow.h"
#include "extenderutil.h"
#include "GetOSVer.h"
#include "extenderutilwifi.h"
#include "language_mainwindow.h"


QString SSID = "";
QString password = "";
extern QString g_port_extender;
extern QString g_extenderType;

ExtenderMainWindow::ExtenderMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    btn_connectContinue_text_idx(2000),
    waitting_text_idx(2003),
    bBackBtnClicked(false),
    flagWifiAutoSel2GRecord(false),
    flagWifiManualSel2GRecord(false),
    flagWifiAutoSel5GRecord(false),
    flagWifiManualSel5GRecord(false),
    bAdvancedSetBack(false),
    flagWizard(false),
    flagAdvanced(false)
{
    ui->setupUi(this);

    this->setWindowTitle("Extender Genie");

    // set the window in central
    QDesktopWidget *desk=QApplication::desktop();
    int wd=desk->width();
    int ht=desk->height();
    this->move((wd-width())/2,(ht-height())/2);

#ifdef Q_OS_WIN
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    ui->wdt_title->raise();

    // For Genie
    ui->btn_about->hide();
    ui->lbl_languageSelect->hide();
    ui->cmb_language->hide();
#elif defined Q_OS_MACX
    this->setWindowFlags(Qt::Window
                         | Qt::WindowTitleHint
                         | Qt::WindowMinimizeButtonHint
                         | Qt::WindowCloseButtonHint
                         | Qt::CustomizeWindowHint);
    ui->btn_about->hide();
    ui->btn_minimize->hide();
    ui->btn_close->hide();
#endif
    initialStyle();
    initializeUi();
    initializeVariable();

    // init waittingpanel
    waitPanel = new WaittingWidget;
    waitPanel->start();

    // create progresspanel for refresh wifilist
    m_refresh_progress_panel = new ProgressWidget(this);
    m_refresh_progress_panel->setFixedSize(25, 25);
    ui->horizontalLayout_showWifiList->addWidget(m_refresh_progress_panel);

    soapapi = new ExtenderSoapApi;

#ifdef Q_OS_MACX
    m_appMenuBar = new QMenuBar();
    QMenu *appMenu = new QMenu(0);
    m_appMenuBar->addMenu(appMenu);

    m_actAbout = new QAction("About Extender Genie",0);
    m_actAbout->setMenuRole(QAction::ApplicationSpecificRole);
    appMenu->addAction(m_actAbout);

    connect(appMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_btn_about_clicked()), Qt::UniqueConnection););
#endif

    // let app go to the first page
    setWaittingUi(ui->vl_wait);
    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));

    // inintial the translation
    initialLanguage();

    // geniewindow translation
    retranslateUi();

    // connect exit signal
    connect(this, SIGNAL(emitExit()), this, SLOT(sltExit()));
}

void ExtenderMainWindow::showFirstPage()
{
    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));
}

void ExtenderMainWindow::initialStyle()
{
    // set the app's style
    QFile qssFile(":/styles/default.qss");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        qDebug() << "===load qss OK";
        this->setStyleSheet(qssFile.readAll());
    }
    qssFile.close();
}

void ExtenderMainWindow::initializeUi()
{
    ui->frm_extenderSet2G->hide();
    ui->frm_extenderSet5G->hide();

    ui->lbl_extenderNtName5G->hide();
    ui->ledt_extenderNtName5G->hide();

    ui->chk_usingCurrentSetting->setChecked(false);

    ui->frm_finishWEP2G->hide();
    ui->frm_finishWEP5G->hide();

    /* set the tablewidget for wifilist
     */
    QStringList header;
    header << tr("SSID") << tr("Security") << tr("Channel") << tr("Signal");

    QHeaderView *headerView2G = new QHeaderView(Qt::Horizontal, this);
    QHeaderView *headerView5G = new QHeaderView(Qt::Horizontal, this);
    //* Qt5.0
    headerView2G->setSectionResizeMode(QHeaderView::Stretch);
    headerView5G->setSectionResizeMode(QHeaderView::Stretch);

    //Qt4.7
    //ui->tblwdt_ManualWifiList5G->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // 2.4G
    ui->tblwdt_wifiList2G->setColumnCount(4);
    ui->tblwdt_wifiList2G->setFrameStyle(QFrame::NoFrame);
    ui->tblwdt_wifiList2G->setHorizontalHeaderLabels(header);
    ui->tblwdt_wifiList2G->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblwdt_wifiList2G->setShowGrid(false);
    ui->tblwdt_wifiList2G->setHorizontalHeader(headerView2G);
    ui->tblwdt_wifiList2G->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblwdt_wifiList2G->setStyleSheet("selection-background-color: #3399FF");
    //ui->tblwdt_wifiList2G->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // 5G
    ui->tblwdt_wifiList5G->setColumnCount(4);
    ui->tblwdt_wifiList5G->setFrameStyle(QFrame::NoFrame);
    ui->tblwdt_wifiList5G->setHorizontalHeaderLabels(header);
    ui->tblwdt_wifiList5G->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblwdt_wifiList5G->setShowGrid(false);
    ui->tblwdt_wifiList5G->setHorizontalHeader(headerView5G);
    ui->tblwdt_wifiList5G->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblwdt_wifiList5G->setStyleSheet("selection-background-color: #3399FF");
    //ui->tblwdt_wifiList5G->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // set validator for forbiding input "@" and ";" in the name of SSID
    QRegExp rx("[^@;]*$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->ledt_extenderNtName2G->setValidator(validator);
    ui->ledt_extenderNtName5G->setValidator(validator);

    ui->lbl_errorImage->clear();
}

void ExtenderMainWindow::initializeVariable()
{
    m_isExtender = "";
    NewSessionID = "";
    m_newStatus = "";

    //auto set
    m_newStatus = "";

    flagWifiAutoSel2G = false;
    flagWifiAutoSel5G = false;
    flagWifiManualSel2G = false;
    flagWifiManualSel5G = false;

    flagWizard = false;
    flagAdvanced = false;
    m_bWifiListRefreshing = false;
    flagWifiSelected2G = false;
    flagWifiSelected5G = false;
    flagRouterWLANSet = 0;
    flagRouterWLANSetRight = 0;
    flagRouterWLANSetError = 0;
    flagExtenderWLANSet = 0;
    flagExtenderWLANSetRight = 0;
    flagExtenderWLANSetError = 0;
    m_setRouterWLANOK = 0;
    flagGetAPListFinished5G = false;
    flagGetAPListFinished2G = false;
    m_isRefreshed = false;

    m_extenderSecuritytypeManual2G = m_extenderSecuritytypeManual5G = "";

    m_extenderSSID2G = "";
    m_extenderSSID5G = "";
    m_extenderSecuritytype2G = "";
    m_extenderSecuritytype5G = "";
    m_extenderPassword2G = "";
    m_extenderPassword5G ="";

    m_newKeyIndex2G = 1;
    m_newKeyIndex5G = 1;
    m_newWEPLength2G = 64;
    m_newWEPLength5G = 64;
    m_extenderNewKeyIndex2G = 1;
    m_extenderNewKeyIndex5G = 1;
    m_extenderNewWEPLength2G = 64;
    m_extenderNewWEPLength5G = 64;

    m_WEPKey12G = "";
    m_WEPKey22G = "";
    m_WEPKey32G = "";
    m_WEPKey42G = "";
    m_WEPKey15G = "";
    m_WEPKey25G = "";
    m_WEPKey35G = "";
    m_WEPKey45G = "";

    m_extenderWEPKey12G = "";
    m_extenderWEPKey22G = "";
    m_extenderWEPKey32G = "";
    m_extenderWEPKey42G = "";
    m_extenderWEPKey15G = "";
    m_extenderWEPKey25G = "";
    m_extenderWEPKey35G = "";
    m_extenderWEPKey45G = "";
}

ExtenderMainWindow::~ExtenderMainWindow()
{
    delete ui;
    waitPanel->stop();
}

void ExtenderMainWindow::initialLanguage()
{
    languageSettings = new QSettings("language.ini", QSettings::IniFormat);

    QString strLan = languageSettings->value("language").toString();
    int intTemp;

    // set the cmbobox for language
    strLan = ExtenderUtil::loadLocalLanguage();
    if (strLan == "en") {
        curLanguage = "en";
        ui->cmb_language->setCurrentIndex(0);
        intTemp = 0;
    } else {
        curLanguage = "cn";
        ui->cmb_language->setCurrentIndex(1);
        intTemp = 1;
    }

    on_cmb_language_currentIndexChanged(intTemp);
}

void ExtenderMainWindow::retranslateUi()
{
    qDebug() << "Genie retranslate Ui";
    this->translate();
    ui->btn_connectContinue->setText(translateText(btn_connectContinue_text_idx));
    this->setWaittingText(waitting_text_idx);
    ui->lbl_waitText2->setText(translateText(waitting_text2_idx));

#ifdef Q_OS_WIN
    //qDebug() << "Genie check the OS version";
    int OS_type = ::GetOSVer();
    if (OS_type == XP) {
        ui->lbl_connectHintImg->resize(430, 335);
        ui->lbl_connectHintImg->setPixmap(QPixmap(QString(":/pics/xp_%1.png").arg(curLanguage)));
    } else if (OS_type == WIN7) {
        ui->lbl_connectHintImg->resize(360, 375);
        ui->lbl_connectHintImg->setPixmap(QPixmap(QString(":/pics/win7_%1.png").arg(curLanguage)));
    } else if (OS_type == WIN8) {
        ui->lbl_connectHintImg->resize(261, 399);
        ui->lbl_connectHintImg->setPixmap(QPixmap(QString(":/pics/win8_%1.png").arg(curLanguage)));
    } else {
        ui->lbl_connectHintImg->resize(286, 424);
        ui->lbl_connectHintImg->setPixmap(QPixmap(QString(":/pics/win7_%1.png").arg(curLanguage)));  // default WIN7
    }
#elif defined Q_OS_MACX
    ui->lbl_connectHintImg->resize(360, 375);
    ui->lbl_connectHintImg->setPixmap(QPixmap(QString(":/pics/mac_%1.png").arg(curLanguage)));
#endif

}

void ExtenderMainWindow::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QMainWindow::changeEvent(event);
}

void ExtenderMainWindow::setWaittingUi(QVBoxLayout *vl)
{
    vl->addWidget(waitPanel);
}

void ExtenderMainWindow::startAuthenticate()
{
    //the first step of soap
    qDebug() << "Genie start authenticate the extender";
    connect(&AuthenticateAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slt_authenticate(QNetworkReply*)), Qt::UniqueConnection);
    soapapi->Authenticate(&AuthenticateAccessManager, g_port_extender);
}

/*
 *=====Soap slots start
 */
void ExtenderMainWindow::slt_authenticate(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;



    if(all.indexOf("<ResponseCode>0") > 0)
    {
        qDebug() << "---Aauthenticate OK";
        // get device model
        soapapi->GetInfo(&GetDeviceInfoAccessManager, g_port_extender);
        connect(&GetDeviceInfoAccessManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_getDeviceInfo(QNetworkReply*)), Qt::UniqueConnection);
    } else {
        qDebug() << "---Aauthenticate ERROR!"; //TODO
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_getDeviceInfo(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    ui->lbl_WN1000RP->hide();
    ui->lbl_WN2500RP->hide();

    if(all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "---GetDeviceInfo OK!";
        QString modelName;
        modelName = ExtenderUtil::GetNodeInfo(all, "ModelName");
        m_serialNumber = ExtenderUtil::GetNodeInfo(all, "SerialNumber");
        bIsResetToDefault = ExtenderUtil::GetNodeInfo(all, "IsResetToDefault").toInt();
        ui->lbl_firewareVersion->clear();
        ui->lbl_firewareVersion->setText(modelName);

        // show device model
        ui->lbl_firewareVersion->show();
        // show device picture
        if (g_extenderType == "1000RP") {
            ui->lbl_WN1000RP->setText("");
            ui->lbl_WN1000RP->show();
        }
        if (g_extenderType == "2500RP") {
            ui->lbl_WN2500RP->setText("");
            ui->lbl_WN2500RP->show();
        }
        ui->wdt_devicePicture->show();

        // the start page
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_start));

    } else {
        qDebug() << "---GetDeviceInfo ERROR!"; //TODO
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_getAPInfo2G(QNetworkReply* reply)
{
    flagGetAPListFinished2G = true;
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<APList>") > 0) {
        qDebug() << "---GetAPInfo2G OK";
        showAPList2G(all);
    }
    else {
        qDebug() << "---GetAPInfo2G ERROR";
    }
}

void ExtenderMainWindow::slt_getAPInfo5G(QNetworkReply* reply)
{
    flagGetAPListFinished5G = true;
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<APList>") > 0) {
        qDebug() << "---GetAPInfo5G OK";
        showAPList5G(all);
    }
    else {
        qDebug() << "---GetAPInfo5G ERROR";
    }
}

void ExtenderMainWindow::slt_getAPListFinished()
{
    // refresh aplist finished, let the btns enable
    ui->rbtn_auto2G->setEnabled(true);
    ui->rbtn_auto5G->setEnabled(true);
    ui->rbtn_manual2G->setEnabled(true);
    ui->rbtn_manual5G->setEnabled(true);

    if (g_extenderType == "1000RP") {
        // control ui
        flagGetAPListFinished5G = true;
        ui->tabwdt_wifiList->removeTab(1);
    }
    if (flagGetAPListFinished2G && flagGetAPListFinished5G) {
        if (m_bWifiListRefreshing) {
            m_refresh_progress_panel->stop();
            ui->btn_refresh->setEnabled(true);
            m_bWifiListRefreshing = false;
            ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wifiList));
        } else {
            soapapi->ConfigurationStarted(&ConfigurationStartedAccessManager, g_port_extender);
            connect(&ConfigurationStartedAccessManager,SIGNAL(finished(QNetworkReply*)),
                    this,SLOT(slt_configurationStarted(QNetworkReply*)), Qt::UniqueConnection);
        }
    }
}

void ExtenderMainWindow::slt_configurationStarted(QNetworkReply *reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "---configurationStarted OK";
        //get new sessionID
        NewSessionID = ExtenderUtil::GetNodeInfo(all, "SessionID");

        if (flagWizard) {
            //the third step of soap
            if (g_extenderType == "1000RP") {
                soapapi->SetExtenderMode(&SetExtenderModeAccessManager, g_port_extender, "Internet Surfing", "Extender", "", "");
                connect(&SetExtenderModeAccessManager, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setExtenderMode(QNetworkReply*)), Qt::UniqueConnection);
            }
            if (g_extenderType == "2500RP") {
                soapapi->SetExtenderMode(&SetExtenderModeAccessManager, g_port_extender, "Internet Surfing", "Extender", "Extender", "5G");
                connect(&SetExtenderModeAccessManager, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setExtenderMode(QNetworkReply*)), Qt::UniqueConnection);
            }
        }
        if (flagAdvanced) {
            QString wlanSSID2GTmp = ui->ledtWlanName2G->text();
            QString wlanEncryptionModes2GTmp = ui->cbxWlanAuthType2G->currentText();
            QString wlanPassphrase2GTmp = ui->ledtWlanPasswrod2G->text();

            QString wlanSSID5GTmp = ui->ledtWlanName5G->text();
            QString wlanEncryptionModes5GTmp = ui->cbxWlanAuthType5G->currentText();
            QString wlanPassphrase5GTmp = ui->ledtWlanPasswrod5G->text();

            bool bWlanSSIDChange2G =  (wlanSSID2GTmp != wlanSSID2G) ? true : false;
            bool bWlanEncryptionModeChange2G = (wlanEncryptionModes2GTmp != wlanEncryptionModes2G) ? true : false;
            bool bWlanPasswordChange2G = (wlanPassphrase2GTmp != wlanPassphrase2G) ? true : false;

            bool bWlanSSIDChange5G =  (wlanSSID5GTmp != wlanSSID5G) ? true : false;
            bool bWlanEncryptionModeChange5G = (wlanEncryptionModes5GTmp != wlanEncryptionModes5G) ? true : false;
            bool bWlanPasswordChange5G = (wlanPassphrase5GTmp != wlanPassphrase5G) ? true : false;

            bool bInfoChange = false;

            connect(this, SIGNAL(setExtenderWLANFinished()), this, SLOT(slt_setExtenderWLANFinished()), Qt::UniqueConnection);

            qDebug() << "~~~~~~~" << m_extenderSSID2G << m_extenderSecuritytype2G << m_extenderPassword2G << m_extenderNewWEPLength2G << m_extenderWEPKey12G;

            setWaittingText(L_MW_WAITTING_Applying);
            setWaittingUi(ui->vl_wait);
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));

            QTimer::singleShot(60000, qApp, SLOT(quit()));

            if (g_extenderType == "1000RP") {
                flagExtenderWLANSet ++;
                flagExtenderWLANSetRight ++;
                if (bWlanSSIDChange2G || bWlanEncryptionModeChange2G || bWlanPasswordChange2G) {
                    bInfoChange = true;
                    if (wlanEncryptionModes2GTmp == "OFF")
                    {
                        soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager2G, g_port_extender, "2.4G",
                                                   wlanSSID2GTmp, "Auto", "Auto");
                        connect(&SetWLANNoSecurityAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else if (wlanEncryptionModes2GTmp == "WEP")
                    {
                        soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager2G, g_port_extender, "2.4G", wlanSSID2GTmp,
                                                  "Auto", "Auto", "Auto", 64, 1, wlanPassphrase2GTmp, wlanPassphrase2GTmp, wlanPassphrase2GTmp, wlanPassphrase2GTmp);
                        connect(&SetWLANWEPByKeysAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else
                    {
                        soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager2G, g_port_extender, "2.4G",
                                                           wlanSSID2GTmp, "Auto","Auto", wlanEncryptionModes2GTmp, wlanPassphrase2GTmp);
                        connect(&SetWLANWPAPSKByPassphraseAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
                    }
                }
            }
            if (g_extenderType == "2500RP") {
                if (bWlanSSIDChange2G || bWlanEncryptionModeChange2G || bWlanPasswordChange2G
                        || bWlanSSIDChange5G || bWlanEncryptionModeChange5G || bWlanPasswordChange5G) {
                    bInfoChange = true;

                    if (wlanEncryptionModes2GTmp == "OFF")
                    {
                        soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager2G, g_port_extender, "2.4G",
                                                   wlanSSID2GTmp, "Auto", "Auto");
                        connect(&SetWLANNoSecurityAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else if (wlanEncryptionModes2GTmp == "WEP")
                    {
                        soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager2G, g_port_extender, "2.4G", wlanSSID2GTmp,
                                                  "Auto", "Auto", "Auto", 64, 1, wlanPassphrase2GTmp, wlanPassphrase2GTmp, wlanPassphrase2GTmp, wlanPassphrase2GTmp);
                        connect(&SetWLANWEPByKeysAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else
                    {
                        soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager2G, g_port_extender, "2.4G",
                                                           wlanSSID2GTmp, "Auto","Auto", wlanEncryptionModes2GTmp, wlanPassphrase2GTmp);
                        connect(&SetWLANWPAPSKByPassphraseAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
                    }

                    if (wlanEncryptionModes5GTmp == "OFF")
                    {
                        soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager5G, g_port_extender, "5G",
                                                   wlanSSID5GTmp, "Auto", "Auto");
                        connect(&SetWLANNoSecurityAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else if (wlanEncryptionModes5GTmp == "WEP")
                    {
                        soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager5G, g_port_extender, "5G", wlanSSID5GTmp,
                                                  "Auto", "Auto", "Auto", 64, 1, wlanPassphrase5GTmp, wlanPassphrase5GTmp, wlanPassphrase5GTmp, wlanPassphrase5GTmp);
                        connect(&SetWLANWEPByKeysAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
                    }
                    else
                    {
                        soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager5G, g_port_extender, "5G",
                                                           wlanSSID5GTmp, "Auto","Auto", wlanEncryptionModes5GTmp, wlanPassphrase5GTmp);
                        connect(&SetWLANWPAPSKByPassphraseAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                                this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
                    }
                }
            }
        }
    } else {
        qDebug() << "---configurationStarted ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::slt_setExtenderMode(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "---SetExtenderMode OK";
        ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wifiList));
    } else {
        qDebug() << "---SetExtenderMode ERROR";
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_setRouterWLANNoSecurity(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    setStepUi(2);

    flagRouterWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0)
    {
        flagRouterWLANSetRight++;
        qDebug() << "---SetRouterWLANNoSecurity OK";
        emit setRouterWLANFinished();
    }
    else
    {
        flagRouterWLANSetError++;
        qDebug() << "---SetRouterWLANNoSecurity ERROR";
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_setRouterWLANWEPByKeys(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    setStepUi(2);

    flagRouterWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0)
    {
        flagRouterWLANSetRight++;
        qDebug() << "---SetRouterWLANWEPByKeys OK";
        emit setRouterWLANFinished();
    } else
    {
        qDebug() << "---SetRouterWLANWEPByKeys ERROR";
        flagRouterWLANSetError++;
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_setRouterWLANWPAPSKByPassphrase(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    setStepUi(2);

    flagRouterWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0)
    {
        flagRouterWLANSetRight++;
        qDebug() << flagRouterWLANSetRight << flagRouterWLANSet;
        qDebug() << "---SetRouterWLANWPAPSKByPassphrase OK";
        emit setRouterWLANFinished();
    }
    else {
        qDebug() << "---SetRouterWLANWPAPSKByPassphrase ERROR";
        flagRouterWLANSetError++;
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
    }
}

void ExtenderMainWindow::slt_setRouterWLANFinished()
{
    // transfer values
    if (!flagWifiAutoSel5G && !flagWifiManualSel5G) {
        m_SSID5G = m_SSID2G;
        m_securitytype5G = m_securitytype2G;
        m_password5G = m_password2G;
        m_passwordWEP5G = m_passwordWEP2G;
        m_newKeyIndex5G = m_newKeyIndex2G;
        m_newWEPLength5G = m_newWEPLength2G;
        m_WEPKey15G = m_WEPKey12G;
        m_WEPKey25G = m_WEPKey22G;
        m_WEPKey35G = m_WEPKey32G;
        m_WEPKey45G = m_WEPKey42G;
    }
    if (!flagWifiAutoSel2G && !flagWifiManualSel2G) {
        m_SSID2G = m_SSID5G;
        m_securitytype2G = m_securitytype5G;
        m_password2G = m_password5G;
        m_passwordWEP2G = m_passwordWEP5G;
        m_newKeyIndex2G = m_newKeyIndex5G;
        m_newWEPLength2G = m_newWEPLength5G;
        m_WEPKey12G = m_WEPKey15G;
        m_WEPKey22G = m_WEPKey25G;
        m_WEPKey32G = m_WEPKey35G;
        m_WEPKey42G = m_WEPKey45G;
    }

    if (g_extenderType == "1000RP") {
        ui->ledt_extenderNtName2G->setText(m_SSID2G +"_EXT");
    }
    if (g_extenderType == "2500RP") {
        ui->ledt_extenderNtName2G->setText(m_SSID2G +"_2GEXT");
        ui->ledt_extenderNtName5G->setText(m_SSID5G + "_5GEXT");
        ui->lbl_extenderNtName5G->show();
        ui->ledt_extenderNtName5G->show();
    }
    ui->ledt_passPhraseWEP2G->setText(m_passwordWEP2G);
    ui->ledt_passPhraseNoWEP2G->setText(m_password2G);
    ui->ledt_key12G->setText(m_WEPKey12G);
    ui->ledt_key22G->setText(m_WEPKey22G);
    ui->ledt_key32G->setText(m_WEPKey32G);
    ui->ledt_key42G->setText(m_WEPKey42G);
    switch (m_newKeyIndex2G) {
    case 1:
        ui->rbtn_key12G->setChecked(true);
        break;
    case 2:
        ui->rbtn_key22G->setChecked(true);
        break;
    case 3:
        ui->rbtn_key32G->setChecked(true);
        break;
    case 4:
        ui->rbtn_key42G->setChecked(true);
        break;
    default:
        break;
    }

    ui->ledt_passPhraseWEP5G->setText(m_passwordWEP5G);
    ui->ledt_passPhraseNoWEP5G->setText(m_password5G);
    ui->ledt_key15G->setText(m_WEPKey15G);
    ui->ledt_key25G->setText(m_WEPKey25G);
    ui->ledt_key35G->setText(m_WEPKey35G);
    ui->ledt_key45G->setText(m_WEPKey45G);

    switch (m_newKeyIndex5G) {
    case 1:
        ui->rbtn_key15G->setChecked(true);
        break;
    case 2:
        ui->rbtn_key25G->setChecked(true);
        break;
    case 3:
        ui->rbtn_key35G->setChecked(true);
        break;
    case 4:
        ui->rbtn_key45G->setChecked(true);
        break;
    default:
        break;
    }

    //control ui
    ui->chk_usingCurrentSetting->setChecked(true);

    if (m_securitytype2G == "OFF") {
        ui->rbtn_none2G->setChecked(true);
        on_rbtn_none2G_clicked(true);
    }else if (m_securitytype2G == "WEP") {
        ui->rbtn_WEP2G->setChecked(true);
        on_rbtn_WEP2G_clicked(true);
    }else if(m_securitytype2G == "WPA-PSK-TKIP") {
        ui->rbtn_WPAPSKTKIP2G->setChecked(true);
        on_rbtn_WPAPSKTKIP2G_clicked(true);
    }else if(m_securitytype2G == "WPA-PSK-AES") {
        ui->rbtn_WPAPSKAES2G->setChecked(true);
        on_rbtn_WPAPSKTKIP2G_clicked(true);
    }else if(m_securitytype2G == "WPA2-PSK-TKIP") {
        ui->rbtn_WPA2PSKTKIP2G->setChecked(true);
        on_rbtn_WPA2PSKAES2G_clicked(true);
    }else if(m_securitytype2G == "WPA2-PSK-AES") {
        ui->rbtn_WPA2PSKAES2G->setChecked(true);
        on_rbtn_WPA2PSKAES2G_clicked(true);
    }else if(m_securitytype2G == "WPA-PSK-TKIP|WPA2-PSK-AES") {
        ui->rtbn_WPAandWAP22G->setChecked(true);
        on_rtbn_WPAandWAP22G_clicked(true);
    }

    if (m_securitytype5G == "OFF") {
        ui->rbtn_none5G->setChecked(true);
        on_rbtn_none5G_clicked(true);
    }else if (m_securitytype5G == "WEP") {
        ui->rbtn_WEP5G->setChecked(true);
        on_rbtn_WEP5G_clicked(true);
    }else if(m_securitytype5G == "WPA-PSK-TKIP") {
        ui->rbtn_WPAPSKTKIP5G->setChecked(true);
        on_rbtn_WPAPSKTKIP5G_clicked(true);
    }else if(m_securitytype5G == "WPA-PSK-AES") {
        ui->rbtn_WPAPSKAES5G->setChecked(true);
        on_rbtn_WPAPSKTKIP5G_clicked(true);
    }else if(m_securitytype5G == "WPA2-PSK-TKIP") {
        ui->rbtn_WPA2PSKTKIP5G->setChecked(true);
        on_rbtn_WPA2PSKAES5G_clicked(true);
    }else if(m_securitytype5G == "WPA2-PSK-AES") {
        ui->rbtn_WPA2PSKAES5G->setChecked(true);
        on_rbtn_WPA2PSKAES5G_clicked(true);
    }else if(m_securitytype5G == "WPA-PSK-TKIP|WPA2-PSK-AES") {
        ui->rtbn_WPAandWPA25G->setChecked(true);
        on_rtbn_WPAandWPA25G_clicked(true);
    }

    if (flagRouterWLANSet == 2)
    {
        // set router wlan finished
        if (flagRouterWLANSetRight == 2)
        {
            qDebug() << "===SetRouterWLANFinished OK";
            ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_deviceToExtender));
        }
        if (flagRouterWLANSetError > 0)
        {
            //
        }
    }
}

//void GenieMainWindow::slt_getRouterWLANVerifyResult(QNetworkReply* reply)
//{
//    QString all = Util::encoding(reply);
//    qDebug() << all;
//}

void ExtenderMainWindow::slt_setWLANNoSecurity(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;
    flagExtenderWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0) {
        flagExtenderWLANSetRight++;
        qDebug() << "---SetWLANNoSecurity OK";
        emit setExtenderWLANFinished();
    } else {
        qDebug() << "---SetWLANNoSecurity ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::slt_setWLANWEPByKeys(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;
    flagExtenderWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0) {
        flagExtenderWLANSetRight++;
        qDebug() << "---SetWLANWEPByKeys OK";
        emit setExtenderWLANFinished();
    } else {
        qDebug() << "---SetWLANWEPByKeys ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::slt_setWLANWPAPSKByPassphrase(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;
    flagExtenderWLANSet++;
    if(all.indexOf("<ResponseCode>000") > 0) {
        flagExtenderWLANSetRight++;
        qDebug() << "---SetWLANWPAPSKByPassphrase OK";
        qDebug() << "---" << flagExtenderWLANSet << flagExtenderWLANSetRight;
        emit setExtenderWLANFinished();
    } else {
        qDebug() << "---SetWLANWPAPSKByPassphrase ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::slt_setExtenderWLANFinished()
{
    if (flagExtenderWLANSet == 2)
    {
        if (flagExtenderWLANSetRight == 2) {
            qDebug() << "===SetExtenderWLANFinished OK";
            soapapi->SetEnable(&SetEnableAccessManager, g_port_extender);
            connect(&SetEnableAccessManager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_setEnable(QNetworkReply*)), Qt::UniqueConnection);
        }
        if (flagExtenderWLANSetError > 0)
        {
            qDebug() << "===SetExtenderWLANFinished ERROR";
        }
    }
}

void ExtenderMainWindow::slt_setEnable(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "--SetEnable Finished OK";
        soapapi->ConfigurationFinished(&ConfigurationFinishedAccessManager, g_port_extender, NewSessionID);
        connect(&ConfigurationFinishedAccessManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_configurationFinished(QNetworkReply*)), Qt::UniqueConnection);
    } else {
        qDebug() << "--SetEnable Finished ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::slt_configurationFinished(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "--ConfigurationFinished OK";
        configurationFinishedResponsTime = ExtenderUtil::GetNodeInfo(all, "ResponseTime").toInt();
        if (flagWizard) {
            slt_extendFinish();
        }
        if (flagAdvanced) {
            qApp->quit();
        }
    } else {
        qDebug() << "--ConfigurationFinished ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

void ExtenderMainWindow::sltResetToFactoryDefault(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if(all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "--sltResetToFactoryDefault OK";
        RebootTime = ExtenderUtil::GetNodeInfo(all, "RebootTime");

        qDebug() << "Genie reboot time is: " << RebootTime;
        QTimer::singleShot(RebootTime.toInt()*1000, qApp, SLOT(quit()));
    } else {
        qDebug() << "--ConfigurationFinished ERROR";
        if (flagWizard)
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_showError));
        if (flagAdvanced)
            sltBackChoosePage();
    }
}

// slots for advanced set
void ExtenderMainWindow::sltGetRouterWLANInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;
    if (all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "--sltGetRouterWLANInfo2G OK";
        routerStatus2G = ExtenderUtil::GetNodeInfo(all, "NewStatus");
        routerSSID2G = ExtenderUtil::GetNodeInfo(all, "NewSSID");
        routerEncryptionModes2G = ExtenderUtil::GetNodeInfo(all, "NewEncryptionModes");

        // when OK, get the other outer infomation
        if (routerEncryptionModes2G == "OFF") {
            routerPassword2G = "None";
        } else if (routerEncryptionModes2G == "WEP") {
            connect(&GetRouterWLANWEPInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetRouterWLANWEPInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetRouterWLANWEPInfo(&GetRouterWLANWEPInfoManager2G, g_port_extender, "2.4G");
        } else {
            connect(&GetRouterWLANWPAInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetRouterWLANWPAInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetRouterWLANWPAInfo(&GetRouterWLANWPAInfoManager2G, g_port_extender, "2.4G");
        }
    }
    else {
        getRouterSetCount++;
        emit emitGetRouterInfo();
        // when error, means no router be connected
        ui->lbRouterInfo2G->hide();
        ui->lbRouterStatusShow2G->hide();
        ui->lbRouterName2G->hide();
        ui->lbRouterNameShow2G->hide();
        ui->lbRouterPassword2G->hide();
        ui->lbRouterStatus2G->hide();
        ui->lbRouterPasswordShow2G->hide();
    }

    if (g_extenderType == "2500RP") {
        // next, get router information for 5G
        connect(&GetRouterWLANInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(sltGetRouterWLANInfo5G(QNetworkReply*)), Qt::UniqueConnection);
        soapapi->GetRouterWLANInfo(&GetRouterWLANInfoManager5G, g_port_extender, "5G");
    }
}

void ExtenderMainWindow::sltGetRouterWLANInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        qDebug() << "--sltGetRouterWLANInfo5G OK";
        routerStatus5G = ExtenderUtil::GetNodeInfo(all, "NewStatus");
        routerSSID5G = ExtenderUtil::GetNodeInfo(all, "NewSSID");
        routerEncryptionModes5G = ExtenderUtil::GetNodeInfo(all, "NewEncryptionModes");

        // when OK, get the other outer infomation
        if (routerEncryptionModes5G == "OFF") {
            routerPassword5G = "None";
        } else if (routerEncryptionModes5G == "WEP") {
            connect(&GetRouterWLANWEPInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetRouterWLANWEPInfo5G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetRouterWLANWEPInfo(&GetRouterWLANWEPInfoManager5G, g_port_extender, "5G");
        } else {
            connect(&GetRouterWLANWPAInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetRouterWLANWPAInfo5G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetRouterWLANWPAInfo(&GetRouterWLANWPAInfoManager5G, g_port_extender, "5G");
        }
    }
    else {
        getRouterSetCount++;
        emit emitGetRouterInfo();
        // when error, means no router be connected
        ui->lbRouterInfo5G->hide();
        ui->lbRouterStatusShow5G->hide();
        ui->lbRouterName5G->hide();
        ui->lbRouterNameShow5G->hide();
        ui->lbRouterPassword5G->hide();
        ui->lbRouterStatus5G->hide();
        ui->lbRouterPasswordShow5G->hide();
    }
}

void ExtenderMainWindow::sltGetRouterWLANWPAInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getRouterSetCount++;
        qDebug() << "--sltGetRouterWLANWPAInfo2G OK";
        routerPassword2G = ExtenderUtil::GetNodeInfo(all, "NewWPAPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetRouterInfo();
}

void ExtenderMainWindow::sltGetRouterWLANWPAInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getRouterSetCount++;
        qDebug() << "--sltGetRouterWLANWPAInfo5G OK";
        routerPassword5G = ExtenderUtil::GetNodeInfo(all, "NewWPAPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetRouterInfo();
}

void ExtenderMainWindow::sltGetRouterWLANWEPInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getRouterSetCount++;
        qDebug() << "--sltGetRouterWLANWEPInfo2G OK";
        routerPassword2G = ExtenderUtil::GetNodeInfo(all, "NewWEPPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetRouterInfo();
}

void ExtenderMainWindow::sltGetRouterWLANWEPInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getRouterSetCount++;
        qDebug() << "--sltGetRouterWLANWEPInfo5G OK";
        routerPassword5G = ExtenderUtil::GetNodeInfo(all, "NewWEPPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetRouterInfo();
}

void ExtenderMainWindow::sltGetWLANInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANInfo2G OK";
        wlanStatus2G = ExtenderUtil::GetNodeInfo(all, "NewStatus");
        wlanSSID2G = ExtenderUtil::GetNodeInfo(all, "NewSSID");
        wlanEncryptionModes2G = ExtenderUtil::GetNodeInfo(all, "NewEncryptionModes");

        // when OK, get the other WLAN infomation
        if (wlanEncryptionModes2G == "OFF") {
            wlanPassword2G = "None";
        } else if (wlanEncryptionModes2G == "WEP") {
            connect(&GetWLANWEPInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetWLANWEPInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetWLANWEPInfo(&GetWLANWEPInfoManager2G, g_port_extender, "2.4G");
        } else {
            connect(&GetWLANWPAInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetWLANWPAInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetWLANWPAInfo(&GetWLANWPAInfoManager2G, g_port_extender, "2.4G");
        }
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::sltGetWLANInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANInfo5G OK";
        wlanStatus5G = ExtenderUtil::GetNodeInfo(all, "NewStatus");
        wlanSSID5G = ExtenderUtil::GetNodeInfo(all, "NewSSID");
        wlanEncryptionModes5G = ExtenderUtil::GetNodeInfo(all, "NewEncryptionModes");

        // when OK, get the other WLAN infomation
        if (wlanEncryptionModes5G == "OFF") {
            wlanPassword5G = "None";
        } else if (wlanEncryptionModes5G == "WEP") {
            connect(&GetWLANWEPInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetWLANWEPInfo5G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetWLANWEPInfo(&GetWLANWEPInfoManager5G, g_port_extender, "5G");
        } else {
            connect(&GetWLANWPAInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(sltGetWLANWPAInfo5G(QNetworkReply*)), Qt::UniqueConnection);
            soapapi->GetWLANWPAInfo(&GetWLANWPAInfoManager5G, g_port_extender, "5G");
        }
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::sltGetWLANWPAInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANWPAInfo2G OK";
        wlanWPAPassphrase2G = ExtenderUtil::GetNodeInfo(all, "NewWPAPassphrase");
        wlanPassword2G = wlanWPAPassphrase2G;
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::sltGetWLANWPAInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANWPAInfo5G OK";
        wlanWPAPassphrase5G = ExtenderUtil::GetNodeInfo(all, "NewWPAPassphrase");
        wlanPassword5G = wlanWPAPassphrase5G;
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::sltGetWLANWEPInfo2G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANWEPInfo2G OK";
        wlanWEPPassphrase2G = ExtenderUtil::GetNodeInfo(all, "NewWEPPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::sltGetWLANWEPInfo5G(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);
    qDebug() << all;

    if (all.indexOf("<ResponseCode>000") > 0) {
        getWLANSetCount++;
        qDebug() << "--sltGetWLANWEPInfo5G OK";
        wlanWEPPassphrase5G = ExtenderUtil::GetNodeInfo(all, "NewWEPPassphrase");
    }
    else {
        emit emitGetError();
    }
    emit emitGetWLANInfo();
}

void ExtenderMainWindow::slt_extendFinish()
{
    //transfer value
    ui->lbl_SSIDFinishShow2G->setText(m_extenderSSID2G);
    ui->lbl_securitytypeFinishShow2G->setText(m_extenderSecuritytype2G);
    ui->lbl_passwordFinishShow2G->setText(m_extenderPassword2G);

    ui->lbl_WEPStrengthShow2G->setText(QString::number(m_extenderNewWEPLength2G));
    ui->lbl_indexShow2G->setText(QString::number(m_newKeyIndex2G));
    ui->lbl_key1Show2G->setText(m_extenderWEPKey12G);
    ui->lbl_key2Show2G->setText(m_extenderWEPKey22G);
    ui->lbl_key3Show2G->setText(m_extenderWEPKey32G);
    ui->lbl_key4Show2G->setText(m_extenderWEPKey42G);

    ui->lbl_SSIDFinishShow5G->setText(m_extenderSSID5G);
    ui->lbl_securitytypeFinishShow5G->setText(m_extenderSecuritytype5G);
    ui->lbl_passwordFinishShow5G->setText(m_extenderPassword5G);

    ui->lbl_WEPStrengthShow5G->setText(QString::number(m_extenderNewWEPLength5G));
    ui->lbl_indexShow5G->setText(QString::number(m_newKeyIndex5G));
    ui->lbl_key1Show5G->setText(m_extenderWEPKey15G);
    ui->lbl_key2Show5G->setText(m_extenderWEPKey25G);
    ui->lbl_key3Show5G->setText(m_extenderWEPKey35G);
    ui->lbl_key4Show5G->setText(m_extenderWEPKey45G);

    if (m_extenderSecuritytype2G == "OFF")
    {
        ui->lbl_securitytypeFinishShow2G->setText("None");
        ui->lbl_passphraseFinish2G->hide();
        ui->lbl_passwordFinishShow2G->hide();
    }
    if (m_extenderSecuritytype2G == "WEP") {
        ui->frm_finishWEP2G->show();
    }
    if (m_extenderSecuritytype5G == "OFF")
    {
        ui->lbl_securitytypeFinishShow5G->setText("None");
        ui->lbl_passphraseFinish5G->hide();
        ui->lbl_passwordFinishShow5G->hide();
    }
    if (m_extenderSecuritytype5G == "WEP") {
        ui->frm_finishWEP5G->show();
    }

    if (g_extenderType == "1000RP") {
        ui->frm_finishShow5G->hide();
        ui->line_finishPageMiddle->hide();
        ui->frm_extenderRadioChoose->hide();
    }

    // waitting for apply
    ExtenderUtil::setExtenderSleep(configurationFinishedResponsTime * 1000);

    ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_extendFinish));
}
/*
 *=====Soap slots end
 */


/*
 *=====Functions start
 */
void ExtenderMainWindow::setStepUi(int step)
{
    ui->lbl_extenderToRouter1000->hide();
    ui->lbl_deviceToExtender1000->hide();
    ui->lbl_applyExtender1000->hide();
    ui->lbl_extenderToRouter2500->hide();
    ui->lbl_deviceToExtender2500->hide();
    ui->lbl_applyExtender2500->hide();

    if (g_extenderType == "1000RP") {
        switch(step)
        {
        case 1:
        {
            ui->lbl_extenderToRouter1000->setText("");
            ui->lbl_extenderToRouter1000->show();
            break;
        }
        case 2:
        {
            ui->lbl_deviceToExtender1000->setText("");
            ui->lbl_deviceToExtender1000->show();
            break;
        }
        case 3:
        {
            ui->lbl_applyExtender1000->setText("");
            ui->lbl_applyExtender1000->show();
            break;
        }
        }
    }
    if (g_extenderType == "2500RP") {
        switch(step)
        {
        case 1:
        {
            ui->lbl_extenderToRouter2500->setText("");
            ui->lbl_extenderToRouter2500->show();
            break;
        }
        case 2:
        {
            ui->lbl_deviceToExtender2500->setText("");
            ui->lbl_deviceToExtender2500->show();
            break;
        }
        case 3:
        {
            ui->lbl_applyExtender2500->setText("");
            ui->lbl_applyExtender2500->show();
            break;
        }
        }
    }
}

void ExtenderMainWindow::showAPList2G(QString str)
{
    QMap<int, QStringList>::iterator iterMap;
    QMap<int, QStringList> APListMap;
    ExtenderUtil::GetAPInfo(str, APListMap);

    if (ui->rbtn_yes->isChecked()) {
        ui->tblwdt_wifiList2G->setRowCount(APListMap.size());
        ui->tblwdt_wifiList2G->setEditTriggers( QAbstractItemView::NoEditTriggers );
        ui->tblwdt_wifiList2G->setSelectionMode( QAbstractItemView::SingleSelection );

        int row = 0;
        int col = 0;
        ui->tblwdt_wifiList2G->clearContents();

        // Add item to wifiList
        for (iterMap = APListMap.begin(); iterMap != APListMap.end(); ++iterMap) {
            qDebug() << iterMap.key() << iterMap.value() << endl;
            QStringList list = iterMap.value();
            foreach (QString item, list) {
                if (g_extenderType == "2500RP") {
                    if (4 == col)
                        item += "%";
                }
                if (col < 2){
                    ui->tblwdt_wifiList2G->setItem(row, col, new QTableWidgetItem(item));
                }
                if (col > 2) {
                    ui->tblwdt_wifiList2G->setItem(row, col - 1, new QTableWidgetItem(item));
                }
                ui->tblwdt_wifiList2G->setRowHeight(row, 20);

                col ++;
            }
            col = 0;
            row ++;

        }

        int wifiListRows = ui->tblwdt_wifiList2G->rowCount();
        if (0 == wifiListRows)
            ui->rbtn_auto2G->setCheckable(false);
        else
            ui->rbtn_auto2G->setCheckable(true);

        //set color change between adjoin rows
        for(int i = 0; i < ui->tblwdt_wifiList2G->rowCount(); i++) {
            for(int j = 0; j < ui->tblwdt_wifiList2G->columnCount(); j++) {
                ui->tblwdt_wifiList2G->item(i, j)->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item = ui->tblwdt_wifiList2G->item(i, j);

                const QColor lightGrayColor = QColor("#e5e5e5");
                const QColor whiteColor = QColor("#ffffff");

                if(i % 2 != 0) {
                    item->setBackgroundColor(lightGrayColor);
                }
                else
                    item->setBackgroundColor(whiteColor);
            }
        }
    }
}

void ExtenderMainWindow::showAPList5G(QString str)
{
    QMap<int, QStringList>::iterator iterMap;
    QMap<int, QStringList> APListMap;
    ExtenderUtil::GetAPInfo(str, APListMap);

    if (ui->rbtn_yes->isChecked()) {
        ui->tblwdt_wifiList5G->setRowCount(APListMap.size());
        ui->tblwdt_wifiList5G->setEditTriggers( QAbstractItemView::NoEditTriggers );
        ui->tblwdt_wifiList5G->setSelectionMode( QAbstractItemView::SingleSelection );

        int row = 0;
        int col = 0;
        ui->tblwdt_wifiList5G->clearContents();

        // Add item to wifiList
        for (iterMap = APListMap.begin(); iterMap != APListMap.end(); ++iterMap) {
            qDebug() << iterMap.key() << iterMap.value() << endl;
            QStringList list = iterMap.value();
            foreach (QString item, list) {
                if (g_extenderType == "2500RP") {
                    if (4 == col)
                        item += "%";
                }
                if (col < 2){
                    ui->tblwdt_wifiList5G->setItem(row, col, new QTableWidgetItem(item));
                }
                if (col > 2) {
                    ui->tblwdt_wifiList5G->setItem(row, col - 1, new QTableWidgetItem(item));
                }
                ui->tblwdt_wifiList5G->setRowHeight(row, 20);
                col ++;
            }
            col = 0;
            row ++;

        }


        int wifiListRows = ui->tblwdt_wifiList5G->rowCount();
        if (0 == wifiListRows)
            ui->rbtn_auto5G->setCheckable(false);
        else
            ui->rbtn_auto5G->setCheckable(true);

        //set color change between adjoin rows
        for(int i = 0; i < ui->tblwdt_wifiList5G->rowCount(); i++) {
            for(int j = 0; j < ui->tblwdt_wifiList5G->columnCount(); j++) {
                ui->tblwdt_wifiList5G->item(i, j)->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item = ui->tblwdt_wifiList5G->item(i, j);

                const QColor lightGrayColor = QColor("#e5e5e5");
                const QColor whiteColor = QColor("#ffffff");

                if(i % 2 != 0) {
                    item->setBackgroundColor(lightGrayColor);
                }
                else
                    item->setBackgroundColor(whiteColor);
            }
        }

        if (ui->tblwdt_wifiList5G->rowCount() == 0)
        {
            ui->rbtn_auto5G->setEnabled(false);
        }
    }
}

void ExtenderMainWindow::setRouterWLANNoSecurity(QString radioModel)
{
    if(radioModel == "2.4G")
    {
        soapapi->SetRouterWLANNoSecurity(&SetRouterWLANNoSecurityAccessManager2G, g_port_extender, "2.4G", m_SSID2G, "Auto", "Auto", 0);
        connect(&SetRouterWLANNoSecurityAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
    }
    if(radioModel == "5G")
    {
        soapapi->SetRouterWLANNoSecurity(&SetRouterWLANNoSecurityAccessManager5G, g_port_extender, "5G", m_SSID5G, "Auto", "Auto", 0);
        connect(&SetRouterWLANNoSecurityAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
    }
}

void ExtenderMainWindow::setRouterWLANWEPByKeys(QString radioModel)
{
    if(radioModel == "2.4G")
    {
        soapapi->SetRouterWLANWEPByKeys(&SetRouterWLANWEPByKeysAccessManager2G, g_port_extender, "2.4G", m_SSID2G,"Auto","Auto","Auto",
                                        m_newWEPLength2G, m_newKeyIndex2G,  m_passwordWEP2G, 0);
        connect(&SetRouterWLANWEPByKeysAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
    }
    if(radioModel == "5G")
    {
        soapapi->SetRouterWLANWEPByKeys(&SetRouterWLANWEPByKeysAccessManager5G, g_port_extender, "5G", m_SSID5G,"Auto","Auto","Auto",
                                        m_newWEPLength5G, m_newKeyIndex5G, m_passwordWEP5G, 0);
        connect(&SetRouterWLANWEPByKeysAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
    }
}

void ExtenderMainWindow::setRouterWLANWPAPSKByPassphrase(QString radioModel)
{
    if(radioModel == "2.4G")
    {
        soapapi->SetRouterWLANWPAPSKByPassphrase(&SetRouterWLANWPAPSKByPassphraseAccessManager2G, g_port_extender, "2.4G",
                                                 m_SSID2G, "Auto","Auto", m_securitytype2G, m_password2G, 0);
        connect(&SetRouterWLANWPAPSKByPassphraseAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
    }

    if(radioModel == "5G")
    {
        soapapi->SetRouterWLANWPAPSKByPassphrase(&SetRouterWLANWPAPSKByPassphraseAccessManager5G, g_port_extender, "5G",
                                                 m_SSID5G, "Auto","Auto", m_securitytype5G, m_password5G, 0);
        connect(&SetRouterWLANWPAPSKByPassphraseAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slt_setRouterWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);

    }
}

void ExtenderMainWindow::setRouterWLAN()
{
    //initial vaule
    flagRouterWLANSet = 0;
    flagRouterWLANSetRight = 0;
    flagRouterWLANSetError = 0;
    //control page goto
    if ((g_extenderType == "1000RP")
            || ((!flagWifiAutoSel2G && !flagWifiManualSel2G)
                || (!flagWifiAutoSel5G && !flagWifiManualSel5G)))
    {
        flagRouterWLANSet++;
        flagRouterWLANSetRight++;
        flagRouterWLANSetError++;
    }

    //transfer value for router2wlan
    if (flagWifiAutoSel2G)
    {
        m_SSID2G = ui->lbl_SSIDNameAuto2G->text();
        m_password2G = ui->ledt_wlanPasswordAutoSet2G->text();
        m_passwordWEP2G = ui->ledt_wlanPasswordWEPAutoSet2G->text();
        setRouterWLANAuto("2.4G");
    }
    if (flagWifiAutoSel5G)
    {
        m_SSID2G = ui->lbl_SSIDNameAuto5G->text();
        m_password5G = ui->ledt_wlanPasswordAutoSet5G->text();
        m_passwordWEP5G = ui->ledt_wlanPasswordWEPAutoSet5G->text();
        setRouterWLANAuto("5G");
    }
    if (flagWifiManualSel2G)
    {
        m_SSID2G = ui->ledt_SSIDNameManual2G->text();
        m_password2G = ui->ledt_wlanPasswordManualSet2G->text();
        m_passwordWEP2G = ui->ledt_wlanPasswordWEPManualSet2G->text();
        setRouterWLANManual("2.4G");
    }
    if (flagWifiManualSel5G)
    {
        m_SSID5G = ui->ledt_SSIDNameManual5G->text();
        m_password5G = ui->ledt_wlanPasswordManualSet5G->text();
        m_passwordWEP5G = ui->ledt_wlanPasswordWEPManualSet5G->text();
        setRouterWLANManual("5G");
    }
}

void ExtenderMainWindow::setRouterWLANManual(QString radioModel)
{
    if (radioModel == "2.4G") {
        if(m_securitytype2G == "OFF")
        {
            setRouterWLANNoSecurity("2.4G");
        }
        if(m_securitytype2G == "WEP")
        {
            setRouterWLANWEPByKeys("2.4G");
        }
        if(m_securitytype2G == "WPA-PSK-TKIP"
                || m_securitytype2G == "WPA-PSK-AES"
                || m_securitytype2G == "WPA2-PSK-TKIP"
                || m_securitytype2G == "WPA2-PSK-AES"
                || m_securitytype2G == "WPA-PSK-TKIP|WPA2-PSK-AES")
        {
            setRouterWLANWPAPSKByPassphrase("2.4G");
        }
    }
    if (radioModel == "5G") {
        if(m_securitytype5G == "OFF")
        {
            setRouterWLANNoSecurity("5G");
        }
        if(m_securitytype5G == "WEP")
        {
            setRouterWLANWEPByKeys("5G");
        }
        if(m_securitytype5G == "WPA-PSK-TKIP"
                || m_securitytype5G == "WPA-PSK-AES"
                || m_securitytype5G == "WPA2-PSK-TKIP"
                || m_securitytype5G == "WPA2-PSK-AES"
                || m_securitytype5G == "WPA-PSK-TKIP|WPA2-PSK-AES")
        {
            setRouterWLANWPAPSKByPassphrase("5G");
        }
    }
}

void ExtenderMainWindow::setRouterWLANAuto(QString radioModel)
{
    if (radioModel == "2.4G") {
        if (m_securitytype2G == "OFF") {
            setRouterWLANNoSecurity("2.4G");
        }
        if (m_securitytype2G == "WEP") {
            setRouterWLANWEPByKeys("2.4G");
        }
        if(m_securitytype2G == "WPA-PSK-TKIP"
                || m_securitytype2G == "WPA-PSK-AES"
                || m_securitytype2G == "WPA2-PSK-TKIP"
                || m_securitytype2G == "WPA2-PSK-AES"
                || m_securitytype2G == "WPA-PSK-TKIP|WPA2-PSK-AES")
        {
            setRouterWLANWPAPSKByPassphrase("2.4G");
        }
    }

    if (radioModel == "5G") {
        if (m_securitytype5G == "OFF") {
            setRouterWLANNoSecurity("5G");
        }
        if (m_securitytype5G == "WEP") {
            setRouterWLANWEPByKeys("5G");
        }
        if(m_securitytype5G == "WPA-PSK-TKIP"
                || m_securitytype5G == "WPA-PSK-AES"
                || m_securitytype5G == "WPA2-PSK-TKIP"
                || m_securitytype5G == "WPA2-PSK-AES"
                || m_securitytype5G == "WPA-PSK-TKIP|WPA2-PSK-AES")
        {
            setRouterWLANWPAPSKByPassphrase("5G");
        }
    }
}
/*
 *=====Functions end
 */

/*
 *=====Main buttons slots start
 */
void ExtenderMainWindow::on_btn_funcChoose_clicked()
{
    flagWizard = false;
    flagAdvanced = false;
    if (ui->rbtn_yes->isChecked()) {
        flagWizard = true;
        setStepUi(1);
        setWaittingUi(ui->vl_wait2);

        waitting_text2_idx = MainWindow_lbl_waitText2;
        ui->lbl_waitText2->setText(translateText(MainWindow_lbl_waitText2));
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_setting));
        ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wait2));

        // the second step of soap
        if (g_extenderType == "1000RP") {
            soapapi->GetAPInfo(&GetAPInfoAccessManager2G, g_port_extender, "2.4G");
            connect(&GetAPInfoAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            connect(&GetAPInfoAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPListFinished()), Qt::UniqueConnection);
        }
        if (g_extenderType == "2500RP") {
            soapapi->GetAPInfo(&GetAPInfoAccessManager2G, g_port_extender, "2.4G");
            connect(&GetAPInfoAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPInfo2G(QNetworkReply*)), Qt::UniqueConnection);
            connect(&GetAPInfoAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPListFinished()), Qt::UniqueConnection);

            soapapi->GetAPInfo(&GetAPInfoAccessManager5G, g_port_extender, "5G");
            connect(&GetAPInfoAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPInfo5G(QNetworkReply*)), Qt::UniqueConnection);
            connect(&GetAPInfoAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(slt_getAPListFinished()), Qt::UniqueConnection);
        }
    }

    if (ui->rbtn_no->isChecked()) {
        flagAdvanced = true;

        // TODO: Advanced design
        waitting_text_idx = MainWindow_lbl_waitText2_1;
        setWaittingText(MainWindow_lbl_waitText2_1);

        ui->frm5G->hide();
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));
        setWaittingUi(ui->vl_wait);

        getRouterSetCount = 0;
        connect(&GetRouterWLANInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(sltGetRouterWLANInfo2G(QNetworkReply*)), Qt::UniqueConnection);
        soapapi->GetRouterWLANInfo(&GetRouterWLANInfoManager2G, g_port_extender, "2.4G");

        connect(this, SIGNAL(emitGetRouterInfo()), this, SLOT(sltGetRouterInfoFinished()), Qt::UniqueConnection);
        connect(this, SIGNAL(emitGetWLANInfo()), this, SLOT(sltGetWLANInfoFinished()), Qt::UniqueConnection);
        connect(this, SIGNAL(emitGetError()), this, SLOT(sltBackChoosePage()), Qt::UniqueConnection);
    }
}

void ExtenderMainWindow::sltGetRouterInfoFinished()
{
    qDebug() << "getRouterSetCount: " << getRouterSetCount;
    if (g_extenderType == "1000RP" && getRouterSetCount == 1) {
        getWLANSetCount = 0;
        qDebug() << "Get router infomation finished";
        connect(&GetWLANInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(sltGetWLANInfo2G(QNetworkReply*)), Qt::UniqueConnection);
        soapapi->GetWLANInfo(&GetWLANInfoManager2G, g_port_extender, "2.4G");
    }

    if (g_extenderType == "2500RP" && getRouterSetCount == 2) {
        getWLANSetCount = 0;
        qDebug() << "Get router infomation finished";
        connect(&GetWLANInfoManager2G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(sltGetWLANInfo2G(QNetworkReply*)), Qt::UniqueConnection);
        soapapi->GetWLANInfo(&GetWLANInfoManager2G, g_port_extender, "2.4G");

        connect(&GetWLANInfoManager5G, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(sltGetWLANInfo5G(QNetworkReply*)), Qt::UniqueConnection);
        soapapi->GetWLANInfo(&GetWLANInfoManager5G, g_port_extender, "5G");
    }
}

void ExtenderMainWindow::sltGetWLANInfoFinished()
{
    qDebug() << getWLANSetCount;
    qDebug() << "IsResetToDefault: " << bIsResetToDefault;
    // if default device, hide some widgets
    if (bIsResetToDefault == 1) {
        ui->lbRouterInfo2G->hide();
        ui->lbRouterStatus2G->hide();
        ui->lbRouterStatusShow2G->hide();
        ui->lbRouterName2G->hide();
        ui->lbRouterNameShow2G->hide();
        ui->lbRouterPassword2G->hide();
        ui->lbRouterPasswordShow2G->hide();

        ui->lbRouterInfo5G->hide();
        ui->lbRouterStatus5G->hide();
        ui->lbRouterStatusShow5G->hide();
        ui->lbRouterName5G->hide();
        ui->lbRouterNameShow5G->hide();
        ui->lbRouterPassword5G->hide();
        ui->lbRouterPasswordShow5G->hide();

        ui->line_4->hide();
        ui->line_7->hide();
    }

    if (g_extenderType == "1000RP" && getWLANSetCount == 2) {
        qDebug() << "Get wlan information finished";
        ui->lbRouterStatusShow2G->setText(routerStatus2G);
        ui->lbRouterNameShow2G->setText(routerSSID2G);
        ui->lbRouterPasswordShow2G->setText(routerPassword2G);
        ui->lbWlanStatus2G->setText(wlanStatus2G);
        ui->ledtWlanName2G->setText(wlanSSID2G);

        if (wlanEncryptionModes2G == "None") {
            ui->cbxWlanAuthType2G->setCurrentIndex(0);
            ui->lbWlanPassword2G->setVisible(false);
            ui->ledtWlanPasswrod2G->setVisible(false);
        } else if (wlanEncryptionModes2G == "WEP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(1);
        } else if (wlanEncryptionModes2G == "WPA-PSK-TKIP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(2);
        } else if (wlanEncryptionModes2G == "WPA-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(3);
        } else if (wlanEncryptionModes2G == "WPA2-PSK-TKIP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(4);
        } else if (wlanEncryptionModes2G == "WPA2-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(5);
        } else if (wlanEncryptionModes2G == "WPA-PSK-TKIP|WPA2-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(6);
        }

        ui->ledtWlanPasswrod2G->setText(wlanPassword2G);

        // show the advanced set page
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pgManualSet));
    }

    if (g_extenderType == "2500RP" && getWLANSetCount == 4) {
        qDebug() << "Get wlan information finished";
        ui->frm5G->show();
        ui->lbRouterStatusShow2G->setText(routerStatus2G);
        ui->lbRouterNameShow2G->setText(routerSSID2G);
        ui->lbRouterPasswordShow2G->setText(routerPassword2G);
        ui->lbWlanStatus2G->setText(wlanStatus2G);
        ui->ledtWlanName2G->setText(wlanSSID2G);
        if (wlanEncryptionModes2G == "None") {
            ui->cbxWlanAuthType2G->setCurrentIndex(0);
            ui->lbWlanPassword2G->setVisible(false);
            ui->ledtWlanPasswrod2G->setVisible(false);
        } else if (wlanEncryptionModes2G == "WEP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(1);
        } else if (wlanEncryptionModes2G == "WPA-PSK-TKIP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(2);
        } else if (wlanEncryptionModes2G == "WPA-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(3);
        } else if (wlanEncryptionModes2G == "WPA2-PSK-TKIP") {
            ui->cbxWlanAuthType2G->setCurrentIndex(4);
        } else if (wlanEncryptionModes2G == "WPA2-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(5);
        } else if (wlanEncryptionModes2G == "WPA-PSK-TKIP|WPA2-PSK-AES") {
            ui->cbxWlanAuthType2G->setCurrentIndex(6);
        }
        ui->ledtWlanPasswrod2G->setText(wlanPassword2G);

        ui->lbRouterStatusShow5G->setText(routerStatus5G);
        ui->lbRouterNameShow5G->setText(routerSSID5G);
        ui->lbRouterPasswordShow5G->setText(routerPassword5G);
        ui->lbWlanStatus5G->setText(wlanStatus5G);
        ui->ledtWlanName5G->setText(wlanSSID5G);
        if (wlanEncryptionModes5G == "None") {
            ui->cbxWlanAuthType5G->setCurrentIndex(0);
            ui->lbWlanPassword5G->setVisible(false);
            ui->ledtWlanPasswrod5G->setVisible(false);
        } else if (wlanEncryptionModes5G == "WEP") {
            ui->cbxWlanAuthType5G->setCurrentIndex(1);
        } else if (wlanEncryptionModes5G == "WPA-PSK-TKIP") {
            ui->cbxWlanAuthType5G->setCurrentIndex(2);
        } else if (wlanEncryptionModes5G == "WPA-PSK-AES") {
            ui->cbxWlanAuthType5G->setCurrentIndex(3);
        } else if (wlanEncryptionModes5G == "WPA2-PSK-TKIP") {
            ui->cbxWlanAuthType5G->setCurrentIndex(4);
        } else if (wlanEncryptionModes5G == "WPA2-PSK-AES") {
            ui->cbxWlanAuthType5G->setCurrentIndex(5);
        } else if (wlanEncryptionModes5G == "WPA-PSK-TKIP|WPA2-PSK-AES") {
            ui->cbxWlanAuthType5G->setCurrentIndex(6);
        }
        ui->ledtWlanPasswrod5G->setText(wlanPassword5G);

        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pgManualSet));
    }
}

void ExtenderMainWindow::sltBackChoosePage()
{
    QString title = translateText(MsgBox_GetInfoError_Title);
    QString content = translateText(MsgBox_GetInfoError_Content);
    QMessageBox msgBox;
    QPushButton *okBtn = msgBox.addButton(QMessageBox::Ok);
    okBtn->setText(translateText(MsgBox_OK));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(title);
    //msgBox.setInformativeText(content);
    msgBox.exec();

    if (msgBox.clickedButton() == okBtn) {
        ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_start));
    }
}

void ExtenderMainWindow::on_btn_refresh_clicked()
{
    qDebug() << "Geine refresh the AP list";
    m_refresh_progress_panel->start();
    ui->btn_refresh->setEnabled(false);
    //control ui
    m_bWifiListRefreshing = true;
    flagWifiSelected2G = false;
    flagWifiSelected5G = false;
    flagWifiAutoSel2G = false;
    flagWifiAutoSel5G = false;
    flagWifiManualSel2G = false;
    flagWifiManualSel5G = false;
    ui->rbtn_auto2G->setChecked(false);
    ui->rbtn_auto5G->setChecked(false);
    ui->rbtn_manual2G->setChecked(false);
    ui->rbtn_manual5G->setChecked(false);
    ui->btn_extenderToRouter->setEnabled(false);
    ui->rbtn_auto2G->setEnabled(false);
    ui->rbtn_auto5G->setEnabled(false);
    ui->rbtn_manual2G->setEnabled(false);
    ui->rbtn_manual5G->setEnabled(false);
    ui->tabwdt_wifiList->setCurrentIndex(ui->tabwdt_wifiList->indexOf(ui->tab_2G));

    //initial flag
    flagGetAPListFinished2G = false;
    flagGetAPListFinished5G = false;

    //clear ui
    ui->ledt_wlanPasswordAutoSet2G->clear();
    ui->cmb_bitAutoSet2G->setCurrentIndex(0);
    ui->ledt_wlanPasswordWEPAutoSet2G->clear();
    ui->rbtn_key1AutoSet2G->setChecked(true);
    ui->ledt_key1AutoSet2G->clear();
    ui->ledt_key2AutoSet2G->clear();
    ui->ledt_key3AutoSet2G->clear();
    ui->ledt_key4AutoSet2G->clear();
    ui->ledt_SSIDNameManual2G->clear();
    ui->cbx_securityTypeChooseManual2G->setCurrentIndex(0);
    ui->cmb_bitManualSet2G->setCurrentIndex(0);
    ui->ledt_wlanPasswordWEPManualSet2G->clear();
    ui->rbtn_key1ManualSet2G->setChecked(true);
    ui->ledt_key1ManualSet2G->clear();
    ui->ledt_key2ManualSet2G->clear();
    ui->ledt_key3ManualSet2G->clear();
    ui->ledt_key4ManualSet2G->clear();
    ui->ledt_wlanPasswordManualSet2G->clear();

    ui->ledt_wlanPasswordAutoSet5G->clear();
    ui->cmb_bitAutoSet5G->setCurrentIndex(0);
    ui->ledt_wlanPasswordWEPAutoSet5G->clear();
    ui->rbtn_key1AutoSet5G->setChecked(true);
    ui->ledt_key1AutoSet5G->clear();
    ui->ledt_key2AutoSet5G->clear();
    ui->ledt_key3AutoSet5G->clear();
    ui->ledt_key4AutoSet5G->clear();
    ui->ledt_SSIDNameManual5G->clear();
    ui->cbx_securityTypeChooseManual5G->setCurrentIndex(0);
    ui->cmb_bitManualSet5G->setCurrentIndex(0);
    ui->ledt_wlanPasswordWEPManualSet5G->clear();
    ui->rbtn_key1ManualSet5G->setChecked(true);
    ui->ledt_key1ManualSet5G->clear();
    ui->ledt_key2ManualSet5G->clear();
    ui->ledt_key3ManualSet5G->clear();
    ui->ledt_key4ManualSet5G->clear();
    ui->ledt_wlanPasswordManualSet5G->clear();

    //connect have done
    if (g_extenderType == "1000RP") {
        soapapi->GetAPInfo(&GetAPInfoAccessManager2G, g_port_extender, "2.4G");
    }
    if (g_extenderType == "2500RP") {
        soapapi->GetAPInfo(&GetAPInfoAccessManager2G, g_port_extender, "2.4G");
        soapapi->GetAPInfo(&GetAPInfoAccessManager5G, g_port_extender, "5G");
    }
}

void ExtenderMainWindow::on_btn_extenderToRouter_clicked()
{
    bool bHasWifiChoosed = false;
    bool bAuto2GChoosed = ui->rbtn_auto2G->isChecked();
    bool bManual2GChoosed = ui->rbtn_manual2G->isChecked();
    bool bAuto5GChoosed = ui->rbtn_auto5G->isChecked();
    bool bManual5GChoosed = ui->rbtn_manual5G->isChecked();
    if (g_extenderType == "1000RP"
            && (bAuto2GChoosed || bManual2GChoosed)) {
        bHasWifiChoosed = true;
    }
    if (g_extenderType == "2500RP"
            && (bAuto2GChoosed || bManual2GChoosed || bAuto5GChoosed || bManual5GChoosed)) {
        bHasWifiChoosed = true;
    }

    flagWifiAutoSel2G = flagWifiManualSel2G = flagWifiAutoSel5G = flagWifiManualSel5G = false;
    if (bAuto2GChoosed)
        flagWifiAutoSel2G = true;
    if (bManual2GChoosed)
        flagWifiManualSel2G = true;
    if (bAuto5GChoosed)
        flagWifiAutoSel5G = true;
    if (bManual5GChoosed)
        flagWifiManualSel5G = true;

    m_SSID2G = m_SSID5G = m_securitytype2G = m_securitytype5G = "";   //First clear, then assignment
    int currentRow2G = ui->tblwdt_wifiList2G->currentRow();
    int currentRow5G = ui->tblwdt_wifiList5G->currentRow();
    if (bAuto2GChoosed) {
        m_SSID2G = getSSIDFromWifiList("2G", currentRow2G);
        m_securitytype2G = getSecuritytypeFromWifiList("2G", currentRow2G);
    }
    if (bAuto5GChoosed) {
        m_SSID5G = getSSIDFromWifiList("5G", currentRow5G);
        m_securitytype5G = getSecuritytypeFromWifiList("5G", currentRow5G);
    }

    // print the information for choosed
    qDebug() << "Choosed SSID and securitytype: " << m_SSID2G << "," <<  m_securitytype2G
             << "," << m_SSID5G << "," << m_securitytype5G;

    if (bHasWifiChoosed) {
        //hide ui
        ui->frm_autoSet2G1->hide();
        ui->frm_autoSet2G2->hide();
        ui->frm_autoSet5G1->hide();
        ui->frm_autoSet5G2->hide();
        ui->frm_manualSet2G1->hide();
        ui->frm_manualSet2G21->hide();
        ui->frm_manualSet2G22->hide();
        ui->frm_manualSet2G23->hide();
        ui->frm_manualSet5G1->hide();
        ui->frm_manualSet5G21->hide();
        ui->frm_manualSet5G22->hide();
        ui->frm_manualSet5G23->hide();
        ui->lbl_wlanPasswordAutoSet2G->show();
        ui->ledt_wlanPasswordAutoSet2G->show();
        ui->lbl_wlanPasswordAutoSet5G->show();
        ui->ledt_wlanPasswordAutoSet5G->show();

        //transfer value
        ui->lbl_SSIDNameAuto2G->setText(m_SSID2G);
        ui->lbl_securityTypeAuto2G->setText(m_securitytype2G);
        ui->lbl_SSIDNameAuto5G->setText(m_SSID5G);
        ui->lbl_securityTypeAuto5G->setText(m_securitytype5G);

        flagWifiOFFSel2G = false;
        flagWifiWEPSel2G = false;
        flagWifiWPAPSKSel2G = false;
        flagWifiOFFSel5G = false;
        flagWifiWEPSel5G = false;
        flagWifiWPAPSKSel5G = false;

        connect(this, SIGNAL(setRouterWLANFinished()), this, SLOT(slt_setRouterWLANFinished()), Qt::UniqueConnection);

        //control btn
        if ((!flagWifiManualSel2G && !flagWifiManualSel5G)
                &&
                ((m_securitytype2G == "OFF" && m_securitytype5G == "OFF")
                 || (m_securitytype2G == "OFF" && !flagWifiAutoSel5G)
                 || (m_securitytype5G == "OFF" && !flagWifiAutoSel2G)))
        {
            setWaittingUi(ui->vl_wait3);
            ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wait3));
            setRouterWLAN();  //setRouterWLAN prior
        }else {
            ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_extenderToRouter));
        }

        //show Ui
        if (flagWifiAutoSel2G)
        {
            if (m_securitytype2G == "OFF")
            {
                flagWifiOFFSel2G = true;
            }
            else if (m_securitytype2G == "WEP")
            {
                ui->frm_autoSet2G1->show();
                ui->frm_autoSet2G2->show();
                ui->lbl_wlanPasswordAutoSet2G->hide();
                ui->ledt_wlanPasswordAutoSet2G->hide();
                flagWifiWEPSel2G = true;
            }
            else
            {
                ui->frm_autoSet2G1->show();
                flagWifiWPAPSKSel2G = true;
            }

        }
        if (flagWifiAutoSel5G)
        {
            if (m_securitytype5G == "OFF")
            {
                flagWifiOFFSel5G = true;
            }
            else if (m_securitytype5G == "WEP")
            {
                ui->frm_autoSet5G1->show();
                ui->frm_autoSet5G2->show();
                ui->lbl_wlanPasswordAutoSet5G->hide();
                ui->ledt_wlanPasswordAutoSet5G->hide();
                flagWifiWEPSel5G = true;
            }
            else
            {
                ui->frm_autoSet5G1->show();
                flagWifiWPAPSKSel5G = true;
            }

        }
        if (flagWifiManualSel2G)
        {
            if (bBackBtnClicked) {
                int tmp = ui->cbx_securityTypeChooseManual2G->currentIndex();
                on_cbx_securityTypeChooseManual2G_currentIndexChanged(tmp);
            }
            ui->frm_manualSet2G1->show();
        }
        if (flagWifiManualSel5G)
        {
            if (bBackBtnClicked) {
                int tmp = ui->cbx_securityTypeChooseManual5G->currentIndex();
                on_cbx_securityTypeChooseManual5G_currentIndexChanged(tmp);
            }
            ui->frm_manualSet5G1->show();
        }
    }
    else {
        QString strTemp = translateText(L_MW_MBX_CHOOSE_WAY_EXTENDE, "mainwindowlanguage");

        QMessageBox::warning(this, tr("Extender Genie"), strTemp, QMessageBox::Ok);
    }
}

void ExtenderMainWindow::on_btn_backExtenderToRouter_clicked()
{
    // record the Ui of pg_extenderToRouter.
    flagWifiAutoSel2GRecord = flagWifiAutoSel2G;
    flagWifiAutoSel5GRecord = flagWifiAutoSel5G;
    flagWifiManualSel2GRecord = flagWifiManualSel2G;
    flagWifiManualSel5GRecord = flagWifiManualSel5G;

    bBackBtnClicked = true;

    ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wifiList));
}

void ExtenderMainWindow::UiRecordAndShow()
{
    // show the Ui by compare the previous flags and now flags
    QList<QWidget *> allWidgetsAutoSet2G = ui->frm_autoSet2G->findChildren<QWidget *>();
    QList<QWidget *> allWidgetsAutoSet5G = ui->frm_autoSet5G->findChildren<QWidget *>();
    QList<QWidget *> allWidgetsManualSet2G = ui->frm_manualSet2G->findChildren<QWidget *>();
    QList<QWidget *> allWidgetsManualSet5G = ui->frm_manualSet5G->findChildren<QWidget *>();

    if (flagWifiAutoSel2GRecord == flagWifiAutoSel2G) {
        for (int i = 0; i < allWidgetsAutoSet2G.count(); i++) {
            if (allWidgetsAutoSet2G[i]->isVisible())
                allWidgetsAutoSet2G[i]->show();
        }
    }
    if (flagWifiManualSel2GRecord == flagWifiManualSel2G) {
        for (int i = 0; i < allWidgetsManualSet2G.count(); i++) {
            if (allWidgetsManualSet2G[i]->isVisible())
                allWidgetsManualSet2G[i]->show();
        }
    }
    if (flagWifiAutoSel5GRecord == flagWifiAutoSel5G) {
        for (int i = 0; i < allWidgetsAutoSet5G.count(); i++) {
            if (allWidgetsAutoSet5G[i]->isVisible())
                allWidgetsAutoSet5G[i]->show();
        }
    }
    if (flagWifiManualSel5GRecord == flagWifiManualSel5G) {
        for (int i = 0; i < allWidgetsManualSet5G.count(); i++) {
            if (allWidgetsManualSet5G[i]->isVisible())
                allWidgetsManualSet5G[i]->show();
        }
    }

    ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_extenderToRouter));
}

void ExtenderMainWindow::on_btn_extenderToRouterContinue_clicked()
{
    qDebug() << "==ExtenderToRouterContinue button clicked";
    if (verifyExtenderToRouterPageOK()) {
        setWaittingUi(ui->vl_wait3);
        ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wait3));

        setRouterWLAN();
    }
}

bool ExtenderMainWindow::verifyExtenderToRouterPageOK()
{
    bool bVisblePasswordAutoSet2G = ui->ledt_wlanPasswordAutoSet2G->isVisible();
    bool bVisbleKey1AutoSet2G = ui->ledt_key1AutoSet2G->isVisible();
    bool bVisbleSSIDNameManual2G = ui->ledt_SSIDNameManual2G->isVisible();
    bool bVisbleSecurityTypeChooseManual2G = ui->cbx_securityTypeChooseManual2G->isVisible();
    bool bVisbleKey1ManualSet2G = ui->ledt_key1ManualSet2G->isVisible();
    bool bVisblePasswordManualSet2G = ui->ledt_wlanPasswordManualSet2G->isVisible();

    //bool bEmptyPasswordAutoSet2G = ui->ledt_wlanPasswordAutoSet2G->text().isEmpty();
    bool bEmptyKey1AutoSet2G = ui->ledt_key1AutoSet2G->text().isEmpty();
    bool bEmptySSIDNameManual2G = ui->ledt_SSIDNameManual2G->text().isEmpty();
    bool bEmptyKey1ManualSet2G = ui->ledt_key1ManualSet2G->text().isEmpty();
    //bool bEmptyPasswordManualSet2G = ui->ledt_wlanPasswordManualSet2G->text().isEmpty();
    int indexSecurityTypeChooseManual2G = ui->cbx_securityTypeChooseManual2G->currentIndex();


    bool bVisblePasswordAutoSet5G = ui->ledt_wlanPasswordAutoSet5G->isVisible();
    bool bVisbleKey1AutoSet5G = ui->ledt_key1AutoSet5G->isVisible();
    bool bVisbleSSIDNameManual5G = ui->ledt_SSIDNameManual5G->isVisible();
    bool bVisbleSecurityTypeChooseManual5G = ui->cbx_securityTypeChooseManual5G->isVisible();
    bool bVisbleKey1ManualSet5G = ui->ledt_key1ManualSet5G->isVisible();
    bool bVisblePasswordManualSet5G = ui->ledt_wlanPasswordManualSet5G->isVisible();

    //bool bEmptyPasswordAutoSet5G = ui->ledt_wlanPasswordAutoSet5G->text().isEmpty();
    bool bEmptyKey1AutoSet5G = ui->ledt_key1AutoSet5G->text().isEmpty();
    bool bEmptySSIDNameManual5G = ui->ledt_SSIDNameManual5G->text().isEmpty();
    bool bEmptyKey1ManualSet5G = ui->ledt_key1ManualSet5G->text().isEmpty();
    //bool bEmptyPasswordManualSet5G = ui->ledt_wlanPasswordManualSet5G->text().isEmpty();
    int indexSecurityTypeChooseManual5G = ui->cbx_securityTypeChooseManual5G->currentIndex();

    QString strNoSSID = translateText(L_MSGBOX_EXTENDERNAME2, "mainwindowlanguage");
    QString strWEPKey = translateText(L_MSGBOX_WEP2, "mainwindowlanguage");
    QString strNoWEPKey = translateText(L_MSGBOX_NOWEP2, "mainwindowlanguage");
    QString strNoSeucrityChoose = translateText(L_MSGBOX_NOCHOOSE, "mainwindowlanguage");

    if (bVisblePasswordAutoSet2G && /*bEmptyPasswordAutoSet2G */ !validPassord(ui->ledt_wlanPasswordAutoSet2G->text())) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordAutoSet2G->setFocus();
        return false;
    }
    else if (bVisbleKey1AutoSet2G && bEmptyKey1AutoSet2G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordWEPAutoSet2G->setFocus();
        return false;
    }
    else if (bVisbleSSIDNameManual2G && bEmptySSIDNameManual2G) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
        ui->ledt_SSIDNameManual2G->setFocus();
        return false;
    }
    else if (bVisbleSecurityTypeChooseManual2G && indexSecurityTypeChooseManual2G == 0) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSeucrityChoose, QMessageBox::Ok);
        return false;
    }
    else if (bVisbleKey1ManualSet2G && bEmptyKey1ManualSet2G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordWEPManualSet2G->setFocus();
        return false;
    }
    else if (bVisblePasswordManualSet2G && /*bEmptyPasswordManualSet2G*/ !validPassord(ui->ledt_wlanPasswordManualSet2G->text())) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordManualSet2G->setFocus();
        return false;
    }
    // 5G
    else if (bVisblePasswordAutoSet5G && /*bEmptyPasswordAutoSet5G */ !validPassord(ui->ledt_wlanPasswordAutoSet5G->text())) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordAutoSet5G->setFocus();
        return false;
    }
    else if (bVisbleKey1AutoSet5G && bEmptyKey1AutoSet5G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordWEPAutoSet5G->setFocus();
        return false;
    }
    else if (bVisbleSSIDNameManual5G && bEmptySSIDNameManual5G) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
        ui->ledt_SSIDNameManual5G->setFocus();
        return false;
    }
    else if (bVisbleSecurityTypeChooseManual5G && indexSecurityTypeChooseManual5G == 0) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSeucrityChoose, QMessageBox::Ok);
        return false;
    }
    else if (bVisbleKey1ManualSet5G && bEmptyKey1ManualSet5G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordWEPManualSet5G->setFocus();
        return false;
    }
    else if (bVisblePasswordManualSet5G && /*bEmptyPasswordManualSet5G*/ !validPassord(ui->ledt_wlanPasswordManualSet5G->text())) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_wlanPasswordManualSet5G->setFocus();
        return false;
    }
    else {
        return true;
    }
}

// verity the input, and show messae.
bool ExtenderMainWindow::verifyExtenderSetPageOK()
{
    bool bVisibleNtName2G = ui->ledt_extenderNtName2G->isVisible();
    bool bVisibleNtName5G = ui->ledt_extenderNtName5G->isVisible();
    bool bEmpltyNtName2G = ui->ledt_extenderNtName2G->text().isEmpty();
    bool bEmpltyNtName5G = ui->ledt_extenderNtName5G->text().isEmpty();
    bool bVisblePhraseWEP2G = ui->ledt_passPhraseWEP2G->isVisible();
    bool bVisblePhraseWEP5G = ui->ledt_passPhraseWEP5G->isVisible();
    bool bEmptyKey12G = ui->ledt_key12G->text().isEmpty();
    bool bEmptyKey15G = ui->ledt_key15G->text().isEmpty();
    bool bVisblePhraseNoWEP2G = ui->ledt_passPhraseNoWEP2G->isVisible();
    bool bVisblePhraseNoWEP5G = ui->ledt_passPhraseNoWEP5G->isVisible();
    bool bEmptyPassPhraseNoWEP2G = ui->ledt_passPhraseNoWEP2G->text().isEmpty();
    bool bEmptyPassPhraseNoWEP5G = ui->ledt_passPhraseNoWEP5G->text().isEmpty();

    QString strNoSSID = translateText(L_MSGBOX_EXTENDERNAME,"mainwindowlanguage");
    QString strWEPKey = translateText(L_MSGBOX_WEP, "mainwindowlanguage");
    QString strNoWEPKey = translateText(L_MSGBOX_NOWEP, "mainwindowlanguage");

    qDebug() << ui->ledt_extenderNtName2G->text();

    if (bVisibleNtName2G &&
            (bEmpltyNtName2G || !validSSID(ui->ledt_extenderNtName2G->text()))) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
        ui->ledt_extenderNtName2G->setFocus();
        qDebug() << "Please input necessary Ntname";
        return false;
    }
    else if (bVisibleNtName5G &&
             (bEmpltyNtName5G || !validSSID(ui->ledt_extenderNtName5G->text()))) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
        ui->ledt_extenderNtName5G->setFocus();
        qDebug() << "Please input necessary Ntname";
        return false;
    }
    else if (bVisblePhraseWEP2G && bEmptyKey12G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_passPhraseWEP2G->setFocus();
        qDebug() << "ledt_key12G is empty";
        return false;
    }
    else if (bVisblePhraseWEP5G && bEmptyKey15G) {
        QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
        ui->ledt_passPhraseWEP5G->setFocus();
        qDebug() << "ledt_key15G is empty";
        return false;
    }
    else if (bVisblePhraseNoWEP2G &&
             (bEmptyPassPhraseNoWEP2G || !validPassord(ui->ledt_passPhraseNoWEP2G->text()))) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_passPhraseNoWEP2G->setFocus();
        qDebug() << "ledt_passPhraseNoWEP2G is empty";
        return false;
    }
    else if (bVisblePhraseNoWEP5G &&
             (bEmptyPassPhraseNoWEP5G || !validPassord(ui->ledt_passPhraseNoWEP5G->text()))) {
        QMessageBox::warning(this, tr("Extender Genie"), strNoWEPKey, QMessageBox::Ok);
        ui->ledt_passPhraseNoWEP5G->setFocus();
        qDebug() << "ledt_passPhraseNoWEP5G is empty";
        return false;
    }
    else {
        return true;
    }
}

bool ExtenderMainWindow::validSSID(const QString &SSID)
{
    if (SSID.length() > 32) {
        return false;
    }
    else {
        return true;
    }
}

bool ExtenderMainWindow::validPassord(const QString &key)
{
    if (key.length() < 8 || key.length() > 64) {
        return false;
    }
    else {
        return true;
    }
}

void ExtenderMainWindow::on_btn_extenderSet_clicked()
{
    if (verifyExtenderSetPageOK()) {
        m_extenderSSID2G = ui->ledt_extenderNtName2G->text();
        m_extenderSSID5G = ui->ledt_extenderNtName5G->text();

        setStepUi(3);

        setWaittingUi(ui->vl_wait4);
        ui->stawdt_firstLayer->setCurrentIndex(ui->stawdt_firstLayer->indexOf(ui->pg_wait4));

        if (ui->chk_usingCurrentSetting->isChecked()) {
            //change value
            m_extenderSecuritytype2G = m_securitytype2G;
            m_extenderSecuritytype5G = m_securitytype5G;
            m_extenderPassword2G =  m_password2G;
            m_extenderPassword5G = m_password5G;

            m_extenderPasswordWEP2G = m_passwordWEP2G;
            m_extenderPasswordWEP5G = m_passwordWEP2G;
            m_extenderNewWEPLength2G = m_newWEPLength2G;
            m_extenderNewWEPLength5G = m_newWEPLength5G;
            m_extenderNewKeyIndex2G = m_newKeyIndex2G;
            m_extenderNewKeyIndex5G = m_newKeyIndex5G;
            m_extenderWEPKey12G = m_WEPKey12G;
            m_extenderWEPKey22G = m_WEPKey22G;
            m_extenderWEPKey32G = m_WEPKey32G;
            m_extenderWEPKey42G = m_WEPKey42G;
            m_extenderWEPKey15G = m_WEPKey15G;
            m_extenderWEPKey25G = m_WEPKey25G;
            m_extenderWEPKey35G = m_WEPKey35G;
            m_extenderWEPKey45G = m_WEPKey45G;
        } else {
            //change value
            m_extenderPassword2G = ui->ledt_passPhraseNoWEP2G->text();
            m_extenderPassword5G = ui->ledt_passPhraseNoWEP5G->text();
            m_extenderPasswordWEP2G = ui->ledt_passPhraseWEP2G->text();
            m_extenderPasswordWEP5G = ui->ledt_passPhraseWEP5G->text();

            m_extenderWEPKey12G = ui->ledt_key12G->text();
            m_extenderWEPKey22G = ui->ledt_key22G->text();
            m_extenderWEPKey32G = ui->ledt_key32G->text();
            m_extenderWEPKey42G = ui->ledt_key42G->text();
            m_extenderWEPKey15G = ui->ledt_key15G->text();
            m_extenderWEPKey25G = ui->ledt_key25G->text();
            m_extenderWEPKey35G = ui->ledt_key35G->text();
            m_extenderWEPKey45G = ui->ledt_key45G->text();

            m_extenderNewWEPLength2G = m_newWEPLength2G;
            m_extenderNewWEPLength5G = m_newWEPLength5G;
            m_extenderNewKeyIndex2G = m_newKeyIndex2G;
            m_extenderNewKeyIndex5G = m_newKeyIndex5G;
        }

        connect(this, SIGNAL(setExtenderWLANFinished()), this, SLOT(slt_setExtenderWLANFinished()), Qt::UniqueConnection);

        if (g_extenderType == "1000RP") {
            flagExtenderWLANSet++;
            flagExtenderWLANSetRight++;
            //flagExtenderWLANSetError++;
            if (m_extenderSecuritytype2G == "OFF") {
                soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager2G, g_port_extender, "2.4G", m_extenderSSID2G,
                                           "Auto", "Auto");
                connect(&SetWLANNoSecurityAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
            } else if (m_extenderSecuritytype2G == "WEP") {
                soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager2G, g_port_extender, "2.4G", m_extenderSSID2G,
                                          "Auto", "Auto", "Auto", m_extenderNewWEPLength2G, m_extenderNewKeyIndex2G, m_extenderWEPKey12G, m_extenderWEPKey22G, m_extenderWEPKey32G, m_extenderWEPKey42G);
                connect(&SetWLANWEPByKeysAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
            } else {
                soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager2G, g_port_extender, "2.4G",
                                                   m_extenderSSID2G, "Auto", "Auto", m_extenderSecuritytype2G, m_extenderPassword2G);
                connect(&SetWLANWPAPSKByPassphraseAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
            }
        }

        if (g_extenderType == "2500RP") {
            flagExtenderWLANSet++;
            flagExtenderWLANSetRight++;
            if (m_extenderSecuritytype2G == "OFF") {
                soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager2G, g_port_extender, "2.4G", m_extenderSSID2G,
                                           "Auto", "Auto");
                connect(&SetWLANNoSecurityAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
            } else if (m_extenderSecuritytype2G == "WEP") {
                soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager2G, g_port_extender, "2.4G", m_extenderSSID2G,
                                          "Auto", "Auto", "Auto", m_extenderNewWEPLength2G, m_extenderNewKeyIndex2G, m_extenderWEPKey12G, m_extenderWEPKey22G, m_extenderWEPKey32G, m_extenderWEPKey42G);
                connect(&SetWLANWEPByKeysAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
            } else {
                soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager2G, g_port_extender, "2.4G",
                                                   m_extenderSSID2G, "Auto", "Auto", m_extenderSecuritytype2G, m_extenderPassword2G);
                connect(&SetWLANWPAPSKByPassphraseAccessManager2G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
            }

            if (m_extenderSecuritytype5G == "OFF") {
                soapapi->SetWLANNoSecurity(&SetWLANNoSecurityAccessManager5G, g_port_extender, "5G", m_extenderSSID5G,
                                           "Auto", "Auto");
                connect(&SetWLANNoSecurityAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANNoSecurity(QNetworkReply*)), Qt::UniqueConnection);
            } else if (m_extenderSecuritytype5G == "WEP") {
                soapapi->SetWLANWEPByKeys(&SetWLANWEPByKeysAccessManager5G, g_port_extender, "5G", m_extenderSSID5G,
                                          "Auto", "Auto", "Auto", m_extenderNewWEPLength5G, m_extenderNewKeyIndex5G, m_extenderWEPKey15G, m_extenderWEPKey25G, m_extenderWEPKey35G, m_extenderWEPKey45G);
                connect(&SetWLANWEPByKeysAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWEPByKeys(QNetworkReply*)), Qt::UniqueConnection);
            } else {
                soapapi->SetWLANWPAPSKByPassphrase(&SetWLANWPAPSKByPassphraseAccessManager5G, g_port_extender, "5G",
                                                   m_extenderSSID5G, "Auto", "Auto",  m_extenderSecuritytype5G, m_extenderPassword5G);
                connect(&SetWLANWPAPSKByPassphraseAccessManager5G, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(slt_setWLANWPAPSKByPassphrase(QNetworkReply*)), Qt::UniqueConnection);
            }
        }
        //break;
    }
}

void ExtenderMainWindow::on_btn_extendFinish_clicked()
{
    if (flagWizard)
    {
        setWaittingUi(ui->vl_wait5);

        if (ui->chk_finish->isChecked()) {

            ui->lbl_wait5->setText(translateText(L_LBL_AUTOCONNECTWIFI));
            ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_connectToWifi));

            if (m_extenderSecuritytype2G == "WEP") {
                switch (m_extenderNewKeyIndex2G) {
                case 1:
                    m_extenderPassword2G = m_extenderWEPKey12G;
                    break;
                case 2:
                    m_extenderPassword2G = m_extenderWEPKey22G;
                    break;
                case 3:
                    m_extenderPassword2G = m_extenderWEPKey32G;
                    break;
                case 4:
                    m_extenderPassword2G = m_extenderWEPKey42G;
                    break;
                default:
                    break;
                }
            }
            if (m_extenderSecuritytype5G == "WEP") {
                switch (m_extenderNewKeyIndex5G) {
                case 1:
                    m_extenderPassword5G = m_extenderWEPKey15G;
                    break;
                case 2:
                    m_extenderPassword5G = m_extenderWEPKey25G;
                    break;
                case 3:
                    m_extenderPassword5G = m_extenderWEPKey35G;
                    break;
                case 4:
                    m_extenderPassword5G = m_extenderWEPKey45G;
                    break;
                default:
                    break;
                }
            }

            ExtenderUtilWiFi wifiUtil;
            //TODO: multi thread
            if (ui->rbtn_extenderRadioChoose2G->isChecked())
            {
                qDebug() << m_extenderSSID2G << "," << m_extenderPassword2G;
                SSID = m_extenderSSID2G;
                password = m_extenderPassword2G;
                wifiUtil.connectToWlan(SSID, password);
            }
            else if (ui->rbtn_extenderRadioChoose5G->isChecked())
            {
                qDebug() << m_extenderSSID5G << "," << m_extenderPassword5G;
                SSID = m_extenderSSID5G;
                password = m_extenderPassword5G;
                wifiUtil.connectToWlan(SSID, password);
            }

            for (int i = 5; i > 0; i--) {
                ui->lbl_countdown->setText(QString::number(i));

                ExtenderUtil::setExtenderSleep(1000);
            }
            this->close();
        }
        else
        {
            this->close();
        }
    }
    if (flagAdvanced)
    {
        this->close();
    }
}

void ExtenderMainWindow::on_btn_close_clicked()
{
    emit emitExit();
}

void ExtenderMainWindow::on_btn_minimize_clicked()
{
    this->showMinimized();
}

void ExtenderMainWindow::on_btn_about_clicked()
{
    //    qDebug() << "About cliked......";
    //    aboutDialog = new AboutDialog;
    //    int mwx = this->pos().x();
    //    int mwy = this->pos().y();
    //    aboutDialog->move(mwx + 370, mwy + 5);
    //    aboutDialog->exec();
}

void ExtenderMainWindow::on_head_clicked()
{
    //QDesktopServices::openUrl(QUrl("http://www.netgear.com/landing/genie/default.aspx"));
}
/*
 *=====Main buttons end
 */

void ExtenderMainWindow::paintEvent(QPaintEvent *)
{
    // draw main border
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor("#4d4d4d"));
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRect(0, 0, this->width(), this->height());
}

void ExtenderMainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        moving = true;
        last = e->globalPos();
        e->accept();
    }
}

void ExtenderMainWindow::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    if ((e->buttons() & Qt::LeftButton) && moving) {
        move(x()+dx, y()+dy);
        e->accept();
    }
}

void ExtenderMainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        moving = false;
}

//bool GenieMainWindow::winEvent(MSG *message, long *result)
//{

//    switch(message->message)
//    {
//    case WM_NCHITTEST:
//        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();
//        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();
//        if(this->childAt(xPos,yPos) == 0)
//        {
//            *result = HTCAPTION;
//        }else{
//            return false;
//        }
//        if(xPos > 18 && xPos < 22)
//            *result = HTLEFT;
//        if(xPos > (this->width() - 22) && xPos < (this->width() - 18))
//            *result = HTRIGHT;
//        if(yPos > 18 && yPos < 22)
//            *result = HTTOP;
//        if(yPos > (this->height() - 22) && yPos < (this->height() - 18))
//            *result = HTBOTTOM;
//        if(xPos > 18 && xPos < 22 && yPos > 18 && yPos < 22)
//            *result = HTTOPLEFT;
//        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > 18 && yPos < 22)
//            *result = HTTOPRIGHT;
//        if(xPos > 18 && xPos < 22 && yPos > (this->height() - 22) && yPos < (this->height() - 18))
//            *result = HTBOTTOMLEFT;
//        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > (this->height() - 22) && yPos < (this->height() - 18))
//            *result = HTBOTTOMRIGHT;

//        return true;

//    }

//    return false;
//}


/*
 *=====Control Ui
 */

void ExtenderMainWindow::on_rbtn_auto2G_clicked(bool checked)
{
    if (checked) {
        ui->rbtn_manual2G->setChecked(false);
        flagWifiAutoSel2G = true;
        int currentRow2G = ui->tblwdt_wifiList2G->currentRow();
        if (currentRow2G == -1) {
            ui->tblwdt_wifiList2G->selectRow(0);
        }
    }
}

void ExtenderMainWindow::on_rbtn_manual2G_clicked(bool checked)
{
    if (checked) {
        ui->rbtn_auto2G->setChecked(false);
        ui->btn_extenderToRouter->setEnabled(true);
        flagWifiManualSel2G = true;
    }
}

void ExtenderMainWindow::on_rbtn_auto5G_clicked(bool checked)
{
    if (checked) {
        ui->rbtn_manual5G->setChecked(false);
        flagWifiAutoSel5G = true;
        int currentRow5G = ui->tblwdt_wifiList5G->currentRow();
        if (currentRow5G == -1) {
            ui->tblwdt_wifiList5G->selectRow(0);
        }
    }
}

void ExtenderMainWindow::on_rbtn_manual5G_clicked(bool checked)
{
    if (checked) {
        ui->rbtn_auto5G->setChecked(false);
        ui->btn_extenderToRouter->setEnabled(true);
        flagWifiManualSel5G = true;
    }
}

void ExtenderMainWindow::on_tblwdt_wifiList2G_itemSelectionChanged()
{
    if (!m_bWifiListRefreshing) {
        ui->btn_extenderToRouter->setEnabled(true);
        if (!flagWifiAutoSel2G && !flagWifiManualSel2G)
            ui->rbtn_auto2G->setChecked(true);
    }
}

void ExtenderMainWindow::on_tblwdt_wifiList5G_itemSelectionChanged()
{
    if (!m_bWifiListRefreshing) {
        ui->btn_extenderToRouter->setEnabled(true);
        if (!flagWifiAutoSel5G && !flagWifiManualSel5G)
            ui->rbtn_auto5G->setChecked(true);
    }
}

void ExtenderMainWindow::on_rbtn_none2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->hide();
        m_extenderSecuritytype2G = "OFF";
    }
}

void ExtenderMainWindow::on_rbtn_WEP2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->show();
        ui->frm_extenderSet2G_3->hide();
        m_extenderSecuritytype2G = "WEP";
    }
}

void ExtenderMainWindow::on_rbtn_WPAPSKTKIP2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->show();
        m_extenderSecuritytype2G = "WPA-PSK-TKIP";
        ui->lbl_securityNameShow2G->setText("WPA-PSK-TKIP");
    }
}

void ExtenderMainWindow::on_rbtn_WPAPSKAES2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->show();
        m_extenderSecuritytype2G = "WPA-PSK-AES";
        ui->lbl_securityNameShow2G->setText("WPA-PSK-AES");
    }
}

void ExtenderMainWindow::on_rbtn_WPA2PSKTKIP2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->show();
        m_extenderSecuritytype2G = "WPA2-PSK-TKIP";
        ui->lbl_securityNameShow2G->setText("WPA2-PSK-TKIP");
    }
}

void ExtenderMainWindow::on_rbtn_WPA2PSKAES2G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->show();
        m_extenderSecuritytype2G = "WPA2-PSK-AES";
        ui->lbl_securityNameShow2G->setText("WPA2-PSK-AES");
    }
}

void ExtenderMainWindow::on_rtbn_WPAandWAP22G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet2G_2->hide();
        ui->frm_extenderSet2G_3->show();
        m_extenderSecuritytype2G = "WPA-PSK-TKIP|WPA2-PSK-AES";
        ui->lbl_securityNameShow2G->setText("WPA-PSK-TKIP|WPA2-PSK-AES");
    }
}

void ExtenderMainWindow::on_rbtn_none5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->hide();
        m_extenderSecuritytype5G = "OFF";
    }
}

void ExtenderMainWindow::on_rbtn_WEP5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->show();
        ui->frm_extenderSet5G_3->hide();
        m_extenderSecuritytype5G = "WEP";
    }
}

void ExtenderMainWindow::on_rbtn_WPAPSKTKIP5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->show();
        m_extenderSecuritytype5G = "WPA-PSK-TKIP";
        ui->lbl_securityNameShow5G->setText("WPA-PSK-TKIP");
    }
}

void ExtenderMainWindow::on_rbtn_WPAPSKAES5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->show();
        m_extenderSecuritytype5G = "WPA-PSK-AES";
        ui->lbl_securityNameShow5G->setText("WPA-PSK-AES");
    }
}

void ExtenderMainWindow::on_rbtn_WPA2PSKAES5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->show();
        m_extenderSecuritytype5G = "WPA2-PSK-AES";
        ui->lbl_securityNameShow5G->setText("WPA2-PSK-AES");
    }
}

void ExtenderMainWindow::on_rbtn_WPA2PSKTKIP5G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->show();
        m_extenderSecuritytype5G = "WPA2-PSK-TKIP";
        ui->lbl_securityNameShow5G->setText("WPA2-PSK-TKIP");
    }
}

void ExtenderMainWindow::on_rtbn_WPAandWPA25G_clicked(bool checked)
{
    if (checked) {
        ui->frm_extenderSet5G_2->hide();
        ui->frm_extenderSet5G_3->show();
        m_extenderSecuritytype5G = "WPA-PSK-TKIP|WPA2-PSK-AES";
        ui->lbl_securityNameShow5G->setText("WPA-PSK-TKIP|WPA2-PSK-AES");
    }
}

void ExtenderMainWindow::on_cmb_bit2G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_extenderNewWEPLength2G = 64;
    }
    if (index == 1) {
        m_extenderNewWEPLength2G = 128;
    }
}

void ExtenderMainWindow::on_cmb_bit5G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_extenderNewWEPLength5G = 64;
    }
    if (index ==1) {
        m_extenderNewWEPLength5G = 128;
    }
}

void ExtenderMainWindow::on_btn_pwdGenerator2G_clicked()
{
    if(ui->ledt_passPhraseWEP2G->text() == "")
    {

    }
    else
    {
        int keyLen = m_extenderNewWEPLength2G == 64 ? 10 : 26;
        ui->ledt_key12G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP2G->text(), keyLen, 3));
        ui->ledt_key22G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP2G->text(), keyLen, 5));
        ui->ledt_key32G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP2G->text(), keyLen, 7));
        ui->ledt_key42G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP2G->text(), keyLen, 9));
    }
}

void ExtenderMainWindow::on_btn_pwdGenerator5G_clicked()
{
    if(ui->ledt_passPhraseWEP5G->text() == "")
    {

    }
    else
    {
        int keyLen = m_extenderNewWEPLength5G == 64 ? 10 : 26;
        ui->ledt_key15G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP5G->text(), keyLen, 3));
        ui->ledt_key25G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP5G->text(), keyLen, 5));
        ui->ledt_key35G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP5G->text(), keyLen, 7));
        ui->ledt_key45G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_passPhraseWEP5G->text(), keyLen, 9));
    }
}

void ExtenderMainWindow::on_chk_usingCurrentSetting_stateChanged(int arg1)
{
    //unchecked
    if (arg1 == 0) {
        if (g_extenderType == "1000RP") {
            ui->frm_extenderSet2G->show();
            ui->frm_extenderSet5G->hide();
        }
        if (g_extenderType == "2500RP") {
            ui->frm_extenderSet2G->show();
            ui->frm_extenderSet5G->show();
        }
    }
    //checked
    if (arg1 == 2) {
        ui->frm_extenderSet2G->hide();
        ui->frm_extenderSet5G->hide();
    }
}

void ExtenderMainWindow::on_cmb_bitAutoSet2G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_newWEPLength2G = 64;
    }
    if (index ==1) {
        m_newWEPLength2G = 128;
    }
}

void ExtenderMainWindow::on_btn_pwdGeneratorAuto2G_clicked()
{
    if (ui->ledt_wlanPasswordWEPAutoSet2G->text().length() == 0)  //
    {

    }
    else
    {
        int keyLen = m_newWEPLength2G == 64 ? 10 : 26;
        ui->ledt_key1AutoSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPAutoSet2G->text(), keyLen, 3));
        ui->ledt_key2AutoSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPAutoSet2G->text(), keyLen, 5));
        ui->ledt_key3AutoSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPAutoSet2G->text(), keyLen, 7));
        ui->ledt_key4AutoSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPAutoSet2G->text(), keyLen, 9));
    }
    //transfer value
    m_WEPKey12G = ui->ledt_key1AutoSet2G->text();
    m_WEPKey22G = ui->ledt_key2AutoSet2G->text();
    m_WEPKey32G = ui->ledt_key3AutoSet2G->text();
    m_WEPKey32G = ui->ledt_key4AutoSet2G->text();
}

void ExtenderMainWindow::on_rbtn_key1AutoSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 1;
    }
}

void ExtenderMainWindow::on_rbtn_key2AutoSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 2;
    }
}

void ExtenderMainWindow::on_rbtn_key3AutoSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 3;
    }
}

void ExtenderMainWindow::on_rbtn_key4AutoSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 4;
    }
}

void ExtenderMainWindow::on_cmb_bitAutoSet5G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_newWEPLength5G = 64;
    }
    if (index ==1) {
        m_newWEPLength5G = 128;
    }
}

void ExtenderMainWindow::on_btn_pwdGeneratorAuto5G_clicked()
{
    if (ui->ledt_wlanPasswordWEPAutoSet5G->text().length() == 0)  //
    {

    }
    else
    {
        if(ui->ledt_wlanPasswordWEPAutoSet5G->text().length() > 0)
        {
            int keyLen = m_newWEPLength5G == 64 ? 10 : 26;
            ui->ledt_key1AutoSet5G->setText(
                        ExtenderUtil::wepKeyGenerater(
                            ui->ledt_wlanPasswordWEPAutoSet5G->text(), keyLen, 3));
            ui->ledt_key2AutoSet5G->setText(
                        ExtenderUtil::wepKeyGenerater(
                            ui->ledt_wlanPasswordWEPAutoSet5G->text(), keyLen, 5));
            ui->ledt_key3AutoSet5G->setText(
                        ExtenderUtil::wepKeyGenerater(
                            ui->ledt_wlanPasswordWEPAutoSet5G->text(), keyLen, 7));
            ui->ledt_key4AutoSet5G->setText(
                        ExtenderUtil::wepKeyGenerater(
                            ui->ledt_wlanPasswordWEPAutoSet5G->text(), keyLen, 9));
        }
        //transfer value
        m_WEPKey15G = ui->ledt_key1AutoSet5G->text();
        m_WEPKey25G = ui->ledt_key2AutoSet5G->text();
        m_WEPKey35G = ui->ledt_key3AutoSet5G->text();
        m_WEPKey35G = ui->ledt_key4AutoSet5G->text();
    }
}

void ExtenderMainWindow::on_rbtn_key1AutoSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 1;
    }
}

void ExtenderMainWindow::on_rbtn_key2AutoSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 2;
    }
}

void ExtenderMainWindow::on_rbtn_key3AutoSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 3;
    }
}

void ExtenderMainWindow::on_rbtn_key4AutoSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 4;
    }
}

void ExtenderMainWindow::on_cbx_securityTypeChooseManual2G_currentIndexChanged(int index)
{
    QString securitytype = ui->cbx_securityTypeChooseManual2G->currentText();
    if (index == 0 || index == 1) {
        ui->frm_manualSet2G21->hide();
        ui->frm_manualSet2G22->hide();
        ui->frm_manualSet2G23->hide();
    }
    if (index == 1)
    {
        flagWifiOFFSel2G = true;
        m_securitytype2G == "OFF";
    }
    if (index == 2) {
        ui->frm_manualSet2G21->show();
        ui->frm_manualSet2G22->show();
        ui->frm_manualSet2G23->hide();
        m_securitytype2G == "WEP";
        flagWifiWEPSel2G = true;
    }
    if (index == 3 || index == 4 || index == 5 || index == 6 || index == 7) {
        ui->frm_manualSet2G21->hide();
        ui->frm_manualSet2G22->hide();
        ui->frm_manualSet2G23->show();
        ui->lbl_securitytypeManual2G->setText(securitytype);
        flagWifiWPAPSKSel2G = true;
        if (index == 3) {
            m_securitytype2G = "WPA-PSK-TKIP";
        }
        if (index == 4) {
            m_securitytype2G = "WPA-PSK-AES";
        }
        if (index == 5) {
            m_securitytype2G = "WPA2-PSK-TKIP";
        }
        if (index == 6) {
            m_securitytype2G = "WPA2-PSK-AES";
        }
        if (index == 7) {
            m_securitytype2G = "WPA-PSK-TKIP|WPA2-PSK-AES";
        }
    }
}

void ExtenderMainWindow::on_cmb_bitManualSet2G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_newWEPLength2G = 64;
    }
    if (index ==1) {
        m_newWEPLength2G = 128;
    }
}

void ExtenderMainWindow::on_btn_pwdGeneratorManual2G_clicked()
{
    if (ui->ledt_wlanPasswordWEPManualSet2G->text().length() == 0)  //
    {

    }
    else
    {
        int keyLen = m_newWEPLength2G == 64 ? 10 : 26;
        ui->ledt_key1ManualSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet2G->text(), keyLen, 3));
        ui->ledt_key2ManualSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet2G->text(), keyLen, 5));
        ui->ledt_key3ManualSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet2G->text(), keyLen, 7));
        ui->ledt_key4ManualSet2G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet2G->text(), keyLen, 9));
    }
}

void ExtenderMainWindow::on_rbtn_key1ManualSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 1;
    }
}

void ExtenderMainWindow::on_rbtn_key2ManualSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 2;
    }
}

void ExtenderMainWindow::on_rbtn_key3ManualSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 3;
    }
}

void ExtenderMainWindow::on_rbtn_key4ManualSet2G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex2G = 4;
    }
}

void ExtenderMainWindow::on_cbx_securityTypeChooseManual5G_currentIndexChanged(int index)
{
    QString securitytype = ui->cbx_securityTypeChooseManual5G->currentText();
    if (index == 0 || index == 1) {
        ui->frm_manualSet5G21->hide();
        ui->frm_manualSet5G22->hide();
        ui->frm_manualSet5G23->hide();
    }
    if (index == 1)
    {
        m_securitytype5G = "OFF";
        flagWifiOFFSel5G = true;
    }
    if (index == 2) {
        ui->frm_manualSet5G21->show();
        ui->frm_manualSet5G22->show();
        ui->frm_manualSet5G23->hide();
        m_securitytype5G = "WEP";
        flagWifiWEPSel5G = true;
    }
    if (index == 3 || index == 4 || index == 5 || index == 6 || index == 7) {
        ui->frm_manualSet5G21->hide();
        ui->frm_manualSet5G22->hide();
        ui->frm_manualSet5G23->show();
        ui->lbl_securitytypeManual5G->setText(securitytype);
        flagWifiWPAPSKSel5G = true;
        if (index == 3) {
            m_securitytype5G = "WPA-PSK-TKIP";
        }
        if (index == 4) {
            m_securitytype5G = "WPA-PSK-AES";
        }
        if (index == 5) {
            m_securitytype5G = "WPA2-PSK-TKIP";
        }
        if (index == 6) {
            m_securitytype5G = "WPA2-PSK-AES";
        }
        if (index == 7) {
            m_securitytype5G = "WPA-PSK-TKIP|WPA2-PSK-AES";
        }
    }
}

void ExtenderMainWindow::on_cmb_bitManualSet5G_currentIndexChanged(int index)
{
    if (index == 0) {
        m_newWEPLength5G = 64;
    }
    if (index ==1) {
        m_newWEPLength5G = 128;
    }
}

void ExtenderMainWindow::on_btn_pwdGeneratorManual5G_clicked()
{
    if(ui->ledt_wlanPasswordWEPManualSet5G->text().length() == 0)
    {

    }
    else
    {
        int keyLen = m_newWEPLength5G == 64 ? 10 : 26;
        ui->ledt_key1ManualSet5G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet5G->text(), keyLen, 3));
        ui->ledt_key2ManualSet5G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet5G->text(), keyLen, 5));
        ui->ledt_key3ManualSet5G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet5G->text(), keyLen, 7));
        ui->ledt_key4ManualSet5G->setText(
                    ExtenderUtil::wepKeyGenerater(
                        ui->ledt_wlanPasswordWEPManualSet5G->text(), keyLen, 9));
    }
}

void ExtenderMainWindow::on_rbtn_key1ManualSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 1;
    }
}

void ExtenderMainWindow::on_rbtn_key2ManualSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 2;
    }
}

void ExtenderMainWindow::on_rbtn_key3ManualSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 3;
    }
}

void ExtenderMainWindow::on_rbtn_key4ManualSet5G_clicked(bool checked)
{
    if (checked) {
        m_newKeyIndex5G = 4;
    }
}

void ExtenderMainWindow::on_rbtn_key12G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex2G = 1;
}

void ExtenderMainWindow::on_rbtn_key22G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex2G = 2;
}

void ExtenderMainWindow::on_rbtn_key32G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex2G = 3;
}

void ExtenderMainWindow::on_rbtn_key42G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex2G = 4;
}

void ExtenderMainWindow::on_rbtn_key15G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex5G = 1;
}

void ExtenderMainWindow::on_rbtn_key25G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex5G = 2;
}

void ExtenderMainWindow::on_rbtn_key35G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex5G = 3;
}

void ExtenderMainWindow::on_rbtn_key45G_clicked(bool checked)
{
    if (checked)
        m_extenderNewKeyIndex5G = 4;
}

void ExtenderMainWindow::on_btn_cancel_clicked()
{
    emit emitRestart();
}

void ExtenderMainWindow::on_btn_ignore_clicked()
{
    emit emitExit();
}

/*=====Control Ui end*/


//=====i18n
void ExtenderMainWindow::translate()
{
    ui->lbl_model->setText(translateText(MainWindow_lbl_model, QString("Model Name: ")));
    ui->lbl_languageSelect->setText(translateText(MainWindow_lbl_languageSelect, QString("Select Language")));
    ui->btn_about->setText(translateText(MainWindow_btn_about, QString("About")));
    ui->lbl_configureNow->setText(translateText(MainWindow_lbl_configureNow, QString("Configure the connection now !")));
    ui->lbl_needHelp->setText(translateText(MainWindow_lbl_needHelp, QString("Need Extender Genie help ?")));
    ui->btn_funcChoose->setText(translateText(MainWindow_btn_funcChoose, QString("Continue")));
    ui->rbtn_yes->setText(translateText(MainWindow_rbtn_yes, QString("Yes")));
    ui->rbtn_no->setText(translateText(MainWindow_rbtn_no, QString("No, I need to manually configure the connection")));
    ui->lbl_waitText2->setText(translateText(MainWindow_lbl_waitText2, QString("Extender Genie is searching for WiFi networks in your neighborhood.This takes approximately 1 minute.")));
    ui->lbl_selectNetwork->setText(translateText(MainWindow_lbl_selectNetwork, QString("Select the name of your existing network")));
    ui->rbtn_auto2G->setText(translateText(MainWindow_rbtn_auto2G, QString("Choose your wireless SSID from follow list")));
    ui->rbtn_auto5G->setText(translateText(MainWindow_rbtn_auto5G, QString("Choose your wireless SSID from follow list")));
    ui->rbtn_manual2G->setText(translateText(MainWindow_rbtn_manual2G, QString("Manually input your wireless SSID")));
    ui->rbtn_manual5G->setText(translateText(MainWindow_rbtn_manual5G, QString("Manually input your wireless SSID")));
    ui->btn_refresh->setText(translateText(MainWindow_btn_refresh, QString("Refresh")));
    ui->btn_extenderToRouter->setText(translateText(MainWindow_btn_extenderToRouter, QString("Continue")));
    ui->lbl_selectedNetwork->setText(translateText(MainWindow_lbl_selectedNetwork, QString("Enter the security settings of the selected WiFi network.")));
    ui->lbl_selectedNetwork2G->setText(translateText(MainWindow_lbl_selectedNetwork2G, QString("The WiFi network in 2.4G.")));
    ui->lbl_SSIDNameAuto2GLabel->setText(translateText(MainWindow_lbl_SSIDNameAuto2GLabel, QString("Name (SSID):")));
    ui->lbl_securityTypeAuto2GLabel->setText(translateText(MainWindow_lbl_securityTypeAuto2GLabel, QString("Security:")));
    ui->lbl_wlanPasswordAutoSet2G->setText(translateText(MainWindow_lbl_wlanPasswordAutoSet2G, QString("PassPhrase:")));
    ui->lbl_securityOptionsManual2G->setText(translateText(MainWindow_lbl_securityOptionsManual2G, QString("Security Options:")));
    ui->lbl_wlanPasswordManualSet2G->setText(translateText(MainWindow_lbl_wlanPasswordManualSet2G, QString("PassPhrase:")));
    ui->lbl_securityOptionsManual5G->setText(translateText(MainWindow_lbl_securityOptionsManual5G, QString("Security Options:")));
    ui->lbl_wlanPasswordManualSet5G->setText(translateText(MainWindow_lbl_wlanPasswordManualSet5G, QString("PassPhrase:")));
    ui->lbl_SecurityWEPAuto2G->setText(translateText(MainWindow_lbl_SecurityWEPAuto2G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrengthAuto2G->setText(translateText(MainWindow_lbl_encryptionStrengthAuto2G, QString("Encryption Strength")));
    ui->lbl_wlanPasswordWEPAutoSet2G->setText(translateText(MainWindow_lbl_wlanPasswordWEPAutoSet2G, QString("PassPhrase :")));
    ui->rbtn_key1AutoSet2G->setText(translateText(MainWindow_rbtn_key1AutoSet2G, QString("Key 1:")));
    ui->rbtn_key2AutoSet2G->setText(translateText(MainWindow_rbtn_key2AutoSet2G, QString("Key 2:")));
    ui->rbtn_key3AutoSet2G->setText(translateText(MainWindow_rbtn_key3AutoSet2G, QString("Key 3:")));
    ui->rbtn_key4AutoSet2G->setText(translateText(MainWindow_rbtn_key4AutoSet2G, QString("Key 4:")));
    ui->lbl_selectedNetworkManual2G->setText(translateText(MainWindow_lbl_selectedNetworkManual2G, QString("The WiFi network in 2.4G.")));
    ui->lbl_SSIDNameManual2G->setText(translateText(MainWindow_lbl_SSIDNameManual2G, QString("Name (SSID):")));
    ui->lbl_securityManual2G->setText(translateText(MainWindow_lbl_securityManual2G, QString("Security:")));
    ui->lbl_securityWEPManual2G->setText(translateText(MainWindow_lbl_securityWEPManual2G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrengthManual2G->setText(translateText(MainWindow_lbl_encryptionStrengthManual2G, QString("Encryption Strength")));
    ui->btn_pwdGeneratorManual2G->setText(translateText(MainWindow_btn_pwdGeneratorManual2G, QString("Generate")));
    ui->lbl_wlanPasswordWEPManualSet2G->setText(translateText(MainWindow_lbl_wlanPasswordWEPManualSet2G, QString("PassPhrase :")));
    ui->lbl_selectedNetwork5G->setText(translateText(MainWindow_lbl_selectedNetwork5G, QString("The WiFi network in 5G.")));
    ui->lbl_SSIDNameAuto5GLabel->setText(translateText(MainWindow_lbl_SSIDNameAuto5GLabel, QString("Name (SSID):")));
    ui->lbl_securityTypeAuto5GLabel->setText(translateText(MainWindow_lbl_securityTypeAuto5GLabel, QString("Security:")));
    ui->lbl_wlanPasswordAutoSet5G->setText(translateText(MainWindow_lbl_wlanPasswordAutoSet5G, QString("PassPhrase:")));
    ui->lbl_SecurityWEPAuto5G->setText(translateText(MainWindow_lbl_SecurityWEPAuto5G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrengthAuto5G->setText(translateText(MainWindow_lbl_encryptionStrengthAuto5G, QString("Encryption Strength")));
    ui->lbl_wlanPasswordWEPAutoSet5G->setText(translateText(MainWindow_lbl_wlanPasswordWEPAutoSet5G, QString("PassPhrase :")));
    ui->rbtn_key1AutoSet5G->setText(translateText(MainWindow_rbtn_key1AutoSet5G, QString("Key 1:")));
    ui->rbtn_key2AutoSet5G->setText(translateText(MainWindow_rbtn_key2AutoSet5G, QString("Key 2:")));
    ui->rbtn_key3AutoSet5G->setText(translateText(MainWindow_rbtn_key3AutoSet5G, QString("Key 3:")));
    ui->rbtn_key4AutoSet5G->setText(translateText(MainWindow_rbtn_key4AutoSet5G, QString("Key 4:")));
    ui->lbl_selectedNetworkManual5G->setText(translateText(MainWindow_lbl_selectedNetworkManual5G, QString("The WiFi network in 5G.")));
    ui->lbl_SSIDNameManual5G->setText(translateText(MainWindow_lbl_SSIDNameManual5G, QString("Name (SSID):")));
    ui->lbl_securityManual5G->setText(translateText(MainWindow_lbl_securityManual5G, QString("Security:")));
    ui->lbl_securityWEPManual5G->setText(translateText(MainWindow_lbl_securityWEPManual5G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrengthManual5G->setText(translateText(MainWindow_lbl_encryptionStrengthManual5G, QString("Encryption Strength")));
    ui->lbl_wlanPasswordWEPManualSet5G->setText(translateText(MainWindow_lbl_wlanPasswordWEPManualSet5G, QString("PassPhrase :")));
    ui->btn_backExtenderToRouter->setText(translateText(MainWindow_btn_backExtenderToRouter, QString("Back")));
    ui->btn_extenderToRouterContinue->setText(translateText(MainWindow_btn_extenderToRouterContinue, QString("Continue")));
    ui->lbl_waitText3->setText(translateText(MainWindow_lbl_waitText3, QString("Extender Genie is setting up the WiFi connection between the existing wireless router and the extender.This takes approximately 2 minutes.")));
    ui->lbl_deviceToExtender1->setText(translateText(MainWindow_lbl_deviceToExtender1, QString("Enter the settings for the Extender network.")));
    ui->lbl_extenderNtName2G->setText(translateText(MainWindow_lbl_extenderNtName2G, QString("2.4GHz Network Name (SSID):")));
    ui->lbl_extenderNtName5G->setText(translateText(MainWindow_lbl_extenderNtName5G, QString("5 GHz Network Name (SSID):")));
    ui->lbl_extenderPortSelect->setText(translateText(MainWindow_lbl_extenderPortSelect, QString("Ethernet ports to WiFi use the following band:")));
    ui->chk_usingCurrentSetting->setText(translateText(MainWindow_chk_usingCurrentSetting, QString("Use the same security mode and password as those for the existing network.")));
    ui->lbl_securityOption2G->setText(translateText(MainWindow_lbl_securityOption2G, QString("Security Options(2.4G)")));
    ui->rbtn_none2G->setText(translateText(MainWindow_rbtn_none2G, QString("None")));
    ui->rbtn_WEP2G->setText(translateText(MainWindow_rbtn_WEP2G, QString("WEP")));
    ui->rbtn_WPAPSKTKIP2G->setText(translateText(MainWindow_rbtn_WPAPSKTKIP2G, QString("WPA-PSK-TKIP")));
    ui->rbtn_WPA2PSKAES2G->setText(translateText(MainWindow_rbtn_WPA2PSKAES2G, QString("WPA2-PSK-AES")));
    ui->rtbn_WPAandWAP22G->setText(translateText(MainWindow_rtbn_WPAandWAP22G, QString("WPA-PSK-TKIP|WPA2-PSK-AES")));
    ui->lbl_securityEncryption2G->setText(translateText(MainWindow_lbl_securityEncryption2G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrength2G->setText(translateText(MainWindow_lbl_encryptionStrength2G, QString("Encryption Strength")));
    ui->btn_pwdGenerator2G->setText(translateText(MainWindow_btn_pwdGenerator2G, QString("Generate")));
    ui->lbl_wlanPasswordWEPManualSet2G->setText(translateText(MainWindow_lbl_wlanPasswordWEPManualSet2G, QString("PassPhrase :")));
    ui->lbl_passPhraseWEP2G->setText(translateText(MainWindow_lbl_passPhraseWEP2G, QString("PassPhrase :")));
    ui->rbtn_key1ManualSet2G->setText(translateText(MainWindow_rbtn_key1ManualSet2G, QString("Key 1:")));
    ui->rbtn_key2ManualSet2G->setText(translateText(MainWindow_rbtn_key2ManualSet2G, QString("Key 2:")));
    ui->rbtn_key3ManualSet2G->setText(translateText(MainWindow_rbtn_key3ManualSet2G, QString("Key 3:")));
    ui->rbtn_key4ManualSet2G->setText(translateText(MainWindow_rbtn_key4ManualSet2G, QString("Key 4:")));
    ui->rbtn_key1ManualSet5G->setText(translateText(MainWindow_rbtn_key1ManualSet5G, QString("Key 1:")));
    ui->rbtn_key2ManualSet5G->setText(translateText(MainWindow_rbtn_key2ManualSet5G, QString("Key 2:")));
    ui->rbtn_key3ManualSet5G->setText(translateText(MainWindow_rbtn_key3ManualSet5G, QString("Key 3:")));
    ui->rbtn_key4ManualSet5G->setText(translateText(MainWindow_rbtn_key4ManualSet5G, QString("Key 4:")));
    ui->lbl_securityName2G->setText(translateText(MainWindow_lbl_securityName2G, QString("Security Name")));
    ui->lbl_passPhraseNoWEP2G->setText(translateText(MainWindow_lbl_passPhraseNoWEP2G, QString("PassPhrase:")));
    ui->lbl_hint2G2->setText(translateText(MainWindow_lbl_hint2G2, QString(" (8-63 characters or 64 hexdigits)")));
    ui->lbl_securityOption5G->setText(translateText(MainWindow_lbl_securityOption5G, QString("Security Options(5G)")));
    ui->rbtn_none5G->setText(translateText(MainWindow_rbtn_none5G, QString("None")));
    ui->lbl_securityEncryption5G->setText(translateText(MainWindow_lbl_securityEncryption5G, QString("Security Encryption (WEP)")));
    ui->lbl_encryptionStrength5G->setText(translateText(MainWindow_lbl_encryptionStrength5G, QString("Encryption Strength")));
    ui->btn_pwdGenerator5G->setText(translateText(MainWindow_btn_pwdGenerator5G, QString("Generate")));
    ui->lbl_passPhraseWEP5G->setText(translateText(MainWindow_lbl_passPhraseWEP5G, QString("PassPhrase :")));
    ui->lbl_key15G->setText(translateText(MainWindow_rbtn_key1Auto5G, QString("Key 1:")));
    ui->lbl_key25G->setText(translateText(MainWindow_rbtn_key2Auto5G, QString("Key 2:")));
    ui->lbl_key35G->setText(translateText(MainWindow_rbtn_key3Auto5G, QString("Key 3:")));
    ui->lbl_key45G->setText(translateText(MainWindow_rbtn_key4Auto5G, QString("Key 4:")));
    ui->lbl_securityName5G->setText(translateText(MainWindow_lbl_securityName5G, QString("Security Name")));
    ui->lbl_passPhraseNoWEP5G->setText(translateText(MainWindow_lbl_passPhraseNoWEP5G, QString("PassPhrase:")));
    ui->lbl_hint5G2->setText(translateText(MainWindow_lbl_hint5G2, QString(" (8-63 characters or 64 hexdigits)")));
    ui->btn_extenderSet->setText(translateText(MainWindow_btn_extenderSet, QString("Continue")));
    ui->lbl_waitText4->setText(translateText(MainWindow_lbl_waitText4, QString("Extender Genie is applying the settings.This takes approximately 1 minute.")));
    ui->lbl_completInstall->setText(translateText(MainWindow_lbl_completInstall, QString("You are now one step away from completing the installation.")));
    ui->lbl_extenderCreated->setText(translateText(MainWindow_lbl_extenderCreated, QString("Your Extender network has been created!")));
    ui->lbl_finishExtenderNetwork2G->setText(translateText(MainWindow_lbl_finishExtenderNetwork2G, QString("Extender network (2.4GHz b/g/n):")));
    ui->lbl_SSIDFinish2G->setText(translateText(MainWindow_lbl_SSIDFinish2G, QString("Wireless network name (SSID) :")));
    ui->lbl_securitytypeFinish2G->setText(translateText(MainWindow_lbl_securitytypeFinish2G, QString("Security Type:")));
    ui->lbl_passphraseFinish2G->setText(translateText(MainWindow_lbl_passphraseFinish2G, QString("Passphrase :")));
    ui->lbl_WEPStrength2G->setText(translateText(MainWindow_lbl_WEPStrength2G, QString("Encryption Strength:")));
    ui->lbl_index2G->setText(translateText(MainWindow_lbl_index2G, QString("KeyIndex2G:")));
    ui->rbtn_key12G->setText(translateText(MainWindow_rbtn_key12G, QString("Key1:")));
    ui->rbtn_key22G->setText(translateText(MainWindow_rbtn_key22G, QString("Key2:")));
    ui->rbtn_key32G->setText(translateText(MainWindow_rbtn_key32G, QString("Key3:")));
    ui->rbtn_key42G->setText(translateText(MainWindow_rbtn_key42G, QString("Key4:")));
    ui->lbl_finishExtenderNetwork5G->setText(translateText(MainWindow_lbl_finishExtenderNetwork5G, QString("Extender network (5GHz a/n):")));
    ui->lbl_SSIDFinish5G->setText(translateText(MainWindow_lbl_SSIDFinish5G, QString("Wireless network name (SSID) :")));
    ui->lbl_securitytypeFinish5G->setText(translateText(MainWindow_lbl_securitytypeFinish5G, QString("Security Type:")));
    ui->lbl_passphraseFinish5G->setText(translateText(MainWindow_lbl_passphraseFinish5G, QString("Passphrase :")));
    ui->lbl_WEPStrength5G->setText(translateText(MainWindow_lbl_WEPStrength5G, QString("Encryption Strength:")));
    ui->lbl_index5G->setText(translateText(MainWindow_lbl_index5G, QString("KeyIndex5G:")));
    ui->rbtn_key15G->setText(translateText(MainWindow_rbtn_key15G, QString("Key1:")));
    ui->rbtn_key25G->setText(translateText(MainWindow_rbtn_key25G, QString("Key2:")));
    ui->rbtn_key35G->setText(translateText(MainWindow_rbtn_key35G, QString("Key3:")));
    ui->rbtn_key45G->setText(translateText(MainWindow_rbtn_key45G, QString("Key4:")));
    ui->chk_finish->setText(translateText(MainWindow_chk_finish, QString("Select this checkbox to connect the extended SSID.")));
    ui->btn_extendFinish->setText(translateText(MainWindow_btn_extendFinish, QString("Finish")));
    ui->lbl_showErrorMessage->setText(translateText(MainWindow_lbl_showErrorMessage, QString("Sorry, there is something wrong with your extender")));
    ui->btn_cancel->setText(translateText(MainWindow_btn_cancel, QString("Restart")));
    ui->btn_ignore->setText(translateText(MainWindow_btn_ignore, QString("Exit")));
    ui->lbl_promptNoConnectExtender->setText(translateText(MainWindow_lbl_promptNoConnectExtender, QString("Extender Genie detected your device unconnected to the extender.")));
    ui->rbtn_needGenieHelp->setText(translateText(MainWindow_rbtn_needGenieHelp, QString("Need Extender Genie help me")));
    ui->rbtn_connectManually->setText(translateText(MainWindow_rbtn_connectManually, QString("I want to connect manually")));
    ui->btn_connectContinue->setText(translateText(MainWindow_btn_connectContinue, QString("Next")));
    ui->lbl_connectHintText->setText(translateText(MainWindow_lbl_connectHintText, QString("Extender Genie detected your device unconnected to the extender！\n\nPlease connet to EXTENDER_EXT according to the left hint, then go on...")));
    ui->lbRouterInfo2G->setText(translateText(MainWindow_lbRouterInfo2G, QString("Connection Status to Existing Network (2.4GHz)")));
    ui->lbRouterName2G->setText(translateText(MainWindow_lbRouterName2G, QString("  Name (SSID):")));
    ui->lbRouterPassword2G->setText(translateText(MainWindow_lbRouterPassword2G, QString("  Password:")));
    ui->lbWlanInfo2G->setText(translateText(MainWindow_lbWlanInfo2G, QString("Extender Wireless Setting (2.4GHz)")));
    ui->lbWlanName2G->setText(translateText(MainWindow_lbWlanName2G, QString("  Name (SSID):")));
    ui->lbWlanAuthType2G->setText(translateText(MainWindow_lbWlanAuthType2G, QString("  Security Type:")));

    ui->lbWlanPassword2G->setText(translateText(MainWindow_lbWlanPassword2G, QString("  Password:")));
    ui->lbRouterInfo5G->setText(translateText(MainWindow_lbRouterInfo5G, QString("Connection Status to Existing Network (5GHz)")));
    ui->lbRouterName5G->setText(translateText(MainWindow_lbRouterName5G, QString("  Name (SSID):")));
    ui->lbRouterPassword5G->setText(translateText(MainWindow_lbRouterPassword5G, QString("")));
    ui->lbWlanInfo5G->setText(translateText(MainWindow_lbWlanInfo5G, QString("Extender Wireless Setting (5GHz)")));
    ui->lbWlanName5G->setText(translateText(MainWindow_lbWlanName5G, QString("  Name (SSID):")));
    ui->lbWlanAuthType5G->setText(translateText(MainWindow_lbWlanAuthType5G, QString("  Security Type:")));
    ui->lbWlanPassword5G->setText(translateText(MainWindow_lbWlanPassword5G, QString("  Password:")));
    ui->btnBack->setText(translateText(MainWindow_btnBack, QString("Back")));
    ui->btnApply->setText(translateText(MainWindow_btnApply, QString("Apply")));
    ui->lbDefaultExtender->setText(translateText(MainWindow_lbDefaultExtender, QString("Factory Reset")));
    ui->btnDefaultExtender->setText(translateText(MainWindow_btnDefaultExtender, QString("Go")));
    ui->rbtn_haveConnected->setText(translateText(MainWindow_rbtn_haveConnected, QString("I have connected the Extender.")));
    ui->btn_nextConnectManually->setText(translateText(MainWindow_btn_nextConnectManually, QString("Next")));
    ui->gbxAdvancedSet->setTitle(translateText(MainWindow_lbTitleManualSet, QString("Advanced Set")));
    ui->lbRouterStatus2G->setText(translateText(MainWindow_lbRouterStatus2G, QString("  Connection Status")));
    ui->lbRouterStatus5G->setText(translateText(MainWindow_lbRouterStatus5G, QString("  Connection Status")));

}

QString ExtenderMainWindow::getSSIDFromWifiList(QString radioModel, int rowNumber)
{
    int rowCount2G = ui->tblwdt_wifiList2G->rowCount();
    int rowCount5G = ui->tblwdt_wifiList5G->rowCount();
    if (radioModel == "2G") {
        if (0 != rowCount2G)
            return ui->tblwdt_wifiList2G->item(rowNumber, 0)->text();
        else
            return "";
    }
    if (radioModel == "5G") {
        if (0 != rowCount5G)
            return ui->tblwdt_wifiList5G->item(rowNumber, 0)->text();
        else
            return "";
    }
    return "";
}

QString ExtenderMainWindow::getSecuritytypeFromWifiList(QString radioModel, int rowNumber)
{
    int rowCount2G = ui->tblwdt_wifiList2G->rowCount();
    int rowCount5G = ui->tblwdt_wifiList5G->rowCount();
    if (radioModel == "2G") {
        if (0 != rowCount2G)
            return ui->tblwdt_wifiList2G->item(rowNumber, 1)->text();
        else
            return "";
    }
    if (radioModel == "5G") {
        if (0 != rowCount5G)
            return ui->tblwdt_wifiList5G->item(rowNumber, 1)->text();
        else
            return "";
    }
    return "";
}

// the two ways for tell user to connect to EXTENDER_EXT
void ExtenderMainWindow::on_btn_connectContinue_clicked()
{
    if (ui->rbtn_needGenieHelp->isChecked()) {
        emit connectNeedGenieHelp();
    }
    if (ui->rbtn_connectManually->isChecked()) {
        emit connectManuallyOK();
    }
}

void ExtenderMainWindow::on_btn_nextConnectManually_clicked()
{
    waitting_text_idx = L_MW_WAITTING_GETCURRENTSETTING;
    setWaittingText(L_MW_WAITTING_GETCURRENTSETTING);
    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));
    emit connectManuallyOK();
}

void ExtenderMainWindow::on_rbtn_connectManually_clicked(bool checked)
{
    if (checked) {
        btn_connectContinue_text_idx = L_MW_BTN_CONNECTCONTINUE_OK;
        ui->btn_connectContinue->setText(translateText(btn_connectContinue_text_idx));
    }
}

void ExtenderMainWindow::on_rbtn_needGenieHelp_clicked(bool checked)
{
    if (checked) {
        btn_connectContinue_text_idx = L_MW_BTN_CONNECTCONTINUE_NEXT;
        ui->btn_connectContinue->setText(translateText(btn_connectContinue_text_idx));
    }
}

void ExtenderMainWindow::jumpPG_connectToExtender()
{
    ui->lbl_connectHintImg->clear();
    ui->btn_nextConnectManually->setEnabled(false);
    ui->rbtn_haveConnected->setChecked(false);

    retranslateUi();

    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_connectToExtendeHintImg));
}

void ExtenderMainWindow::jumpPG_wait()
{
    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));
}

void ExtenderMainWindow::setWaittingText(int waitTextIdx)
{
    waitting_text_idx = waitTextIdx;
    ui->lbl_waitText->setText(translateText(waitTextIdx));
}

void ExtenderMainWindow::on_btnBack_clicked()
{
    routerStatus2G = "";
    routerStatus5G = "";
    routerSSID2G = "";
    routerSSID5G = "";
    routerEncryptionModes2G = "";
    routerEncryptionModes5G = "";
    routerWPAPassphrase2G = "";
    routerWPAPassphrase5G = "";
    routerWEPPassphrase2G = "";
    routerWEPPassphrase5G = "";
    wlanStatus2G = "";
    wlanStatus5G = "";
    wlanSSID2G = "";
    wlanSSID5G = "";
    wlanEncryptionModes2G = "";
    wlanEncryptionModes5G = "";
    wlanWPAPassphrase2G = "";
    wlanWPAPassphrase5G = "";
    wlanWEPPassphrase2G = "";
    wlanWEPPassphrase5G = "";

    bAdvancedSetBack = true;

    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_start));
}

void ExtenderMainWindow::on_btnApply_clicked()
{
    if (verifyManualSetOK()) {
        soapapi->ConfigurationStarted(&ConfigurationStartedAccessManager, g_port_extender);
        connect(&ConfigurationStartedAccessManager,SIGNAL(finished(QNetworkReply*)),
                this,SLOT(slt_configurationStarted(QNetworkReply*)), Qt::UniqueConnection);
    }
}

bool ExtenderMainWindow::verifyManualSetOK()
{
    QString SSID2G = ui->lbWlanName2G->text();
    QString authType2G = ui->cbxWlanAuthType2G->currentText();
    QString password2G = ui->ledtWlanPasswrod2G->text();
    bool bledtWlanPasswrod2G = ui->ledtWlanPasswrod2G->isVisible();

    QString SSID5G = ui->lbWlanName5G->text();
    QString authType5G = ui->cbxWlanAuthType5G->currentText();
    QString password5G = ui->ledtWlanPasswrod5G->text();
    bool bledtWlanPasswrod5G = ui->ledtWlanPasswrod5G->isVisible();

    QString strNoSSID = translateText(L_MSGBOX_EXTENDERNAME, "mainwindowlanguage");
    QString strWEPKey = translateText(L_MSGBOX_WEP, "mainwindowlanguage");

    if (g_extenderType == "1000RP") {
        if (SSID2G.isEmpty() || !validSSID(SSID2G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
            ui->ledtWlanName2G->setFocus();
            qDebug() << "ledtWlanName2G is empty";
            return false;
        } else if (authType2G == "WEP" && !(password2G.length() == 10 || password2G.length() == 26)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod2G->setFocus();
            qDebug() << "ledtWlanPasswrod2G is invalid";
            return false;
        } else if (bledtWlanPasswrod2G && !validPassord(password2G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod2G->setFocus();
            qDebug() << "ledtWlanPasswrod2G is empty";
            return false;
        } else {
            return true;
        }
    }

    else if (g_extenderType == "2500RP") {
        if (SSID2G.isEmpty() || !validSSID(SSID2G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
            ui->ledtWlanName2G->setFocus();
            qDebug() << "ledtWlanName2G is empty";
            return false;
        } else if (authType2G == "WEP" && !(password2G.length() == 10 || password2G.length() == 26)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod2G->setFocus();
            qDebug() << "ledtWlanPasswrod2G is invalid";
            return false;
        } else if (bledtWlanPasswrod2G && !validPassord(password2G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod2G->setFocus();
            qDebug() << "ledtWlanPasswrod2G is empty";
            return false;
        } else if (SSID5G.isEmpty() || !validSSID(SSID5G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strNoSSID, QMessageBox::Ok);
            ui->ledtWlanName5G->setFocus();
            qDebug() << "ledtWlanName5G is empty";
            return false;
        } else if (authType5G == "WEP" && !(password5G.length() == 10 || password5G.length() == 26)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod5G->setFocus();
            qDebug() << "ledtWlanPasswrod5G is invalid";
            return false;
        } else if (bledtWlanPasswrod5G && !validPassord(password5G)) {
            QMessageBox::warning(this, tr("Extender Genie"), strWEPKey, QMessageBox::Ok);
            ui->ledtWlanPasswrod5G->setFocus();
            qDebug() << "ledtWlanPasswrod5G is empty";
            return false;
        } else {
            return true;
        }
    }

    return true;

}

void ExtenderMainWindow::on_cbxWlanAuthType2G_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->lbWlanPassword2G->setVisible(false);
        ui->ledtWlanPasswrod2G->setVisible(false);
    } else {
        ui->lbWlanPassword2G->setVisible(true);
        ui->ledtWlanPasswrod2G->setVisible(true);
    }
}

void ExtenderMainWindow::on_cbxWlanAuthType5G_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->lbWlanPassword5G->setVisible(false);
        ui->ledtWlanPasswrod5G->setVisible(false);
    } else {
        ui->lbWlanPassword5G->setVisible(true);
        ui->ledtWlanPasswrod5G->setVisible(true);
    }
}

void ExtenderMainWindow::on_btnDefaultExtender_clicked()
{
    setWaittingText(L_MW_WAITTING_Applying);
    setWaittingUi(ui->vl_wait);
    ui->stawdt_main->setCurrentIndex(ui->stawdt_main->indexOf(ui->pg_wait));

    soapapi->ResetToFactoryDefault(&ResetToFactoryDefaultManager, g_port_extender, m_serialNumber);
    connect(&ResetToFactoryDefaultManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(sltResetToFactoryDefault(QNetworkReply*)), Qt::UniqueConnection);
}

void ExtenderMainWindow::on_rbtn_haveConnected_clicked(bool checked)
{
    if (checked) {
        ui->btn_nextConnectManually->setEnabled(true);
    } else {
        ui->btn_nextConnectManually->setEnabled(false);
    }
}

void ExtenderMainWindow::on_cmb_language_currentIndexChanged(int index)
{
    emit emitSetCurrentLanguage(index);
}

void ExtenderMainWindow::onLanguageChanged()
{
    emit emitSetCurrentLanguage(0);
}

QString ExtenderMainWindow::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx, &text);
    return text.isEmpty() ? defaultText : text;
}

// For Exit Dialog
void ExtenderMainWindow::sltExit()
{
    this->showNormal();
    this->activateWindow();

    QUiLoader loader;
    QFile file(":/ui/exitdialog.ui");
    bool bTemp = file.open(QFile::ReadOnly);
    Q_ASSERT(bTemp);
    QWidget *wdt = loader.load(&file, this);
    QDialog *dlg = static_cast<QDialog *>(wdt);

    _setFramelessStyle(wdt);

    dlg->setWindowFlags(dlg->windowFlags() | Qt::WindowStaysOnTopHint);
    QLabel *labTitle = wdt->findChild<QLabel *>("label_title");
    Q_ASSERT(labTitle);
    labTitle->setText(translateText(ExitDlg_Title, "Exit"));
    QLabel *labContent = wdt->findChild<QLabel *>("label_content");
    Q_ASSERT(labContent);
    labContent->setText(translateText(ExitDlg_Content, "Are you sure you want to exit EXTENDER genie?"));
    QPushButton *okBtn = wdt->findChild<QPushButton *>("ok");
    QPushButton *cancelBtn = wdt->findChild<QPushButton *>("cancel");
    okBtn->setText(translateText(ExitDlg_Btn_OK, "OK"));
    cancelBtn->setText(translateText(ExitDlg_Btn_Cancel, "Cancel"));

    if (1 == dlg->exec()) {
        delete dlg;
        // This ConfigurationFinished for match ConfigurationStarted, so the whole process finished.
        soapapi->ConfigurationFinished(&ConfigurationFinishedAccessManager, g_port_extender, NewSessionID);
        qDebug() << "Extender exit!";
        this->close();
    } else {
        this->showNormal();
    }
}

void ExtenderMainWindow::_setFramelessStyle(QWidget *w)
{
    QDialog *dlg = qobject_cast<QDialog *>(w);

#ifdef Q_OS_MACX
    Qt::WindowFlags flag= (dlg)?(Qt::Dialog| Qt::FramelessWindowHint|Qt::Popup):(Qt::FramelessWindowHint);
#else
    Qt::WindowFlags flag= (dlg)?(Qt::Dialog| Qt::FramelessWindowHint):(Qt::FramelessWindowHint);

    if(dlg) {
        dlg->setModal(true);
    }
#endif
    w->setWindowFlags(flag);
#ifdef Q_OS_WIN32
    w->setAttribute(Qt::WA_TranslucentBackground);
#endif

    w->installEventFilter(this);

    QFile f(":/ui/framelessframe.qss");
    bool btemp = f.open(QIODevice::ReadOnly);
    Q_ASSERT(btemp);
    QString stemp = f.readAll();
    w->setStyleSheet(stemp);
}

bool ExtenderMainWindow::eventFilter(QObject *obj, QEvent *env)
{
    QEvent::Type type = env->type();
    QWidget *w = qobject_cast<QWidget *>(obj);

    if((QEvent::MouseButtonPress == type
        || QEvent::MouseButtonRelease == type
        || QEvent::MouseMove == type)
            && (w)) {
        QMouseEvent *mouseenv = static_cast<QMouseEvent *>(env);

        switch(type)
        {
        case QEvent::MouseButtonPress:
            if(mouseenv->button() & Qt::LeftButton) {
                mDrag = true;
                mDragPoint = mouseenv->globalPos() - w->frameGeometry().topLeft();
            }
            break;

        case QEvent::MouseButtonRelease:
            if(mouseenv->button() & Qt::LeftButton) {
                mDrag = false;
            }
            break;

        case QEvent::MouseMove:
            if ((mouseenv->buttons() & Qt::LeftButton) && mDrag && !w->isFullScreen()) {
                w->move(mouseenv->globalPos() - mDragPoint);
            }
            break;
        }
    }

    return false;
}

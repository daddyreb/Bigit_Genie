#include <QRegExp>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

#include "extendercontrol.h"
#include "extenderutil.h"
#include "language_mainwindow.h"

// define global variables, g_prot have two types: 80, 5000
// g_extender have two types: 1000RP, 2500RP
QString g_port_extender = "";
QString g_extenderType = "";
extern QString g_host;

ExtenderControl *ExtenderControl::m_instance = NULL;

ExtenderControl::ExtenderControl(): QObject()
  ,sp(NULL)
  ,mw(NULL)
  ,wifiUtil(NULL)
  ,waitting_text_idx(2003)
{
    sp = new ExtenderSoapApi;
    wifiUtil = new ExtenderUtilWiFi();
    iniConnect();
}

ExtenderControl::~ExtenderControl()
{
    delete sp;
    delete mw;
    delete wifiUtil;
}

ExtenderControl *ExtenderControl::GetInstance()
{
    if (m_instance == NULL) {
        m_instance = new ExtenderControl();
    }
    return m_instance;
}

void ExtenderControl::iniConnect()
{
    connect(&GetCurrentSettingAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slt_getCurrentSetting(QNetworkReply*)), Qt::UniqueConnection);
    connect(this, SIGNAL(emitConnectInitExtender()),
            this, SLOT(slt_connectInitExtender()), Qt::UniqueConnection);
}

void ExtenderControl::changeEvent(QEvent *event)
{
    if(event->type()== QEvent::LanguageChange)
        retranslateUi();
    ExtenderControl::changeEvent(event);
}

void ExtenderControl::retranslateUi()
{
    mw->setWaittingText(waitting_text_idx);
}

void ExtenderControl::extenderStart()
{    
    if (mw) {
        delete mw;
    }
    mw = new ExtenderMainWindow();

    connect(mw, SIGNAL(connectManuallyOK()), this, SLOT(slt_startGetCurrentSetting()), Qt::UniqueConnection);
    connect(mw, SIGNAL(emitRestart()), this, SLOT(slt_restart()));
    connect(mw, SIGNAL(translateText(int,QString*)), this, SIGNAL(translateText(int,QString*)));


    mw->showFirstPage();

    qDebug() << "Genie start get current setting";
    waitting_text_idx = L_MW_WAITTING_GETCURRENTSETTING;
    mw->setWaittingText(L_MW_WAITTING_GETCURRENTSETTING);
    sp->GetCurrentSetting(&GetCurrentSettingAccessManager, "80");

    retranslateUi();
    mw->show();
}

void ExtenderControl::slt_connectInitExtender()
{
    QString SSID = "NETGEAR_EXT";
    bool bInitExtenderConnected = false;
    bInitExtenderConnected = wifiUtil->isSSIDConnected(SSID);
    if (!bInitExtenderConnected) {
        qDebug() << "Genie start conncet NETGEAR_EXT";
        bInitExtenderConnected = wifiUtil->ConnectInitExtender();
    }
    if (bInitExtenderConnected) {
        qDebug() << "Genie have connected NETGEAR_EXT";
        waitting_text_idx = L_MW_WAITTING_GETCURRENTSETTING;
        mw->setWaittingText(L_MW_WAITTING_GETCURRENTSETTING);
        // neccessary beause of Extender is not stable
        ExtenderUtil::setExtenderSleep(10000);
        // check Extender by GetCurrentSetting
        qDebug() << "Genie start get current setting";
        sp->GetCurrentSetting(&GetCurrentSettingAccessManager, "80");
    } else {
        qDebug() << "Genie not connected NETGEAR_EXT";
        mw->jumpPG_connectToExtender();
    }
}

void ExtenderControl::slt_getCurrentSetting(QNetworkReply* reply)
{
    QString all = ExtenderUtil::encoding(reply);

    QString retStr = checkIsExtender(all);

    if (retStr == "2500RP") {
        qDebug() << "Genie get current setting OK";
        waitting_text_idx = L_MW_WAITTING_AUTHENTICATE;
        mw->setWaittingText(L_MW_WAITTING_AUTHENTICATE);
        g_port_extender = "5000";
        g_extenderType = "2500RP";
        qDebug() << "g_port: 5000, g_extenderType = 2500RP";
        mw->startAuthenticate();
    } else if (retStr == "1000RP") {
        qDebug() << "Genie get current setting OK";
        waitting_text_idx = L_MW_WAITTING_AUTHENTICATE;
        mw->setWaittingText(L_MW_WAITTING_AUTHENTICATE);
        g_port_extender = "80";
        g_extenderType = "1000RP";
        qDebug() << "g_port: 80, g_extenderType = 1000RP";
        mw->startAuthenticate();
    } else {

        // TODO: By Dawny, shouldn't connect SSID automaticaly.
        qDebug() << "Genie get current setting ERROR";
        // not RP, start connect initial Extender
        waitting_text_idx = L_MW_WAITTING_CONNECTTOEXTENDER;
        mw->setWaittingText(L_MW_WAITTING_CONNECTTOEXTENDER);
        emit emitConnectInitExtender();
    }
}

void ExtenderControl::slt_startGetCurrentSetting()
{
    qDebug() << "Genie start get current setting again";
    sp->GetCurrentSetting(&GetCurrentSettingAccessManager, "80");
}

QString ExtenderControl::checkIsExtender(QString all)
{
    QTextStream stream(&all);
    stream << all;

    while (!stream.atEnd()) {
        QString content = stream.readLine();
        QStringList lineList = content.split("\r");
        foreach (QString line, lineList) {
            if (line.contains("Model=WN2500RP", Qt::CaseInsensitive))
                return "2500RP";
            if (line.contains("Model=WN1000RP", Qt::CaseInsensitive))
                return "1000RP";
        }
    }

    return "NotRP";
}

void ExtenderControl::setHostIP(QString &strIP)
{
    g_host = strIP;
}

QString ExtenderControl::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx, &text);
    return text.isEmpty() ? defaultText : text;
}

bool ExtenderControl::getIsRunning()
{
    // if mw is null, so don't check if is hidden.
    if (mw == NULL || mw->isHidden())
        return false;
    else
        return true;
}

void ExtenderControl::slt_restart()
{
    mw->close();
    this->extenderStart();
}

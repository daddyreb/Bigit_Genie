#include "plc_tcheckfirmwareupgrade.h"
#include "mainpipeinterface.h"

#include <QtDebug>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QNetworkRequest>
#include <QUrl>

PLC_TCheckFirmwareUpgrade::PLC_TCheckFirmwareUpgrade(const QString &targetDevMac) :
    PLCTransaction(TC_CHECK_FIRMWARE_UPGRADE)
  ,m_targetDevMac(targetDevMac)
  ,m_newversionAvlaible(false)
{
}


int PLC_TCheckFirmwareUpgrade::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf);

    m_newversionAvlaible = false;
    m_upgradeUrl.clear();

    QString version, region, model;
    if(!mainPipeInf->PLCGetDeviceFWUpgradeVersion(m_targetDevMac, version) ||
            !mainPipeInf->PLCGetDeviceFWUpgradeRegion(m_targetDevMac, region) ||
            !mainPipeInf->PLCGetDeviceModelName(m_targetDevMac, model) ||
            version.isEmpty() || region.isEmpty() || model.isEmpty()){

        return (m_transactionState = TS_FAILED);
    }

    CheckFirmwareUpgradeHandler cfuh;
    QString versionOnServer;
    if(!cfuh.Check(model, region, versionOnServer, m_upgradeUrl)){
        return (m_transactionState = TS_FAILED);
    }

    m_newversionAvlaible = VersionGreat(versionOnServer, version);

    return (m_transactionState = TS_SUCCESS);
}

bool PLC_TCheckFirmwareUpgrade::GetUpgradeUrl(QString &downloadUrl) const
{
    downloadUrl = m_upgradeUrl;
    return m_newversionAvlaible;
}

bool PLC_TCheckFirmwareUpgrade::VersionGreat(const QString &newVersion, const QString oldVersion) const
{
    QStringList newVerParts = newVersion.split(".");
    QStringList oldVerParts = oldVersion.split(".");

    int newPartsCount = newVerParts.size();
    int oldPartsCount = oldVerParts.size();

    bool converOK1 = false;
    bool converOK2 = false;
    int converValue1 = 0;
    int converValue2 = 0;

    for(int idx = 0 ; idx < newPartsCount && idx < oldPartsCount ; ++idx){
        converValue1 = newVerParts[idx].toInt(&converOK1);
        converValue2 = oldVerParts[idx].toInt(&converOK2);

        if(!converOK1 || !converOK2){
            return false;
        }

        if(converValue1 != converValue2){
            return converValue1 > converValue2;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define UPDATE_SERVER_URL "http://updates1.netgear.com"
#define UPDATE_URL_TEMPLATE (UPDATE_SERVER_URL"/%1/%2/fileinfo.txt")

CheckFirmwareUpgradeHandler::CheckFirmwareUpgradeHandler(QObject *parent) :
    QThread(parent)
  ,m_dataValid(false)
{
}

CheckFirmwareUpgradeHandler::~CheckFirmwareUpgradeHandler()
{
    quit();
    wait();
}

bool CheckFirmwareUpgradeHandler::Check(const QString &model, const QString &region, QString &versionOnServer, QString &firmwareUrl)
{
    if(isRunning()){
        return false;
    }

    m_model = model.toLower();
    m_region = region.toLower();
    m_downloadUrl.clear();
    m_versionOnServer.clear();
    m_dataValid = false;

    start();

    //wait for complete
    wait();

    versionOnServer = m_versionOnServer;
    firmwareUrl = m_downloadUrl;

    return m_dataValid;
}


void CheckFirmwareUpgradeHandler::finished(QNetworkReply *reply)
{
    QString newVersion;
    QString imageLine;

    {
        QRegExp regFileMatch("(\\.*file\\.*=\\.*).*[v|V](\\d+(\\.\\d+)+).*\\.(chk|img|exe)");
        QRegExp regVersionMatch("\\d+(\\.\\d+){3}");

        QTextStream txtStream(reply);
        QString line = txtStream.readLine();

        while(!line.isNull()){
            line = txtStream.readLine();
            if(regFileMatch.indexIn(line) != -1){
                imageLine = regFileMatch.capturedTexts()[0];
                if(regVersionMatch.indexIn(line) != -1){
                    newVersion = regVersionMatch.capturedTexts()[0];
                }

                break;
            }
        }
    }

    reply->deleteLater();


    QRegExp urlRx(QString("^(/%1/.*/).*$").arg(m_model));

    int equsignIdx = imageLine.indexOf("=");


    if(equsignIdx == -1 || imageLine.length() <= (equsignIdx + 1) || newVersion.isEmpty()){
        goto lexit;
    }

    imageLine = imageLine.mid(equsignIdx + 1);
    imageLine = imageLine.trimmed();
    if(imageLine.isEmpty()){
        goto lexit;
    }


    if(urlRx.exactMatch(imageLine)){
        m_downloadUrl = QString(UPDATE_SERVER_URL) + imageLine;
    }else{
        m_downloadUrl = QString(UPDATE_SERVER_URL) + "/" + m_model + "/" + m_region + "/" +imageLine;
    }

    m_versionOnServer = newVersion;
    m_dataValid = true;

    //qDebug () << "Download URL = " << m_downloadUrl;

lexit:
    quit();

}

void CheckFirmwareUpgradeHandler::run()
{
    QNetworkAccessManager nam;
    connect(&nam, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*)),Qt::DirectConnection);

    QNetworkRequest req(QUrl(QString(UPDATE_URL_TEMPLATE).arg(m_model, m_region)));
    nam.get(req);

    exec();
}

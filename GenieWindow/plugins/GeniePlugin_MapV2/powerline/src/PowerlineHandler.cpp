#include "PowerlineHandler.h"
#include <QDir>
#include <QtDebug>
#include "plcappengine.h"
#include <QCoreApplication>
#include "PowerlineDefine.h"




PowerlineHandler::PowerlineHandler(QObject *parent) :
    QObject(parent)
{


    m_appEngine = new PLCAppEngine(this);
    Q_ASSERT(m_appEngine);
    if(!init())
    {
        if(m_appEngine != NULL)
        {
			m_appEngine->Cleanup();
            delete m_appEngine;
            m_appEngine = NULL;
        }
        qDebug() << "Init Appengine failed!!!";
        return;
    }

    //m_appEngine->T_SetDeviceName("00-1F-84-00-00-06","newName");
}

PowerlineHandler::~PowerlineHandler()
{
    if(m_appEngine != NULL)
    {
		m_appEngine->Cleanup();
        delete m_appEngine;
        m_appEngine = NULL;
    }

	qDebug() << "PowerlineHandler exit";
}
//===============================================
//  initialization funtion
//===============================================
bool PowerlineHandler::init()
{
    connect(m_appEngine, SIGNAL(TransactionFinished(TransactionPointer)), this, SLOT(OnTransactionFinished(TransactionPointer)));
    connect(m_appEngine, SIGNAL(PLCDeviceChangeScanned(TopologyDataTransfer)), this, SLOT(OnPLCDeviceChangeScanned(TopologyDataTransfer)));

    bool bRet = m_appEngine->Startup(PLC_MAINPIPE_PLUGIN_FILE, PLC_DRIVER_DIR);
    qDebug() << "PLC App Engine Start up " << bRet;

    return bRet;
}
//===============================================
//  slot funtion
//===============================================
void PowerlineHandler::OnPLCDeviceChangeScanned(TopologyDataTransfer topData)
{
	topData.get()->dump();
    emit OnLCDScanned(topData);

}
void PowerlineHandler::OnTransactionFinished(TransactionPointer transaction)
{
    emit OnTransFinished(transaction);
}
//===============================================
//  public funtion
//===============================================
bool PowerlineHandler::backToFactory(const QString &strMac)
{
    return m_appEngine->T_DeviceResetDefault(strMac);
}
bool PowerlineHandler::turnLedOff(const QString &strMac, int nState)
{
    return m_appEngine->T_SetDeviceLED(strMac, nState);
}
bool PowerlineHandler::modifyDevName(const QString &strMac, const QString &name)
{
    return m_appEngine->T_SetDeviceName(strMac, name);
}
bool PowerlineHandler::applySecurity(const QString &targetDevMac, const QString &securityKey, const QString &devPwd)
{
    return m_appEngine->T_SetDeviceSecurityKey(targetDevMac, securityKey, devPwd);
}
bool PowerlineHandler::applySecurityAll(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds)
{
	return m_appEngine->T_BatchSetDevicesSecurityKey(targetDevMacs, securityKey, devPwds);
}
bool PowerlineHandler::setDeviceWLANInfo(const QString &targetDevIP , const WLANInfo &wlanInfo, bool autowep2wap)
{
    return m_appEngine->T_SetDeviceWLANInfo(targetDevIP, wlanInfo, autowep2wap);
}
bool PowerlineHandler::getDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber)
{
    return m_appEngine->T_GetDeviceWLANInfo(targetDevIP, tagNumber);
}

void PowerlineHandler::refreshSecurity()
{}
bool PowerlineHandler::refreshScanDev()
{
	return m_appEngine->ReScanPLCDevice();
}
bool PowerlineHandler::chkUpdate(const QString &strMac)
{
	return m_appEngine->T_CheckFirmwareUpgrade(strMac);
}

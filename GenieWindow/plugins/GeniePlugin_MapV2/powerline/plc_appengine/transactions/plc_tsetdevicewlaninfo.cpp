#include "plc_tsetdevicewlaninfo.h"
#include "mainpipeinterface.h"

PLC_TSetDeviceWLANInfo::PLC_TSetDeviceWLANInfo(const QString &targetDevIP, const WLANInfo &wlanInfo, bool autowep2wpapsk) :
    PLCTransaction(TC_SET_DEVICE_WLAN_INFO)
  ,m_autowep2wpapsk(autowep2wpapsk)
  ,m_targetDevIP(targetDevIP)
  ,m_wlanInfo(wlanInfo)
{
}

int PLC_TSetDeviceWLANInfo::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)

    WLANInfo wlanInfo = m_wlanInfo;

    if(m_autowep2wpapsk && !wlanInfo.m_basicEncryptionModes.compare("wep", Qt::CaseInsensitive) &&
            !wlanInfo.m_wlanPassphrase.isEmpty()){
        wlanInfo.m_wpaEncryptionModes = "WPA2-PSK";
    }

    m_transactionState = (SOAP_OK == mainPipeInf->SoapSetWLANInfo(m_targetDevIP, 80, wlanInfo)
                          ? TS_SUCCESS : TS_FAILED);

    return m_transactionState;
}

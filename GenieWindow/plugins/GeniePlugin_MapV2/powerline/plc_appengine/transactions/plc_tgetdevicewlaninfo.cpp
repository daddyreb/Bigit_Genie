#include "plc_tgetdevicewlaninfo.h"
#include "mainpipeinterface.h"


PLC_TGetDeviceWLANInfo::PLC_TGetDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber) :
    PLCTransaction(TC_GET_DEVICE_WLAN_INFO)
  ,m_targetDevIP(targetDevIP)
  ,m_tagNumber(tagNumber)
{
}

int PLC_TGetDeviceWLANInfo::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)

    m_transactionState = (SOAP_OK == mainPipeInf->SoapGetWLANInfo(m_targetDevIP, 80, m_wlanInfo)
                          ? TS_SUCCESS : TS_FAILED);

    return m_transactionState;
}


bool PLC_TGetDeviceWLANInfo::GetWLANInfo(WLANInfo &wlanInfo) const
{
    if(TS_SUCCESS == m_transactionState){
        wlanInfo = m_wlanInfo;
    }

    return (TS_SUCCESS == m_transactionState);
}

unsigned long PLC_TGetDeviceWLANInfo::GetTagNumber() const
{
    return m_tagNumber;
}

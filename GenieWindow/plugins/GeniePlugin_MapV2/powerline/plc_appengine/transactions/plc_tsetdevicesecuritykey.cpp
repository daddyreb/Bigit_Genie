#include "plc_tsetdevicesecuritykey.h"
#include "mainpipeinterface.h"

PLC_TSetDeviceSecurityKey::PLC_TSetDeviceSecurityKey(const QString &targetDevMac, const QString &securityKey, const QString &devPwd) :
    PLCTransaction(TC_SET_DEVICE_SECURITY_KEY)
  ,m_targetDevMac(targetDevMac)
  ,m_securityKey(securityKey)
  ,m_devicePassword(devPwd)
{
}

int PLC_TSetDeviceSecurityKey::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)

    m_transactionState = (mainPipeInf->PLCSetDeviceSecurityKey(m_targetDevMac, m_securityKey, m_devicePassword)
                          ? TS_SUCCESS : TS_FAILED);

    return m_transactionState;
}

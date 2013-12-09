#include "plc_tdeviceresetdefault.h"
#include "mainpipeinterface.h"

PLC_TDeviceResetDefault::PLC_TDeviceResetDefault(const QString &targetDevMac) :
    PLCTransaction(TC_DEVICE_RESET_DEFAULT)
  ,m_targetDevMac(targetDevMac)
{
}

int PLC_TDeviceResetDefault::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)

    m_transactionState = (mainPipeInf->PLCSetDeviceResetDefault(m_targetDevMac) ? TS_SUCCESS : TS_FAILED);
    return m_transactionState;
}

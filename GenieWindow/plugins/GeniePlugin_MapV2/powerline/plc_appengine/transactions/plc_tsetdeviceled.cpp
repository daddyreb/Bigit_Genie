#include "plc_tsetdeviceled.h"
#include "mainpipeinterface.h"

PLC_TSetDeviceLED::PLC_TSetDeviceLED(const QString &targetDevMac, int LEDState) :
    PLCTransaction(TC_SET_DEVICE_LED)
  ,m_targetDevMac(targetDevMac)
  ,m_newLEDState(LEDState)
{
}

int PLC_TSetDeviceLED::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)
    m_transactionState = (mainPipeInf->PLCSetDeviceLedStatus(m_targetDevMac, m_newLEDState) ? TS_SUCCESS : TS_FAILED);
    qDebug () << "PLC_TSetDeviceLED::Execute state = " << m_transactionState;
    return m_transactionState;
}

#include "plc_tsetdevicename.h"
#include "mainpipeinterface.h"

#include <QtDebug>

PLC_TSetDeviceName::PLC_TSetDeviceName(const QString &targeDevMac, const QString &newName) :
    PLCTransaction(TC_SET_DEVICE_NAME)
  ,m_targetDevMac(targeDevMac)
  ,m_devNewName(newName)
{
}

int PLC_TSetDeviceName::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(infoInf)

    m_transactionState = (mainPipeInf->PLCSetDeviceName(m_targetDevMac, m_devNewName) ? TS_SUCCESS : TS_FAILED);
    qDebug () << "Set Deivce " << m_targetDevMac << " name " << (m_transactionState == TS_SUCCESS ?
                                                                     "sucess" : "failed");
    return m_transactionState;
}

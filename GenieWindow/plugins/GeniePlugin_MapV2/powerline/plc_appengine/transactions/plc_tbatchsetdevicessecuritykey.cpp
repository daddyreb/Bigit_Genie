#include "plc_tbatchsetdevicessecuritykey.h"
#include "mainpipeinterface.h"
#include "plcdeviceinfointerface.h"
#include <QThread>

class MiniSleep : public QThread
{
public:
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
};

PLC_TBatchSetDevicesSecurityKey::PLC_TBatchSetDevicesSecurityKey(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds) :
    PLCTransaction(TC_BATCH_SET_DEVICE_SECURITY_KEY)
  ,m_targetDevMacs(targetDevMacs)
  ,m_securityKey(securityKey)
  ,m_devicePasswords(devPwds)
{
}

int PLC_TBatchSetDevicesSecurityKey::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    m_SuccessDevDetails.clear();
    m_PasswordOkDevDetails.clear();

    QString loclDevMac = infoInf->CurrentLocalDevice();

    const int targetCount = m_targetDevMacs.size();
	const int localIdx = m_targetDevMacs.indexOf(loclDevMac);

    if(localIdx != -1){
        //local device should be set at last
        m_targetDevMacs.swap(localIdx, targetCount - 1);
		m_devicePasswords.swap(localIdx, targetCount - 1);
    }

    //verify DAK code
    bool verifyAllOk = true;
    int verifyCount = (localIdx != -1 ? targetCount - 1 : targetCount);
    for(int index = 0 ; index < verifyCount; ++index){
        if(mainPipeInf->PLCCheckDevicePassword(m_targetDevMacs[index], m_devicePasswords[index])){
            m_PasswordOkDevDetails[m_targetDevMacs[index]] = m_devicePasswords[index];
        }else{
            verifyAllOk = false;
        }
    }

    if(!verifyAllOk){
        return (m_transactionState = TS_FAILED);
    }

    //sleep 10 seconds after verify DAK code
    MiniSleep::msleep(10000);

    if(targetCount == 0 || targetCount != m_devicePasswords.size()){
        m_transactionState = TS_FAILED;
    }else{
        for(int idx = 0 ; idx < targetCount ; ++idx){
            if(mainPipeInf->PLCSetDeviceSecurityKey(m_targetDevMacs[idx], m_securityKey, m_devicePasswords[idx])){
                m_SuccessDevDetails[m_targetDevMacs[idx]] = m_devicePasswords[idx];
            }
        }

        m_transactionState = (m_SuccessDevDetails.size() == targetCount ? TS_SUCCESS : TS_FAILED);
    }


    return m_transactionState;
}

QMap<QString, QString> PLC_TBatchSetDevicesSecurityKey::GetSuccessDeviceDetails() const
{
    return m_SuccessDevDetails;
}

QMap<QString, QString> PLC_TBatchSetDevicesSecurityKey::GetPasswordOkDeviceDetails() const
{
    return m_PasswordOkDevDetails;
}

QStringList PLC_TBatchSetDevicesSecurityKey::GetTargetDeviceMacs() const
{
    return m_targetDevMacs;
}

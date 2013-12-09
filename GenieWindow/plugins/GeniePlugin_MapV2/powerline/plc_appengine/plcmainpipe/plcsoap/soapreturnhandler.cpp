#include "soapreturnhandler.h"
#include "mainpipedefines.h"

SoapReturnHandler::SoapReturnHandler(QObject *parent) :
    QObject(parent)
  ,m_returnCode(SOAP_FAILED)
  ,m_resultMap(0)
  ,m_returnedFlag(false)
{
}

void SoapReturnHandler::OnSoapReturn(int soapCode, const QMap<QString, QString> *resultMap)
{
    m_returnMutex.lock();

    m_returnedFlag = true;
    m_returnCode = soapCode;
    m_resultMap = resultMap;

    m_returnedCdt.wakeAll();
    m_returnMutex.unlock();
}

int SoapReturnHandler::WaitForSoapReturn(const QMap<QString, QString> **resultMap)
{
    m_returnMutex.lock();

    while(!m_returnedFlag){
        m_returnedCdt.wait(&m_returnMutex);
    }
    *resultMap = m_resultMap;
    m_returnedFlag = false;//reset for using next time

    m_returnMutex.unlock();

    return m_returnCode;
}

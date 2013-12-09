#ifndef QGENIEDIAGNOSECOMMON_H
#define QGENIEDIAGNOSECOMMON_H
#include <QString>

typedef struct InfTestResult
{
    bool							m_bDisabled;//网卡是否禁用，禁用为TRUE，启用为FALSE
    int							m_iMediaType;//1 有线网卡,2无线网卡,5 1394NIC
    QString						m_strDevDesc;//网卡设备描述
    QString						m_strDevGuid;//网卡设备GUID
    int							m_iEnabledDhcp;//启用了DHCP为1，禁用为0,-1未知
    int							m_iGatewayok;//是否有可用的网关,有为1,没有为0,未知为-1,2网关不通外网
    int							m_iDnsserverok;//是否有可用的DNS,有为1,没有为0,未知为-1
    int							m_IsWirelessRadioOff;//0打开，1硬关闭，2软关闭，-1未知状态

    InfTestResult()
    {
        m_bDisabled                               =  false;
        m_iMediaType                            =   1;
        m_iEnabledDhcp                         =  -1;
        m_iGatewayok                             =  -1;
        m_iDnsserverok                          =  -1;
        m_IsWirelessRadioOff                =  -1;
    }

}INFTESTRESULT,*PINFTESTRESULT;
#endif // QGENIEDIAGNOSECOMMON_H

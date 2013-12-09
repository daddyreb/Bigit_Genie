#ifndef QGENIEDIAGNOSECOMMON_H
#define QGENIEDIAGNOSECOMMON_H
#include <QString>

typedef struct InfTestResult
{
    bool							m_bDisabled;//�����Ƿ���ã�����ΪTRUE������ΪFALSE
    int							m_iMediaType;//1 ��������,2��������,5 1394NIC
    QString						m_strDevDesc;//�����豸����
    QString						m_strDevGuid;//�����豸GUID
    int							m_iEnabledDhcp;//������DHCPΪ1������Ϊ0,-1δ֪
    int							m_iGatewayok;//�Ƿ��п��õ�����,��Ϊ1,û��Ϊ0,δ֪Ϊ-1,2���ز�ͨ����
    int							m_iDnsserverok;//�Ƿ��п��õ�DNS,��Ϊ1,û��Ϊ0,δ֪Ϊ-1
    int							m_IsWirelessRadioOff;//0�򿪣�1Ӳ�رգ�2��رգ�-1δ֪״̬

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

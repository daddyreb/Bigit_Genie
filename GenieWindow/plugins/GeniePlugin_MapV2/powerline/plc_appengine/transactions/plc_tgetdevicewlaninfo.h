#ifndef PLC_TGETDEVICEWLANINFO_H
#define PLC_TGETDEVICEWLANINFO_H
#include "plctransaction.h"
#include "mainpipedefines.h"

class PLC_TGetDeviceWLANInfo : public PLCTransaction
{
public:
    PLC_TGetDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

    bool GetWLANInfo(WLANInfo &wlanInfo) const;
    unsigned long GetTagNumber() const;

private:
    const QString m_targetDevIP;
    WLANInfo m_wlanInfo;
    unsigned long m_tagNumber;
};

#endif // PLC_TGETDEVICEWLANINFO_H

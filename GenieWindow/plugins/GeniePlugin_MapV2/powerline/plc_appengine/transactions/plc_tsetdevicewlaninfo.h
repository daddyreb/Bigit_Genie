#ifndef PLC_TSETDEVICEWLANINFO_H
#define PLC_TSETDEVICEWLANINFO_H

#include "plctransaction.h"
#include "mainpipedefines.h"

class PLC_TSetDeviceWLANInfo : public PLCTransaction
{
public:
    PLC_TSetDeviceWLANInfo(const QString &targetDevIP, const WLANInfo &wlanInfo, bool autowep2wpapsk = true);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

private:
    bool m_autowep2wpapsk;
    const QString m_targetDevIP;
    const WLANInfo m_wlanInfo;
};

#endif // PLC_TSETDEVICEWLANINFO_H

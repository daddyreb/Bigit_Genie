#include "plc_tresynchscan.h"
#include "plcdeviceinfointerface.h"

PLC_TReSynchScan::PLC_TReSynchScan() :
    PLCTransaction(PLCTransaction::TC_RE_SYNCH_SCAN)
{
    m_needEmitSignal = false;
}

int PLC_TReSynchScan::Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    Q_UNUSED(mainPipeInf);

    return (infoInf->ReSynchScan() ? TS_SUCCESS : TS_FAILED);
}

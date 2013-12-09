#ifndef PLC_TRESYNCHSCAN_H
#define PLC_TRESYNCHSCAN_H

#include "plctransaction.h"
class MainPipeInterface;
class PLCDeviceInfoInterface;

class PLC_TReSynchScan : public PLCTransaction
{
public:
    PLC_TReSynchScan();

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);
};

#endif // PLC_TRESYNCHSCAN_H

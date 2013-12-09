#ifndef PLCTRANSACTION_H
#define PLCTRANSACTION_H

#include "pointertransfer.h"

class MainPipeInterface;
class PLCDeviceInfoInterface;

class PLCTransaction
{
public:
    enum TransactionState
    {
        TS_PENDING
        ,TS_SUCCESS
        ,TS_FAILED
    };

    enum TransactionCode
    {
        TC_SET_DEVICE_NAME
        ,TC_SET_DEVICE_LED
        ,TC_SET_DEVICE_SECURITY_KEY
        ,TC_BATCH_SET_DEVICE_SECURITY_KEY
        ,TC_SET_DEVICE_QOS
        ,TC_DEVICE_RESET_DEFAULT
        ,TC_SET_DEVICE_WLAN_INFO
        ,TC_MODIFY_CODE//just as a delimitor ,saparate modify and nonemodify transaction code
        ,TC_RE_SYNCH_SCAN
        ,TC_CHECK_FIRMWARE_UPGRADE
        ,TC_GET_DEVICE_WLAN_INFO
    };

public:
    explicit PLCTransaction(int tranCode):
        m_transactionCode(tranCode)
      ,m_transactionState(TS_PENDING)
      ,m_needEmitSignal(true){}

    virtual ~PLCTransaction(){}

public:
    virtual int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf) = 0;

    int Transaction() const {return m_transactionCode;}
    int State() const {return m_transactionState;}
    bool NeedEmitNotifySignal() const {return m_needEmitSignal;}


protected:
    int m_transactionCode;
    int m_transactionState;
    bool m_needEmitSignal;
};

typedef PointerTransfer<PLCTransaction> TransactionPointer;


#endif // PLCTRANSACTION_H

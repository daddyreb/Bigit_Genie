
#include <QHostAddress>
#include <QUdpSocket>
#include <QTime>
#include <QtEndian>
#include <QtDebug>

#include "nethelproutines.h"
#include "helperroutines.h"


#include <iphlpapi.h>
#include <stdlib.h>



#include <windows.h>

NetHelpRoutines::NetHelpRoutines()
{
}

bool NetHelpRoutines::GetPLCDeviceIP(const QString &devMac ,QString &devIP)
{
    bool bResult = false;

    if((bResult = SearchArpForIP(devMac, devIP)))
        return bResult;

    //upnp M search
    const QByteArray requestData("\x4D\x2D\x53\x45\x41\x52\x43\x48\x20\x2A\x20\x48\x54\x54\x50\x2F"
                                 "\x31\x2E\x31\x0D\x0A\x4D\x58\x3A\x20\x35\x0D\x0A\x53\x54\x3A\x20"
                                 "\x75\x70\x6E\x70\x3A\x72\x6F\x6F\x74\x64\x65\x76\x69\x63\x65\x0D"
                                 "\x0A\x4D\x41\x4E\x3A\x20\x22\x73\x73\x64\x70\x3A\x64\x69\x73\x63"
                                 "\x6F\x76\x65\x72\x22\x0D\x0A\x48\x6F\x73\x74\x3A\x20\x32\x33\x39"
                                 "\x2E\x32\x35\x35\x2E\x32\x35\x35\x2E\x32\x35\x30\x3A\x31\x39\x30"
                                 "\x30\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x4C\x65\x6E\x67\x74"
                                 "\x68\x3A\x20\x30\x0D\x0A\x0D\x0A");

    const QByteArray detectBytes("Detect Bytes");


    QUdpSocket detectSocket;
    if(!detectSocket.bind(5826,QUdpSocket::ShareAddress)){
        return bResult;
    }

    const QHostAddress targetAddr("239.255.255.250");
    detectSocket.writeDatagram(requestData, targetAddr, 1900);


    QHostAddress srcAddr;
    QTime tBegin = QTime::currentTime();

    while(detectSocket.waitForReadyRead(5000)){

        if(-1 == detectSocket.readDatagram(0, 0 ,&srcAddr ,0)){
            break;
        }

        detectSocket.writeDatagram(detectBytes, srcAddr, 2000);//IP datagram to force the arp entry to be generated

        int secsDelt = tBegin.secsTo(QTime::currentTime());
        if(secsDelt < 0 || secsDelt > 20){//max 20 seconds or time error occured
            break;
        }

        if((bResult = SearchArpForIP(devMac, devIP)))
            return bResult;
    }

    if(!bResult){
        bResult = SearchArpForIP(devMac, devIP);
    }

    return bResult;
}

bool NetHelpRoutines::SearchArpForIP(const QString &devMac ,QString &devIP)
{
    devIP.clear();

    bool bResult = false;

    if(PLC_MAC_ADDRESS_SIZE != (sizeof(quint64) - 2)){
        qDebug () << "MAC Address lengh = " << PLC_MAC_ADDRESS_SIZE << " NOT SUPPORTED!!!";
        return bResult;
    }

    ULONG ulSize = sizeof(MIB_IPNETTABLE);
    PMIB_IPNETTABLE pmibTable = (PMIB_IPNETTABLE)malloc(ulSize);
    if(0 == pmibTable){
        return bResult;
    }

    DWORD retCode = NO_ERROR;

    if((retCode = GetIpNetTable(pmibTable, &ulSize, FALSE)) == ERROR_INSUFFICIENT_BUFFER){
        free(pmibTable);
        pmibTable = (PMIB_IPNETTABLE)malloc(ulSize);
        if(0 == pmibTable){
            return bResult;
        }

        retCode = GetIpNetTable(pmibTable, &ulSize, FALSE);
    }

    if(NO_ERROR == retCode){

        BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE];
        BYTE DeviceMAC_PLUS_ONE[PLC_MAC_ADDRESS_SIZE];
        BYTE DeviceMAC_MINUS_ONE[PLC_MAC_ADDRESS_SIZE];

        PLCDeviceMacAddrFromString(devMac, DeviceMAC);

        Add2MacAddrValue(DeviceMAC, DeviceMAC_PLUS_ONE, 1);
        Add2MacAddrValue(DeviceMAC, DeviceMAC_MINUS_ONE, -1);

        for(DWORD idx = 0 ; idx < pmibTable->dwNumEntries ; ++idx){
            if(2 == pmibTable->table[idx].dwType){//invalid entry
                continue;
            }

            if(pmibTable->table[idx].dwPhysAddrLen != PLC_MAC_ADDRESS_SIZE){
                continue;
            }

            if((!memcmp(DeviceMAC, pmibTable->table[idx].bPhysAddr, PLC_MAC_ADDRESS_SIZE)) ||
                    (!memcmp(DeviceMAC_PLUS_ONE, pmibTable->table[idx].bPhysAddr, PLC_MAC_ADDRESS_SIZE)) ||
                    (!memcmp(DeviceMAC_MINUS_ONE, pmibTable->table[idx].bPhysAddr, PLC_MAC_ADDRESS_SIZE))){

                DWORD dwNetIP;
                qToBigEndian(pmibTable->table[idx].dwAddr, (uchar*)&dwNetIP);
                QHostAddress ipAddress(dwNetIP);
                devIP = ipAddress.toString();
                bResult = true;
                if(!devIP.contains("192.168.0.")){
                    break;
                }
            }

        }
    }

    free(pmibTable);
    pmibTable = 0;

    return bResult;
}

bool NetHelpRoutines::GetGatewayMacs(QStringList &macList)
{
    ULONG macAddr[2];       /* for 6-byte hardware addresses */
    ULONG physAddrLen = 6;  /* default to length of six bytes */
    unsigned char *bPhysAddr = (unsigned char *)macAddr;
    bool ret = false;

    QStringList gatewayIPs;
    if(!GetGatewayIPs(gatewayIPs)){
        return ret;
    }

    macList.clear();

    foreach(QString ip, gatewayIPs){
        physAddrLen = 6;
        QHostAddress destIP(ip);
        if((destIP.toIPv4Address() != 0) &&
                (SendARP(qToBigEndian(destIP.toIPv4Address()), 0, macAddr, &physAddrLen) == NO_ERROR) &&
                physAddrLen){

            QString mac;
            char macBuf[24];
            for (ULONG i = 0; i < physAddrLen; i++) {
                if (i != (physAddrLen - 1))
                    sprintf(macBuf, "%02X:", (int) bPhysAddr[i]);
                else
                    sprintf(macBuf, "%02X", (int) bPhysAddr[i]);

                mac += macBuf;
            }

            macList.append(mac);

            ret = true;
        }
    }

    return ret;
}

bool NetHelpRoutines::GetGatewayIPs(QStringList &ipList)
{
    PIP_ADAPTER_INFO pAdapterInfo       = NULL;
    PIP_ADAPTER_INFO pAdapter			= NULL;
    DWORD			 dwRetVal           = 0;
    bool			 bIsok				= false;
    ULONG			 ulOutBufLen		= 0;

    pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO));
    ulOutBufLen  = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
    }

    if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        ipList.clear();

        pAdapter = pAdapterInfo;
        while (pAdapter) {
            for (PIP_ADDR_STRING p = &(pAdapter->GatewayList); p != NULL; p = p->Next){
                if ((strlen(p->IpAddress.String) != 0) &&
                        strcmp(p->IpAddress.String, "0.0.0.0")){
                    ipList.append(QString::fromLatin1(p->IpAddress.String));
                }
            }

            pAdapter = pAdapter->Next;
        }//while

        bIsok = true;

    }//if

    free(pAdapterInfo);

    return bIsok;
}

/*****************************************************************
|
|   Neptune - Network
|
| Copyright (c) 2002-2008, Axiomatic Systems, LLC.
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|     * Redistributions of source code must retain the above copyright
|       notice, this list of conditions and the following disclaimer.
|     * Redistributions in binary form must reproduce the above copyright
|       notice, this list of conditions and the following disclaimer in the
|       documentation and/or other materials provided with the distribution.
|     * Neither the name of Axiomatic Systems nor the
|       names of its contributors may be used to endorse or promote products
|       derived from this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY AXIOMATIC SYSTEMS ''AS IS'' AND ANY
| EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
| WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL AXIOMATIC SYSTEMS BE LIABLE FOR ANY
| DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
| (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
| ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
| (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
 ****************************************************************/

#ifndef _SVT_NETWORK_H_
#define _SVT_NETWORK_H_
#include <QtNetwork>
#include <QtCore>
#include "SvtNetworkTool.h"

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/


/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const unsigned int SVT_NETWORK_MAX_MAC_ADDRESS_LENGTH  = 8;

/*----------------------------------------------------------------------
|   flags
+---------------------------------------------------------------------*/
#define SVT_NETWORK_INTERFACE_FLAG_LOOPBACK       0x01
#define SVT_NETWORK_INTERFACE_FLAG_PROMISCUOUS    0x02
#define SVT_NETWORK_INTERFACE_FLAG_BROADCAST      0x04
#define SVT_NETWORK_INTERFACE_FLAG_MULTICAST      0x08
#define SVT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT 0x10

/*----------------------------------------------------------------------
|   workarounds
+---------------------------------------------------------------------*/
#if defined(_WIN32)
#if defined(SetPort)
#undef SetPort
#endif
#endif

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
//typedef unsigned int NPT_IpPort;

/*----------------------------------------------------------------------
|   SVT_IpAddress
+---------------------------------------------------------------------*/
class SVT_NETWORKTOOL_EXPORT SVT_IpAddress
{
public:
    // class members
    static const SVT_IpAddress Any;

    // constructors and destructor
    SVT_IpAddress();
    SVT_IpAddress(unsigned long address);
    SVT_IpAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

    // methods
//    int       ResolveName(const char* name,
//                                 NPT_Timeout timeout = NPT_TIMEOUT_INFINITE);
    int       Parse(const char* name);
    int       Set(unsigned long address);
    int       Set(const unsigned char bytes[4]);
    int       Set(const QHostAddress &host);
    int       Set(const QString &addr);
    const unsigned char* asBytes() const;
    unsigned long    asLong() const;
    QString       toString() const;

    // operators
    bool             operator==(const SVT_IpAddress& other) const;

    bool isValid() const;
    static SVT_IpAddress fromDNS(const QString &addr);
    
private:
    // members
    unsigned char m_Address[4];
};

/*----------------------------------------------------------------------
|   SVT_MacAddress
+---------------------------------------------------------------------*/
class SVT_NETWORKTOOL_EXPORT SVT_MacAddress
{
public:
    // typedef enum
//    typedef enum {
//        TYPE_UNKNOWN,
//        TYPE_LOOPBACK,
//        TYPE_ETHERNET,
//        TYPE_PPP,
//        TYPE_IEEE_802_11
//    } Type;
    
    // constructors and destructor
    SVT_MacAddress();
    SVT_MacAddress(/*Type           type,*/
                   const unsigned char* addr, 
                   unsigned int   length);
    
    // methods
    void SetAddress(/*Type type, */const unsigned char* addr,
                                    unsigned int length);
    void SetAddress(const QString &addr);
//    Type                 GetType() const    { return m_Type; }
    const unsigned char* asBytes() const { return m_Address; }
    unsigned int         GetLength() const  { return m_Length; }
    QString           toString() const;
    bool isValid() const;
    bool operator==(const SVT_MacAddress &other) const;
    
private:
    // members
//    Type          m_Type;
    unsigned char m_Address[SVT_NETWORK_MAX_MAC_ADDRESS_LENGTH];
    unsigned int  m_Length;
};



/*----------------------------------------------------------------------
|   NPT_NetworkInterfaceAddress
+---------------------------------------------------------------------*/
class SVT_NETWORKTOOL_EXPORT SVT_NetworkInterface
{
public:
    enum SVT_NIFACE_FLAG
    {
        NF_WIRELESS=1<<0,
        NF_ON=1<<1,
        NF_DEFALT=1<<2,
        NF_ISVISUAL=1<<3
    };


    // constructors and destructor
    SVT_NetworkInterface();

    // methods
    const SVT_IpAddress& getIp() const ;
    const SVT_IpAddress& getMask() const;
    const SVT_IpAddress& getGateway() const ;
    const SVT_MacAddress& getMac() const ;

    bool isDefault() const;
    bool isWireless() const;
    bool isOn() const;
    bool isVisual() const;
    void set(const quint32 &idx,const QString name,const SVT_IpAddress &ip,const SVT_IpAddress &mask,const SVT_IpAddress &gateway,
             const SVT_MacAddress &mac,const quint32 &flag);
    SVT_NetworkInterface & operator =(const SVT_NetworkInterface &other);

    bool isValid() ;

public:
    // members
    SVT_IpAddress m_ip;
    SVT_IpAddress m_mask;
    SVT_IpAddress m_gateway;
    SVT_MacAddress m_mac;
    QString m_name;
    quint32 m_flags;
    quint32 m_idx;
};

/*----------------------------------------------------------------------
|   NPT_NetworkInterface
+---------------------------------------------------------------------*/
/* class NPT_NetworkInterface
{
public:
    // class methods
    static int GetNetworkInterfaces(NPT_List<NPT_NetworkInterface*>& interfaces);

    // constructors and destructor
    NPT_NetworkInterface(const char*           name,
                         const SVT_MacAddress& mac,
                         NPT_Flags             flags);
    NPT_NetworkInterface(const char*           name,
                         NPT_Flags             flags);
   ~NPT_NetworkInterface() {}

    // methods
    int AddAddress(const NPT_NetworkInterfaceAddress& address);
    const QString& GetName() const {
        return m_Name;
    }
    const SVT_MacAddress& GetMacAddress() const {
        return m_MacAddress;
    }
    void SetMacAddress(SVT_MacAddress::Type type,
                       const unsigned char* addr, 
                       unsigned int         length) {
        m_MacAddress.SetAddress(type, addr, length);
    }
    NPT_Flags GetFlags() const { return m_Flags; }
    const NPT_List<NPT_NetworkInterfaceAddress>& GetAddresses() const {
        return m_Addresses;
    }    
    
    bool IsAddressInNetwork(const SVT_IpAddress& address) {
        NPT_List<NPT_NetworkInterfaceAddress>::Iterator iter = m_Addresses.GetFirstItem();
        while (iter) {
            if ((*iter).IsAddressInNetwork(address)) return true;
           ++iter;
        }
        return false;
    }
    
private:
    // members
    QString                            m_Name;
    SVT_MacAddress                        m_MacAddress;
    NPT_Flags                             m_Flags;
    NPT_List<NPT_NetworkInterfaceAddress> m_Addresses;
};
 */

#endif // _NPT_NETWORK_H_

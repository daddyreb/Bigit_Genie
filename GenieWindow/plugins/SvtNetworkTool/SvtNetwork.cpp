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

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <QtCore>
#include "SvtNetwork.h"

/*----------------------------------------------------------------------
|   SVT_IpAddress::Any
+---------------------------------------------------------------------*/
const SVT_IpAddress SVT_IpAddress::Any;

/*----------------------------------------------------------------------
|   SVT_IpAddress::SVT_IpAddress
+---------------------------------------------------------------------*/
SVT_IpAddress::SVT_IpAddress()
{
    m_Address[0] = m_Address[1] = m_Address[2] = m_Address[3] = 0;
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::SVT_IpAddress
+---------------------------------------------------------------------*/
SVT_IpAddress::SVT_IpAddress(unsigned long address)
{
	
    Set(address);
}


/*----------------------------------------------------------------------
|   SVT_IpAddress::SVT_IpAddress
+---------------------------------------------------------------------*/
SVT_IpAddress::SVT_IpAddress(unsigned char a, 
                             unsigned char b, 
                             unsigned char c, 
                             unsigned char d)
{
    m_Address[0] = a;
    m_Address[1] = b;
    m_Address[2] = c;
    m_Address[3] = d;
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::Parse
+---------------------------------------------------------------------*/
int
SVT_IpAddress::Parse(const char* name)
{
    // check the name
    if (name == NULL) return -1;

    // clear the address
    m_Address[0] = m_Address[1] = m_Address[2] = m_Address[3] = 0;

    // parse
    unsigned int  fragment;
    bool          fragment_empty = true;
    unsigned char address[4];
    unsigned int  accumulator;
    for (fragment = 0, accumulator = 0; fragment < 4; ++name) {
        if (*name == '\0' || *name == '.') {
            // fragment terminator
            if (fragment_empty) return -1;
            address[fragment++] = accumulator;
            if (*name == '\0') break;
            accumulator = 0;
            fragment_empty = true;
        } else if (*name >= '0' && *name <= '9') {
            // numerical character
            accumulator = accumulator*10 + (*name - '0');
            if (accumulator > 255) return -1;
            fragment_empty = false; 
        } else {
            // invalid character
            return -1;
        }
    }

    if (fragment == 4 && *name == '\0' && !fragment_empty) {
        m_Address[0] = address[0];
        m_Address[1] = address[1];
        m_Address[2] = address[2];
        m_Address[3] = address[3];
        return 0;
    } else {
        return -1;
    }
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::AsLong
+---------------------------------------------------------------------*/
unsigned long
SVT_IpAddress::asLong() const
{
    return 
        (((unsigned long)m_Address[0])<<24) |
        (((unsigned long)m_Address[1])<<16) |
        (((unsigned long)m_Address[2])<< 8) |
        (((unsigned long)m_Address[3]));
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::AsBytes
+---------------------------------------------------------------------*/
const unsigned char* 
SVT_IpAddress::asBytes() const
{
    return m_Address;
}


bool SVT_IpAddress::isValid() const
{
    return *((unsigned long *)((char *)m_Address))!=0;
}

/* static */
SVT_IpAddress SVT_IpAddress::fromDNS(const QString &addr)
{
    QHostInfo hinfo = QHostInfo::fromName(addr);
    quint32 ip=0;
    if ( hinfo.error() == QHostInfo::NoError && !hinfo.addresses().isEmpty())
    {
		for(int i = 0; i < hinfo.addresses().length(); i++)
		{
			ip=hinfo.addresses()[i].toIPv4Address();
			if(ip != 0)
				break;
		}
    }
    SVT_IpAddress ret=SVT_IpAddress(ip);
    return ret;
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::ToString
+---------------------------------------------------------------------*/
QString
SVT_IpAddress::toString() const
{
    QString address;
    address += QString::number(m_Address[0]);
    address += '.';
    address += QString::number(m_Address[1]);
    address += '.';
    address += QString::number(m_Address[2]);
    address += '.';
    address += QString::number(m_Address[3]);

    return address;
}


/*----------------------------------------------------------------------
|   SVT_IpAddress::Set
+---------------------------------------------------------------------*/
int
SVT_IpAddress::Set(const unsigned char bytes[4])
{
    m_Address[0] = bytes[0];
    m_Address[1] = bytes[1];
    m_Address[2] = bytes[2];
    m_Address[3] = bytes[3];

    return 0;
}

int SVT_IpAddress::Set(const QHostAddress &host)
{
    return Set(host.toIPv4Address());
}

int SVT_IpAddress::Set(const QString &addr)
{
    m_Address[0] = m_Address[1] = m_Address[2] = m_Address[3] = 0;
    QStringList templst=addr.split(".");
    if(templst.count()>=4)
    {
        for(int i=0;i<4;i++)
        {
            m_Address[i]=templst[i].toInt()&0xff;
        }
    }
	return 0;
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::Set
+---------------------------------------------------------------------*/
int
SVT_IpAddress::Set(unsigned long address)
{
    m_Address[0] = (unsigned char)((address >> 24) & 0xFF);
    m_Address[1] = (unsigned char)((address >> 16) & 0xFF);
    m_Address[2] = (unsigned char)((address >>  8) & 0xFF);
    m_Address[3] = (unsigned char)((address      ) & 0xFF);

    return 0;
}

/*----------------------------------------------------------------------
|   SVT_IpAddress::operator==
+---------------------------------------------------------------------*/
bool
SVT_IpAddress::operator==(const SVT_IpAddress& other) const
{
    return other.asLong() == asLong();
}

/*----------------------------------------------------------------------
|   SVT_MacAddress::SVT_MacAddress
+---------------------------------------------------------------------*/

SVT_MacAddress::SVT_MacAddress()
    :m_Length(0)
{
    memset((char *)m_Address,'\0',sizeof(m_Address));
}

SVT_MacAddress::SVT_MacAddress(/*Type                  type,*/
                               const unsigned char*  address, 
                               unsigned int          length)
{
    SetAddress(/*type,*/ address, length);
}

/*----------------------------------------------------------------------
|   SVT_MacAddress::SetAddress
+---------------------------------------------------------------------*/
void
SVT_MacAddress::SetAddress(/*Type                 type,*/
                           const unsigned char* address, 
                           unsigned int         length)
{
    //m_Type = type;
    if (length > SVT_NETWORK_MAX_MAC_ADDRESS_LENGTH) {
        length = SVT_NETWORK_MAX_MAC_ADDRESS_LENGTH;
    }
    m_Length = length;
    for (unsigned int i=0; i<length; i++) {
        m_Address[i] = address[i];
    }
}

void SVT_MacAddress::SetAddress(const QString &addr)
{
    memset((char *)m_Address,'\0',sizeof(m_Address));
    QStringList templst=addr.split(":");
    if(templst.count() >=6)
    {
        for(int i=0;i<6;i++)
        {
            m_Address[i] = templst[i].toUInt(0,16)&0xff;
        }
        m_Length=6;
    }
}

/*----------------------------------------------------------------------
|   SVT_MacAddress::ToString
+---------------------------------------------------------------------*/
QString
SVT_MacAddress::toString() const
{
    QString result;
 
    if (m_Length) {
        char s[3*SVT_NETWORK_MAX_MAC_ADDRESS_LENGTH];
        const char hex[17] = "0123456789abcdef";
        for (unsigned int i=0; i<m_Length; i++) {
            s[i*3  ] = hex[m_Address[i]>>4];
            s[i*3+1] = hex[m_Address[i]&0xf];
            s[i*3+2] = ':';
        }
        s[3*m_Length-1] = '\0';
        result = s;
    }

    return result;
}

bool SVT_MacAddress::isValid() const
{
    return *((unsigned long *)(&m_Address[0]))!=0 || *((unsigned long *)(&m_Address[4]))!=0;
}

bool SVT_MacAddress::operator==(const SVT_MacAddress &other) const
{
    return 0==memcmp(m_Address,other.m_Address,6);
}



SVT_NetworkInterface::SVT_NetworkInterface()
    :m_flags(0)
{

}

// methods
const SVT_IpAddress& SVT_NetworkInterface::getIp() const {
    return m_ip;
}
const SVT_IpAddress& SVT_NetworkInterface::getMask() const {
    return m_mask;
}
const SVT_IpAddress& SVT_NetworkInterface::getGateway() const {
    return m_gateway;
}
const SVT_MacAddress& SVT_NetworkInterface::getMac() const {
    return m_mac;
}

bool SVT_NetworkInterface::isDefault() const
{
    return (NF_DEFALT&m_flags);
}

bool SVT_NetworkInterface::isWireless() const
{
    return (NF_WIRELESS&m_flags);
}

bool SVT_NetworkInterface::isOn() const
{
    return (NF_ON&m_flags);
}

bool SVT_NetworkInterface::isVisual() const
{
    return (NF_ISVISUAL&m_flags);
}



void SVT_NetworkInterface::set(const quint32 &idx,const QString name,const SVT_IpAddress &ip,const SVT_IpAddress &mask,const SVT_IpAddress &gateway,
         const SVT_MacAddress &mac,const quint32 &flag)
{
    m_idx=idx;
    m_ip=ip;
    m_name=name;
    m_mask=mask;
    m_gateway=gateway;
    m_mac=mac;
    m_flags=flag;
}

SVT_NetworkInterface & SVT_NetworkInterface::operator =(const SVT_NetworkInterface &other)
{
    m_idx=other.m_idx;
    m_ip=other.m_ip;
    m_name=other.m_name;
    m_mask=other.m_mask;
    m_gateway=other.m_gateway;
    m_mac=other.m_mac;
    m_flags=other.m_flags;
    return *this;
}

bool SVT_NetworkInterface::isValid()
{
    return m_ip.isValid() && m_mac.isValid();
}










/*----------------------------------------------------------------------
|   SVT_NetworkInterface::SVT_NetworkInterface
+---------------------------------------------------------------------*/ 
// SVT_NetworkInterface::SVT_NetworkInterface(const char*           name,
                                           // const SVT_MacAddress& mac,
                                           // NPT_Flags             flags) :
    // m_Name(name),
    // m_MacAddress(mac),
    // m_Flags(flags)
// {
// }

// /*----------------------------------------------------------------------
// |   SVT_NetworkInterface::SVT_NetworkInterface
// +---------------------------------------------------------------------*/ 
// SVT_NetworkInterface::SVT_NetworkInterface(const char* name,
                                           // NPT_Flags   flags) :
    // m_Name(name),
    // m_Flags(flags)
// {
// }

// /*----------------------------------------------------------------------
// |   SVT_NetworkInterface::AddAddress
// +---------------------------------------------------------------------*/ 
// int
// SVT_NetworkInterface::AddAddress(const SVT_NetworkInterfaceAddress& address)
// {
    // return m_Addresses.Add(address);
// }



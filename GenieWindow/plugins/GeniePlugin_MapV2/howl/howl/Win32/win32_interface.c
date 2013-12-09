/*
 * Copyright 2003, 2004 Porchdog Software. All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *
 *		1. Redistributions of source code must retain the above copyright notice,
 *		   this list of conditions and the following disclaimer.   
 *		2. Redistributions in binary form must reproduce the above copyright notice,
 *		   this list of conditions and the following disclaimer in the documentation
 *		   and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY PORCHDOG SOFTWARE ``AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE HOWL PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *	OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The views and conclusions contained in the software and documentation are those
 *	of the authors and should not be interpreted as representing official policies,
 *	either expressed or implied, of Porchdog Software.
 */

#include "win32_interface.h"
#include <salt/debug.h>
#include <iphlpapi.h>


static sw_uint32
index_to_mask(
				sw_uint32 index );


sw_result
sw_network_interfaces(
				sw_uint32					*	count,
				sw_network_interface	**	netifs)
{
	sw_win32_network_interface	netif;
	sw_win32_network_interface	head;
	ULONG								size;
	IP_ADAPTER_INFO			*	adapters;
	IP_ADAPTER_INFO			*	adapter;
	DWORD								res;
	int								i;
	sw_result						err = SW_OKAY;

	size	=	0;

	res	=	GetAdaptersInfo(NULL, &size);
	err	=	sw_translate_error(res == ERROR_BUFFER_OVERFLOW, res);
	sw_check_okay_log(err, exit);

	adapters = (IP_ADAPTER_INFO*) sw_malloc(size);
	err = sw_translate_error(adapters, SW_E_MEM);
	sw_check_okay_log(err, exit);
	
	res = GetAdaptersInfo(adapters, &size);
	err = sw_translate_error(res == ERROR_SUCCESS, res);
	sw_check_okay_log(err, exit);

	*count	=	0;
	netif		=	NULL;
	head		=	NULL;

	for (adapter = adapters; adapter; adapter = adapter->Next)
	{
		MIB_IFROW						mibInfo;
		sw_win32_network_interface	last;

		//if (adapter->Type != MIB_IF_TYPE_ETHERNET)
		//{
		//	continue;
		//}

		mibInfo.dwIndex = adapter->Index;

		GetIfEntry(&mibInfo);

		if ((mibInfo.dwOperStatus != MIB_IF_OPER_STATUS_OPERATIONAL) &&
		    (mibInfo.dwOperStatus != MIB_IF_OPER_STATUS_UNREACHABLE))
		{
			continue;
		}

		if (adapter->IpAddressList.IpAddress.String[0] == '0')
		{
			continue;
		}

		last = netif;

		netif = (sw_win32_network_interface) sw_malloc(sizeof(struct _sw_win32_network_interface));
		err = sw_translate_error(netif, SW_E_MEM);
		sw_check_okay_log(err, exit);

		strncpy((char*) netif->m_super.m_name, adapter->Description, 256);
		memcpy(netif->m_super.m_mac_address.m_id, adapter->Address, 6);
		sw_ipv4_address_init_from_name(&netif->m_super.m_ipv4_address, adapter->IpAddressList.IpAddress.String);
		sw_ipv4_address_init_from_saddr( &netif->m_super.m_netmask, index_to_mask( adapter->Index ) );
		netif->m_super.m_index	=	adapter->Index;
		netif->m_next				=	NULL;

		(*count)++;

		if (head == NULL)
		{
			head = netif;
		}

		if (last)
		{
			last->m_next = netif;
		}
	}

	sw_free(adapters);

	*netifs = (sw_network_interface*) sw_malloc(sizeof(sw_network_interface*) * (*count));
	err = sw_translate_error(*netifs, SW_E_MEM);
	sw_check_okay_log(err, exit);

	for (i = 0, netif = head; netif; i++, netif = netif->m_next)
	{
		(*netifs)[i] = &netif->m_super;
	}

exit:

	return err;
}


sw_result
sw_network_interfaces_fina(
					sw_uint32 					nifc,
					sw_network_interface * 	nifv)
{
	sw_uint32		i;
	sw_result	err;

	for (i = 0; i < nifc; i++)
	{
		err = sw_network_interface_fina(nifv[i]);
		sw_assert(err == SW_OKAY);
	}

	sw_free(nifv);

	return SW_OKAY;
}


sw_result
sw_network_interface_fina(
					sw_network_interface	nif)
{
	sw_win32_network_interface wnif = (sw_win32_network_interface) nif;

	sw_free(wnif);

	return SW_OKAY;
}


static  sw_uint32
index_to_mask( sw_uint32 index )
{
	PMIB_IPADDRTABLE  pIPAddrTable	= NULL;
	DWORD					dwSize			= 0;
	sw_result			err				= 0;
	sw_uint32			netmask			= 0;
	DWORD					i;

	while ( GetIpAddrTable( pIPAddrTable, &dwSize, 0 ) == ERROR_INSUFFICIENT_BUFFER )
	{
		pIPAddrTable = (MIB_IPADDRTABLE *) realloc( pIPAddrTable, dwSize );
		sw_check( pIPAddrTable, exit, err = WSAENOBUFS );
   }

	for ( i = 0; i < pIPAddrTable->dwNumEntries; i++ )
	{
		if ( pIPAddrTable->table[i].dwIndex == index )
		{
			netmask = pIPAddrTable->table[i].dwMask;
			break;
		}
	}

exit:

   if ( pIPAddrTable )
   {
      free( pIPAddrTable );
   }

	return netmask;
}

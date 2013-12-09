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

#include "interface_i.h"
#include <salt/debug.h>


sw_result
sw_network_interface_name(
				sw_network_interface		netif,
				sw_string					name,
				sw_uint32						len)
{
	sw_strncpy(name, (const char*) netif->m_name, len);
	return SW_OKAY;
}


sw_result
sw_network_interface_mac_address(
				sw_network_interface		netif,
				sw_mac_address			*	addr)
{
	sw_memcpy(addr->m_id, netif->m_mac_address.m_id, 6);
	return SW_OKAY;
}


sw_result
sw_network_interface_ipv4_address(
				sw_network_interface		netif,
				sw_ipv4_address		*	addr)
{
	return sw_ipv4_address_init_from_address(addr, netif->m_ipv4_address);
}


sw_result
sw_network_interface_netmask(
				sw_network_interface		netif,
				sw_ipv4_address		*	addr)
{
	return sw_ipv4_address_init_from_address( addr, netif->m_netmask );
}


sw_result
sw_network_interface_index(
				sw_network_interface		netif,
				sw_uint32					*	index)
{
	*index = netif->m_index;
	return SW_OKAY;
}


sw_result
sw_network_interface_linked(
				sw_network_interface		netif,
				sw_bool					*	linked)
{
	*linked = netif->m_linked;
	return SW_OKAY;
}

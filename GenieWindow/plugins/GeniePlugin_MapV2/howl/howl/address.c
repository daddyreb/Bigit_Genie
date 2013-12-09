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

#include "socket_i.h"
#include "salt_i.h"
#include <salt/debug.h>
#include <stdio.h>

#if defined(__sun)

#	define INADDR_NONE	-1

#endif


#if defined(__VXWORKS__)

extern const char * sysEnetNameGet();

#endif


sw_ipv4_address
sw_ipv4_address_any()
{
	sw_ipv4_address any;
	sw_ipv4_address_init(&any);
	return any;
}


sw_ipv4_address
sw_ipv4_address_loopback()
{
	sw_ipv4_address loopback;
	sw_ipv4_address_init_from_name(&loopback, "127.0.0.1");
	return loopback;
}


sw_result
sw_ipv4_address_init(
				sw_ipv4_address	*	self)
{
	self->m_addr = htonl(INADDR_ANY);
	return SW_OKAY;
}


sw_result
sw_ipv4_address_init_from_saddr(
				sw_ipv4_address	*	self,
				sw_saddr			addr)
{
	self->m_addr = addr;
	return SW_OKAY;
}


sw_result
sw_ipv4_address_init_from_name(
				sw_ipv4_address	*	self,
				sw_const_string		addr)
{
	sw_result err = SW_OKAY;

	//
	// check to see whether this is dotted decimal or not
	//
	int part[4];
	int ret;

	ret = sscanf(addr, "%d.%d.%d.%d", &part[0], &part[1], &part[2], &part[3]);

	if (ret == 4)
	{
		self->m_addr = inet_addr(addr);
	}
	else
	{
		struct hostent * hEntry = gethostbyname(addr);
		sw_check(hEntry != NULL, exit, err = SW_E_UNKNOWN);

		self->m_addr = *(sw_uint32*) hEntry->h_addr;
	}

	sw_check(self->m_addr != INADDR_NONE, exit, err = SW_E_UNKNOWN);

exit:

	return err;
}


sw_result
sw_ipv4_address_init_from_address(
				sw_ipv4_address	*	self,
				sw_ipv4_address		addr)
{
	self->m_addr = addr.m_addr;
	return SW_OKAY;
}


sw_result
sw_ipv4_address_init_from_this_host(
				sw_ipv4_address	*	self)
{
#if defined(__VXWORKS__)
		
	sw_int8 current_addr[INET_ADDR_LEN];      
		
	if (ifAddrGet(sysEnetNameGet(), current_addr) != OK)
	{
		sw_debug(SW_LOG_ERROR, "sw_ipv4_address_init_from_this_host", "ifAddrGet() failed\n");
		return SW_E_INIT;
	}
      
	self->m_addr = inet_addr(current_addr);

#else
		
	struct sockaddr_in	addr;
	sw_result				err;
	sw_sockdesc_t			desc;
	sw_socklen_t			len;

	desc = socket(AF_INET, SOCK_DGRAM, 0);
	sw_check(desc != SW_INVALID_SOCKET, exit, err = SW_E_UNKNOWN);

	sw_memset(&addr, 0, sizeof(addr));
	addr.sin_family		=	AF_INET;
	addr.sin_addr.s_addr =	inet_addr("192.168.1.1");
	addr.sin_port			=	htons(5555);

	err = connect(desc, (struct sockaddr*) &addr, sizeof(addr));
	sw_check_okay(err, exit);
	
	sw_memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);
	err = getsockname(desc, (struct sockaddr*) &addr, &len);
	sw_check_okay(err, exit);

	self->m_addr = addr.sin_addr.s_addr;

#endif

exit:

	if (desc != SW_INVALID_SOCKET)
	{
		sw_close_socket(desc);
	}

	if (err != SW_OKAY)
	{
		err = sw_ipv4_address_init_from_address(self, sw_ipv4_address_loopback());
	}

	return err;
}


sw_result
sw_ipv4_address_fina(
				sw_ipv4_address	self)
{
	SW_UNUSED_PARAM(self);

	return SW_OKAY;
}


sw_bool
sw_ipv4_address_is_any(
				sw_ipv4_address	self)
{
	return (sw_bool) (self.m_addr == htonl(INADDR_ANY));
}
				

sw_saddr
sw_ipv4_address_saddr(
				sw_ipv4_address self)
{
	return self.m_addr;
}


sw_string
sw_ipv4_address_name(
				sw_ipv4_address	self,
				sw_string	name,
				sw_uint32		len)
{
	struct in_addr addr;
	addr.s_addr = self.m_addr;
	sw_strncpy(name, inet_ntoa(addr), len);
	return name;
}


sw_result
sw_ipv4_address_decompose(
				sw_ipv4_address		self,
				sw_uint8		*	a1,
				sw_uint8		*	a2,
				sw_uint8		*	a3,
				sw_uint8		*	a4)
{
	SW_UNUSED_PARAM(self);
	SW_UNUSED_PARAM(a1);
	SW_UNUSED_PARAM(a2);
	SW_UNUSED_PARAM(a3);
	SW_UNUSED_PARAM(a4);

	return SW_E_UNKNOWN;
}


sw_bool
sw_ipv4_address_equals(
				sw_ipv4_address	self,
				sw_ipv4_address	addr)
{
	return (sw_bool) (self.m_addr == addr.m_addr);
}

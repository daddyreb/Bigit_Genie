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

/*
 * Copyright (c) 2003, 2004 Porchdog Software. All rights reserved.
 */
/*
    Change History (most recent first):
    
*/


#ifndef	__MDNS_PLATFORM_H
#define	__MDNS_PLATFORM_H

#include "mDNSClientAPI.h"
#include	<salt/salt.h>
#include <salt/socket.h>
#include <salt/address.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#ifdef	__cplusplus
extern "C"
{
#endif

#define SW_IFNAMSIZ	20

/*
 * PlatformNetworkInterface is a record extension of the core NetworkInterfaceInfo
 * type that supports extra fields needed by salt
 *
 * IMPORTANT: m_super must be the first field in the structure because
 * we cast between pointers to the two different types regularly.
 */
struct														PlatformNetworkInterface_struct;
typedef struct PlatformNetworkInterface_struct	PlatformNetworkInterface;


struct PlatformNetworkInterface_struct
{
	NetworkInterfaceInfo	m_super;
	sw_socket				m_multicast_socket;
	sw_int8					m_name[256];
};


struct mDNS_PlatformSupport_struct
{
	sw_salt			m_salt;
	sw_string	*	m_filters;
	sw_uint32			m_num_filters;
	sw_bool			m_running;
};


mDNSexport mStatus
mDNSPlatformRun(
						mDNS	*	const m);


mDNSexport mStatus
mDNSPlatformStopRun(
						mDNS	*	const m);


mDNSexport mStatus
PlatformConvertResultToStatus(
						sw_result							result);


sw_result
refresh_interface_list(
						mDNS	*	const m);


#ifdef	__cplusplus
}
#endif


#endif

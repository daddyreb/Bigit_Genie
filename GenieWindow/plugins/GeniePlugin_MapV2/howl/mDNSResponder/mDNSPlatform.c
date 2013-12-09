/*
 * Copyright (c) 2003, 2004 Porchdog Software. All rights reserved.
 */
/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):

*/


#include "mDNSClientAPI.h"
#include "mDNSPlatform.h"
#include "DNSServices.h"
#include <salt/salt.h>
#include <salt/interface.h>
#include <salt/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


/*
 *	the following functions can be called for platform specific code
 */
static sw_result
platform_setup_names(
						mDNS						*	const m,
						sw_const_string					friendly_name,
						sw_const_string					scary_name);
						

static sw_result
platform_setup_sockets(
						mDNS						*	const	m,
						PlatformNetworkInterface	*	intf);


static sw_result
platform_fina_socket(
						mDNS						*	const m,
						PlatformNetworkInterface	*	intf);



static sw_result
setup_interface_list(
						mDNS	*	const	m);


static sw_result
free_interface_list(
						mDNS	*	const m);


static sw_result
setup_interface(
						mDNS						*	const	m,
						sw_network_interface				netif);



static sw_result
free_interface(
						mDNS												*	const	m,
						struct PlatformNetworkInterface_struct	*			intf);

								
static void
get_user_specified_friendly_computer_name(
								sw_const_string		name,
								domainlabel	*	const	namelabel);


static void
get_user_specified_rfc1034_computer_name(
								sw_const_string		name,
								domainlabel	*	const	namelabel);



static sw_result
network_interface_event_handler(
								sw_network_interface_handler	handler,
								sw_salt								salt,
								sw_network_interface				netif,
								sw_opaque							extra);



static sw_result
socket_event_handler(
								sw_socket_handler		handler,
								sw_salt					salt,
								sw_socket				socket,
								sw_socket_event		events,
								sw_opaque				extra);



int													gMDNSPlatformVerboseLevel;
static struct mDNS_PlatformSupport_struct	gMDNSPlatformSupport;
 

#define ConvertSWTime(X) (((X).m_secs << 10) | ((X).m_usecs * 16 / 15625))


mDNSexport mDNS_PlatformSupport*
mDNSPlatformSupport()
{
	sw_memset(&gMDNSPlatformSupport, 0, sizeof(gMDNSPlatformSupport));
	return &gMDNSPlatformSupport;
}


/*
 * mDNS core calls this routine to initialise the platform-
 * specific data.
 */
mDNSexport mStatus
mDNSPlatformInit(
				mDNS	*	const	m)
{
	char			scary_name[64];
	sw_result	err = SW_OKAY;
    
	sw_assert(m != NULL);

	/*
	 * Tell mDNS core the names of this machine.
	 */
	gethostname(scary_name, 64);
	err = platform_setup_names(m, "Computer", scary_name);
	sw_check_okay(err, exit);

	/*
	 * Tell mDNS core about the network interfaces on this machine.
	 */
	err = setup_interface_list(m);
	sw_check_okay(err, exit);

	/*
	 * hook up salt event handling
	 */
	err = sw_salt_register_network_interface(gMDNSPlatformSupport.m_salt, NULL, m, network_interface_event_handler, NULL);
	sw_check_okay(err, exit);

	/*
	 * We don't do asynchronous initialization using salt,
	 * so by the time we get here the setup will already have
	 * succeeded or failed.  If it succeeded, 
	 * we should just call mDNSCoreInitComplete() immediately.
	 */
	mDNSCoreInitComplete(m, mStatus_NoError);
	
exit:

	return PlatformConvertResultToStatus(err);
}


/*
 * mDNS core calls this routine to clean up the platform-specific
 * data.  In our case all we need to do is to tear down every
 * network interface.
 */
mDNSexport void
mDNSPlatformClose(
				mDNS *const m)
{
	sw_assert(m != NULL);

	free_interface_list(m);
}


mDNSexport mDNSs32
mDNSPlatformTimeNow()
{
	sw_time now;

	sw_time_init_now(&now);

	return (ConvertSWTime(now));
}


/*
 * mDNS core calls this routine when it needs to send a packet.
 */
mDNSexport mStatus
mDNSPlatformSendUDP(
				const mDNS			*	const m,
				const DNSMessage	*	const msg,
				const mDNSu8		*	const end,
				mDNSInterfaceID		InterfaceID,
				mDNSIPPort				srcPort,
				const mDNSAddr		*	dst,
				mDNSIPPort				dstPort)
{
	PlatformNetworkInterface	*	thisIntf;
	sw_bool								found;
	sw_result							result;
	sw_ipv4_address					addr;

	SW_UNUSED_PARAM(srcPort);

	/*
	 * not handling IPv6 right now
	 */
	if (dst->type == mDNSAddrType_IPv6)
	{
		return 0;
	}

	sw_assert(m != NULL);
	sw_assert(msg != NULL);
	sw_assert(end != NULL);
	sw_assert((((char *) end) - ((char *) msg)) > 0);

	/*
	 * initialize destination address
	 */
	sw_ipv4_address_init_from_saddr(&addr, dst->ip.v4.NotAnInteger);

	/*
	 * Nor from a zero source port
	 */
	sw_assert(dstPort.NotAnInteger != 0);
	
	/*
	 * Loop through all the interfaces looking for one whose address
	 * matches the source address, and send on that.
	 */
	thisIntf = 	(PlatformNetworkInterface *) (m->HostInterfaces);
	result	=	SW_OKAY;
	found 	= 	SW_FALSE;
	while (thisIntf && !found)
	{
		if (thisIntf->m_super.InterfaceID == InterfaceID)
		{
			sw_size_t bytes_written;

			result = sw_socket_sendto(thisIntf->m_multicast_socket, (sw_octets) msg, (char*) end - (char*) msg, &bytes_written, addr, ntohs(dstPort.NotAnInteger));

			if (result != SW_OKAY)
			{
				sw_int8 host[16];

				sw_debug(SW_LOG_ERROR, "error sending packet to %s\n", sw_ipv4_address_name(addr, host, 16));
			}
			
			found = SW_TRUE;
		}
		
		thisIntf = (PlatformNetworkInterface*) (thisIntf->m_super.next);
	}
	
	return PlatformConvertResultToStatus(result);
}


mDNSexport void
mDNSPlatformLock(
					const mDNS *const m) 
{
	SW_UNUSED_PARAM(m);

	sw_salt_lock(gMDNSPlatformSupport.m_salt);
}


/*
 * salt is single threaded, so locking calls are no-ops
 */
mDNSexport void
mDNSPlatformUnlock(
					const mDNS *const m) 
{ 
	SW_UNUSED_PARAM(m);

	sw_salt_unlock(gMDNSPlatformSupport.m_salt);
}


mDNSexport void
mDNSPlatformStrCopy(
					const void	*	src,
					void			*	dst)
{
	sw_strcpy(dst, src);
}


mDNSexport mDNSu32
mDNSPlatformStrLen(
					const void	*	src)
{ 
	return (mDNSu32) sw_strlen(src);
}


mDNSexport void
mDNSPlatformMemCopy(
					const void	*	src,
					void			*	dst,
					mDNSu32			len) 
{ 
	sw_memcpy(dst, src, len); 
}


mDNSexport mDNSBool
mDNSPlatformMemSame(
					const void	*	src,
					const void	*	dst,
					mDNSu32			len)
{ 
	return sw_memcmp(dst, src, len) == 0; 
}


mDNSexport void
mDNSPlatformMemZero(
					void	*	dst,
					mDNSu32	len) 
{ 
	sw_memset(dst, 0, len); 
}


mDNSexport void*
mDNSPlatformMemAllocate(
					mDNSu32 len)
{
	return (void*) malloc(len);
}


void
mDNSPlatformMemFree(
					void	*	mem)
{
	free(mem);
}


mStatus
mDNSPlatformTimeInit(
					mDNSs32	*	timenow)
{
	sw_time now;

	sw_time_init_now(&now);

	*timenow = (ConvertSWTime(now));

	return 0;
}


mDNSInterfaceID
mDNSPlatformInterfaceIDfromInterfaceIndex(const mDNS *const m, mDNSu32 index)
{
	SW_UNUSED_PARAM(m);

	return (mDNSInterfaceID) index;
}


mDNSu32
mDNSPlatformInterfaceIndexfromInterfaceID(const mDNS *const m, mDNSInterfaceID id)
{
	SW_UNUSED_PARAM(m);

	return (mDNSu32) id;
}

mStatus
mDNSPlatformInterfaceNameToID(
					mDNS					*	const	m,
					const char			*			inName,
					mDNSInterfaceID	*			outID)
{
	PlatformNetworkInterface * intf;

	SW_UNUSED_PARAM(m);

	intf = (PlatformNetworkInterface *) (m->HostInterfaces);

	while (intf)
	{
		if (strcmp(inName, intf->m_name) == 0)
		{
			*outID = (mDNSInterfaceID) intf;
			return mStatus_NoError;
		}

		intf = (PlatformNetworkInterface*) intf->m_super.next;
	}

	return mStatus_NoSuchNameErr;
}


mStatus
mDNSPlatformInterfaceIDToInfo(
					mDNS								*	const	m,
					mDNSInterfaceID							inID,
					mDNSPlatformInterfaceInfo	*			outInfo)
{
	PlatformNetworkInterface * intf;

	intf = (PlatformNetworkInterface *) (m->HostInterfaces);

	while (intf)
	{
		if (inID == (mDNSInterfaceID) intf) 
		{
			outInfo->name	=	intf->m_name;
			outInfo->ip		=	intf->m_super.ip;

			return mStatus_NoError;
		}

		intf = (PlatformNetworkInterface*) intf->m_super.next;
	}

	return mStatus_NoSuchNameErr;
}


mStatus
mDNSPlatformRun(
						mDNS	*	const m)
{
	gMDNSPlatformSupport.m_running = SW_TRUE;

	sw_debug(SW_LOG_NOTICE, "starting up...");
	
	while (gMDNSPlatformSupport.m_running == SW_TRUE)
	{
		/*
		 * Give the mDNS core a chance to do its
		 * work and determine next event time.
		 */
		mDNSs32 interval = mDNS_Execute(m) - mDNSPlatformTimeNow();

		if (interval < 0)
		{
			interval = 0;
		}
		else if (interval > (0x7FFFFFFF / 1000))
		{
			interval = 0x7FFFFFFF / mDNSPlatformOneSecond;
		}
		else
		{
			interval = (interval * 1000) / mDNSPlatformOneSecond;
		}

		//
		// Wait until something occurs
		// (e.g. cancel, incoming packet, or timeout).
		//
		sw_salt_step(gMDNSPlatformSupport.m_salt, (sw_uint32*) &interval);
	}

	sw_debug(SW_LOG_NOTICE, "shutting down...");

	mDNS_Close(m);

	return (0);
}


mStatus
mDNSPlatformStopRun(
						mDNS	*	const m)
{
	sw_assert(m);

	gMDNSPlatformSupport.m_running = SW_FALSE;

	return 0;
}


static sw_result
platform_setup_names(
						mDNS						*	const m,
						sw_const_string					friendly_name,
						sw_const_string					scary_name)
{
	/*
	 * Set up the nice label
	 */
	m->nicelabel.c[0] = 0;
	get_user_specified_friendly_computer_name(friendly_name, &m->nicelabel);
	
	/*
	 * Set up the RFC 1034-compliant label
	 */
	m->hostlabel.c[0] = 0;
	get_user_specified_rfc1034_computer_name(scary_name, &m->hostlabel);
	
	mDNS_GenerateFQDN(m);
	
	/*
	 * not much can go wrong
	 */
	return SW_OKAY;
}


/*
 * Sets up a multicast send/receive socket for the specified
 * port on the interface specified by the IP addrelss intfAddr.
 */
sw_result
platform_setup_sockets(
			mDNS								*	const	m,
			PlatformNetworkInterface	*			intf)
{
	sw_ipv4_address	local_address;
 	sw_ipv4_address	multicast_address;
	sw_result			err;

	sw_assert(intf != NULL);
	
	/*
	 * create the local address
	 */
	err = sw_ipv4_address_init_from_saddr(&local_address, intf->m_super.ip.ip.v4.NotAnInteger);
	sw_check_okay(err, exit);
	
	/*
	 * create the multicast address
	 */
	err = sw_ipv4_address_init_from_saddr(&multicast_address, AllDNSLinkGroup.NotAnInteger);
	sw_check_okay(err, exit);

	/*
	 * create the multicast socket
	 */
	err = sw_multicast_socket_init(&intf->m_multicast_socket);
	sw_check_okay(err, exit);
	
	/*
	 * now try and bind
	 * 
	 * NOTE: winsock allows us to bind to our multicast socket to a
	 * local address.  this means that we won't have to worry about
	 * receiving packets bound for another interface.  binding to a
	 * local address on Posix doesn't seem to work as well. this means
	 * we have to filter out packets after calling recvfrom, because
	 * we might have received a packet bound for another interface
	 */
#if defined(WIN32)
	err = sw_socket_bind(intf->m_multicast_socket, local_address, ntohs(MulticastDNSPort.NotAnInteger));
#else
	err = sw_socket_bind(intf->m_multicast_socket, sw_ipv4_address_any(), ntohs(MulticastDNSPort.NotAnInteger));
#endif
	sw_check_okay(err, exit);

	/*
	 * Add multicast group membership on this interface
	 */
	err = sw_socket_join_multicast_group(intf->m_multicast_socket, local_address, multicast_address, 255);
	sw_check_okay(err, exit);

	/*
	 * we want to use this socket in non-blocking mode
	 */
	err = sw_socket_set_blocking_mode(intf->m_multicast_socket, SW_FALSE);
	sw_check_okay(err, exit);

	/*
	 * set up socket with salt
	 */
	err = sw_salt_register_socket(gMDNSPlatformSupport.m_salt, intf->m_multicast_socket, SW_SOCKET_READ, (sw_socket_handler) m, socket_event_handler, (sw_opaque) intf);
	sw_check_okay(err, exit);

exit:

	return err;
}


static sw_result
platform_fina_socket(
			mDNS						*	const	m,
			PlatformNetworkInterface	*	intf)
{
	SW_UNUSED_PARAM(m);

	/*
	 * remove from salt event loop and free
	 */
	sw_salt_unregister_socket(gMDNSPlatformSupport.m_salt, intf->m_multicast_socket);

	sw_socket_fina(intf->m_multicast_socket);

	/*
	 * and we're good
	 */
	return SW_OKAY;
}


mDNSexport mStatus
PlatformConvertResultToStatus(
							sw_result result)
{
	mStatus status;
    
	if (result == SW_OKAY)
	{
		status = mStatus_NoError;
	}
	else
	{
		status = mStatus_UnknownErr;
	}
	
	return status;
}


sw_result
refresh_interface_list(
					mDNS	*	const m)
{
	sw_result result;

	/*
	 * must lock this as it might be called from another thread
	 */
	mDNSPlatformLock(m);

	/*
	 * reset the list
	 */
	result = setup_interface_list(m);

	/*
	 * and unlock 
	 */
	mDNSPlatformUnlock(m);

	return PlatformConvertResultToStatus(result);
}


static sw_result
setup_interface_list(
					mDNS	*	const m)
{
	sw_network_interface	*	netifs;
	sw_uint32						count;
	sw_uint32						i;
	sw_bool						found = SW_FALSE;
	sw_result					err   = SW_OKAY;

	free_interface_list(m);

	err = sw_network_interfaces(&count, &netifs);
	sw_check_okay(err, exit);

	for (i = 0; i < count; i++)
	{
		/* did we opt to run on a selected interface? */
		if (gMDNSPlatformSupport.m_num_filters != 0)
		{
			sw_int8				iname[SW_IFNAMSIZ];
			sw_ipv4_address	address;
			sw_uint32			saddr;
			unsigned				j;

			sw_network_interface_name(netifs[i], iname, SW_IFNAMSIZ);
			sw_network_interface_ipv4_address(netifs[i], &address);

			for (j = 0; j < gMDNSPlatformSupport.m_num_filters; j++)
			{
				if (sw_strcmp(gMDNSPlatformSupport.m_filters[j], iname) == 0)
				{
					err = setup_interface(m, netifs[i]);
					sw_check_okay(err, exit);
					found = SW_TRUE;
					break;
				}
				else
				{
					saddr = inet_addr(gMDNSPlatformSupport.m_filters[j]);

					if (saddr == sw_ipv4_address_saddr(address))
					{
						err = setup_interface(m, netifs[i]);
						sw_check_okay(err, exit);
						found = SW_TRUE;
						break;
					}
				}
			}
		}
		else
		{
			found = SW_TRUE;
			err = setup_interface(m, netifs[i]);
			sw_check_okay(err, exit);
		}
	}

	sw_network_interfaces_fina(count, netifs);

	if (!found)
	{
		sw_debug(SW_LOG_WARNING, "no network interfaces\n");
	}

exit:

	return err;
}


static sw_result
free_interface_list(
					mDNS	*	const m)
{
	sw_assert(m != NULL);
    
	while (m->HostInterfaces)
	{
		struct PlatformNetworkInterface_struct	*	intf;
 
		intf = (struct PlatformNetworkInterface_struct*) (m->HostInterfaces);
		
		/*
		 * Deregister this interface with mDNS.
		 */
		mDNS_DeregisterInterface(m, &intf->m_super);

		free_interface(m, intf);
	}
	
	return SW_OKAY;
}


static sw_result
setup_interface(
					mDNS			*	const m,
					sw_network_interface	netif)
{
	sw_ipv4_address									address;
	sw_ipv4_address									netmask;
	sw_int8												host[16];
	struct PlatformNetworkInterface_struct	*	intf;
	sw_uint32											index;
	sw_result											err;
	
	/*
	 * Allocate memory for the info item.
	 */
	intf = NULL;
	intf = (struct PlatformNetworkInterface_struct*) sw_malloc(sizeof(struct PlatformNetworkInterface_struct));
	err = sw_translate_error(intf, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(intf, 0, sizeof(struct PlatformNetworkInterface_struct));

	/*
	 * initialize core data structure
	 */
	sw_network_interface_ipv4_address(netif, &address);
	sw_network_interface_netmask(netif, &netmask);

	intf->m_super.ip.type						=	mDNSAddrType_IPv4;
	intf->m_super.ip.ip.v4.NotAnInteger 	=	sw_ipv4_address_saddr(address);
	intf->m_super.mask.type						=	mDNSAddrType_IPv4;
	intf->m_super.mask.ip.v4.NotAnInteger	= sw_ipv4_address_saddr( netmask );
	intf->m_super.Advertise       			=	m->AdvertiseLocalAddresses;
	sw_network_interface_index(netif, &index);
	intf->m_super.InterfaceID					=	(mDNSInterfaceID) index;

	/*
	 * Set up multicast portion of interface.
	 */
	err = platform_setup_sockets(m, intf);
	sw_check_okay(err, exit);

	/*
	 * Set up name
	 */
	err = sw_network_interface_name(netif, intf->m_name, 256);
	sw_check_okay(err, exit);

	/*
	 * Register this interface with mDNS.
	 */
	err = mDNS_RegisterInterface(m, &intf->m_super);
	sw_check_okay(err, exit);

	/*
	 * Success!
	 */
	sw_debug(SW_LOG_NOTICE, "registered interface %s\n", sw_ipv4_address_name(address, host, 16));

exit:

	return err;
}


static sw_result
free_interface(
				mDNS												*	const m,
				struct PlatformNetworkInterface_struct	*			intf)
{
	sw_ipv4_address	addr;
	sw_int8				host[16];
	
	sw_assert(intf);
	
	sw_ipv4_address_init_from_saddr(&addr, intf->m_super.ip.ip.v4.NotAnInteger);
	sw_debug(SW_LOG_NOTICE, "deregistered interface %s\n", sw_ipv4_address_name(addr, host, 16));
	
	/*
	 * deallocate
	 */
	platform_fina_socket(m, intf);
	sw_free(intf);

	return SW_OKAY;
}


/*
 * On OS X this gets the text of the field labelled "Computer Name" in the Sharing Prefs Control Panel
 * Other platforms can either get the information from the appropriate place,
 * or they can alternatively just require all registering services to provide an explicit name
 */
static void
get_user_specified_friendly_computer_name(
											sw_const_string		name,
											domainlabel	*	const namelabel)
{
	MakeDomainLabelFromLiteralString(namelabel, name);
}


/*
 * This gets the current hostname, truncating it at the first dot if necessary
 */
static void
get_user_specified_rfc1034_computer_name(
											sw_const_string		name,
											domainlabel	*	const	namelabel)
{
	int len = 0;
	sw_strncpy((char*) &namelabel->c[1], name, MAX_DOMAIN_LABEL);
	/* gethostname(&namelabel->c[1], MAX_DOMAIN_LABEL); */
	while (len < MAX_DOMAIN_LABEL && namelabel->c[len+1] && namelabel->c[len+1] != '.') len++;
	namelabel->c[0] = (mDNSu8) len;
}


/*
 * This routine is called when there is a change in the
 * network interfaces associated with this machine
 */
static sw_result
network_interface_event_handler(
								sw_network_interface_handler	handler,
								sw_salt								salt,
								sw_network_interface				netif,
								sw_opaque							extra)
{
	mDNS	*	m;

	SW_UNUSED_PARAM(salt);
	SW_UNUSED_PARAM(netif);
	SW_UNUSED_PARAM(extra);

	m	=	(mDNS*) handler;

	refresh_interface_list(m);

	return SW_OKAY;
}


/*
 * This routine is called where the main loop detects that 
 * data is available on a socket.
 */
static sw_result
socket_event_handler(
			sw_socket_handler	handler,
			sw_salt				salt,
			sw_socket			socket,
			sw_socket_event	events,
			sw_opaque			extra)
{
	mDNS								*	m;
	PlatformNetworkInterface	*	intf;
	mDNSAddr								intfAddr;
	mDNSAddr								senderAddr,
											destAddr;
	mDNSIPPort							senderPort;
	sw_size_t							packetLen;
	DNSMessage							packet;
	sw_ipv4_address					from;
	sw_port								from_port;
	sw_result							err;

	SW_UNUSED_PARAM(salt);
	SW_UNUSED_PARAM(events);

	m			=	(mDNS*) handler;
	intf		=	(PlatformNetworkInterface*) extra;

	/* sw_debug_memory_inuse(); */

	sw_assert(m    != NULL);
	sw_assert(intf != NULL);
	sw_assert(intf->m_multicast_socket == socket);

	err = sw_socket_recvfrom(intf->m_multicast_socket, (sw_octets) &packet, sizeof(packet), &packetLen, &from, &from_port, NULL, NULL);
	sw_check_okay(err, exit);

	if (packetLen >= 0)
	{
		intfAddr                		=	intf->m_super.ip;
		senderAddr.type					=	mDNSAddrType_IPv4;
		senderAddr.ip.v4.NotAnInteger =	sw_ipv4_address_saddr(from);
		senderPort.NotAnInteger			=	htons(from_port);

		/*
		 * Convince mDNS Core that this isn't a spoof packet.  
		 * We're punting on this because salt doesn't give
		 * us the ability to see what the destination address
		 * is.
		 */
        
		destAddr.type						=	mDNSAddrType_IPv4;
		destAddr.ip.v4.NotAnInteger	=	AllDNSLinkGroup.NotAnInteger;
	}
    
	if (packetLen >= 0 && packetLen < sizeof(DNSMessageHeader))
	{
		sw_debug(SW_LOG_ERROR, "socket_event_handler packet length (%d) too short", packetLen);
		goto exit;
	}

	mDNSCoreReceive(m, &packet, (mDNSu8 *)&packet + packetLen, &senderAddr, senderPort, &destAddr, MulticastDNSPort, intf->m_super.InterfaceID, 255); 

exit:

	return err;
}


mDNSexport void
logf(const char *format, ...)
{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, 512, format, ptr)] = 0;
	va_end(ptr);
#if defined(WIN32)
	OutputDebugString((const char*) buffer);
#endif
	fprintf(stderr, "%s\n", buffer);
	fflush(stderr);
}


/* // Note, this uses mDNS_vsprintf instead of standard "vsprintf", because mDNS_vsprintf knows */
/* // how to print special data types like IP addresses and length-prefixed domain names */
mDNSexport void
debugf_(const char *format, ...)
{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, 512, format, ptr)] = 0;
	va_end(ptr);
	if (gMDNSPlatformVerboseLevel >= 1)
	{
#if defined(WIN32)
		OutputDebugString((const char*) buffer);
#endif
		fprintf(stderr, "%s\n", buffer);
		fflush(stderr);
	}
}


mDNSexport void
verbosedebugf_(const char *format, ...)
{
	unsigned char buffer[512];
	va_list ptr;
	va_start(ptr,format);
	buffer[mDNS_vsnprintf((char *)buffer, 512, format, ptr)] = 0;
	va_end(ptr);
	if (gMDNSPlatformVerboseLevel >= 2)
	{
#if defined(WIN32)
		OutputDebugString((const char*) buffer);
#endif
		fprintf(stderr, "%s\n", buffer);
		fflush(stderr);
	}
}



mDNSexport void
LogMsg(const char *format, ...) IS_A_PRINTF_STYLE_FUNCTION(1,2)
{
	SW_UNUSED_PARAM(format);
}

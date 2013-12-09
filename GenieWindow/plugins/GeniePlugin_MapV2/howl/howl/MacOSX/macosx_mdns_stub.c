/*
 * Copyright 2003, 2004, 2004 Porchdog Software. All rights reserved.
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

#include <libc.h>
#include <arpa/nameser.h>
#include <CoreFoundation/CoreFoundation.h>
#include <MacOSX/macosx_mdns_stub.h>
#include <salt/debug.h>


/*
 * static function declarations 
 */
static sw_result
sw_mdns_stub_add_to_run_loop(
				dns_service_discovery_ref		client);


static void
sw_mdns_stub_handle_mach_message(
				CFMachPortRef						port,
				void								*	msg,
				CFIndex								size,
				void								*	info);


static void
sw_mdns_stub_handle_publish_reply(
				DNSServiceRegistrationReplyErrorType	errorCode,
				void											*	context);


static void
sw_mdns_stub_handle_browse_reply(
				DNSServiceBrowserReplyResultType	resultType,
				const char							*	replyName,
				const char							*	replyType,
				const char							*	replyDomain,
				DNSServiceDiscoveryReplyFlags		flags,
				void									*	context);


static void
sw_mdns_stub_handle_resolve_reply(
				struct sockaddr					*	interface,
				struct sockaddr					*	address,
				const char							*	txtRecord,
				DNSServiceDiscoveryReplyFlags		flags,
				void									*	context);


/*
 * sw_discovery_init implementation
 * -------------------------
 * This is the client side to publishing services using DNS-SD.
 *
 */
sw_result
sw_discovery_init_with_flags(
					sw_discovery			*	self,
					sw_discovery_init_flags	flags)
{
	return sw_discovery_init(self);
}

					
sw_result
sw_mdns_stub_init(
					sw_mdns_stub	*	self,
					sw_salt				salt,
					sw_discovery		discovery,
					sw_uint16			port)
{
	sw_result err;

	*self = (sw_mdns_stub) sw_malloc(sizeof(struct _sw_mdns_stub));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);
	
	/*
	 * initialize easy stuff
	 */
	(*self)->m_salt		= salt;
	(*self)->m_discovery = discovery;

exit:

	return err;
}


sw_result
sw_mdns_stub_fina(
				sw_mdns_stub	self)
{
	sw_free(self);

	return SW_OKAY;
}


sw_result
sw_mdns_stub_check_version(
				sw_mdns_stub	self)
{
	return SW_OKAY;
}
	

sw_result
sw_mdns_stub_publish_host(
				sw_mdns_stub					self,
				sw_uint32						interface_index,
				sw_const_string				name,
				sw_const_string				domain,
				sw_ipv4_address				address,
				sw_discovery_publish_reply	reply,
				sw_opaque						extra,
				sw_discovery_oid			*	oid)
{
	return SW_E_UNKNOWN;
}


sw_result
sw_mdns_stub_publish(
				sw_mdns_stub						self,
				sw_uint32							interface_index,
				sw_const_string					name,
				sw_const_string					reg_type,
				sw_const_string					domain,
				sw_const_string					host,
				sw_port								port,
				sw_octets							text_record,
				sw_uint32							text_record_len,
				sw_discovery_publish_reply		reply,
				sw_opaque_t							extra,
				sw_discovery_oid				*	oid)
{
	sw_mdns_stub_publish_node	*	node = NULL;
	sw_result							err  = SW_OKAY;

	node = (sw_mdns_stub_publish_node*) sw_malloc(sizeof(sw_mdns_stub_publish_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	node->m_oid = DNSServiceRegistrationCreate(name, reg_type, (domain) ? domain : "", htons(port), "", sw_mdns_stub_handle_publish_reply, node);
	sw_check(node->m_oid, exit, err = SW_E_UNKNOWN);

	node->m_self			=	self;
	node->m_reply			=	reply;
	node->m_extra			=	extra;
	*oid 						=	(sw_discovery_publish_id) node;

	if (text_record)
	{
		DNSServiceRegistrationUpdateRecord(node->m_oid, 0, text_record_len, text_record, 255);
	}

	err = sw_mdns_stub_add_to_run_loop(node->m_oid);
	sw_check_okay(err, exit);

exit:

	if (err && node)
	{
		sw_free(node);
	}

	return err;
}


sw_result
sw_mdns_stub_publish_update(
						sw_mdns_stub					self,
						sw_discovery_publish_id		id,
						sw_octets						text_record,
						sw_uint32							text_record_len)
{
	sw_mdns_stub_publish_node * node = (sw_mdns_stub_publish_node*) id;

	sw_assert(node);

	if (text_record)
	{
		DNSServiceRegistrationUpdateRecord(node->m_oid, 0, text_record_len, text_record, 255);
	}

	return SW_OKAY;
}


sw_result
sw_mdns_stub_browse_domains(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	return SW_E_UNKNOWN;
}


sw_result
sw_mdns_stub_browse_services(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_const_string					type,
						sw_const_string					domain,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_mdns_stub_browse_node	*	node;
	sw_result							err;

	node = (sw_mdns_stub_browse_node*) sw_malloc(sizeof(struct _sw_mdns_stub_browse_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	node->m_oid				=	DNSServiceBrowserCreate(type, (domain) ? domain : "", sw_mdns_stub_handle_browse_reply, node);
	node->m_self			=	self;
	node->m_reply			=	reply;
	node->m_extra			=	extra;
	*oid						=	(sw_discovery_browse_id) node;

	err = sw_mdns_stub_add_to_run_loop(node->m_oid);
	sw_check_okay(err, exit);

exit:

	if (err && node)
	{
		sw_free(node);
	}

	return err;
}


sw_result
sw_mdns_stub_resolve(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_const_string					name,
						sw_const_string					reg_type,
						sw_const_string					domain,
						sw_discovery_resolve_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_mdns_stub_resolve_node	*	node	= NULL;
	sw_result							err	= SW_OKAY;

	node = (sw_mdns_stub_resolve_node*) sw_malloc(sizeof(struct _sw_mdns_stub_resolve_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	node->m_oid = DNSServiceResolverResolve(name, reg_type, domain, sw_mdns_stub_handle_resolve_reply, node);
	node->m_self			=	self;
	node->m_name			=	strdup(name);
	node->m_type			=	strdup(reg_type);
	node->m_domain			=	(domain) ? strdup(domain) : NULL;
	node->m_reply			=	reply;
	node->m_extra			=	extra;
	*oid						=	(sw_discovery_resolve_id) node;

	err = sw_mdns_stub_add_to_run_loop(node->m_oid);
	sw_check_okay(err, exit);

exit:

	if (err && node)
	{
		sw_free(node);
	}

	return err;
}


sw_result
sw_mdns_stub_query_record(
				sw_mdns_stub							self,
				sw_uint32								interface_index,
				sw_uint32								flags,
				sw_const_string						fullname,
				sw_uint16								rrtype,
				sw_uint16								rrclass,
				sw_discovery_query_record_reply	reply,	
				sw_opaque								extra,
				sw_discovery_oid					*	oid)
{
	return SW_E_UNKNOWN;
}


sw_result
sw_mdns_stub_cancel(
						sw_mdns_stub		self,
						sw_discovery_oid	oid)
{
	sw_mdns_stub_resolve_node * node = (sw_mdns_stub_resolve_node*) oid; 

	DNSServiceDiscoveryDeallocate(node->m_oid);
	sw_free(node);

	return SW_OKAY;
}


sw_result
sw_mdns_stub_run(
							sw_mdns_stub	self)
{
	return sw_salt_run(self->m_salt);
}


sw_result
sw_mdns_stub_stop_run(
							sw_mdns_stub	self)
{
	return sw_salt_stop_run(self->m_salt);
}


int
sw_mdns_stub_socket(
							sw_mdns_stub	self)
{
	return -1;
}


sw_result
sw_mdns_stub_read_socket(
							sw_mdns_stub	self)
{
	return SW_E_UNKNOWN;
}


sw_result
sw_mdns_stub_salt(
							sw_mdns_stub	self,
							sw_salt		*	salt)
{
	*salt = self->m_salt;
	return SW_OKAY;
}


static sw_result
sw_mdns_stub_add_to_run_loop(
								dns_service_discovery_ref	client)
{
	Boolean					shouldFreeInfo;
	CFMachPortRef			cfMachPort;
	CFMachPortContext		context = { 0, 0, NULL, NULL, NULL };
	mach_port_t				port;
	CFRunLoopSourceRef	rls;
	sw_result				err = SW_OKAY;	

	port = DNSServiceDiscoveryMachPort(client);
	sw_check(port, exit, err = SW_E_UNKNOWN);

	cfMachPort	=	CFMachPortCreateWithPort(kCFAllocatorDefault, port, sw_mdns_stub_handle_mach_message, &context, &shouldFreeInfo);
	rls			=	CFMachPortCreateRunLoopSource(NULL, cfMachPort, 0);

	CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
	CFRelease(rls);

exit:

	return err;
}


static void
sw_mdns_stub_handle_mach_message(
								CFMachPortRef	port,
								void			*	msg,
								CFIndex			size,
								void			*	info)
{
	DNSServiceDiscovery_handleReply(msg);
}


static void
sw_mdns_stub_handle_publish_reply(
								DNSServiceRegistrationReplyErrorType	errorCode,
								void											*	context)
{
	sw_mdns_stub_publish_node * node = (sw_mdns_stub_publish_node*) context;
	
	switch (errorCode)
	{
		case kDNSServiceDiscoveryNoError:
		{
			(node->m_reply)(node->m_self->m_discovery, SW_DISCOVERY_PUBLISH_STARTED, (sw_discovery_publish_id) node, node->m_extra);
		}
		break;

		case kDNSServiceDiscoveryNameConflict:
		{
			(node->m_reply)(node->m_self->m_discovery, SW_DISCOVERY_PUBLISH_NAME_COLLISION, (sw_discovery_publish_id) node, node->m_extra);
		}
		break;

		default:
		{
			sw_assert(0);
		}
	}
}


static void
sw_mdns_stub_handle_browse_reply(
								DNSServiceBrowserReplyResultType	resultType,
								const char							*	replyName,
								const char							*	replyType,
								const char							*	replyDomain,
								DNSServiceDiscoveryReplyFlags		flags,
								void									*	context)
{
	sw_mdns_stub_browse_node 	*	node = (sw_mdns_stub_browse_node*) context;
	
	switch (resultType)
	{
		case DNSServiceBrowserReplyAddInstance:
		{
			(node->m_reply)(node->m_self->m_discovery, (sw_discovery_oid) node, SW_DISCOVERY_BROWSE_ADD_SERVICE, 0, replyName, replyType, replyDomain, node->m_extra);
		}
		break;
		
		case DNSServiceBrowserReplyRemoveInstance:
		{
			(node->m_reply)(node->m_self->m_discovery, (sw_discovery_oid) node, SW_DISCOVERY_BROWSE_REMOVE_SERVICE, 0, replyName, replyType, replyDomain, node->m_extra);
		}
		break;
		
		default:
		{
			sw_assert(0);
		}
		break;
	}
}


static void
sw_mdns_stub_handle_resolve_reply(
								struct sockaddr					*	netInterface,
								struct sockaddr					*	netAddress,
								const char							*	txtRecord,
								DNSServiceDiscoveryReplyFlags		flags,
								void									*	context)
{
	sw_mdns_stub_resolve_node	*	node = (sw_mdns_stub_resolve_node*) context;
	struct sockaddr_in			*	ip = (struct sockaddr_in*) netAddress;
	sw_ipv4_address					address;
	sw_port							port;
	
	sw_ipv4_address_init_from_saddr(&address, ip->sin_addr.s_addr);
	port = ntohs(ip->sin_port);
	
	(node->m_reply)(node->m_self->m_discovery, (sw_discovery_oid) node, 0, node->m_name, node->m_type, node->m_domain, address, port, (sw_octets) txtRecord, strlen(txtRecord), node->m_extra);
}


sw_result
sw_mdns_servant_new(
					sw_mdns_servant	*	self,
					sw_uint16				port,
					sw_string			*	filters,
					sw_uint32					num_filters)
{
	return SW_E_UNKNOWN;
}


sw_result
sw_mdns_servant_delete(
					sw_mdns_servant		self)
{
	return SW_E_UNKNOWN;
}


sw_uint16
sw_mdns_servant_port(
					sw_mdns_servant		self)
{
	return -1;
}

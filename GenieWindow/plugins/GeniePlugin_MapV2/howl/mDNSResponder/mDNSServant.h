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

#ifndef _mDNSServant_h
#define _mDNSServant_h

//#include "howl_global.h"

#include <mdns_stub.h>
#include <corby/orb.h>
#include <corby/object.h>
#include <discovery_i.h>
#include "mDNSClientAPI.h"
#include "DNSServices.h"


#if defined(__cplusplus)
extern "C"
{
#endif


//#define MDNS_URL			"swop://127.0.0.1:5335/dns-sd"
#define SERVICE_OID		"DNS-SD"
#define PROTOCOL_TAG		"swop"


struct														_sw_mdns_servant_client_node;
typedef struct _sw_mdns_servant_client_node	*	sw_mdns_servant_client_node;


struct _sw_mdns_servant
{
	sw_salt								m_salt;
	sw_corby_orb						m_orb;
	sw_uint16							m_port;
	sw_string						*	m_filters;
	sw_uint32							m_num_filters;
	sw_mdns_servant_client_node	m_clients;
};


typedef sw_result
(*sw_mdns_servant_cleanup_func)(
						sw_mdns_servant	servant,
						sw_opaque			id);


struct _sw_mdns_servant_client_node
{
	sw_mdns_servant								m_self;
	sw_string										m_host_name;
	sw_ipv4_address								m_host_address;
	sw_string										m_name;
	sw_string										m_reg_type;
	sw_string										m_domain;
	sw_port											m_port;
	sw_string										m_attrs;
	DNSDomainRegistrationRef					m_domain_registration_ref;
	DNSHostRegistrationRef						m_host_registration_ref;
	DNSRegistrationRef							m_registration_ref;
	DNSBrowserRef									m_browser_ref;
	DNSResolverRef									m_resolver_ref;
	DNSQueryRecordRef								m_query_record_ref;
	sw_discovery_publish_reply					m_publish_reply;
	sw_discovery_browse_reply					m_browse_reply;
	sw_discovery_resolve_reply					m_resolve_reply;
	sw_discovery_query_record_reply			m_query_record_reply;
	sw_opaque										m_extra;
	sw_discovery_oid								m_oid;
	struct _sw_mdns_servant_client_node	*	m_verp;
	struct _sw_mdns_servant_client_node	*	m_txen;

	/*
	 * this stuff is for cleaning up on client
	 * termination
	 */
	sw_corby_channel								m_channel;

	sw_result										(*m_cleanup_internal)(
															sw_mdns_servant						servant,
															struct _sw_mdns_servant_client_node	*	node);

	sw_mdns_servant_cleanup_func				m_cleanup;
};


sw_result
sw_mdns_servant_new(
						sw_mdns_servant	*	self,
						sw_port					port,
						sw_string			*	filters,
						sw_uint32				num_filters);


sw_result
sw_mdns_servant_init(
						sw_mdns_servant		self);


sw_result 
sw_mdns_servant_delete(
						sw_mdns_servant		self);


sw_result
sw_mdns_servant_fina(
						sw_mdns_servant		self);


sw_result
sw_mdns_servant_load_file(
						sw_mdns_servant		self,
						sw_const_string		file);


sw_uint16
sw_mdns_servant_port(
						sw_mdns_servant		self);


sw_result
sw_mdns_servant_cleanup_client(
						sw_mdns_servant		self,
						sw_corby_channel		channel);


sw_result
sw_mdns_servant_publish_host(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				domain,
						sw_ipv4_address				address,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);


sw_result
sw_mdns_servant_publish(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				reg_type,
						sw_const_string				domain,
						sw_const_string				host,
						sw_port							port,
						sw_octets						text_record,
						sw_uint32						text_record_len,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);


sw_result
sw_mdns_servant_publish_update(
						sw_mdns_servant				self,
						sw_corby_channel				channel,
						sw_discovery_publish_id		id,
						sw_octets						text_record,
						sw_uint32						text_record_len);


sw_result
sw_mdns_servant_browse_domains(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);
						
						
sw_result
sw_mdns_servant_browse_services(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_const_string				type,
						sw_const_string				domain,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);
						
						
sw_result
sw_mdns_servant_resolve(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				reg_type,
						sw_const_string				domain,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);


sw_result
sw_mdns_servant_query_record(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_uint32						flags,
						sw_const_string				fullname,
						sw_uint16						rrtype,
						sw_uint16						rrclass,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid);


sw_result
sw_mdns_servant_cancel(
						sw_mdns_servant	self,
						sw_corby_channel	channel,
						sw_discovery_oid	oid);


#if defined(__cplusplus)
}
#endif


#endif

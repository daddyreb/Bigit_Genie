#ifndef _sw_mdns_h
#define _sw_mdns_h

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

#include <salt/salt.h>
#include <discovery/discovery.h>


#ifdef __cplusplus
extern "C"
{
#endif


struct										_sw_mdns_stub;
typedef struct _sw_mdns_stub		*	sw_mdns_stub;
struct										_sw_mdns_servant;
typedef struct _sw_mdns_servant	*	sw_mdns_servant;


sw_result
sw_mdns_stub_init(
						sw_mdns_stub	*	self,
						sw_salt				salt,
						sw_discovery		discovery,
						sw_port				port);


sw_result
sw_mdns_stub_fina(
						sw_mdns_stub		self);


sw_result
sw_mdns_stub_check_version(
						sw_mdns_stub		self);


sw_result
sw_mdns_stub_publish_host(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_const_string					name,
						sw_const_string					domain,
						sw_ipv4_address					address,
						sw_discovery_publish_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid);


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
						sw_opaque							extra,
						sw_discovery_oid				*	oid);


sw_result
sw_mdns_stub_publish_update(
						sw_mdns_stub						self,
						sw_discovery_oid					oid,
						sw_octets							text_record,
						sw_uint32							text_record_len);


sw_result
sw_mdns_stub_browse_domains(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid);

						
sw_result
sw_mdns_stub_browse_services(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_const_string					type,
						sw_const_string					domain,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid);

						
sw_result
sw_mdns_stub_resolve(
						sw_mdns_stub						self,
						sw_uint32							interface_index,
						sw_const_string					name,
						sw_const_string					reg_type,
						sw_const_string					domain,
						sw_discovery_resolve_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid);


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
						sw_discovery_oid					*	oid);


sw_result
sw_mdns_stub_cancel(
						sw_mdns_stub		self,
						sw_discovery_oid	oid);


sw_sockdesc_t
sw_mdns_stub_socket(
						sw_mdns_stub					self);


sw_result
sw_mdns_stub_read_socket(
						sw_mdns_stub					self);


#ifdef __cplusplus
}
#endif


#endif

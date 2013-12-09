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

#ifndef _notosx_mdns_h
#define _notosx_mdns_h


#include <mdns_stub.h>
#include <corby/orb.h>
#include <corby/object.h>
#include <discovery_i.h>


#if defined(__cplusplus)
extern "C"
{
#endif


//#define MDNS_URL			"swop://127.0.0.1:5335/dns-sd"
#define SERVICE_OID		"DNS-SD"
#define PROTOCOL_TAG		"swop"


struct														_sw_mdns_stub_pending_op;
typedef struct _sw_mdns_stub_pending_op		*	sw_mdns_stub_pending_op;
struct														_sw_mdns_servant_client_node;
typedef struct _sw_mdns_servant_client_node	*	sw_mdns_servant_client_node;


struct _sw_mdns_stub
{
	sw_discovery				m_discovery;
	sw_salt						m_salt;
	sw_corby_orb				m_orb;
	sw_corby_object			m_self;
	sw_bool						m_bound;
	sw_corby_object			m_service;
	sw_result					m_check_version_result;
	sw_mdns_stub_pending_op	m_pending_ops;
};


struct _sw_mdns_stub_pending_op
{
	sw_discovery_publish_reply				m_publish_reply;
	sw_discovery_browse_reply				m_browse_reply;
	sw_discovery_resolve_reply				m_resolve_reply;
	sw_discovery_query_record_reply		m_query_record_reply;
	sw_opaque									m_extra;
	sw_discovery_oid							m_oid;
	struct _sw_mdns_stub_pending_op	*	m_txen;
};


#if defined(__cplusplus)
}
#endif


#endif

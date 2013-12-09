#ifndef _corby_orb_i_h
#define _corby_orb_i_h

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

#include <corby/orb.h>
#include <salt/address.h>


#ifdef __cplusplus
extern "C"
{
#endif 


#define SW_CORBY_TAG_LEN 32

struct													_sw_corby_orb_servant_node;
typedef struct _sw_corby_orb_servant_node		sw_corby_orb_servant_node;
struct													_sw_corby_orb_swop_listener;
typedef struct _sw_corby_orb_swop_listener	sw_corby_orb_swop_listener;
struct													_sw_corby_orb_protocol_spec;
typedef struct _sw_corby_orb_protocol_spec	sw_corby_orb_protocol_spec;


struct _sw_corby_orb
{
	sw_salt									m_salt;
	sw_corby_orb_protocol_spec		*	m_specs;
	sw_corby_orb_servant_node		*	m_servants;
	struct _sw_corby_channel		*	m_channels;
	struct _sw_corby_channel		*	m_channel_cache;
	sw_corby_orb_swop_listener		*	m_swop_listeners;
	sw_corby_orb_delegate				m_delegate;

	struct
	{
		sw_corby_orb_observer				m_observer;
		sw_corby_orb_observer_func			m_func;
		sw_opaque								m_extra;
	} m_observer_info;
};


struct _sw_corby_orb_protocol_spec
{
   sw_int8										m_name[SW_CORBY_TAG_LEN];
	sw_corby_protocol_tag						m_tag;
	sw_ipv4_address								m_address;
	sw_uint16										m_port;
   struct _sw_corby_orb_protocol_spec	*	m_txen;
};


struct _sw_corby_orb_servant_node
{
   sw_corby_servant								m_servant;
   sw_corby_servant_cb							m_cb;
	sw_int8										m_oid[SW_CORBY_OID_LEN];
   sw_size_t										m_oid_len;
   struct _sw_corby_orb_servant_node	*	m_txen;
};


struct _sw_corby_orb_swop_listener
{
   sw_socket										m_socket;
   sw_socket_options								m_options;
   struct _sw_corby_orb_swop_listener	*	m_txen;
};


sw_result
sw_corby_orb_register_channel_events(
					sw_corby_orb						self,
					struct _sw_corby_channel	*	channel,
					sw_uint32							events);


#ifdef __cplusplus
}
#endif 


#endif

#ifndef _corby_channel_i_h
#define _corby_channel_i_h

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

#include <corby/channel.h>
#include <salt/address.h>
#include "message_i.h"
#include "buffer_i.h"
#include "profile.h"


#ifdef __cplusplus
extern "C"
{
#endif 

#define SW_CORBY_CHANNEL_DEFAULT_BUFFER_SIZE	4192
#define SW_MAX_HOST_LEN								32
#define SW_MAX_OID_LEN								32


struct _sw_corby_orb;

struct _sw_corby_channel
{
	/*
	 * used for handling async sends
	 */
	struct _sw_corby_orb			*	m_orb;
	struct _sw_corby_buffer		*	m_send_queue_head;
	struct _sw_corby_buffer		*	m_send_queue_tail;

	/*
	 * used for unmarshaling the message
	 */
   sw_corby_message					m_message;
   sw_corby_buffer					m_send_buffer;
   sw_corby_buffer					m_read_buffer;
   sw_socket							m_socket;
	sw_corby_protocol_tag			m_to_tag;
	sw_ipv4_address					m_to;
	sw_port							m_to_port;
	sw_ipv4_address					m_from;
	sw_port							m_from_port;

	sw_corby_channel_delegate		m_delegate;
	sw_opaque_t							m_app_data;
	sw_uint32							m_cache_refs;
	sw_uint32							m_refs;

   sw_corby_channel					m_nextc;
   sw_corby_channel					m_prevc;

   sw_corby_channel					m_nexts;
   sw_corby_channel					m_prevs;

	enum State
	{
		Waiting,
		Reading
	} m_state;
};


sw_result
sw_corby_channel_init(
							sw_corby_channel			*	self,
							struct _sw_corby_orb		*	orb,
							sw_socket						socket,
							sw_socket_options				options,
							sw_size_t						bufsize);
                                    

sw_result
sw_corby_channel_init_with_profile(
							sw_corby_channel			*	self,
							struct _sw_corby_orb		*	orb,
							sw_const_corby_profile		service,
							sw_socket_options				options,
							sw_size_t						bufsize);


sw_result
sw_corby_channel_queue_send_buffer(
							sw_corby_channel	self,
							sw_corby_buffer	buffer);


sw_result
sw_corby_channel_flush_send_queue(
							sw_corby_channel	self);


#ifdef __cplusplus
}
#endif 


#endif

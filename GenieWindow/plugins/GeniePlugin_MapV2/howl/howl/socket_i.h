#ifndef _salt_socket_i_h
#define _salt_socket_i_h

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

#include <salt/socket.h>
#include <salt/address.h>


#ifdef __cplusplus
extern "C"
{
#endif 


#if defined(__PALMOS__)
#else


sw_result
sw_tcp_socket_super_init(
						sw_socket				socket);


sw_result
sw_tcp_socket_super_init_with_desc(
						sw_socket				socket,
						sw_sockdesc_t			desc);


sw_result
sw_udp_socket_super_init(
						sw_socket				socket);


sw_result
sw_multicast_socket_super_init(
						sw_socket				socket);



sw_result
sw_socket_super_fina(
						sw_socket				socket);


typedef sw_result
(*sw_socket_connect_func)(
						sw_socket					socket,
						struct _sw_ipv4_address	address,
						sw_port					port);

                                 
typedef sw_result
(*sw_socket_send_func)(
						sw_socket		socket,
						sw_octets		buffer,
						sw_size_t		len,
						sw_size_t	*	bytesWritten);


typedef sw_result	
(*sw_socket_sendto_func)(
						sw_socket			socket,
						sw_octets			buffer,
						sw_size_t			len,
						sw_size_t		*	bytesWritten,
						sw_ipv4_address	to,
						sw_port			port);
											

typedef sw_result 
(*sw_socket_recv_func)(
						sw_socket		socket,
						sw_octets		buffer,
						sw_size_t		max,
						sw_size_t	*	len);
                              
											
typedef sw_result	
(*sw_socket_recvfrom_func)(
						sw_socket				socket,
						sw_octets				buffer,
						sw_size_t				max,
						sw_size_t			*	len,
						sw_ipv4_address	*	from,
						sw_port			*	port,
						sw_ipv4_address	*	dest,
						sw_uint32				*	interface_index);
											
                                 
typedef sw_result
(*sw_socket_close_func)(
						sw_socket	socket);
                                 

struct _sw_socket
{
   sw_socket_connect_func		m_connect_func;
   sw_socket_send_func			m_send_func;
	sw_socket_sendto_func		m_sendto_func;
   sw_socket_recv_func			m_recv_func;
	sw_socket_recvfrom_func		m_recvfrom_func;
   sw_socket_close_func			m_close_func;
   struct sockaddr_in			m_addr;
   sw_bool						m_connected;
	struct sockaddr_in			m_dest_addr;
   sw_sockdesc_t					m_desc;

	/*
	 * for async ops
	 */
	sw_bool						m_registered;
	sw_salt							m_salt;
	sw_socket_event				m_events;
	sw_socket_handler				m_handler;
	sw_socket_handler_func		m_func;
	sw_opaque_t						m_extra;
};


struct _sw_socket_option_debug
{
	sw_int32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_nodelay
{
	sw_int32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_dontroute
{
	sw_int32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_keepalive
{
	sw_int32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_linger
{
	struct linger	m_val;
	sw_bool			m_modified;
};
   

struct _sw_socket_option_reuseaddr
{
	sw_int32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_sndbuf
{
	sw_uint32	m_val;
	sw_bool	m_modified;
};
   

struct _sw_socket_option_rcvbuf
{
	sw_uint32	m_val;
	sw_bool	m_modified;
};
   
   
struct _sw_socket_options
{
   struct _sw_socket_option_debug		m_debug;
   struct _sw_socket_option_nodelay		m_nodelay;
   struct _sw_socket_option_dontroute	m_dontroute;
   struct _sw_socket_option_keepalive	m_keepalive;
   struct _sw_socket_option_linger		m_linger;
   struct _sw_socket_option_reuseaddr	m_reuseaddr;
   struct _sw_socket_option_sndbuf		m_sndbuf;
   struct _sw_socket_option_rcvbuf		m_rcvbuf;

};
   
#endif

#ifdef __cplusplus
}
#endif 


#endif

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
#include "tlist.h"
#include <salt/address.h>
#include <salt/debug.h>

#if !defined(_WIN32_WCE)
#	include <errno.h>
#endif
#include <stdio.h>

#if defined(WIN32)

#	include <ws2tcpip.h>

#endif

#if defined(__sun)

#	include <sys/filio.h>

#endif


/*
   Private Declarations
*/
static sw_result
sw_socket_init(
            sw_socket					self,
            sw_bool					connected,
            sw_socket_connect_func	connect_func,
            sw_socket_send_func		send_func,
				sw_socket_sendto_func	sendto_func,
            sw_socket_recv_func		recv_func,
				sw_socket_recvfrom_func	recvfrom_func,
            sw_socket_close_func		close_func);

               
static sw_result
sw_socket_tcp_connect(
            sw_socket			self,
            sw_ipv4_address	address,
            sw_port			port);
               
               
static sw_result
sw_socket_tcp_send(
            sw_socket		self,
            sw_octets		buffer,
            sw_size_t		size,
				sw_size_t	*	bytesWritten);


static sw_result
sw_socket_tcp_sendto(
				sw_socket			self,
				sw_octets			buffer,
				sw_size_t			size,
				sw_size_t		*	bytesWritten,
				sw_ipv4_address	to,
				sw_port			to_port);
				

static sw_result
sw_socket_tcp_recv(
            sw_socket		self,
            sw_octets		buffer,
            sw_size_t		max,
            sw_size_t	*	len);


static sw_result
sw_socket_tcp_recvfrom(
            sw_socket				self,
            sw_octets				buffer,
            sw_size_t				max,
            sw_size_t			*	len,
				sw_ipv4_address	*	from,
				sw_port			*	from_port,
				sw_ipv4_address	*	dest,
				sw_uint32			*	interface_index);


static sw_result
sw_socket_tcp_close(
            sw_socket		socket);
               
               
static sw_result
sw_socket_udp_connect(
            sw_socket			self,
            sw_ipv4_address	address,
            sw_port			port);


static sw_result
sw_socket_udp_send(
            sw_socket		self,
            sw_octets		buffer,
            sw_size_t		size,
				sw_size_t	*	bytesWritten);


static sw_result
sw_socket_udp_sendto(
				sw_socket			self,
				sw_octets			buffer,
				sw_size_t			len,
				sw_size_t	*		bytesWritten,
				sw_ipv4_address	to,
				sw_port			to_port);
				
				
static sw_result
sw_socket_udp_really_sendto(
				sw_socket				self,
				sw_octets				buffer,
				sw_size_t				len,
				sw_size_t			*	bytesWritten,
				struct sockaddr	*	to,
				sw_socklen_t			to_len);


static sw_result
sw_socket_udp_recv(
            sw_socket		self,
            sw_octets		buffer,
            sw_size_t		max,
            sw_size_t	*	len);


static sw_result
sw_socket_udp_recvfrom(
				sw_socket				self,
				sw_octets				buffer,
				sw_size_t				max,
				sw_size_t			*	len,
				sw_ipv4_address	*	from,
				sw_port			*	from_port,
				sw_ipv4_address	*	dest,
				sw_uint32			*	interface_index);
				

static sw_result
sw_socket_udp_really_recvfrom(
				sw_socket				self,
				sw_octets				buffer,
				sw_size_t				max,
				sw_size_t			*	len,
				struct sockaddr	*	from,
				sw_socklen_t			from_len,
				struct in_addr		*	dest,
				sw_uint32			*	interface_index);


static sw_result
sw_socket_udp_close(
            sw_socket	socket);


/*
   IMPLEMENTATION
*/
sw_result
sw_tcp_socket_super_init(
               sw_socket self)
{
	sw_result err;

	err = sw_socket_init(self, SW_FALSE, &sw_socket_tcp_connect, &sw_socket_tcp_send, &sw_socket_tcp_sendto, &sw_socket_tcp_recv, &sw_socket_tcp_recvfrom, &sw_socket_tcp_close);
	sw_check_okay(err, exit);

	self->m_desc = socket(AF_INET, SOCK_STREAM, 0);
	err = sw_translate_error(self->m_desc != SW_INVALID_SOCKET, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_tcp_socket_super_init_with_desc(
					sw_socket		self,
					sw_sockdesc_t	desc)
{
	sw_result err;

	err = sw_socket_init(self, SW_FALSE, &sw_socket_tcp_connect, &sw_socket_tcp_send, &sw_socket_tcp_sendto, &sw_socket_tcp_recv, &sw_socket_tcp_recvfrom, &sw_socket_tcp_close);
	sw_check_okay(err, exit);

	self->m_desc = desc;

exit:

	return err;
}


sw_result
sw_udp_socket_super_init(
               sw_socket self)
{
	sw_result err;

	err = sw_socket_init(self, SW_FALSE, &sw_socket_udp_connect, &sw_socket_udp_send, &sw_socket_udp_sendto, &sw_socket_udp_recv, &sw_socket_udp_recvfrom, &sw_socket_udp_close);
	sw_check_okay(err, exit);

 	self->m_desc = socket(AF_INET, SOCK_DGRAM, 0);
	err = sw_translate_error(self->m_desc != SW_INVALID_SOCKET, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_multicast_socket_super_init(
               sw_socket	self)
{
	int			opt = 1;
	int			res;
	sw_result	err;
   
	err = sw_socket_init(self, SW_FALSE, &sw_socket_udp_connect, &sw_socket_udp_send, &sw_socket_udp_sendto, &sw_socket_udp_recv, &sw_socket_udp_recvfrom, &sw_socket_udp_close);
	sw_check_okay(err, exit);

	self->m_desc = socket(AF_INET, SOCK_DGRAM, 0);
	err = sw_translate_error(self->m_desc != SW_INVALID_SOCKET, sw_socket_errno());
	sw_check_okay_log(err, exit);

#if defined(__APPLE__) || defined(__VXWORKS__) || defined(__FreeBSD__) || defined(__NetBSD__)
   res = setsockopt(self->m_desc, SOL_SOCKET, SO_REUSEPORT, (char*) &opt, sizeof(opt));
#else
	res = setsockopt(self->m_desc, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt));
#endif

	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_socket_super_fina(
               sw_socket self)
{
	if (self != NULL)
	{
		sw_socket_close(self);
	}
   
   return SW_OKAY;
}


sw_ipv4_address
sw_socket_ipv4_address(
					sw_socket self)
{
	sw_ipv4_address address;

	sw_ipv4_address_init_from_saddr(&address, self->m_addr.sin_addr.s_addr);

	return address;
}


sw_port
sw_socket_port(
					sw_socket self)
{
	return ntohs(self->m_addr.sin_port);
}


sw_sockdesc_t
sw_socket_desc(
					sw_socket self)
{
	return self->m_desc;
}


sw_result
sw_socket_bind(
               sw_socket			self,
					sw_ipv4_address	address,
               sw_port			port)
{
	sw_int8			host[16];
   sw_socklen_t	len;
	int				res;
	sw_result		err = SW_OKAY;
   
	sw_debug(SW_LOG_VERBOSE, "sw_socket_bind() : fd = %d, addr = %s, port = %d\n", self->m_desc, sw_ipv4_address_name(address, host, 16), port);

   /*
      now bind
   */
	sw_memset(&self->m_addr, 0, sizeof(self->m_addr));
	self->m_addr.sin_family	=	AF_INET;
   
   /*
      if they haven't chosen a network interface, choose one for them
   */
	self->m_addr.sin_addr.s_addr	= 	sw_ipv4_address_saddr(address);
	self->m_addr.sin_port			=	htons(port);
   len									=	sizeof(self->m_addr);
	res = bind(self->m_desc, (struct sockaddr*) &self->m_addr, len);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
   
   /*
      now get our port
   */
   res = getsockname(self->m_desc, (struct sockaddr*) &self->m_addr, &len);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_socket_join_multicast_group(
               sw_socket			self,
					sw_ipv4_address	local_address,
					sw_ipv4_address	multicast_address,
               sw_uint32			ttl)
{
	struct in_addr				addr;
   struct ip_mreq				mreq;	
#if defined(__sun) || defined(__VXWORKS__) || defined(__NetBSD__) || defined(__OpenBSD__)
	sw_uint8					real_ttl				= (sw_uint8) ttl;
#else
	sw_uint32					real_ttl				= ttl;
#endif
	int							res;
	sw_result					err;

   /*
      initialize the group membership
   */
	sw_memset(&addr, 0, sizeof(addr));
	addr.s_addr = sw_ipv4_address_saddr(local_address);
   sw_memset(&mreq, 0, sizeof(mreq));

#if defined(__VXWORKS__)

	/*
		VxWorks doesn't like it if we try and use INADDR_ANY for use in
		setting up multicast sockets.

		So we please it.
	*/
	if (sw_ipv4_address_is_any(local_address))
	{
		sw_ipv4_address current_address;

		err = sw_ipv4_address_init_from_this_host(&current_address);
		sw_check_okay(err, exit);

		mreq.imr_interface.s_addr	= sw_ipv4_address_saddr(current_address);
	}
	else
	{
		mreq.imr_interface = addr;
	}

#else

   mreq.imr_interface = addr;

#endif

	mreq.imr_multiaddr.s_addr = sw_ipv4_address_saddr(multicast_address);

	res = setsockopt(self->m_desc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

	res = setsockopt(self->m_desc, IPPROTO_IP, IP_MULTICAST_IF, (char*) &addr, sizeof(addr));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

	res = setsockopt(self->m_desc, IPPROTO_IP, IP_TTL, (char*) &ttl, sizeof(ttl));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

	res = setsockopt(self->m_desc, IPPROTO_IP, IP_MULTICAST_TTL, (char*) &real_ttl, sizeof(real_ttl));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_socket_listen(
               sw_socket	self,
               int			qsize)
{
	int			res;
	sw_result	err;

	res = listen(self->m_desc, qsize);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
	
exit:

	return err;
}


sw_result
sw_socket_connect(
               sw_socket			self,
               sw_ipv4_address	address,
               sw_port			port)
{
   return (*self->m_connect_func)(self, address, port);
}


sw_result
sw_socket_accept(
               sw_socket		self,
               sw_socket	*	socket)
{
   struct sockaddr_in	addr;
   sw_sockdesc_t			desc;
   sw_socklen_t			len;
	int						nodelay = 1;
	struct linger			l;
	int						res;
	sw_result				err;
   
	len = sizeof(addr);
   sw_memset(&addr, 0, sizeof(addr));
	desc = accept(self->m_desc, (struct sockaddr*) &addr, &len);
	err = sw_translate_error(desc != SW_INVALID_SOCKET, sw_socket_errno());
	sw_check_okay(err, exit);

	res = setsockopt(desc, IPPROTO_TCP, TCP_NODELAY, (char*) &nodelay, sizeof(nodelay));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
 
	l.l_onoff	= 0;
	l.l_linger	= 0;

	res = setsockopt(desc, SOL_SOCKET, SO_LINGER, (char*) &l, sizeof(l));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

	err = sw_tcp_socket_init_with_desc(socket, desc);
	sw_check_okay(err, exit);

exit:

	return err;
}
                                    

sw_result
sw_socket_send(
               sw_socket		self,
               sw_octets		buffer,
               sw_size_t		len,
					sw_size_t	*	bytesWritten)
{
   return (*self->m_send_func)(self, buffer, len, bytesWritten);
}


sw_result
sw_socket_sendto(
					sw_socket			self,
					sw_octets			buffer,
					sw_size_t			len,
					sw_size_t		*	bytesWritten,
					sw_ipv4_address	to,
					sw_port			to_port)
{
	return (*self->m_sendto_func)(self, buffer, len, bytesWritten, to, to_port);
}


sw_result
sw_socket_recv(
               sw_socket		self,
               sw_octets		buffer,
               sw_size_t		max,
               sw_size_t	*	len)
{
   return (*self->m_recv_func)(self, buffer, max, len);
}


sw_result
sw_socket_recvfrom(
               sw_socket				self,
               sw_octets				buffer,
               sw_size_t				max,
               sw_size_t			*	len,
					sw_ipv4_address	*	from,
					sw_port			*	from_port,
					sw_ipv4_address	*	dest,
					sw_uint32			*	interface_index)
{
   return (*self->m_recvfrom_func)(self, buffer, max, len, from, from_port, dest, interface_index);
}


sw_result
sw_socket_set_blocking_mode(
				sw_socket	self,
				sw_bool		blocking_mode)
{
	sw_uint32	i = (blocking_mode) ? 0 : 1;
	int			res;
	sw_result	err;

#if defined(WIN32)
	res = ioctlsocket(self->m_desc, FIONBIO, &i);
#else
	res = ioctl(self->m_desc, FIONBIO, &i);
#endif

	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_socket_set_options(
               sw_socket				self,
               sw_socket_options		options)
{   
	int			res;
	sw_result	err = SW_OKAY;

   if (options->m_debug.m_modified)
   {   
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_DEBUG, (char*) &options->m_debug.m_val, sizeof(options->m_debug.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_debug.m_modified = SW_FALSE;
   }
   
#if !defined(__VXWORKS__)
   if (options->m_nodelay.m_modified)
   {      
      res = setsockopt(self->m_desc, IPPROTO_TCP, TCP_NODELAY, (char*) &options->m_nodelay.m_val, sizeof(options->m_nodelay.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_nodelay.m_modified = SW_FALSE;
   }
#endif
   
   if (options->m_dontroute.m_modified)
   {      
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_DONTROUTE, (char*) &options->m_dontroute.m_val, sizeof(options->m_dontroute.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_dontroute.m_modified = SW_FALSE;
   }
   
   if (options->m_keepalive.m_modified)
   {      
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_KEEPALIVE, (char*) &options->m_keepalive.m_val, sizeof(options->m_keepalive.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_keepalive.m_modified = SW_FALSE;
   }

   if (options->m_linger.m_modified)
   {
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_LINGER, (char*) &options->m_linger.m_val, sizeof(options->m_linger.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_linger.m_modified = SW_FALSE;
   }

   if (options->m_reuseaddr.m_modified)
   {      
#if defined(__APPLE__) || defined(__VXWORKS__)
		res = setsockopt(self->m_desc, SOL_SOCKET, SO_REUSEPORT, (char*) &options->m_reuseaddr.m_val, sizeof(options->m_reuseaddr.m_val));
#else
		res = setsockopt(self->m_desc, SOL_SOCKET, SO_REUSEADDR, (char*) &options->m_reuseaddr.m_val, sizeof(options->m_reuseaddr.m_val));
#endif
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_reuseaddr.m_modified = SW_FALSE;
   }

   if (options->m_sndbuf.m_modified)
   {      
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_SNDBUF, (char*) &options->m_sndbuf.m_val, sizeof(options->m_sndbuf.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_sndbuf.m_modified = SW_FALSE;
   }

   if (options->m_rcvbuf.m_modified)
   {      
      res = setsockopt(self->m_desc, SOL_SOCKET, SO_RCVBUF, (char*) &options->m_rcvbuf.m_val, sizeof(options->m_rcvbuf.m_val));
		err = sw_translate_error(res == 0, sw_socket_errno());
		sw_check_okay_log(err, exit);
      options->m_rcvbuf.m_modified = SW_FALSE;
   }

exit:

	return err;
}


sw_result
sw_socket_close(
               sw_socket	self)
{
   return (*self->m_close_func)(self);
}


static sw_result
sw_socket_init(
               sw_socket					self,
               sw_bool						connected,
               sw_socket_connect_func	connect_func,
               sw_socket_send_func		send_func,
					sw_socket_sendto_func	sendto_func,
               sw_socket_recv_func		recv_func,
					sw_socket_recvfrom_func	recvfrom_func,
               sw_socket_close_func		close_func)
{
	self->m_connected			=	connected;
   self->m_connect_func		=	connect_func;
   self->m_send_func			=	send_func;
	self->m_sendto_func		=	sendto_func;
   self->m_recv_func			=	recv_func;
	self->m_recvfrom_func	=	recvfrom_func;
   self->m_close_func		=	close_func;
	self->m_desc				=	SW_INVALID_SOCKET;
   
   return SW_OKAY;
}


/*
	TCP implementation
*/
static sw_result
sw_socket_tcp_connect(
               sw_socket			self,
               sw_ipv4_address	address,
               sw_port			port)
{
	sw_int8			host[16];
   sw_socklen_t	len;
	int				nodelay = 1;
	struct linger	l;
	int				res;
	sw_result		err;
   
	sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_connect() : host = %s, port = %d\n", sw_ipv4_address_name(address, host, 16), port);

   sw_memset(&self->m_dest_addr, 0, sizeof(self->m_dest_addr));
	self->m_dest_addr.sin_family		=	AF_INET;
	self->m_dest_addr.sin_addr.s_addr = 	sw_ipv4_address_saddr(address);
	self->m_dest_addr.sin_port			=	htons(port);

	res = connect(self->m_desc, (struct sockaddr*) &self->m_dest_addr, sizeof(self->m_dest_addr));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

	len = sizeof(self->m_addr);

	res = getsockname(self->m_desc, (struct sockaddr*) &self->m_addr, &len);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
   
   self->m_connected = SW_TRUE;

	res = setsockopt(self->m_desc, IPPROTO_TCP, TCP_NODELAY, (char*) &nodelay, sizeof(nodelay));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
 
	l.l_onoff	= 0;
	l.l_linger	= 0;

	res = setsockopt(self->m_desc, SOL_SOCKET, SO_LINGER, (char*) &l, sizeof(l));
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);
      
exit:

	return err;
}


static sw_result
sw_socket_tcp_send(
               sw_socket		self,
               sw_octets		bytes,
               sw_size_t		len,
					sw_size_t	*	bytesWritten)
{
	int			res;
	sw_result	err;
   
   sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_send() entering: fd = %d\n", self->m_desc);

#if defined(WIN32)
	res = send(self->m_desc, (const char*) bytes, (int) len, 0);
#else
	while (((res = send(self->m_desc, bytes, len, 0)) == -1) && (errno == EINTR));
#endif

	err = sw_translate_error(res != SW_SOCKET_ERROR, sw_socket_errno());
	sw_check_okay_log(err, exit);

	*bytesWritten = res;
   
   sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_send() sent: %d bytes on fd %d\n", res, self->m_desc);

exit:

	return err;
}


static sw_result
sw_socket_tcp_sendto(
               sw_socket			self,
               sw_octets			buffer,
               sw_size_t			len,
					sw_size_t		*	bytesWritten,
					sw_ipv4_address	to,
					sw_port				to_port)
{
	SW_UNUSED_PARAM(to);
	SW_UNUSED_PARAM(to_port);

	return sw_socket_tcp_send(self, buffer, len, bytesWritten);
}


static sw_result
sw_socket_tcp_recv(
               sw_socket		self,
               sw_octets		buffer,
               sw_size_t		max,
               sw_size_t	*	len)
{
	int			res;
	sw_result	err;

   sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_recv() entering: fd = %d, buffer = %x, max = %d)\n", self->m_desc, buffer, max);

#if defined(WIN32)
	res = recv(self->m_desc, (char*) buffer, (int) max, 0);
#else
	while (((res = recv(self->m_desc, buffer, max, 0)) == -1) && (errno == EINTR));
#endif

	err = sw_translate_error((res != SW_SOCKET_ERROR) || (sw_socket_errno() == SW_E_WOULDBLOCK), sw_socket_errno());
	sw_check_okay_log(err, exit);

	if (res > 0)
	{
		*len = res;
	}
	else
	{
		if (res == 0)
		{
			err = SW_E_EOF;
		}

		*len = 0;
	}
	
	sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_recv() received: %d bytes on fd %d\n", *len, self->m_desc);

exit:

	return err;
}


static sw_result
sw_socket_tcp_recvfrom(
               sw_socket				self,
               sw_octets				buffer,
               sw_size_t				max,
               sw_size_t			*	len,
					sw_ipv4_address	*	from,
					sw_port				*	from_port,
					sw_ipv4_address	*	dest,
					sw_uint32			*	interface_index)
{
	SW_UNUSED_PARAM(dest);
	SW_UNUSED_PARAM(interface_index);

	sw_ipv4_address_init_from_saddr(from, self->m_dest_addr.sin_addr.s_addr);
	*from_port = ntohs(self->m_dest_addr.sin_port);
	return sw_socket_tcp_recv(self, buffer, max, len);
}


static sw_result
sw_socket_tcp_close(
               sw_socket	self)
{
	int			res;
	sw_result	err;

	sw_debug(SW_LOG_VERBOSE, "sw_socket_tcp_close() : fd = %d\n", self->m_desc);

#if defined(WIN32)
	res = shutdown(self->m_desc, SD_SEND);
#else
	res = shutdown(self->m_desc, 2);

	/*
	 * Patch from Michael (macallan18@earthlink.net)
	 * 
	 * we should only fail here if errno isn't ENOTCONN or ENOENT ( Solaris 9 sets errno to ENOENT 
	 * if the socket isn't connected, despite the man page for shutdown saying it would use
	 * ENOTCONN )
	 */
	err = sw_translate_error((res == 0) || (errno == ENOENT) || (errno == ENOTCONN), sw_socket_errno());
   sw_check_okay(err, exit);
#endif

	res = sw_close_socket(self->m_desc);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:
   
	self->m_connected = SW_FALSE;

	return err;
}


/*
   UDP/mcast implementation
*/
static sw_result
sw_socket_udp_connect(
               sw_socket			self,
               sw_ipv4_address	address,
               sw_port			port)
{      
   sw_memset(&self->m_dest_addr, 0, sizeof(self->m_dest_addr));
	self->m_dest_addr.sin_family			=	AF_INET;
	self->m_dest_addr.sin_addr.s_addr	=	sw_ipv4_address_saddr(address);
	self->m_dest_addr.sin_port				=	htons(port);
   self->m_connected							=	SW_TRUE;
   
   return SW_OKAY;
}


static sw_result
sw_socket_udp_send(
               sw_socket		self,
               sw_octets		buffer,
               sw_size_t		size,
					sw_size_t	*	bytesWritten)
{
	return sw_socket_udp_really_sendto(self, buffer, size, bytesWritten, (struct sockaddr*) &self->m_dest_addr, sizeof(self->m_dest_addr));
}


static sw_result
sw_socket_udp_sendto(		
               sw_socket			self,
               sw_octets			buffer,
               sw_size_t			len,
					sw_size_t		*	bytesWritten,
					sw_ipv4_address	to,
					sw_port			port)

{
	struct sockaddr_in addr;

	sw_memset(&addr, 0, sizeof(addr));
	addr.sin_family		= 	AF_INET;
	addr.sin_addr.s_addr	=	sw_ipv4_address_saddr(to);
	addr.sin_port			=	htons(port);
	
	return sw_socket_udp_really_sendto(self, buffer, len, bytesWritten, (struct sockaddr*) &addr, sizeof(addr));
}


static sw_result
sw_socket_udp_really_sendto(
               sw_socket				self,
               sw_octets				buffer,
               sw_size_t				len,
					sw_size_t			*	bytesWritten,
					struct sockaddr	*	to,
					sw_socklen_t			to_len)
{
	int			res;
	sw_result	err;

	struct sockaddr_in * addr = (struct sockaddr_in*) to;

   sw_debug(SW_LOG_VERBOSE, "entering sw_socket_udp_really_sendto: dest %s %d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
   
#if defined(WIN32)
	res = sendto(self->m_desc, (const char*) buffer, (int) len, 0, to, to_len);
#else
	while (((res = sendto(self->m_desc, buffer, len, 0, to, to_len)) == -1) && (errno == EINTR));
#endif

	err = sw_translate_error(res != SW_SOCKET_ERROR, sw_socket_errno());
	sw_check_okay_log(err, exit);

	*bytesWritten = res;
  
	sw_debug(SW_LOG_VERBOSE, "sw_socket_udp_really_sendto: sent %d bytes\n", res);

exit:

	return err;
}


static sw_result
sw_socket_udp_recv(
               sw_socket		self,
               sw_octets		buffer,
               sw_size_t		max,
               sw_size_t	*	len)
{
   struct sockaddr_in	from;
	struct in_addr			dest;
	sw_uint32				interface_index;

	return sw_socket_udp_really_recvfrom(self, buffer, max, len, (struct sockaddr*) &from, sizeof(from), &dest, &interface_index);
}


static sw_result
sw_socket_udp_recvfrom(
					sw_socket				self,
					sw_octets				buffer,
					sw_size_t				max,
					sw_size_t			*	len,
					sw_ipv4_address	*	from,
					sw_port				*	from_port,
					sw_ipv4_address	*	dest,
					sw_uint32			*	interface_index)
{
	struct sockaddr_in	from_addr;
	struct in_addr			dest_addr;
	sw_result				err;

	err = sw_socket_udp_really_recvfrom(self, buffer, max, len, (struct sockaddr*) &from_addr, sizeof(from_addr), &dest_addr, interface_index);
	sw_check_okay(err, exit);

	sw_ipv4_address_init_from_saddr(from, from_addr.sin_addr.s_addr);
	*from_port	= ntohs(from_addr.sin_port);  

	if (dest)
	{
		sw_ipv4_address_init_from_saddr(dest, dest_addr.s_addr);
	}

exit:

	return err;
}


static sw_result
sw_socket_udp_really_recvfrom(
               sw_socket				self,
               sw_octets				buffer,
               sw_size_t				max,
               sw_size_t			*	len,
					struct sockaddr	*	from,
					sw_socklen_t			from_len,
					struct in_addr		*	dest,
					sw_uint32			*	interface_index)
{
	int			res;
	sw_result	err;

	SW_UNUSED_PARAM(interface_index);

	sw_assert(len);
	sw_assert(from);
	sw_assert(dest);

#if defined(WIN32)
	res = recvfrom(self->m_desc, (char*) buffer, (int) max, 0, from, &from_len);
#else
	while (((res = recvfrom(self->m_desc, buffer, max, 0, from, &from_len)) == -1) && (errno == EINTR));
#endif

	err = sw_translate_error(res != SW_SOCKET_ERROR, sw_socket_errno());
	sw_check_okay_log(err, exit);

   *len = res;

   sw_debug(SW_LOG_VERBOSE, "sw_socket_udp_recv: received %d bytes\n", res);

exit:

	return err;
}


static sw_result
sw_socket_udp_close(
               sw_socket	self)
{
	int			res;
	sw_result	err;

	res = sw_close_socket(self->m_desc);
	err = sw_translate_error(res == 0, sw_socket_errno());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_socket_options_init(
               sw_socket_options	*	options)
{
	sw_result err = SW_OKAY;

	*options = (sw_socket_options) sw_malloc(sizeof(struct _sw_socket_options));
	err = sw_translate_error(*options, SW_E_MEM);
	sw_check_okay_log(err, exit);
   
   sw_memset(*options, 0, sizeof(struct _sw_socket_options));

exit:

	return err;
}
                                    

sw_result
sw_socket_options_fina(
               sw_socket_options	options)
{
   sw_free(options);
   
   return SW_OKAY;
}
                                    
                                    
sw_result
sw_socket_options_set_debug(
               sw_socket_options	options,
               sw_bool				val)
{
   options->m_debug.m_val 			= val;
   options->m_debug.m_modified	=	SW_TRUE;
   return SW_OKAY;
}
                               
                               
sw_result
sw_socket_options_set_nodelay(
               sw_socket_options	options,
               sw_bool				val)
{
   options->m_nodelay.m_val 		= val;
   options->m_nodelay.m_modified	=	SW_TRUE;
   return SW_OKAY;
}
                                    
                                    
sw_result
sw_socket_options_set_dontroute(
               sw_socket_options	options,
               sw_bool				val)
{
   options->m_dontroute.m_val			=	val;
   options->m_dontroute.m_modified	=	SW_TRUE;
   return SW_OKAY;
}
                                    
                             
sw_result
sw_socket_options_set_keepalive(
               sw_socket_options	options,
               sw_bool				val)
{
   options->m_keepalive.m_val			=	val;
   options->m_keepalive.m_modified	=	SW_TRUE;
   return SW_OKAY;
}


sw_result
sw_socket_options_set_linger(
               sw_socket_options	options,
               sw_bool				val,
               sw_uint32				linger)
{
   options->m_linger.m_val.l_onoff	=	val;
   options->m_linger.m_val.l_linger	=	(sw_uint16) linger;
   options->m_linger.m_modified		=	SW_TRUE;
   return SW_OKAY;
}
                                    
                                    
sw_result
sw_socket_options_set_reuseaddr(
               sw_socket_options	options,
               sw_bool				val)
{
   options->m_reuseaddr.m_val			=	val;
   options->m_reuseaddr.m_modified	=	SW_TRUE;
   return SW_OKAY;
}


sw_result
sw_socket_options_set_sndbuf(
               sw_socket_options	options,
               sw_uint32				val)
{
   options->m_sndbuf.m_val			=	val;
   options->m_sndbuf.m_modified	=	SW_TRUE;
   return SW_OKAY;
}


sw_result
sw_socket_options_set_rcvbuf(
               sw_socket_options	options,
               sw_uint32				val)
{
   options->m_rcvbuf.m_val			=	val;
   options->m_rcvbuf.m_modified	=	SW_TRUE;
   return SW_OKAY;
}

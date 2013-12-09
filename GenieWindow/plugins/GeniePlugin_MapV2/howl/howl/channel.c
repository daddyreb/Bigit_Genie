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

#include "channel_i.h"
#include "message_i.h"
#include "buffer_i.h"
#include "orb_i.h"
#include <corby/corby.h>
#include <salt/socket.h>
#include <salt/debug.h>
#include <stdio.h>

/*
   private declarations
*/
static sw_corby_channel	g_channel_cache;

static sw_result
sw_corby_channel_will_send(
								sw_corby_channel		self,
								sw_octets				bytes,
								sw_uint32					len);
					

static sw_result
sw_corby_channel_did_read(
								sw_corby_channel		self,
								sw_octets				bytes,
								sw_uint32					len);


static sw_result
sw_corby_channel_message_header(
                        sw_corby_channel		self,
                        sw_swop_message_type	type);
               
static sw_result
sw_corby_channel_parse_request(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
								sw_string			*	op,
								sw_uint32				*	op_len,
                        sw_corby_buffer	*	buffer);
                        
static sw_result
sw_corby_channel_parse_reply(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);

static sw_result
sw_corby_channel_parse_cancel_request(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);

static sw_result
sw_corby_channel_parse_locate_request(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);
                        
static sw_result
sw_corby_channel_parse_locate_reply(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);
                        
static sw_result
sw_corby_channel_parse_locate_forward(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);
                        
static sw_result
sw_corby_channel_parse_close_connection(
                        sw_corby_channel		self,
                        sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer);
                        
static sw_result
sw_corby_channel_message_error(
                        sw_corby_channel		self);

static sw_uint32
sw_corby_channel_request_id();


sw_result
sw_corby_channel_init(
                        sw_corby_channel		*	self,
								struct _sw_corby_orb	*	orb,
								sw_socket					socket,
								sw_socket_options			options,
								sw_size_t					bufsize)
{
	sw_result err;

 	*self = (sw_corby_channel) sw_malloc(sizeof(struct _sw_corby_channel));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*self, 0, sizeof(struct _sw_corby_channel));

   if (options)
   {
		err = sw_socket_set_options(socket, options);
		sw_check_okay(err, exit);
	}
   
	err = sw_ipv4_address_init(&(*self)->m_from);
	sw_check_okay(err, exit);

	(*self)->m_orb		=	orb;
   (*self)->m_socket	=	socket;
	(*self)->m_refs	=	1;
	(*self)->m_state	=	Waiting;
   
	err = sw_corby_message_init(&(*self)->m_message);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_init_with_size(&(*self)->m_send_buffer, (bufsize) ? bufsize : SW_CORBY_CHANNEL_DEFAULT_BUFFER_SIZE);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_init_with_size(&(*self)->m_read_buffer, (bufsize) ? bufsize : SW_CORBY_CHANNEL_DEFAULT_BUFFER_SIZE);
	sw_check_okay(err, exit);

exit:

	if (err && *self)
	{
		sw_corby_channel_fina(*self);
	}
   
   return err;
}


sw_result
sw_corby_channel_init_with_profile(
                        sw_corby_channel		*	self,
								struct _sw_corby_orb	*	orb,
								sw_const_corby_profile	profile,
								sw_socket_options			options,
								sw_size_t					bufsize)
{
	sw_corby_channel	channel;
	sw_socket			socket;
	sw_result			err;

	/*
	 * initialize
	 */
	socket	=	NULL;
	*self		=	NULL;
	err		=	SW_OKAY;

	/*
	 * let's try and see if we already have a connection to this guy
	 */
	for (channel = orb->m_channel_cache; channel; channel = channel->m_nextc)
	{
		if ((channel->m_to_tag == profile->m_tag) && sw_ipv4_address_equals(channel->m_to, profile->m_address) && (channel->m_to_port == profile->m_port))
		{
			sw_int8 name_buf[16];
			
			sw_debug(SW_LOG_NOTICE, "sharing connection to %s, %d\n", sw_ipv4_address_name(profile->m_address, name_buf, 16), profile->m_port);
			
			/*
			 * found it
			 */
			channel->m_cache_refs++;
			channel->m_refs++;
			*self = channel;
			goto exit;
		}
	}

	/*
	 * couldn't find it, so make a new one
	 */
	switch (profile->m_tag)
	{
		case SW_TAG_INTERNET_IOP:
      
			err = sw_tcp_socket_init(&socket);
			sw_check_okay(err, exit);

         break;
         
		case SW_TAG_UIOP:
      
			err = sw_udp_socket_init(&socket);
			sw_check_okay(err, exit);

			err = sw_socket_bind(socket, sw_ipv4_address_any(), 0);
			sw_check_okay(err, exit);
         
			break;
         
		case SW_TAG_MIOP:
      
			err = sw_multicast_socket_init(&socket);
			sw_check_okay(err, exit);
         
			err = sw_socket_bind(socket, sw_ipv4_address_any(), profile->m_port);
			sw_check_okay(err, exit);
         
			break;

		default:
			err = SW_E_INIT;
			goto exit;
	}
	
	/*
	 * now let's try to connect...for connectionless protocols it just stashes this info to be used later
	 */
	err = sw_socket_connect(socket, profile->m_address, profile->m_port);
	sw_check_okay(err, exit);

	err = sw_corby_channel_init(self, orb, socket, options, bufsize);
	sw_check_okay(err, exit);

	/*
	 * if we connected okay, then add to list of extant channels
	 */
	err = sw_ipv4_address_init_from_address(&(*self)->m_to, profile->m_address);
	sw_check_okay(err, exit);

	(*self)->m_to_tag				=	profile->m_tag;
	(*self)->m_to_port			=	profile->m_port;
	(*self)->m_send_queue_head	=	NULL;
	(*self)->m_send_queue_tail	=	NULL;
	(*self)->m_nextc				=	g_channel_cache;
	(*self)->m_prevc				=	NULL;
	
	if (orb->m_channel_cache)
	{
		orb->m_channel_cache->m_prevc	= (*self);
	}
	
	orb->m_channel_cache = (*self);
	(*self)->m_cache_refs++;

exit:

	if (err != SW_OKAY)
	{
		if (*self != NULL)
		{
			sw_corby_channel_fina(*self);
		}
		else if (socket != NULL)
		{
			sw_socket_fina(socket);
		}
	}

	return err;
}


sw_result
sw_corby_channel_fina(
							sw_corby_channel	self)
{
	sw_assert(self != NULL);

	if (self != NULL)
	{
		sw_debug(SW_LOG_VERBOSE, "sw_corby_channel_fina() : reference count = %d\n", self->m_refs);

		sw_assert(self->m_cache_refs <= self->m_refs);

		if (self->m_orb)
		{
			if ((self->m_cache_refs != 0) && (--self->m_cache_refs == 0))
			{
				if (self->m_prevc == NULL)
				{
					self->m_orb->m_channel_cache = self->m_nextc;
	
					if (self->m_nextc != NULL)
					{
						self->m_nextc->m_prevc = NULL;
					}
				}
				else if (self->m_nextc == NULL)
				{
					self->m_prevc->m_nextc = NULL;
				}
				else
				{
					self->m_prevc->m_nextc = self->m_nextc;
					self->m_nextc->m_prevc = self->m_prevc;
				}
			}
		}

		if (--self->m_refs == 0)
		{
			if (self->m_delegate && self->m_delegate->m_cleanup_func)
			{
				(*self->m_delegate->m_cleanup_func)(self);
			}

			sw_corby_message_fina(self->m_message);
			sw_corby_buffer_fina(self->m_send_buffer);
			sw_corby_buffer_fina(self->m_read_buffer);
			sw_socket_fina(self->m_socket);

			sw_free(self);
		}
	}
	
   return SW_OKAY;
}


sw_result
sw_corby_channel_start_request(
                        sw_corby_channel			self,
								sw_const_corby_profile	profile,
                        sw_corby_buffer		*	buffer,
								sw_const_string			op,
                        sw_uint32					oplen,
                        sw_bool						reply_expected)
{
   sw_uint32 request_id;
	sw_result	err;

   /*
	 * reset my pointers
	 */
   self->m_send_buffer->m_bptr	= self->m_send_buffer->m_base;
   self->m_send_buffer->m_eptr	= self->m_send_buffer->m_base;
   
   /*
      blop out the message header
   */
   err = sw_corby_channel_message_header(self, SW_SWOP_REQUEST);
	sw_check_okay(err, exit);

	/*
		service context
	*/
	err = sw_corby_buffer_put_uint32(self->m_send_buffer, 0);
	sw_check_okay(err, exit);

	/*
		request id
	*/
	request_id = sw_corby_channel_request_id();
	err = sw_corby_buffer_put_uint32(self->m_send_buffer, request_id);
	sw_check_okay(err, exit);

	/*
		reply expected
	*/
	err = sw_corby_buffer_put_uint8(self->m_send_buffer, reply_expected);
	sw_check_okay(err, exit);

	/*
		object key
	*/
	err = sw_corby_buffer_put_sized_octets(self->m_send_buffer, (sw_const_octets) profile->m_oid, profile->m_oid_len);
	sw_check_okay(err, exit);

	/*
		operation
	*/
	err = sw_corby_buffer_put_sized_octets(self->m_send_buffer, (sw_octets) op, oplen);
	sw_check_okay(err, exit);

	/*
		principal
	*/
	err = sw_corby_buffer_put_uint32(self->m_send_buffer, 0);
	sw_check_okay(err, exit);

   *buffer = self->m_send_buffer;

exit:

	if (err != SW_OKAY)
	{
		*buffer = NULL;
	}

   return err;
}


sw_result
sw_corby_channel_start_reply(
               sw_corby_channel			self,
               sw_corby_buffer		*	buffer,
               sw_uint32					request_id,
               sw_corby_reply_status	status)
{
	sw_result err;

	/*
	 * don't do this if this guy isn't expecting a reply
	 */
	sw_check(self->m_message->m_body.m_request_header.m_reply_expected, exit, err = SW_E_UNKNOWN);
	
   /*
      blop the SWOP message header
   */
	err = sw_corby_channel_message_header(self, SW_SWOP_REPLY);
	sw_check_okay(err, exit);
	
	/*
		service context list
	*/
	err = sw_corby_buffer_put_uint32(self->m_send_buffer, 0);
	sw_check_okay(err, exit);
	

	/*
		request id
	*/
	err = sw_corby_buffer_put_uint32(self->m_send_buffer, request_id);
	sw_check_okay(err, exit);

	/*
		status
	*/
   switch (status)
	{
		case SW_CORBY_NO_EXCEPTION:
			
			err = sw_corby_buffer_put_uint32(self->m_send_buffer, SW_CORBY_NO_EXCEPTION);
			sw_check_okay(err, exit);
			break;

		case SW_CORBY_SYSTEM_EXCEPTION:
			
			err = sw_corby_buffer_put_uint32(self->m_send_buffer, SW_CORBY_SYSTEM_EXCEPTION);
			sw_check_okay(err, exit);
			break;

		case SW_CORBY_USER_EXCEPTION:
			
         err = sw_corby_buffer_put_uint32(self->m_send_buffer, SW_CORBY_USER_EXCEPTION);
			sw_check_okay(err, exit);
			break;
			
		case SW_CORBY_LOCATION_FORWARD:
			
         /* unhandled */
			break;
	}

   *buffer = self->m_send_buffer;

exit:

	if (err != SW_OKAY)
	{
		*buffer = NULL;
	}
   
	return err;
}


sw_result
sw_corby_channel_send(
                        sw_corby_channel					self,
                        sw_corby_buffer					buffer,
								sw_corby_buffer_observer		observer,
								sw_corby_buffer_written_func	func,
								sw_opaque_t							extra)
{
	sw_size_t	bytes_written;
   sw_uint32	msg_size;
	sw_result	err;

	sw_assert(buffer == self->m_send_buffer);

	/*
		fill in the size of the message
	*/
	msg_size = (sw_uint32) (buffer->m_eptr - buffer->m_bptr);

	sw_corby_channel_will_send(self, buffer->m_base, msg_size);

	*((sw_uint32*) (buffer->m_base + 8))	= (msg_size - sizeof(struct _sw_swop_message_header));


	/*
	 * side effect buffer
	 */
	buffer->m_observer			=	observer;
	buffer->m_written_func		=	func;
	buffer->m_observer_extra	=	extra;

	/*
	 * check if we have anything in the send queue
	 */
	if (self->m_send_queue_head != NULL)
	{
		/*
		 * queue this buffer and return
		 */
		sw_corby_channel_queue_send_buffer(self, buffer);

		return SW_E_INPROGRESS;
	}

	/*
	 * else write it to socket
	 */
	err = sw_socket_send(self->m_socket, buffer->m_bptr, msg_size, &bytes_written);

	if (err != SW_OKAY)
	{
		if (err == SW_E_WOULDBLOCK)
		{
			/*
		 	 * register for write events
		    */
			sw_corby_orb_register_channel_events(self->m_orb, self, SW_SOCKET_WRITE|SW_SOCKET_READ);

			/*
			 * queue the buffer, and create a new send buffer
			 */
			sw_corby_channel_queue_send_buffer(self, buffer);

			err = SW_E_INPROGRESS;
		}

		goto exit;
	}

	/*
	 * check to see if we wrote the whole message
	 */
	if (bytes_written < msg_size)
	{
		buffer->m_bptr	+=	bytes_written;

		/*
	 	 * treat like EWOULDBLOCK
		 */
		sw_corby_orb_register_channel_events(self->m_orb, self, SW_SOCKET_WRITE|SW_SOCKET_READ);

		sw_corby_channel_queue_send_buffer(self, buffer);

		err = SW_E_INPROGRESS;

		goto exit;
	}
	
	/*
	 * if we got here, we successfully sent the whole message,
	 * so reset the pointers
	*/
	buffer->m_bptr	=	buffer->m_base;
	buffer->m_eptr	=	buffer->m_base;

exit:

	return err;
}


sw_result
sw_corby_channel_recv(
						sw_corby_channel		self,
						sw_salt				*	salt,
						sw_corby_message	*	message,
						sw_uint32			*	request_id,
						sw_string			*	op,
						sw_uint32			*	op_len,
						sw_corby_buffer	*	buffer,
						sw_uint8			*	endian,
						sw_bool				block)
{
	static sw_const_string
	message_type[] =
	{
		"Request",
		"Reply",
		"CancelRequest",
		"LocateRequest",
		"LocateReply",
		"CloseConnection",
		"MessageError"
	};

	sw_size_t	buflen;
	sw_size_t	len;
	sw_result	err = SW_OKAY;

	SW_UNUSED_PARAM(request_id);

   sw_debug(SW_LOG_VERBOSE, "entering sw_corby_channel_recv()\n");

	if (self->m_state == Waiting)
	{
		self->m_message->m_header = NULL;
	}

	/*
	 * side effect the salt reference
	 */
	if (salt)
	{
		(*salt) = self->m_orb->m_salt;
	}
   
   while (SW_TRUE)
   {
      buflen 	= (self->m_read_buffer->m_eptr - self->m_read_buffer->m_bptr);
 		*buffer	= NULL;
		
		if (message)
   	{
			*message = NULL;
  		}
   
		sw_debug(SW_LOG_VERBOSE, "  %s %s, buffer(m_base = %x, m_bptr = %x, m_eptr = %x, m_end = %x, buflen = %d)\n",
                     (block) ? "block" : "!block",
                     (self->m_message->m_header) ? "message_header" : "!message_header",
                     self->m_read_buffer->m_base,
                     self->m_read_buffer->m_bptr,
                     self->m_read_buffer->m_eptr,
                     self->m_read_buffer->m_end,
                     buflen);
      
      /*
         beginning a new message
      */
      if (self->m_message->m_header == NULL)
      {
			self->m_state = Reading;

         /*
            do we need to shift stuff down and reset pointers?
         */
         if (buflen && (self->m_read_buffer->m_bptr != self->m_read_buffer->m_base))
         {
				sw_debug(SW_LOG_VERBOSE, "  shifting buffer pointers %d bytes\n", buflen);
				sw_memcpy(self->m_read_buffer->m_base, self->m_read_buffer->m_bptr, buflen);
         }

         self->m_read_buffer->m_bptr = self->m_read_buffer->m_base;
         self->m_read_buffer->m_eptr = self->m_read_buffer->m_base + buflen;
         
         /*
            do we have enough to read a message header
         */
         if (buflen >= sizeof(struct _sw_swop_message_header))
         {
            self->m_message->m_header = (sw_swop_message_header*) self->m_read_buffer->m_base;

				sw_debug(SW_LOG_VERBOSE, "  SWOP magic = %c %c %c %c\n", 
                     self->m_message->m_header->m_magic[0],
                     self->m_message->m_header->m_magic[1],
                     self->m_message->m_header->m_magic[2],
                     self->m_message->m_header->m_magic[3]);

            /*
               check magic string
            */
            sw_check
						(
						((self->m_message->m_header->m_magic[0] == 'S') &&
						 (self->m_message->m_header->m_magic[1] == 'W') &&
						 (self->m_message->m_header->m_magic[2] == 'O') &&
						 (self->m_message->m_header->m_magic[3] == 'P')),
						exit,
						err = SW_E_CORBY_BAD_MESSAGE
						);
            
            sw_debug(SW_LOG_VERBOSE, "  SWOP version = %d %d\n", 
                     self->m_message->m_header->m_major,
                     self->m_message->m_header->m_minor);

            /*
               check version number
            */
				sw_check
						(
						((self->m_message->m_header->m_major <= SW_SWOP_MAJOR) &&
						 (self->m_message->m_header->m_minor <= SW_SWOP_MINOR)),
						exit,
						err = SW_E_CORBY_BAD_VERSION
						);
						
            sw_debug(SW_LOG_VERBOSE, "  SWOP endian = %d\n", 
                     self->m_message->m_header->m_endian);

				sw_debug(SW_LOG_VERBOSE, "  SWOP message type = %s\n", message_type[self->m_message->m_header->m_msg_type]);

            /*
               fix size
            */
            if (self->m_message->m_header->m_endian != SW_ENDIAN)
            {
               self->m_message->m_header->m_msg_size = SW_SWAP32(self->m_message->m_header->m_msg_size);
            }

            sw_debug(SW_LOG_VERBOSE, "  SWOP size = %d\n", 
                     self->m_message->m_header->m_msg_size);
 
            /*
               make sure we have enough buffer space
            */
            if (self->m_message->m_header->m_msg_size > ((self->m_read_buffer->m_end - self->m_read_buffer->m_base) - sizeof(struct _sw_swop_message_header)))
            {
               sw_uint32 new_size;
               
               new_size								= self->m_message->m_header->m_msg_size + sizeof(struct _sw_swop_message_header);
					self->m_read_buffer->m_base = (sw_octets) sw_realloc(self->m_read_buffer->m_base, new_size);
					sw_check(self->m_read_buffer->m_base, exit, err = SW_E_MEM);

               self->m_read_buffer->m_bptr		= self->m_read_buffer->m_base;
               self->m_read_buffer->m_eptr		= self->m_read_buffer->m_base + buflen;
               self->m_read_buffer->m_end		= self->m_read_buffer->m_base + new_size;
               self->m_message->m_header	= (struct _sw_swop_message_header*) self->m_read_buffer->m_base;
            }
            
            self->m_read_buffer->m_bptr += sizeof(struct _sw_swop_message_header);
            buflen -= sizeof(struct _sw_swop_message_header);
         }
      }

      /*
         do we have the whole message?
      */
      if (self->m_message->m_header && (self->m_message->m_header->m_msg_size <= buflen))
      {
			/*
			 * we've read the whole message, so we're now
			 * waiting for a new message
			 */
			self->m_state = Waiting;

			/*
 				side effect endian
 			*/
 			if (endian)
      	{
         	*endian = self->m_message->m_header->m_endian;
 			}

			sw_corby_channel_did_read(self, self->m_read_buffer->m_base, self->m_message->m_header->m_msg_size + SW_SWOP_HEADER_SIZE);

         switch (self->m_message->m_header->m_msg_type)
         {
            case SW_SWOP_REQUEST:

					return sw_corby_channel_parse_request(self, message, op, op_len, buffer);

            case SW_SWOP_REPLY:
      
					return sw_corby_channel_parse_reply(self, message, buffer);

				case SW_SWOP_CANCEL_REQUEST:

					return sw_corby_channel_parse_cancel_request(self, message, buffer);

            case SW_SWOP_LOCATE_REQUEST:

					return sw_corby_channel_parse_locate_request(self, message, buffer);

            case SW_SWOP_LOCATE_REPLY:

					return sw_corby_channel_parse_locate_reply(self, message, buffer);

            case SW_SWOP_CLOSE_CONNECTION:

					return sw_corby_channel_parse_close_connection(self, message, buffer);

            default:
      
               return sw_corby_channel_message_error(self);
         }
      }
      
      /*
         if we get here, then we need to read
      */
      if (block)
      {
			err = sw_socket_recvfrom(self->m_socket, self->m_read_buffer->m_eptr, self->m_read_buffer->m_end - self->m_read_buffer->m_eptr, &len, &self->m_from, &self->m_from_port, NULL, NULL);
			sw_check_okay(err, exit);

			/*
			 * returning 0 is fair because the socket might have been set up
			 * in non-blocking mode.  so if len == 0, we just return from
			 * this function gracefully
			 */
			if (len > 0)
			{
				self->m_read_buffer->m_eptr += len;
				buflen =	(self->m_read_buffer->m_eptr -  self->m_read_buffer->m_bptr);
			}
			else
			{
				break;
			}
      }
		else
		{
			break;
		}
	}

exit:

	return err;
}


sw_result
sw_corby_channel_last_recv_from(
							sw_corby_channel		channel,
							sw_ipv4_address	*	from,
							sw_port			*	from_port)
{
	sw_ipv4_address_init_from_address(from, channel->m_from);
	*from_port = channel->m_from_port;
	return SW_OKAY;
}


sw_result
sw_corby_channel_ff(
							sw_corby_channel		channel,
							sw_corby_buffer		buffer)
{
	sw_assert(channel);
	sw_assert(channel->m_message);
	sw_assert(channel->m_message->m_header);

	buffer->m_bptr = (buffer->m_base + channel->m_message->m_header->m_msg_size + SW_SWOP_HEADER_SIZE);

	return SW_OKAY;
}


sw_socket
sw_corby_channel_socket(
							sw_corby_channel	channel)
{
	return channel->m_socket;
}


sw_result
sw_corby_channel_retain(
							sw_corby_channel	channel)
{
	channel->m_refs++;
	return SW_OKAY;
}


sw_result
sw_corby_channel_set_delegate(
							sw_corby_channel				self,
							sw_corby_channel_delegate	delegate)
{
	self->m_delegate = delegate;
	return SW_OKAY;
}


sw_corby_channel_delegate
sw_corby_channel_get_delegate(
							sw_corby_channel				self)
{
	return self->m_delegate;
}


void
sw_corby_channel_set_app_data(
							sw_corby_channel				self,
							sw_opaque						app_data)
{
	self->m_app_data = app_data;
}


sw_opaque
sw_corby_channel_get_app_data(
							sw_corby_channel				self)
{
	return self->m_app_data;
}


#ifdef __cplusplus
}
#endif 

#if 0
sw_result
sw_corby_channel_locate_request(
                        sw_corby_channel	*	channel,
                        sw_addr	addr)
{
 sw_uint32_t_t request_id;

	if (swop_msg_put_msg_hdr(msg, SWOP_LOCATE_REQUEST) != 0)
	{
		return -1;
	}

	request_id = get_request_id();
	sw_msg_put_ulong(msg, request_id);
	sw_msg_put_seq(msg, msg->m_sock->m_oid.m_buf, msg->m_sock->m_oid.m_elems);

	msg->m_context.m_client.m_expected_type	= SWOP_LOCATE_REPLY;
	msg->m_context.m_client.m_request_id		= request_id;

	return 0;
}
#endif


static sw_result
sw_corby_channel_parse_request(
                        sw_corby_channel		channel,
                        sw_corby_message	*	message,
								sw_string			*	op,
								sw_uint32				*	op_len,
                        sw_corby_buffer	*	buffer)
{
   sw_swop_request_header	*	request_header;
   sw_uint32						dummy;
   sw_uint8							endian;
	sw_result						err;
   
   request_header	=	&channel->m_message->m_body.m_request_header;
   endian 			= 	channel->m_message->m_header->m_endian;

	/*
		service context list
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &dummy, endian);
	sw_check_okay(err, exit);

	if (dummy > 0)
	{
		/*
		 * we are going to ignore this as best we can
		 */
		for (; dummy > 0; dummy--)
		{
			sw_uint32	tag;
			sw_uint32 len;
			
			/*
			 * get tag
			 */
			err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &tag, endian);
			sw_check_okay(err, exit);
			
			/*
			 * get len
			 */
			err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &len, endian);
			sw_check_okay(err, exit);
			
			/*
			 * skip bytes..FIXME I'm not checking for underflow here
			 */
			channel->m_read_buffer->m_bptr += len;
		}
	}

	/*
		request id
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &request_header->m_request_id, endian);
	sw_check_okay(err, exit);

	/*
		reply expected
	*/
	err = sw_corby_buffer_get_uint8(channel->m_read_buffer, &request_header->m_reply_expected);
	sw_check_okay(err, exit);

	/*
		object key
	*/
	request_header->m_oid_len = 64;
   err = sw_corby_buffer_get_sized_octets(channel->m_read_buffer, request_header->m_oid, &request_header->m_oid_len, endian);
	sw_check_okay(err, exit);
	
	/*
		operation
	*/
	request_header->m_op_len = 64;
	err = sw_corby_buffer_get_cstring(channel->m_read_buffer, request_header->m_op, &request_header->m_op_len, endian);
	sw_check_okay(err, exit);

	/*
		principal
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &dummy, endian);
	sw_check_okay(err, exit);

   /*
      we're good
   */
   if (message)
   {
      (*message) = channel->m_message;
   }

	if (op)
	{
		(*op)	= request_header->m_op;
	}

	if (op_len)
	{
		(*op_len) = request_header->m_op_len;
	}

   (*buffer)	=	channel->m_read_buffer;

exit:

	return err;
}


static sw_result
sw_corby_channel_parse_reply(
                     sw_corby_channel		channel,
                     sw_corby_message	*	message,
                     sw_corby_buffer	*	buffer)
{
   sw_uint32					dummy;
   sw_uint32					request_id;
   sw_corby_reply_status	reply_status;
   sw_bool					endian;
	sw_result					err;
   
   endian = channel->m_message->m_header->m_endian;

   /*
		service context list
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &dummy, endian);
	sw_check_okay(err, exit);
				
	/*
		request id
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &request_id, endian);
	sw_check_okay(err, exit);

	/*
		reply status type
	*/
	err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &dummy, endian);
	sw_check_okay(err, exit);

   reply_status = (sw_corby_reply_status) dummy;
	
   switch (reply_status)
	{
      case SW_CORBY_NO_EXCEPTION:
		{
		}
		break;
         
		case SW_CORBY_LOCATION_FORWARD:
		{
         return sw_corby_channel_parse_locate_forward(channel, message, buffer);
		}
		break;

		case SW_CORBY_SYSTEM_EXCEPTION:
		{
			sw_int8		exception[256];
			sw_uint32		major;
			sw_uint32		minor;
			sw_uint32		len;
			
			len = 256;
			err = sw_corby_buffer_get_cstring(channel->m_read_buffer, exception, &len, endian);
			sw_check_okay(err, exit);

			err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &major, endian);
			sw_check_okay(err, exit);

			err = sw_corby_buffer_get_uint32(channel->m_read_buffer, &minor, endian);
			sw_check_okay(err, exit);
			
			if (minor == SW_OKAY)
			{
				minor = SW_E_CORBY_UNKNOWN;
			}
			
			return minor;
		}
		break;

		case SW_CORBY_USER_EXCEPTION:
		{
			return SW_E_CORBY_UNKNOWN;
		}
		break;
	}

   /*
      we're good
   */
   if (message)
   {
      (*message)	=	channel->m_message;
   }
   
   (*buffer)	=	channel->m_read_buffer;

exit:

	return err;
}


sw_result
sw_corby_channel_parse_cancel_request(
                              sw_corby_channel		channel,
                              sw_corby_message	*	message,
                              sw_corby_buffer	*	buffer)
{
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(buffer);

#if 0
 swop_cancel_request_header cancel_request_header;

	/*
		set context
	*/
	cancel_request_header = &msg->m_body.m_cancel_request_header;

	/*
		request id
	*/
	sw_msg_get_uint32(msg, &cancel_request_header->m_request_id);

	/*
		don't do anything with this right now
	*/
	return 0;
#endif
   return SW_OKAY;
}


static sw_result
sw_corby_channel_parse_locate_request(
                              sw_corby_channel		channel,
                              sw_corby_message	*	message,
                              sw_corby_buffer	*	buffer)
{
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(buffer);

#if 0
 swop_locate_request_header	locate_request_header;
 int									result;
 swop_server						server;
 int									error;
 sw_addr							addr;

	/*
		set the context
	*/
	locate_request_header	= &msg->m_body.m_locate_request_header;
	server						= (swop_server) msg->m_context.m_server.m_server;
	result						= -1;
	addr							= NULL;

	/*
		request id
	*/
	sw_msg_get_ulong(msg, &locate_request_header->m_request_id);

	/*
		object key
	*/
	sw_msg_get_ulong(msg, &locate_request_header->m_object_key.m_elems);
	locate_request_header->m_object_key.m_buf = msg->m_bptr;
	msg->m_bptr += locate_request_header->m_object_key.m_elems;

	/*
		get our answer
	*/
	error = server->m_cb(
							(sw_server) server, 
							locate_request_header->m_object_key, 
							NULL,
							0,
							NULL,
							&addr);


	/*
		put the locate reply message header in the buffer
	*/
	if (swop_msg_put_msg_hdr(msg, SWOP_LOCATE_REPLY) != 0)
	{
		goto done;
	}
		
	/*
		request id
	*/
	if (sw_msg_put_ulong(msg, locate_request_header->m_request_id) != 0)
	{
		goto done;
	}

	/*
		now check the answer
	*/
	if (!error)
	{
		sw_msg_put_ulong(msg, SWOP_OBJECT_HERE);
	}
	else if (addr != NULL)
	{
		sw_msg_put_ulong(msg, SWOP_OBJECT_FORWARD);
		sw_msg_put_addr(msg, addr);
	}
	else
	{
		sw_msg_put_ulong(msg, SWOP_UNKNOWN_OBJECT);
	}

	/*
		and send back a reply
	*/
	if (sw_msg_send(msg) != 0)
	{
		goto done;
	}

	/*
		if we got here, we're good
	*/
	result = 0;

done:

	sw_addr_free(addr);
	sw_msg_free(msg);
	return result;
#endif

   return SW_OKAY;
}


static sw_result
sw_corby_channel_parse_locate_reply(
                        sw_corby_channel		channel,
								sw_corby_message	*	message,
                        sw_corby_buffer	*	buffer)
{
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(buffer);

#if 0
 swop_locate_reply_header locate_reply_header;

	/*
		set the context
	*/
	locate_reply_header = &msg->m_body.m_locate_reply_header;

	/*
		request id
	*/
	sw_msg_get_ulong(msg, &locate_reply_header->m_request_id);

	/*
		locate request status
	*/
	sw_msg_get_ulong(msg, (sw_uint32_t_t*) &locate_reply_header->m_locate_status);

	/*
		now do some figuring
	*/
	switch (msg->m_body.m_locate_reply_header.m_locate_status)
	{
		case SWOP_OBJECT_HERE:
			break;
			
		case SWOP_UNKNOWN_OBJECT:
			{
				sw_error_from(E_SW_BAD_OBJECT);
				return -1;
			}
			break;

		case SWOP_OBJECT_FORWARD:
			{
				return swop_msg_parse_locate_forward(msg);
			}
			break;
			
		default:
			/* unhandled error */
			break;
	}

	return 0;
#endif

   return SW_OKAY;
}


sw_result
sw_corby_channel_parse_close_connection(
                                 sw_corby_channel		channel,
                                 sw_corby_message	*	message,
                                 sw_corby_buffer	*	buffer)
{
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(buffer);

   return SW_OKAY;
}


static sw_result
sw_corby_channel_message_error(
                                 sw_corby_channel		channel)
{
	SW_UNUSED_PARAM(channel);

#if 0
 sw_iop_ior ior;

	/*
		if client, pop the top ior unless the top is the bottom
	*/
	if ((msg->m_sock->m_service) &&
	    (ior = msg->m_sock->m_service->m_ior->m_after))
	{
	 sw_iop_ior old = msg->m_sock->m_service->m_ior;

		msg->m_sock->m_service->m_ior = ior;
		swop_ior_free(old);
	}

	return 0;
#endif

   return SW_OKAY;
}


static sw_result
sw_corby_channel_parse_locate_forward(
                              sw_corby_channel		channel,
                              sw_corby_message	*	message,
                              sw_corby_buffer	*	buffer)
{
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(buffer);

#if 0
 sw_addr addr;

	sw_assert(msg);
	sw_assert(msg->m_sock);
	sw_assert(msg->m_sock->m_service);

	if (sw_msg_get_addr(msg, &addr, PANDORA_DEEP) != 0)
	{
		return -1;
	}

	/*
		link the new one to the old one
	*/
	addr->m_ior->m_after = msg->m_sock->m_service->m_ior;
	msg->m_sock->m_service->m_ior = addr->m_ior;

	sw_error_from(E_SW_MESSAGE_RETRY);
	return -1;
#endif

   return SW_OKAY;
}


static sw_result
sw_corby_channel_will_send(
					sw_corby_channel	self,
					sw_octets			bytes,
					sw_uint32				len)
{
	if (self->m_delegate && self->m_delegate->m_will_send_func)
	{
		return (*self->m_delegate->m_will_send_func)(self, bytes, len, self->m_delegate->m_extra);
	}

	return SW_OKAY;
}
					

static sw_result
sw_corby_channel_did_read(
					sw_corby_channel	self,
					sw_octets			bytes,
					sw_uint32				len)
{
	if (self->m_delegate && self->m_delegate->m_did_read_func)
	{
		return (*self->m_delegate->m_did_read_func)(self, bytes, len, self->m_delegate->m_extra);
	}

	return SW_OKAY;
}


static sw_result
sw_corby_channel_message_header(
                        sw_corby_channel		channel,
                        sw_swop_message_type	type)
{
	sw_uint8 header[12] = { 'S', 'W', 'O', 'P', SW_SWOP_MAJOR, SW_SWOP_MINOR, SW_ENDIAN, type };

	/*
		(re)set pointers
	*/
	channel->m_send_buffer->m_bptr	=	channel->m_send_buffer->m_base;
	channel->m_send_buffer->m_eptr	=	channel->m_send_buffer->m_base;

	/*
		marshal the header
	*/
	sw_memcpy(channel->m_send_buffer->m_base, header, sizeof(header));
	channel->m_send_buffer->m_eptr += sizeof(header);
	
   return SW_OKAY;
}


static sw_uint32
sw_corby_channel_request_id()
{
   static sw_uint32 request_id;
   
   return ++request_id;
}


sw_result
sw_corby_channel_queue_send_buffer(
							sw_corby_channel	self,
							sw_corby_buffer	buffer)
{
	sw_assert(buffer == self->m_send_buffer);

	if (self->m_send_queue_head == NULL)
	{
		self->m_send_queue_head = buffer;
	}

	if (self->m_send_queue_tail != NULL)
	{
		self->m_send_queue_tail->m_next	=	buffer;
		buffer->m_next							=	NULL;
		self->m_send_queue_tail				=	buffer;
	}
	else
	{
		self->m_send_queue_tail = buffer;
	}

	return sw_corby_buffer_init_with_size(&self->m_send_buffer, sw_corby_buffer_size(buffer));
}


sw_result
sw_corby_channel_flush_send_queue(
							sw_corby_channel	self)
{
	sw_result err = SW_OKAY;

	while (self->m_send_queue_head)
	{
		sw_size_t			bytes_written;
		sw_corby_buffer	buffer	=	self->m_send_queue_head;
		sw_uint32			msg_size =	(sw_uint32) (buffer->m_eptr - buffer->m_bptr);

		err = sw_socket_send(self->m_socket, buffer->m_bptr, msg_size, &bytes_written);
		sw_check_okay(err, exit);

		if (bytes_written < msg_size)
		{
			buffer->m_bptr += bytes_written;
			continue;
		}

		self->m_send_queue_head	= buffer->m_next;

		if (buffer->m_written_func != NULL)
		{
			(buffer->m_written_func)(buffer->m_observer, buffer, SW_OKAY, msg_size, buffer->m_observer_extra);
		}

		sw_corby_buffer_fina(buffer);
	}

	/*
	 * if we got here, then there's nothing left on the list,
	 * so set the tail to NULL
	 */
	self->m_send_queue_tail = NULL;

exit:

	return err;
}

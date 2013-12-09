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

#include "orb_i.h"
#include "channel_i.h"
#include "message_i.h"
#include "channel_i.h"
#include "object_i.h"
#include <salt/address.h>
#include <salt/socket.h>
#include <salt/debug.h>
#include <stdio.h>
#include <ctype.h>

extern sw_uint16 g_port;

static sw_string	g_default_repository_id = "IDL:omg.org/CORBA/OBJECT:1.0";


/*
   private declarations
*/
static sw_result
sw_corby_orb_select(
							sw_socket_handler	handler,
							sw_salt				salt,
							sw_socket			socket,
							sw_socket_event	events,
							sw_opaque			extra);


static void
sw_corby_orb_handle_system_exception(
							sw_corby_orb					orb,
							sw_corby_channel				channel,
							sw_swop_request_header	*	request_header,
							sw_result						result);


static sw_result
sw_corby_orb_protocol_lookup(
							sw_corby_orb				orb,
							sw_const_string			tag,
							sw_corby_orb_protocol_spec	** spec,
							sw_string					addr,
							sw_port				*	port);

static int
is_wildcard(
			sw_const_string str);


/*
   implementation of orb server
*/
sw_result
sw_corby_orb_init(
            sw_corby_orb						*	self,
            sw_salt									salt,
            const sw_corby_orb_config		*	config,
				sw_corby_orb_observer				observer,
            sw_corby_orb_observer_func			func,
            sw_opaque_t								extra)
{
	sw_result	err = SW_OKAY;
   sw_uint32	i;
   
	*self = (sw_corby_orb) sw_malloc(sizeof(struct _sw_corby_orb));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);
   
   (*self)->m_salt							=	salt;
	(*self)->m_specs							=	NULL;
   (*self)->m_servants						=	NULL;
	(*self)->m_channel_cache				=	NULL;
   (*self)->m_channels						=	NULL;
   (*self)->m_swop_listeners				=	NULL;
	(*self)->m_delegate						=	NULL;
   (*self)->m_observer_info.m_observer	=	observer;
   (*self)->m_observer_info.m_func		=	func;
   (*self)->m_observer_info.m_extra		=	extra;

	/*
	 * initialize address
	 */

   /*
	 * parse through all configuration strings
    */
	for (i = 0; config[i].m_name != NULL; i++)
   {
		sw_ipv4_address					address;
		sw_corby_channel					channel;
		sw_string							token;
		sw_socket							socket;
      sw_corby_orb_protocol_spec	*	spec;
      sw_socket_options					options;

		/*
		 * create the addres
		 */
		if (is_wildcard(config[i].m_host))
		{
			err = sw_ipv4_address_init(&address);
			sw_check_okay(err, exit);
		}
		else
		{
			err = sw_ipv4_address_init_from_name(&address, config[i].m_host);
			sw_check_okay(err, exit);
		}
		
		/*
		 *  parse the options
       */
		err = sw_socket_options_init(&options);
		sw_check_okay(err, exit);

		if (config[i].m_options != NULL)
		{
			for (token = strtok(config[i].m_options, " "); token; token = strtok(NULL, " "))
			{
         	if (sw_strcmp(token, "DEBUG") == 0)
         	{
            	sw_socket_options_set_debug(options, SW_TRUE);
         	}
         	else if (sw_strcmp(token, "DONTROUTE") == 0)
         	{
            	sw_socket_options_set_dontroute(options, SW_TRUE);
         	}
         	else if (sw_strcmp(token, "KEEPALIVE") == 0)
         	{
            	sw_socket_options_set_keepalive(options, SW_TRUE);
         	}
         	else if (sw_strcmp(token, "REUSEADDR") == 0)
         	{
            	sw_socket_options_set_reuseaddr(options, SW_TRUE);
         	}
         	else if (sw_strcmp(token, "NODELAY") == 0)
         	{
            	sw_socket_options_set_nodelay(options, SW_TRUE);
         	}
      	}
		}
	
		/*
		 * build the socket
		 */
		switch (config[i].m_tag)
		{
			case SW_TAG_INTERNET_IOP:
			{
				sw_corby_orb_swop_listener * listener;

				err = sw_tcp_socket_init(&socket);
				sw_check_okay(err, exit);

				if (0 == config[i].m_port)
				{
					err = sw_socket_bind(socket, address, 0);
				}
				else
				{
					err = sw_socket_bind(socket, address, g_port);
				}
				sw_check_okay(err, exit);

				err = sw_socket_listen(socket, 5);
				sw_check_okay(err, exit);

				listener = (sw_corby_orb_swop_listener*) sw_malloc(sizeof(struct _sw_corby_orb_swop_listener));
				err = sw_translate_error(listener, SW_E_MEM);
				sw_check_okay_log(err, exit);

            listener->m_socket 			= 	socket;
            listener->m_options			=	options;
            listener->m_txen				= 	(*self)->m_swop_listeners;
            (*self)->m_swop_listeners	= 	listener;
				channel							=	NULL;
				err = sw_salt_register_socket(salt, socket, SW_SOCKET_READ, (*self), sw_corby_orb_select, NULL);
				sw_check_okay(err, exit);
			}
			break;
			
			case SW_TAG_UIOP:
			{				
				err = sw_udp_socket_init(&socket);
				sw_check_okay(err, exit);

				//err = sw_socket_bind(socket, address, config[i].m_port);
				//err = sw_socket_bind(socket, address, g_port);
				if (0 == config[i].m_port)
				{
					err = sw_socket_bind(socket, address, 0);
				}
				else
				{
					err = sw_socket_bind(socket, address, g_port);
				}
				sw_check_okay(err, exit);

				err = sw_corby_channel_init(&channel, (*self), socket, options, 0);
				sw_check_okay(err, exit);

				err = sw_corby_orb_register_channel((*self), channel);
				sw_check_okay(err, exit);
			}
			break;
			
			case SW_TAG_MIOP:
			{
				sw_ipv4_address multicast_address;
				
				err = sw_multicast_socket_init(&socket);
				sw_check_okay(err, exit);

				//err = sw_socket_bind(socket, address, config[i].m_port);
				//err = sw_socket_bind(socket, address, g_port);
				if (0 == config[i].m_port)
				{
					err = sw_socket_bind(socket, address, 0);
				}
				else
				{
					err = sw_socket_bind(socket, address, g_port);
				}
				sw_check_okay(err, exit);

				err = sw_ipv4_address_init_from_name(&multicast_address, SW_MIOP_ADDR);
				sw_check_okay(err, exit);

				err = sw_socket_join_multicast_group(socket, sw_ipv4_address_any(), multicast_address, 255);
				sw_check_okay(err, exit);

				err = sw_corby_channel_init(&channel, (*self), socket, options, 0);
				sw_check_okay(err, exit);

				err = sw_corby_orb_register_channel((*self), channel);
				sw_check_okay(err, exit);
			}
			break;

			default:
			{
				err = SW_E_UNKNOWN;
				goto exit;
			}
			break;
		}
		
      
		/*
		 * lastly let's save the info
		 */
		spec = (sw_corby_orb_protocol_spec*) sw_malloc(sizeof(struct _sw_corby_orb_protocol_spec));
		err = sw_translate_error(spec, SW_E_MEM);
		sw_check_okay_log(err, exit);

		sw_strncpy(spec->m_name, config[i].m_name, SW_CORBY_TAG_LEN);
      spec->m_tag	= config[i].m_tag;
		
		/*
			if our address is INADDR_ANY, then we'll stash it as our real address
		*/
		if (config[i].m_tag == SW_TAG_MIOP)
		{
			err = sw_ipv4_address_init_from_name(&spec->m_address, SW_MIOP_ADDR);
			sw_check_okay(err, exit);
		}
		else
		{
			if (sw_ipv4_address_is_any(address))
			{
				err = sw_ipv4_address_init_from_this_host(&spec->m_address);
				sw_check_okay(err, exit);
			}
			else
			{
				err = sw_ipv4_address_init_from_address(&spec->m_address, address);
				sw_check_okay(err, exit);
			}
		}
		      
      spec->m_port		=	sw_socket_port(socket);
      spec->m_txen		=	(*self)->m_specs;
      (*self)->m_specs	=	spec;
   }

exit:

	if (err != SW_OKAY)
	{
		if (*self)
		{
			sw_corby_orb_fina(*self);
			*self = NULL;
		}
	}

	return err;
}


sw_result
sw_corby_orb_fina(
            sw_corby_orb self)
{
	/*
	 * free the listeners
	 */
	while (self->m_swop_listeners)
	{
		sw_corby_orb_swop_listener * listener;

		listener = self->m_swop_listeners;

		sw_salt_unregister_socket(self->m_salt, listener->m_socket);

		self->m_swop_listeners = listener->m_txen;

		sw_socket_options_fina(listener->m_options);
		sw_socket_fina(listener->m_socket);
		sw_free(listener);
	}

	/*
	 * now free the channels
	 */
	while (self->m_channels)
	{
		sw_corby_channel channel;

		channel = self->m_channels;

		sw_salt_unregister_socket(self->m_salt, channel->m_socket);

		self->m_channels = channel->m_nexts;

		sw_corby_channel_fina(channel);
	}

	/*
	 * free the specs
	 */
	while (self->m_specs)
	{
		sw_corby_orb_protocol_spec * spec;

		spec = self->m_specs;

		self->m_specs = spec->m_txen;

		sw_free( spec );
	}

   sw_free(self);

   return SW_OKAY;
}


sw_result
sw_corby_orb_register_servant(
            sw_corby_orb			self,
				sw_corby_servant		servant,
            sw_corby_servant_cb	cb,
            sw_const_string		oid,
				sw_corby_object	*	object,
				sw_const_string		protocol_name)
{
   sw_corby_orb_servant_node	*	node	=	NULL;
	sw_result							err	=	SW_OKAY;
   
   node = (sw_corby_orb_servant_node*) sw_malloc(sizeof(sw_corby_orb_servant_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

   node->m_cb				= cb;
   node->m_servant		= servant;
	
	sw_memcpy(node->m_oid, oid, sw_strlen(oid));
	node->m_oid_len = sw_strlen(oid);

   node->m_txen			= self->m_servants;
   self->m_servants		= node;
	
	/*
	 * create an object if that's what we want to do
	 */
	if (object)
	{
		sw_corby_ior						ior;
		sw_corby_profile					profile;
		sw_corby_orb_protocol_spec	*	spec;
		
		err = sw_corby_object_init(object);
		sw_check_okay(err, exit);

		err = sw_corby_ior_init(&ior);
		sw_check_okay(err, exit);

		ior->m_repository_id = (sw_string) sw_malloc(strlen(g_default_repository_id) + 1);
		err = sw_translate_error(ior->m_repository_id, SW_E_MEM);
		sw_check_okay_log(err, exit);

		sw_strcpy(ior->m_repository_id, g_default_repository_id);

		for (spec = self->m_specs; spec; spec = spec->m_txen)
		{
			/*
			 * we only want to create a profile if the caller passed in a protocol name
			 * and it matches, or they didn't, which means use them all
			 */
			if ((protocol_name == NULL) || (sw_strcmp(spec->m_name, protocol_name) == 0))
			{
				err = sw_corby_profile_init(&profile);
				sw_check_okay(err, exit);

				profile->m_tag	=	spec->m_tag;
				profile->m_major	=	1;
				profile->m_minor	=	0;
				err = sw_ipv4_address_init_from_address(&profile->m_address, spec->m_address);
				sw_check_okay(err, exit);

				profile->m_port		=	spec->m_port;
				profile->m_oid_len	=	(sw_uint32) node->m_oid_len;
				profile->m_oid =  (sw_string) sw_malloc(profile->m_oid_len);
				err = sw_translate_error(profile->m_oid, SW_E_MEM);
				sw_check_okay_log(err, exit);

				sw_memcpy(profile->m_oid, oid, profile->m_oid_len);
				profile->m_txen = ior->m_profiles;
				ior->m_profiles = profile;
				ior->m_num_profiles++;
			}
		}
		
		(*object)->m_iors = ior;
	}

exit:

	return err;
}


sw_result
sw_corby_orb_unregister_servant(
            sw_corby_orb		self,
            sw_const_string	oid)
{
   sw_corby_orb_servant_node * node = NULL;
	sw_corby_orb_servant_node * last = NULL;

	if ( oid )
	{
   	for (node = self->m_servants; node; node = node->m_txen)
   	{
      	if ( ( sw_strlen( oid ) == node->m_oid_len ) && ( sw_memcmp(node->m_oid, oid, node->m_oid_len ) == 0 ) )
      	{
				if (last == NULL)
				{
					self->m_servants = node->m_txen;
				}
				else
				{
					last->m_txen = node->m_txen;
				}
	
				break;
      	}
	
			last = node;
   	}
	}

	if ( node )
	{
		sw_free( node );
	}
   
   return SW_OKAY;
}


sw_result
sw_corby_orb_register_bidirectional_object(
				sw_corby_orb					self,
				struct _sw_corby_object	*	object)
{
	sw_corby_channel	channel;
	sw_result			err;

	err = sw_corby_object_channel(object, &channel);
	sw_check_okay(err, exit);

	err = sw_corby_channel_retain(channel);
	sw_check_okay(err, exit);

	err = sw_corby_orb_register_channel(self, channel);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_orb_register_channel(
					sw_corby_orb		self,
					sw_corby_channel	channel)
{
	sw_result err;

	err = sw_corby_orb_register_channel_events(self, channel, SW_SOCKET_READ);
	sw_check_okay(err, exit);

	channel->m_orb		=	self;
   channel->m_prevs	=	NULL;
   channel->m_nexts	=	self->m_channels;

   if (self->m_channels)
   {
		self->m_channels->m_prevs = channel;
	}
				
	self->m_channels = channel;

exit:

	return err;
}


sw_result
sw_corby_orb_set_delegate(
				sw_corby_orb					self,
				sw_corby_orb_delegate		delegate)
{
	self->m_delegate = delegate;
	return SW_OKAY;
}
 

sw_corby_orb_delegate
sw_corby_orb_get_delegate(
				sw_corby_orb					self)
{
	return self->m_delegate;
}


sw_result
sw_corby_orb_set_observer(
				sw_corby_orb					self,
				sw_corby_orb_observer		observer,
				sw_corby_orb_observer_func	func,
				sw_opaque						extra)
{
	self->m_observer_info.m_observer	=	observer;
	self->m_observer_info.m_func		=	func;
	self->m_observer_info.m_extra		=	extra;

	return SW_OKAY;
}


sw_result
sw_corby_orb_protocol_to_address(
							sw_corby_orb		self,
							sw_const_string	tag,
							sw_string			addr,
							sw_port		*	port)
{
   sw_corby_orb_protocol_spec	*	spec;
	sw_result							err;

	err = sw_corby_orb_protocol_lookup(self, tag, &spec, addr, port);
	sw_check(err == SW_OKAY, exit, err = SW_E_PROTOCOL_NOT_FOUND);

exit:

	return err;
}
		

sw_result
sw_corby_orb_protocol_to_url(
            		sw_corby_orb		self,
            		sw_const_string	tag,
            		sw_const_string	name,
            		sw_string			url,
						sw_size_t			url_len)
{
	sw_int8							addr[64];
	sw_port							port;
	sw_corby_orb_protocol_spec	*	spec;
	sw_result							err;

	SW_UNUSED_PARAM(url_len);
   
	err = sw_corby_orb_protocol_lookup(self, tag, &spec, addr, &port);
	sw_check(err == SW_OKAY, exit, err = SW_E_PROTOCOL_NOT_FOUND);

   switch (spec->m_tag)
   {
      case SW_TAG_INTERNET_IOP:
      
			sprintf(url, "swop://%s:%d/%s", addr, port, name);
			break;
         
		case SW_TAG_UIOP:
         
			sprintf(url, "uiop://%s:%d/%s", addr, port, name);
         break;
         
      case SW_TAG_MIOP:
      
         sprintf(url, "miop://%s:%d/%s", SW_MIOP_ADDR, spec->m_port, name);
         break;
   }

exit:

	return err;
}


sw_result
sw_corby_orb_register_channel_events(
					sw_corby_orb		self,
					sw_corby_channel	channel,
					sw_uint32				events)
{
	sw_result err;

	err = sw_salt_register_socket(self->m_salt, sw_corby_channel_socket(channel), events, self, sw_corby_orb_select, channel);
	sw_check_okay(err, exit);

exit:

	return err;
}


static sw_result
sw_corby_orb_select(
               sw_socket_handler	handler,
               sw_salt				salt,
               sw_socket			socket,
               sw_socket_event	events,
               sw_opaque_t			extra)
{
   sw_corby_orb		self;
   sw_corby_channel	channel		= NULL;
	sw_socket			new_socket	= NULL;
	sw_result			err			= SW_OKAY;
      
	SW_UNUSED_PARAM(salt);

	self		= (sw_corby_orb) handler;
   channel	= (sw_corby_channel) extra;
   
	sw_debug(SW_LOG_VERBOSE, "sw_corby_orb_select() : fd %d\n", sw_socket_desc(socket));

   /*
      first check to see if it's a channel or not.
   */
   if (channel)
   {  
		if (events & SW_SOCKET_WRITE)
		{
			if (sw_corby_channel_flush_send_queue(channel) != SW_OKAY)
			{
				return SW_OKAY;
			}

			sw_corby_orb_register_channel_events(self, channel, SW_SOCKET_READ);
		}
		else if (events & SW_SOCKET_READ)
		{
			sw_corby_orb_read_channel(self, channel);
   	}
	}
   
   /*
      else it's a tcp server
   */
   else
   {
      sw_corby_orb_swop_listener * listener;
      
      for (listener = self->m_swop_listeners; listener; listener = listener->m_txen)
      {
         if (listener->m_socket == socket)
         {
				err = sw_socket_accept(listener->m_socket, &new_socket);

				//
				// check for situations where the socket honestly
				// got an EWOULDBLOCK
				//
				if (err)
				{
					if (err == SW_E_WOULDBLOCK)
					{
						err = 0;
					}

					goto exit;
				}

				err = sw_corby_channel_init(&channel, self, new_socket, listener->m_options, 0);
				sw_check_okay(err, exit);

				if ((self->m_delegate != NULL) && (self->m_delegate->m_accept_channel_func != NULL))
				{
					err = (*self->m_delegate->m_accept_channel_func)(self, channel);
					sw_check_okay(err, exit);
				}
				else
				{
					err = sw_corby_orb_register_channel(self, channel);
					sw_check_okay(err, exit);
				}

				break;
         }
      }
   }

exit:

	if (err != SW_OKAY)
	{
		if (channel != NULL)
		{
			sw_corby_channel_fina(channel);	
		}

		if (new_socket != NULL)
		{
			sw_socket_fina(new_socket);
		}
	}
	
	return err;
}


sw_result
sw_corby_orb_read_channel(
					sw_corby_orb		self,
					sw_corby_channel	channel)
{
	sw_corby_message	message;
	sw_corby_buffer	buffer;
	sw_uint8			endian;
	sw_bool			block;
	sw_result			err = SW_OKAY;
	
	block = SW_TRUE;
      
   do
   {
      if ((err = sw_corby_channel_recv(channel, NULL, &message, NULL, NULL, NULL, &buffer, &endian, block)) != SW_OKAY)
      {
         /*
				only biff the connection upon E_EOF. connectionless
				protocols still want to process more messages on
				this channels
         */
         if (err == SW_E_EOF)
         {
				sw_debug(SW_LOG_VERBOSE, "sw_corby_orb_select() : EOF on fd %d\n", sw_socket_desc(channel->m_socket));
            sw_salt_unregister_socket(self->m_salt, channel->m_socket);

            if (self->m_observer_info.m_observer)
            {
					(*self->m_observer_info.m_func)(
												self->m_observer_info.m_observer,
												self->m_salt,
												self,
												channel,
												self->m_observer_info.m_extra);
            }
            
            /*
				 * take it out of list of channels
				 */
				if (channel->m_prevs == NULL)
				{
					self->m_channels = channel->m_nexts;

					if (channel->m_nexts != NULL)
					{
						channel->m_nexts->m_prevs = NULL;
					}
				}
				else if (channel->m_nexts == NULL)
				{
					channel->m_prevs->m_nexts = NULL;
				}
				else
				{
               channel->m_prevs->m_nexts = channel->m_nexts;
					channel->m_nexts->m_prevs = channel->m_prevs;
            }
            
            /*
				 * and get rid of it
				 */
				sw_corby_channel_fina(channel);
         }
      }
      else if (message)
      {
         sw_corby_orb_dispatch_message(self, channel, message, buffer, endian);
         block = SW_FALSE;
      }
   }
   while (message);

	return err;
}


sw_result
sw_corby_orb_dispatch_message(
                  sw_corby_orb			self,
                  sw_corby_channel		channel,
                  sw_corby_message		message,
                  sw_corby_buffer		buffer,
                  sw_uint8					endian)
{
	sw_result err = SW_OKAY;

   switch (message->m_header->m_msg_type)
   {
      case SW_SWOP_REQUEST:
      {
         sw_swop_request_header		*	request_header = &message->m_body.m_request_header;
         sw_corby_orb_servant_node	*	node;
         sw_bool								found;
			sw_uint32								size;
			
			/*
			 * stash the message size
			 */
			size = message->m_header->m_msg_size;
         
         for (found = SW_FALSE, node = self->m_servants; (node && !found); node = node->m_txen)
         {
            if ((node->m_oid_len == request_header->m_oid_len) &&
                (sw_memcmp(node->m_oid, (sw_string) request_header->m_oid, node->m_oid_len) == 0))
            {
					/*
					 * intercept _is_a calls...corby doesn't have this, so just return true for all cases
					 */
					if ((request_header->m_op[0] == '_') && (sw_strcmp("_is_a", request_header->m_op) == 0))
					{
						sw_corby_buffer outbuf;
						
						err = sw_corby_channel_start_reply(channel, &outbuf, request_header->m_request_id, SW_CORBY_NO_EXCEPTION);
						sw_check_okay(err, exit);
						
						err = sw_corby_buffer_put_uint8(outbuf, SW_TRUE);
						sw_check_okay(err, exit);
						
						err = sw_corby_channel_send(channel, outbuf, NULL, NULL, NULL);
						sw_check_okay(err, exit);
					}
					else
					{
						if ((err = (*node->m_cb)(node->m_servant, self->m_salt, self, channel, message, buffer, request_header->m_op, request_header->m_op_len, request_header->m_request_id, endian)) != SW_OKAY)
						{
							sw_corby_orb_handle_system_exception(self, channel, request_header, err);
						}
					}
					
					found = SW_TRUE;
            }
         }
         
         if (!found)
         {
				sw_debug(SW_LOG_ERROR, "unknown object '%s'\n", (sw_string) request_header->m_oid);
				sw_corby_orb_handle_system_exception(self, channel, request_header, SW_E_CORBY_OBJECT_NOT_EXIST);
			}
			
			/*
			 * make sure we fix the pointers if for some reason the upcall didn't read all the parameters, or we didn't find
			 * the object.  Or something.  This shouldn't happen when everything is working correctly, but errors can screw
			 * up buffer processing.  In an ideal world, the orb wouldn't have to worry about low level things like buffer
			 * pointers, but alas this isn't an ideal world.
			 */
			sw_assert(!request_header->m_reply_expected || ((buffer->m_base == buffer->m_bptr) && (buffer->m_base == buffer->m_eptr)));

			sw_corby_channel_ff(channel, buffer);
      }
      break;
      
      case SW_SWOP_CANCEL_REQUEST:
      {
      }
      break;
      
      case SW_SWOP_LOCATE_REQUEST:
      {
      }
      break;
      
      case SW_SWOP_CLOSE_CONNECTION:
      {
      }
      break;
      
      default:
      {
      }
   }

exit:

	return err;
}


static void
sw_corby_orb_handle_system_exception(
							sw_corby_orb					orb,
							sw_corby_channel				channel,
							sw_swop_request_header	*	request_header,
							sw_result						result)
{
	sw_corby_buffer	buffer;
	sw_result			err = SW_OKAY;

	SW_UNUSED_PARAM(orb);

	/*
	 * if the guy is not expecting a reply, then don't send one
	 */
	if (request_header->m_reply_expected == SW_FALSE)
	{
		return;
	}
		
	/*
	 * now start the reply
	 */
	err = sw_corby_channel_start_reply(channel, &buffer, request_header->m_request_id, SW_CORBY_SYSTEM_EXCEPTION);
	sw_check_okay(err, exit);
	
	switch (result)
	{
		case SW_E_CORBY_OBJECT_NOT_EXIST:
		{
			err = sw_corby_buffer_put_cstring(buffer, "IDL:omg.org/CORBA/OBJECT_NOT_EXIST:1.0");
			sw_check_okay(err, exit);
		}
		break;
		
		case SW_E_CORBY_BAD_OPERATION:
		{
			err = sw_corby_buffer_put_cstring(buffer, "IDL:omg.org/CORBA/BAD_OPERATION:1.0");
			sw_check_okay(err, exit);
		}
		break;
		
		case SW_E_CORBY_MARSHAL:
		{
			err = sw_corby_buffer_put_cstring(buffer, "IDL:omg.org/CORBA/MARSHAL:1.0");
			sw_check_okay(err, exit);
		}
		break;
		
		default:
		{
			err = sw_corby_buffer_put_cstring(buffer, "IDL:omg.org/CORBA/UNKNOWN:1.0");
			sw_check_okay(err, exit);
		}
		break;
	}
	
	err = sw_corby_buffer_put_uint32(buffer, 0);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, result);
	sw_check_okay(err, exit);

	err = sw_corby_channel_send(channel, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	return;
}


static int
is_wildcard(
		sw_const_string str)
{
   return (sw_strcmp(str, "*") == 0) ? 1 : 0;
}


	/*
		now find the guy
	*/
   /*
	(server->m_cb)(
					(sw_server) server, 
					request_header->m_object_key, 
					request_header->m_op, 
					request_header->m_oplen,
					msg,
					&new_addr);
   */

/*
	{
		swop_msg_put_msg_hdr(msg, SWOP_REPLY);

		if (addr == NULL)
		{
			sw_msg_put_ulong(msg, SWOP_LOCATION_FORWARD);
			sw_msg_put_addr(msg, addr);
		}
		else
		{
		 char		*	repid	= "IDL:omg.org/CORBA/INV_OBJREF:1.0";
		 sw_uint32_t_t len	= strlen(repid) + 1;

			sw_msg_put_ulong(msg, SWOP_SYSTEM_EXCEPTION);
			sw_msg_put_ulong(msg, len);
			sw_msg_put_seq(msg, repid, len);

			sw_msg_put_ulong(msg, 0);
			sw_msg_put_ulong(msg, 1);
		}

		if (sw_msg_send(msg) != 0)
		{
			goto done;
		}

	}
*/


static sw_result
sw_corby_orb_protocol_lookup(
							sw_corby_orb						self,
							sw_const_string					name,
							sw_corby_orb_protocol_spec	** spec,
							sw_string							addr,
							sw_port						*	port)
{
	sw_result err = SW_OKAY;

   for ((*spec) = self->m_specs; ((*spec) && (sw_strcmp((*spec)->m_name, name) != 0)); (*spec) = (*spec)->m_txen);
   
	sw_check(*spec, exit, err = SW_E_PROTOCOL_NOT_FOUND);

	sw_ipv4_address_name((*spec)->m_address, addr, 16);
	*port = (*spec)->m_port;

exit:
	
	return err;
}

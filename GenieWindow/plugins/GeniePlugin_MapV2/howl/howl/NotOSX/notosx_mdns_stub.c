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

#define SW_MDNS_PROTOCOL_VERSION  1


#if !defined(WIN32)
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <sys/time.h>
#	if !defined(__VXWORKS__)
#		include <sys/uio.h>
#	endif
#	include <netinet/in.h>
#	include <unistd.h>
#endif

#include <NotOSX/notosx_mdns_stub.h>
#include <discovery/text_record.h>
#include <salt/salt.h>
#include <corby/orb.h>
#include <corby/channel.h>
#include <corby/message.h>
#include <corby/buffer.h>
#include <salt/debug.h>
#include <stdio.h>



/*
 * static function declarations 
 */
static sw_discovery_oid g_oid;

static sw_uint32
sw_mdns_stub_next_oid()
{
	if (++g_oid == 0)
	{
		++g_oid;
	}

	return g_oid;
}


static sw_result
sw_mdns_stub_bind(
							sw_mdns_stub		self);


static sw_result
sw_mdns_stub_dispatcher(
							sw_corby_servant	servant,
							sw_salt				salt,
							sw_corby_orb		orb,
							sw_corby_channel	channel,
							sw_corby_message	message,
							sw_corby_buffer	buffer,
							sw_const_string	op,
							sw_uint32			op_len,
							sw_uint32			request_id,
							sw_uint8				endian);


static sw_mdns_stub_pending_op
sw_mdns_stub_lookup(
							sw_mdns_stub		self,
							sw_discovery_oid	oid);


static void
sw_mdns_stub_free_node(
							sw_mdns_stub		self,
							sw_discovery_oid 	oid);


/*
 * sw_network_service implementation
 * -------------------------
 * This is the client side to publishing services using DNS-SD.
 *
 */
sw_result
sw_mdns_stub_init(
					sw_mdns_stub	*	self,
					sw_salt				salt,
					sw_discovery		discovery,
					sw_uint16			port)
{
	static sw_corby_orb_config config[] =
	{
		{ "swop", SW_TAG_INTERNET_IOP, "127.0.0.1", 0 },
		{ NULL }
	};

	sw_int8			mDNSURL[128];
	sw_result		err;
	
	/*
	 * try to allocate space
	 */
	*self = (sw_mdns_stub) sw_malloc(sizeof(struct _sw_mdns_stub));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * initialize the easy stuff
	 */
	(*self)->m_salt		=	salt;
	(*self)->m_discovery =	discovery;	
	(*self)->m_bound		=	SW_FALSE;

	/*
	 * Patch from Jakub Stachowski (stachowski@hypair.net)
	 * 
	 * - m_pending_ops was not initialized
	 */

	(*self)->m_pending_ops = NULL;

	/*
	 * create an orb so we can handle replies
	 */
	err = sw_corby_orb_init(&(*self)->m_orb, (*self)->m_salt, config, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
		register our server
	*/
	err = sw_corby_orb_register_servant((*self)->m_orb, (*self), (sw_corby_servant_cb) sw_mdns_stub_dispatcher, SERVICE_OID, &(*self)->m_self, NULL);
	sw_check_okay(err, exit);

	/*
		create client for mDNS service
	*/
	sprintf(mDNSURL, "swop://127.0.0.1:%d/dns-sd", (int) port);

	err = sw_corby_object_init_from_url(&(*self)->m_service, (*self)->m_orb, mDNSURL, NULL, 0);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_mdns_stub_fina(
							sw_mdns_stub	self)
{
	sw_corby_orb_unregister_servant(self->m_orb, SERVICE_OID);
	sw_corby_object_fina(self->m_self);
	sw_corby_object_fina(self->m_service);
	sw_corby_orb_fina(self->m_orb);

	sw_free( self );

	return SW_OKAY;
}


sw_result
sw_mdns_stub_check_version(
							sw_mdns_stub	self)
{
	static sw_string			op 		=	"check_version";
	static sw_uint32			op_len	=	14;
	sw_corby_buffer 			buffer;
	struct timeval				tv;
	fd_set						fds;
	int							res;
	sw_result					err;

	/*
	 * very coarse grain thread safe-ing
	 */
	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	/*
	 * now do garden variety initialization
	 */
	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint8(buffer, SW_MDNS_PROTOCOL_VERSION);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
	 * now wait for reply...this is a hack, as the only requests I can
	 * make are oneways, and I need to wait for a reply to this
	 */
	tv.tv_sec	=	5;
	tv.tv_usec	=	0;
	FD_ZERO(&fds);
	FD_SET(sw_mdns_stub_socket(self), &fds);
	res									=	select(sw_mdns_stub_socket(self) + 1, &fds, NULL, NULL, &tv);
	self->m_check_version_result	=	SW_E_UNKNOWN;
	
	if (res == 1)
	{
		sw_mdns_stub_read_socket(self);
	}

	err = self->m_check_version_result;

exit:

	sw_salt_unlock(self->m_salt);
	
	return err;
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
	static sw_string			op 		=	"publish_host";
	static sw_uint32			op_len	=	13;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	/*
	 * very coarse grain thread safe-ing
	 */
	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_publish_reply	= 	reply;
	node->m_extra				= 	extra;
	node->m_oid					=	sw_mdns_stub_next_oid();
	*oid							=	node->m_oid;
	
	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, name);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, sw_ipv4_address_saddr(address));
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;

exit:

	if (err && node)
	{
		sw_free(node);
		node = NULL;
	}

	sw_salt_unlock(self->m_salt);
	
	return err;
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
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	static sw_string			op 		=	"publish";
	static sw_uint32			op_len	=	8;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	/*
	 * very coarse grain thread safe-ing
	 */
	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_publish_reply	= 	reply;
	node->m_extra				= 	extra;
	node->m_oid					=	sw_mdns_stub_next_oid();
	*oid							=	node->m_oid;
	
	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, name);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, reg_type);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, host);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, port);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, text_record_len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(buffer, text_record, text_record_len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;

exit:

	if (err && node)
	{
		sw_free(node);
	}

	sw_salt_unlock(self->m_salt);
	
	return err;
}


sw_result
sw_mdns_stub_publish_update(
						sw_mdns_stub		self,
						sw_discovery_oid	oid,
						sw_octets			text_record,
						sw_uint32			text_record_len)
{
	static sw_string			op 		=	"publish_update";
	static sw_uint32			op_len	=	15;
	sw_corby_buffer 			buffer;
	sw_result					err;

	/*
	 * very coarse grain thread safe-ing
	 */
	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, text_record_len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(buffer, text_record, text_record_len);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	sw_salt_unlock(self->m_salt);
	
	return err;
}


sw_result
sw_mdns_stub_browse_domains(
								sw_mdns_stub						self,
								sw_uint32							interface_index,
								sw_discovery_browse_reply		reply,
								sw_opaque							extra,
								sw_discovery_oid				*	oid)
{
	static sw_string			op 		=	"browse_domains";
	static sw_uint32			op_len	=	15;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	sw_salt_lock(self->m_salt);
	
	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	/*
	 * try to allocate space
	 */
	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_browse_reply		=	reply;
	node->m_extra				=	extra;
	node->m_oid					=	sw_mdns_stub_next_oid();
	*oid							=	node->m_oid;

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);
	
	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;

exit:

	if (err && node)
	{
		sw_free(node);
	}

	sw_salt_unlock(self->m_salt);
	
	return err;
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
	static sw_string			op 		=	"browse_services";
	static sw_uint32			op_len	=	16;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	/*
	 * try to allocate space
	 */
	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_browse_reply		=	reply;
	node->m_extra				=	extra;
	node->m_oid					=	sw_mdns_stub_next_oid();
	*oid							=	node->m_oid;

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, type);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);
	
	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;

exit:

	if (err && node)
	{
		sw_free(node);
	}

	sw_salt_unlock(self->m_salt);
	
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
	static sw_string			op 		=	"resolve";
	static sw_uint32			op_len	=	8;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	sw_salt_lock(self->m_salt);
	
	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	/*
	 * try to allocate space
	 */
	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_resolve_reply	=	reply;
	node->m_extra				=	extra;
	node->m_oid					=	sw_mdns_stub_next_oid();
	*oid							=	node->m_oid;

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, name);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, reg_type);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;
	
exit:

	if (err && node)
	{
		sw_free(node);
	}

	sw_salt_unlock(self->m_salt);
	
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
	static sw_string			op 		=	"query_record";
	static sw_uint32			op_len	=	13;
	sw_corby_buffer 			buffer;
	sw_mdns_stub_pending_op	node		=	NULL;
	sw_result					err;

	sw_salt_lock(self->m_salt);
	
	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	/*
	 * try to allocate space
	 */
	node = (sw_mdns_stub_pending_op) sw_malloc(sizeof(struct _sw_mdns_stub_pending_op));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/*
	 * now do garden variety initialization
	 */
	node->m_query_record_reply	=	reply;
	node->m_extra					=	extra;
	node->m_oid						=	sw_mdns_stub_next_oid();
	*oid								=	node->m_oid;

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, flags);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, fullname);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, rrtype);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, rrclass);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_object(buffer, self->m_self);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, node->m_oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

	/*
	 * stash the node
	 */
	node->m_txen			= self->m_pending_ops;
	self->m_pending_ops	= node;
	
exit:

	if (err && node)
	{
		sw_free(node);
	}

	sw_salt_unlock(self->m_salt);
	
	return err;
}


sw_result
sw_mdns_stub_cancel(
					sw_mdns_stub		self,
					sw_discovery_oid	oid)
{
	static sw_string	op			=	"cancel";
	static sw_uint32	op_len	=	7;
	sw_corby_buffer	buffer;
	sw_result			err;

	sw_salt_lock(self->m_salt);

	/*
	 * try and bind to service
	 */
	err = sw_mdns_stub_bind(self);
	sw_check_okay(err, exit);

	err = sw_corby_object_start_request(self->m_service, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(self->m_service, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	sw_mdns_stub_free_node(self, oid);

	sw_salt_unlock(self->m_salt);

	return err;
}


sw_sockdesc_t
sw_mdns_stub_socket(
						sw_mdns_stub	self)
{
	sw_corby_channel	channel;
	sw_socket			socket;
	sw_sockdesc_t		desc = SW_INVALID_SOCKET;
	sw_result			err;

	err = sw_corby_object_channel(self->m_service, &channel);
	sw_check_okay(err, exit);
	sw_check(channel, exit, err = SW_E_UNKNOWN);

	socket = sw_corby_channel_socket(channel);
	sw_check(socket, exit, err = SW_E_UNKNOWN);

	desc = sw_socket_desc(socket);

exit:

	return desc;
}


sw_result
sw_mdns_stub_read_socket(
						sw_mdns_stub	self)
{
	sw_corby_channel	channel;

	if (sw_corby_object_channel(self->m_service, &channel) != SW_OKAY)
	{
		return SW_E_UNKNOWN;
	}

	return sw_corby_orb_read_channel(self->m_orb, channel);
}


static sw_result
sw_mdns_stub_bind(
						sw_mdns_stub	self)
{
	sw_result err = SW_OKAY;

	sw_assert(self);
	sw_assert(self->m_orb);
	sw_assert(self->m_service);

	if (self->m_bound == SW_FALSE)
	{
		/*
	 	 * set us up for bi-directionality
		 */
		err = sw_corby_orb_register_bidirectional_object(self->m_orb, self->m_service);
		sw_check_okay(err, exit);

		self->m_bound = SW_TRUE;
	}

exit:

	return err;
}


static sw_result
sw_mdns_stub_dispatcher(
								sw_corby_servant		servant,
								sw_salt					salt,
								sw_corby_orb			orb,
								sw_corby_channel		channel,
								sw_corby_message		message,
								sw_corby_buffer		buffer,
								sw_const_string		op,
								sw_uint32				op_len,
								sw_uint32				request_id,
								sw_uint8					endian)
{
	sw_mdns_stub	self = (sw_mdns_stub) servant;
	sw_result		err;

	SW_UNUSED_PARAM(salt);
	SW_UNUSED_PARAM(orb);
	SW_UNUSED_PARAM(channel);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(op_len);
	SW_UNUSED_PARAM(request_id);

	if (sw_strcmp("publish_reply", op) == 0)
	{
		sw_uint8							status;
		sw_mdns_stub_pending_op		node;
		sw_discovery_oid				oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint8(buffer, &status);
		sw_check_okay(err, exit);

		node = sw_mdns_stub_lookup(self, oid);
		sw_check(node, exit, err = SW_E_UNKNOWN);

		(node->m_publish_reply)(self->m_discovery, oid, (sw_discovery_publish_status) status, node->m_extra);
	}
	else if (sw_strcmp("browse_reply", op) == 0)
	{
		sw_uint8							status;
		sw_mdns_stub_pending_op		node;
		sw_uint32						interface_index;
		sw_string						name;
		sw_string						type;
		sw_string						domain;
		sw_uint32						dummy;
		sw_discovery_oid				oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint8(buffer, &status);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &name, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &type, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &dummy, endian);
		sw_check_okay(err, exit);

		node = sw_mdns_stub_lookup(self, oid);
		sw_check(node, exit, err = SW_E_UNKNOWN);
		
		(node->m_browse_reply)(self->m_discovery, oid, (sw_discovery_browse_status) status, interface_index, name, type, domain, node->m_extra);
	}
	else if (sw_strcmp("resolve_reply", op) == 0)
	{
		sw_uint32						interface_index;
		sw_string						name;
		sw_string						type;
		sw_string						domain;
		sw_ipv4_address				address;
		sw_port							port;
		sw_string						text_record;
		sw_octets						raw_text_record;
		sw_uint32						raw_text_record_len;
		sw_uint32						dummy;
		sw_mdns_stub_pending_op		node;
		sw_discovery_oid				oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &name, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &type, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_ipv4_address_init_from_saddr(&address, dummy);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint16(buffer, &port, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_sized_octets(buffer, &raw_text_record, &raw_text_record_len, endian);
		sw_check_okay(err, exit);

		node = sw_mdns_stub_lookup(self, oid);
		sw_check(node, exit, err = SW_E_UNKNOWN);
		
		(node->m_resolve_reply)(self->m_discovery, oid, interface_index, name, type, domain, address, port, raw_text_record, raw_text_record_len, node->m_extra);
	}
	else if (sw_strcmp("query_record_reply", op) == 0)
	{
		sw_uint32						interface_index;
		sw_uint32						flags;
		sw_string						fullname;
		sw_uint16						rrtype;
		sw_uint16						rrclass;
		sw_uint32						rrdatalen;
		sw_octets						rrdata;
		sw_uint32						ttl;
		sw_uint32						dummy;
		sw_mdns_stub_pending_op		node;
		sw_uint32						oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &flags, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &fullname, &dummy, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint16(buffer, &rrtype, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint16(buffer, &rrclass, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_sized_octets(buffer, &rrdata, &rrdatalen, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &ttl, endian);
		sw_check_okay(err, exit);

		node = sw_mdns_stub_lookup(self, oid);
		sw_check(node, exit, err = SW_E_UNKNOWN);
		
		(node->m_query_record_reply)(self->m_discovery, oid, flags, interface_index, fullname, rrtype, rrclass, (sw_uint16) rrdatalen, rrdata, ttl, node->m_extra);
	}
	else if (sw_strcmp("check_version_reply", op) == 0)
	{
		sw_uint8 version;

		err = sw_corby_buffer_get_uint32(buffer, (sw_uint32*) &self->m_check_version_result, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint8(buffer, &version);
		sw_check_okay(err, exit);

		if (self->m_check_version_result != SW_OKAY)
		{
			sw_debug(SW_LOG_WARNING, "mdns version mismatch. server version is %d\n", (int) version);
		}
	}
	else
	{
		return SW_E_CORBY_BAD_OPERATION;
	}

exit:

	return err;
}


static sw_mdns_stub_pending_op
sw_mdns_stub_lookup(
							sw_mdns_stub		self,
							sw_discovery_oid	oid)
{
	sw_mdns_stub_pending_op	node;
	
	for (node = self->m_pending_ops; node; node = node->m_txen)
	{
		if (node->m_oid == oid)
		{
			return node;
		}
	}
	
	return NULL;
}


static void
sw_mdns_stub_free_node(
							sw_mdns_stub		self,
							sw_discovery_oid	oid)
{
	sw_mdns_stub_pending_op	node;
	sw_mdns_stub_pending_op	last = NULL;
	
	for (node = self->m_pending_ops; node; node = node->m_txen)
	{
		if (node->m_oid == oid)
		{
			if (last == NULL)
			{
				self->m_pending_ops = node->m_txen;
			}
			else
			{
				last->m_txen = node->m_txen;
			}

			sw_free(node);

			break;
		}

		last = node;
	}
}

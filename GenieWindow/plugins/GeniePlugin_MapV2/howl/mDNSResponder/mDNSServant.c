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


#if defined(WIN32)

#	pragma warning(disable:4311)

#else

#	include <sys/types.h>
#	include <sys/socket.h>
#	include <sys/time.h>
#	if !defined(__VXWORKS__)
#		include <sys/uio.h>
#	endif
#	include <netinet/in.h>
#	include <unistd.h>

#endif

#include "mDNSServant.h"
#include <discovery/text_record.h>
#include <salt/salt.h>
#include <salt/debug.h>
#include <corby/orb.h>
#include <corby/channel.h>
#include <corby/message.h>
#include <corby/buffer.h>
#include <salt/debug.h>
#include <stdio.h>


extern sw_uint16 g_port;

static sw_result
sw_mdns_servant_check_version(
				sw_mdns_servant					self,
				const sw_corby_object			client,
				sw_corby_channel					channel,
				sw_uint8								version);


static sw_result
sw_mdns_servant_add_node(
				sw_mdns_servant					self,
				sw_mdns_servant_client_node	node);
				
				
static sw_mdns_servant_client_node
sw_mdns_servant_lookup_node(
				sw_mdns_servant		self,
				sw_corby_channel		channel,
				sw_uint32					id);
				

static sw_result
sw_mdns_servant_free_node(
				sw_mdns_servant					self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_publish_domain_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_publish_host_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_publish_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_browse_domains_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_browse_services_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);


static sw_result
sw_mdns_servant_resolve_cleanup(
				sw_mdns_servant		self,
				sw_mdns_servant_client_node	node);
				

static sw_result
sw_mdns_servant_query_record_cleanup(
				sw_mdns_servant					self,
				sw_mdns_servant_client_node	node);


static void
sw_mdns_servant_publish_host_callback(
				void							*	inContext, 
				DNSHostRegistrationRef		inRef, 
				DNSStatus						inStatusCode, 
				void							*	inData);


static sw_result
sw_mdns_servant_publish_callback(
				void								*	inContext,
				DNSRegistrationRef            inRef,
				DNSStatus                  	inStatusCode,
				const DNSRegistrationEvent *  inEvent);


static sw_result
sw_mdns_servant_publish_callback2(
				void								*	inContext,
				DNSRegistrationRef            inRef,
				DNSStatus                  	inStatusCode,
				const DNSRegistrationEvent *  inEvent);


static sw_result
sw_mdns_servant_browse_callback(
				void							*	inContext,
				DNSBrowserRef					inRef,
				DNSStatus						inStatusCode,
				const DNSBrowserEvent	*	inEvent);

			
static sw_result
sw_mdns_servant_resolve_callback(
				void							*	inContext,
				DNSResolverRef					inRef,
				DNSStatus                  inStatusCode,
				const DNSResolverEvent	*	inEvent);


static sw_result
sw_mdns_servant_query_record_callback(
				void					*	inContext, 
				DNSQueryRecordRef 	inRef, 
				DNSStatus 				inStatusCode,
				mDNSu32					inFlags,
				mDNSu32					inInterfaceIndex,
				const char			*	inFullName,
				mDNSu16					inRRClass,
				mDNSu16					inRRType,
				mDNSu16					inRDataLen,
				void					*	inRData,
				mDNSu32					inTTL );


static sw_result
sw_mdns_servant_dispatcher(
            sw_corby_servant    	servant,
            sw_salt					salt,
            sw_corby_orb			orb,
            sw_corby_channel  	channel,
            sw_corby_message  	message,
            sw_corby_buffer   	buffer,
            sw_const_string   	op,
            sw_uint32          	op_len,
            sw_uint32					request_id,
            sw_uint8					endian);


static sw_result
sw_mdns_servant_connection_notifier(
				sw_corby_orb_observer			handler,
				sw_salt								salt,
				sw_corby_orb						orb,
				struct _sw_corby_channel	*	channel,
				sw_opaque							extra);


static sw_result
sw_mdns_servant_cleanup(
				sw_mdns_servant			self,
				sw_opaque					extra);


static sw_result
sw_mdns_servant_publish_reply(
				sw_discovery						discovery,
				sw_discovery_oid					oid,
				sw_discovery_publish_status	status,
				sw_opaque							extra);
				

static sw_result
sw_mdns_servant_browse_reply(
				sw_discovery						discovery,
				sw_discovery_oid					oid,
				sw_discovery_browse_status	status,
				sw_uint32							interface_index,
				sw_const_string					name,
				sw_const_string					type,
				sw_const_string					domain,
				sw_opaque							extra);

			
static sw_result
sw_mdns_servant_resolve_reply(
				sw_discovery						discovery,
				sw_discovery_oid					oid,
				sw_uint32							interface_index,
				sw_const_string					name,
				sw_const_string					type,
				sw_const_string					domain,
				sw_ipv4_address					address,
				sw_port								port,
				sw_octets							text_record,
				sw_uint32							text_record_len,
				sw_opaque							extra);


static sw_result
sw_mdns_servant_query_record_reply(
				sw_discovery								session,
				sw_discovery_oid							oid,
				sw_discovery_query_record_status		status,
				sw_uint32									interface_index,
				sw_const_string							full_name,
				sw_uint16									rr_type,
				sw_uint16									rr_class,
				sw_uint16									rr_data_len,	
				sw_const_octets							rr_data,
				sw_uint32									ttl,
				sw_opaque									extra);


static sw_result
sw_mdns_servant_getline(
				sw_mdns_servant		self,
				FILE					*	f,
				sw_string				line,
				sw_size_t			*	len);


static sw_result
sw_mdns_servant_parse_conf_line(
				sw_mdns_servant		self,
				sw_string				line,
				sw_size_t				len,
				sw_string				name,
				sw_string				type,
				sw_string				domain,
				sw_uint16			*	port,
				sw_text_record		*	text_record,
				sw_const_string		file,
				int						lineno);


/*
 * This is a bit strange, however we need to implement this
 * here now that the libraries are split
 */
sw_result
sw_discovery_init_with_flags(
						sw_discovery			*	self,
						sw_discovery_init_flags	flags)
{
	sw_result err = SW_OKAY;

	*self = (sw_discovery) sw_malloc(sizeof(struct _sw_discovery));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*self, 0, sizeof(struct _sw_discovery));

	/*
	 * initialize salt
	 */
	err = sw_salt_init(&(*self)->m_salt, 0, NULL);
	sw_check_okay(err, exit);

	(*self)->m_servant_fina_func = sw_mdns_servant_delete;

	if (flags & SW_DISCOVERY_USE_SHARED_SERVICE)
	{
		//err = sw_mdns_stub_init(&(*self)->m_stub, (*self)->m_salt, (*self), 5335);
		err = sw_mdns_stub_init(&(*self)->m_stub, (*self)->m_salt, (*self), g_port);

		if (!err)
		{
			if ((flags & SW_DISCOVERY_SKIP_VERSION_CHECK) == 0)
			{
				err = sw_mdns_stub_check_version((*self)->m_stub);

				if (err)
				{
					sw_mdns_stub_fina((*self)->m_stub);
					(*self)->m_stub = NULL;
				}
			}
		}
	}

	if (err)
	{
		if (flags & SW_DISCOVERY_USE_PRIVATE_SERVICE)
		{
			err = sw_mdns_servant_new(&(*self)->m_servant, 0, NULL, 0);
			sw_check_okay(err, exit);
	
			err = sw_mdns_stub_init(&(*self)->m_stub, (*self)->m_salt, (*self), sw_mdns_servant_port((*self)->m_servant));
			sw_check_okay(err, exit);
		}
	}

exit:

	if (err != SW_OKAY)
	{
		if (*self)
		{
			sw_discovery_fina(*self);
			*self = NULL;
		}
	}

	return err;
}


#define CACHE_SIZE 750


sw_result
sw_mdns_servant_init(
				sw_mdns_servant	self)
{
	sw_int8		scratch[256];
	sw_result	err;
	
	/*
	 * only want to bind to loopback so only clients on our own machine can connect.  Use port 5335
	 * just 'cause that's the one mDNS uses.  It could be anything, just needs to be well known.
	 */
	static sw_corby_orb_config	config[] =      
   {
      { "swop", SW_TAG_INTERNET_IOP,  "127.0.0.1", 5335 },
      { NULL }
   };

	self->m_clients	=	NULL;

	/*
	 * set the port
	 */
	config[0].m_port = self->m_port;

	/*
	 * initialize salt
	 */
	err = sw_salt_init(&self->m_salt, 0, NULL);
	sw_check_okay(err, exit);

	/*
	 * now initialize the orb
	 */
	err = sw_corby_orb_init(&self->m_orb, self->m_salt, config, self, sw_mdns_servant_connection_notifier, NULL);
	sw_check_okay(err, exit);

	/*
	 * get my port number
	 */
	err = sw_corby_orb_protocol_to_address(self->m_orb, "swop", scratch, &self->m_port);
	sw_check_okay(err, exit);

	/*
	 * register ourselves with the orb
	 */
	err = sw_corby_orb_register_servant(self->m_orb, self, (sw_corby_servant_cb) sw_mdns_servant_dispatcher, "dns-sd", NULL, NULL);
	sw_check_okay(err, exit);

	err = DNSServicesInitialize(self->m_salt, kDNSFlagAdvertise, CACHE_SIZE, self->m_filters, self->m_num_filters);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_mdns_servant_fina(
						sw_mdns_servant	self)
{
	sw_corby_orb_fina(self->m_orb);
	sw_salt_fina(self->m_salt);

	return SW_OKAY;
}


sw_result
sw_mdns_servant_load_file(
						sw_mdns_servant	self,
						sw_const_string	file)
{
	enum { LINE_LEN = 1024 };
	int			lineno = 0;
	sw_int8		line[LINE_LEN];
	sw_size_t	len;
	sw_result	err = SW_OKAY;

	FILE		*	f = fopen(file, "r");
	sw_check(f, exit, err = SW_E_UNKNOWN);

	while (len = LINE_LEN, sw_mdns_servant_getline(self, f, line, &len) == SW_OKAY)
	{
		lineno++;

		if ( len && ( line[0] != '#') )
		{
			sw_int8 			name[256];
			sw_int8 			type[256];
			sw_int8 			domain[256];
			sw_uint16		port;
			sw_text_record	text_record;
	
			if (sw_mdns_servant_parse_conf_line(self, line, len, name, type, domain, &port, &text_record, file, lineno ) == SW_OKAY)
			{
				DNSStatus				status;
				DNSRegistrationRef	ref;

				sw_debug( SW_LOG_VERBOSE, "registering %s, %s, %s, %d, %s", name, type, domain, port, text_record );

				status = DNSRegistrationCreate(
									kDNSRegistrationFlagPreFormattedTextRecord|kDNSRegistrationFlagAutoRenameOnConflict,
									name,
									type,
									domain,
									port,
									sw_text_record_bytes(text_record),
									sw_text_record_len(text_record),
									NULL,
									NULL,
									(DNSRegistrationCallBack) sw_mdns_servant_publish_callback2,
									NULL,
									&ref);

				if ( !status )
				{
					sw_debug( SW_LOG_NOTICE, "registered %s\n", name );
				}
				else
				{
					sw_debug( SW_LOG_ERROR, "error encountered while registering %s\n", name );
				}

				sw_text_record_fina(text_record);
			}
		}
		else
		{
			sw_debug( SW_LOG_ERROR, "%s:%d: syntax error", file, lineno );
			line[ len ] = '\0';
		}
	}

exit:

	if (f)
	{
		fclose(f);
	}

	return err;
}


sw_result
sw_mdns_servant_check_version(
						sw_mdns_servant			self,
						const sw_corby_object	client,
						sw_corby_channel			channel,
						sw_uint8						version)
{
	static sw_string					op 		=	"check_version_reply";
	static sw_uint32					op_len	=	20;
	sw_corby_buffer 					buffer;
	sw_mdns_servant_client_node	node;
	sw_result							err;

	SW_UNUSED_PARAM(channel);

	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	err = sw_corby_object_start_request(client, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	/*
	 * now check
	 */
	if (version == SW_MDNS_PROTOCOL_VERSION)
	{
		err = sw_corby_buffer_put_uint32(buffer, SW_OKAY);
		sw_check_okay(err, exit);
	}
	else
	{
		err = sw_corby_buffer_put_uint32(buffer, SW_E_UNKNOWN);
		sw_check_okay(err, exit);
	}

	err = sw_corby_buffer_put_uint8(buffer, SW_MDNS_PROTOCOL_VERSION);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(client, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_mdns_servant_publish_host(
						sw_mdns_servant			self,
						sw_uint32					interface_index,
						sw_const_string			name,
						sw_const_string			domain,
						sw_ipv4_address			address,
						const sw_corby_object	client,
						sw_corby_channel			channel,
						sw_discovery_oid			oid)
{
	DNSNetworkAddress					addr;
	sw_mdns_servant_client_node	node;
	sw_result							err;

	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_publish_reply		=	sw_mdns_servant_publish_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_publish_host_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;
	
	addr.addressType				=	kDNSNetworkAddressTypeIPv4;
	addr.u.ipv4.addr.v32			=	sw_ipv4_address_saddr(address);

	err = DNSHostRegistrationCreate(
						kDNSHostRegistrationFlagNone,
						name,
						domain,
						&addr,
						interface_index,
						sw_mdns_servant_publish_host_callback,
						node,
						&node->m_host_registration_ref);
	sw_check_okay(err, exit);
						
exit:

	return err;
}


static sw_result
sw_mdns_servant_publish_host_cleanup(
								sw_mdns_servant		self,
								sw_mdns_servant_client_node	node)
{
	sw_debug(SW_LOG_VERBOSE, "cleaning up %d\n", node->m_oid);

	/*
	 * we cleanup the client node in the callback function
	 */
	DNSHostRegistrationRelease(node->m_host_registration_ref, 0);

	if (node->m_cleanup != NULL)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


sw_result
sw_mdns_servant_publish(
						sw_mdns_servant				self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				type,
						sw_const_string				domain,
						sw_const_string				host,
						sw_port							port,
						sw_octets						text_record,
						sw_uint32							text_record_len,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err = SW_OKAY;

	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_publish_reply		=	sw_mdns_servant_publish_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_publish_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;
	
	err = DNSRegistrationCreate(
						kDNSRegistrationFlagPreFormattedTextRecord|kDNSRegistrationFlagAutoRenameOnConflict,
						name,
						type,
						domain,
						port,
						text_record,
						text_record_len,
						host,
						interface_index,
						(DNSRegistrationCallBack) sw_mdns_servant_publish_callback,
						node,
						&node->m_registration_ref);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_mdns_servant_publish_update(
						sw_mdns_servant				self,
						sw_corby_channel				channel,
						sw_discovery_oid	oid,
						sw_octets						text_record,
						sw_uint32							text_record_len)
{
	sw_mdns_servant_client_node	node;
	sw_result							err = SW_OKAY;

	sw_assert(self);
	
	sw_debug(SW_LOG_NOTICE, "looking up client %d\n", oid);

	node = sw_mdns_servant_lookup_node(self, channel, oid);
	sw_check(node, exit, err = SW_E_UNKNOWN);

	DNSRegistrationUpdate(node->m_registration_ref,	0, NULL, text_record, text_record_len, 120);

exit:

	return err;
}


static sw_result
sw_mdns_servant_publish_cleanup(
								sw_mdns_servant		self,
								sw_mdns_servant_client_node	node)
{
	/*
	 * we cleanup the client node in the callback function
	 */
	DNSRegistrationRelease(node->m_registration_ref, 0);

	if (node->m_cleanup != NULL)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


sw_result
sw_mdns_servant_browse_domains(
						sw_mdns_servant			self,
						sw_uint32					interface_index,
						const sw_corby_object	client,
						sw_corby_channel			channel,
						sw_discovery_oid			oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err;
	
	/*
	 * try to allocate space
	 */
	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_browse_reply			= 	sw_mdns_servant_browse_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_browse_domains_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;

	err = DNSBrowserCreate(0, (DNSBrowserCallBack) sw_mdns_servant_browse_callback, node, &node->m_browser_ref);
	sw_check_okay(err, exit);

	err = DNSBrowserStartDomainSearch(node->m_browser_ref, 0, interface_index);
	sw_check_okay(err, exit);
	
exit:

	return err;
}


static sw_result
sw_mdns_servant_browse_domains_cleanup(
									sw_mdns_servant		self,
									sw_mdns_servant_client_node	node)
{
	DNSBrowserRelease(node->m_browser_ref, 0);

	if (node->m_cleanup != NULL)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


sw_result
sw_mdns_servant_browse_services(
						sw_mdns_servant	self,
						sw_uint32					interface_index,
						sw_const_string			type,
						sw_const_string			domain,
						const sw_corby_object	client,
						sw_corby_channel			channel,
						sw_discovery_oid			oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err;
	
	/*
	 * try to allocate space
	 */
	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_browse_reply			= 	sw_mdns_servant_browse_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_browse_services_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;

	err = DNSBrowserCreate(0, (DNSBrowserCallBack) sw_mdns_servant_browse_callback, node, &node->m_browser_ref);
	sw_check_okay(err, exit);

	err = DNSBrowserStartServiceSearch(node->m_browser_ref, 0, interface_index, type, domain);
	sw_check_okay(err, exit);
	
exit:

	return err;
}


static sw_result
sw_mdns_servant_browse_services_cleanup(
							sw_mdns_servant		self,
							sw_mdns_servant_client_node	node)
{
	DNSBrowserRelease(node->m_browser_ref, 0);

	if (node->m_cleanup)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


sw_result
sw_mdns_servant_resolve(
						sw_mdns_servant		self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				type,
						sw_const_string				domain,
						const sw_corby_object		client,
						sw_corby_channel				channel,
						sw_discovery_oid				oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err;
	
	/*
	 * try to allocate space
	 */
	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_resolve_reply		= 	sw_mdns_servant_resolve_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_resolve_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;

	err = DNSResolverCreate(0, 0, name, type, domain, (DNSResolverCallBack) sw_mdns_servant_resolve_callback, node, NULL, &node->m_resolver_ref);
	sw_check_okay(err, exit);

exit:

	return err;
}


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
						sw_discovery_oid				oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err;

	SW_UNUSED_PARAM(flags);

	/*
	 * try to allocate space
	 */
	node = (sw_mdns_servant_client_node) sw_malloc(sizeof(struct _sw_mdns_servant_client_node));
	err = sw_translate_error(node, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(node, 0, sizeof(struct _sw_mdns_servant_client_node));
	sw_mdns_servant_add_node(self, node);

	/*
	 * now do garden variety initialization
	 */
	node->m_self					=	self;
	node->m_query_record_reply	= 	sw_mdns_servant_query_record_reply;
	node->m_extra					=	client;
	node->m_channel				=	channel;
	node->m_cleanup_internal	=	sw_mdns_servant_query_record_cleanup;
	node->m_cleanup				=	sw_mdns_servant_cleanup;
	node->m_oid						=	oid;
	
printf("%s creating record \n");
	err = DNSQueryRecordCreate(0, interface_index, fullname, rrtype, rrclass, (DNSQueryRecordCallBack) sw_mdns_servant_query_record_callback, node, &node->m_query_record_ref);
	sw_check_okay(err, exit);

exit:

printf("%s done\n");
	return err;
}


sw_result
sw_mdns_servant_cancel(
						sw_mdns_servant	self,
						sw_corby_channel	channel,
						sw_discovery_oid	oid)
{
	sw_mdns_servant_client_node	node;
	sw_result							err;

	sw_debug(SW_LOG_VERBOSE, "looking up client %d\n", oid);
	
	node = sw_mdns_servant_lookup_node(self, channel, oid);
	sw_check(node, exit, err = SW_E_UNKNOWN);

	err = (*node->m_cleanup_internal)(self, node);
	sw_check_okay(err, exit);

exit:

	return err;
}


static sw_result
sw_mdns_servant_resolve_cleanup(
							sw_mdns_servant		self,
							sw_mdns_servant_client_node	node)
{
	DNSResolverRelease(node->m_resolver_ref, 0);

	if (node->m_cleanup)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_query_record_cleanup(
							sw_mdns_servant					self,
							sw_mdns_servant_client_node	node)
{
	DNSQueryRecordRelease(node->m_query_record_ref);

	if (node->m_cleanup)
	{
		(*node->m_cleanup)(self, node->m_extra);
	}

	sw_mdns_servant_free_node(self, node);

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_dispatcher(
            sw_corby_servant	servant,
            sw_salt				salt,
            sw_corby_orb		orb,
            sw_corby_channel  channel,
            sw_corby_message  message,
            sw_corby_buffer   buffer,
            sw_const_string   op,
            sw_uint32         op_len,
            sw_uint32			request_id,
            sw_uint8				endian)
{
	sw_mdns_servant	self = (sw_mdns_servant) servant;
	sw_result			err;

	SW_UNUSED_PARAM(salt);
	SW_UNUSED_PARAM(orb);
	SW_UNUSED_PARAM(message);
	SW_UNUSED_PARAM(op_len);
	SW_UNUSED_PARAM(request_id);

	if (sw_strcmp("publish_host", op) == 0)
	{
		sw_uint32							len;
		sw_uint32			interface_index;
		sw_string						name;
		sw_string						domain;
		sw_uint32							saddr;
		sw_ipv4_address				address;
		sw_corby_object				client;
		sw_discovery_oid	oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &name, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &saddr, endian);
		sw_check_okay(err, exit);

		err = sw_ipv4_address_init_from_saddr(&address, saddr);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);
		
		err = sw_mdns_servant_publish_host(self, interface_index, name, domain, address, client, channel, oid);

		if (err)
		{
			err = sw_mdns_servant_publish_reply(NULL, oid, SW_DISCOVERY_PUBLISH_INVALID, client);
			sw_check_okay(err, exit);
		}
	}
	else if (sw_strcmp("publish", op) == 0)
	{
		sw_uint32							len;
		sw_uint32			interface_index;
		sw_string						name;
		sw_string						type;
		sw_string						domain;
		sw_string						host;
		sw_port							port;
		sw_octets						text_record;
		sw_uint32							text_record_len;
		sw_corby_object				client;
		sw_discovery_oid	oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &name, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &type, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &host, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint16(buffer, &port, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_sized_octets(buffer, &text_record, &text_record_len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);
		
		err = sw_mdns_servant_publish(self, interface_index, name, type, domain, host, port, text_record, text_record_len, client, channel, oid);

		if (err)
		{
			err = sw_mdns_servant_publish_reply(NULL, oid, SW_DISCOVERY_PUBLISH_INVALID, client);
			sw_check_okay(err, exit);
		}
	}
	else if (sw_strcmp("publish_update", op) == 0)
	{
		sw_discovery_oid	oid;
		sw_octets			text_record;
		sw_uint32			text_record_len;

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_sized_octets(buffer, &text_record, &text_record_len, endian);
		sw_check_okay(err, exit);

		err = sw_mdns_servant_publish_update(self, channel, oid, text_record, text_record_len);
		sw_check_okay(err, exit);
	}
	else if (sw_strcmp("browse_domains", op) == 0)
	{
		sw_uint32			interface_index;
		sw_corby_object	client;
		sw_discovery_oid	oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);
		
		err = sw_mdns_servant_browse_domains(self, interface_index, client, channel, oid);

		if (err)
		{
			err = sw_mdns_servant_browse_reply(NULL, oid, SW_DISCOVERY_BROWSE_INVALID, interface_index, NULL, NULL, NULL, client);
			sw_check_okay(err, exit);
		}
	}
	else if (sw_strcmp("browse_services", op) == 0)
	{
		sw_uint32			len;
		sw_uint32			interface_index;
		sw_string			type;
		sw_string			domain;
		sw_corby_object	client;
		sw_discovery_oid	oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &type, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);
		
		err = sw_mdns_servant_browse_services(self, interface_index, type, domain, client, channel, oid);

		if (err)
		{
			err = sw_mdns_servant_browse_reply(NULL, oid, SW_DISCOVERY_BROWSE_INVALID, interface_index, NULL, NULL, NULL, client);
			sw_check_okay(err, exit);
		}
	}
	else if (sw_strcmp("resolve", op) == 0)
	{
		sw_uint32			len;
		sw_uint32			interface_index;
		sw_string			name;
		sw_string			type;
		sw_string			domain;
		sw_corby_object	client;
		sw_discovery_oid	oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &name, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &type, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &domain, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);

		err = sw_mdns_servant_resolve(self, interface_index, name, type, domain, client, channel, oid);
		sw_check_okay(err, exit);
	}
	else if (sw_strcmp("query_record", op) == 0)
	{
		sw_uint32			interface_index;
		sw_uint32			flags;
		sw_string			fullname;
		sw_uint16			rrtype;
		sw_uint16			rrclass;
		sw_corby_object	client;
		sw_discovery_oid	oid;
		sw_uint32			len;

		err = sw_corby_buffer_get_uint32(buffer, &interface_index, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &flags, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_zerocopy_cstring(buffer, &fullname, &len, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint16(buffer, &rrtype, endian);
		sw_check_okay(err, exit);	

		err = sw_corby_buffer_get_uint16(buffer, &rrclass, endian);
		sw_check_okay(err, exit);	

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);

		err = sw_mdns_servant_query_record(self, interface_index, flags, fullname, rrtype, rrclass, client, channel, oid);
		sw_check_okay(err, exit);
	}
	else if (sw_strcmp("cancel", op) == 0)
	{
		sw_discovery_oid oid;
		
		err = sw_corby_buffer_get_uint32(buffer, &oid, endian);
		sw_check_okay(err, exit);

		err = sw_mdns_servant_cancel(self, channel, oid);
		sw_check_okay(err, exit);
	}
	else if (sw_strcmp("check_version", op) == 0)
	{
		sw_corby_object	client;
		sw_uint8 			version;

		err = sw_corby_buffer_get_object(buffer, &client, endian);
		sw_check_okay(err, exit);

		err = sw_corby_object_set_channel(client, channel);
		sw_check_okay(err, exit);

		err = sw_corby_buffer_get_uint8(buffer, &version);
		sw_check_okay(err, exit);

		err = sw_mdns_servant_check_version(self, client, channel, version);
		sw_check_okay(err, exit);
	}
	else
	{
		err = SW_E_CORBY_BAD_OPERATION;
	}

exit:

	return err;
}


static sw_result
sw_mdns_servant_connection_notifier(
				sw_corby_orb_observer			handler,
				sw_salt								salt,
				sw_corby_orb						orb,
				struct _sw_corby_channel	*	channel,
				sw_opaque							extra)
{
	sw_mdns_servant servant = (sw_mdns_servant) handler;

	SW_UNUSED_PARAM(salt);
	SW_UNUSED_PARAM(orb);
	SW_UNUSED_PARAM(extra);

	return sw_mdns_servant_cleanup_client(servant, channel);
}


static sw_result
sw_mdns_servant_cleanup(
				sw_mdns_servant	self,
				sw_opaque			extra)
{
	sw_corby_object object = (sw_corby_object) extra;

	SW_UNUSED_PARAM(self);

	sw_corby_object_fina(object);

	return SW_TRUE;
}


static sw_result
sw_mdns_servant_publish_reply(
				sw_discovery						discovery,
				sw_discovery_oid					oid,
				sw_discovery_publish_status	status,
				sw_opaque							extra)
{
	static sw_string		op			=	"publish_reply";
	static sw_uint32		op_len	=	14;
	sw_corby_object		client;
	sw_corby_buffer		buffer;
	sw_result				err;

	SW_UNUSED_PARAM(discovery);

	sw_debug(SW_LOG_VERBOSE, "sw_mdns_servant_publish_reply\n");

	client	= (sw_corby_object) extra;
	
	err = sw_corby_object_start_request(client, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint8(buffer, (sw_uint8) status);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(client, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);
	
exit:

	return err;
}


static sw_result
sw_mdns_servant_browse_reply(
				sw_discovery					discovery,
				sw_discovery_oid				oid,
				sw_discovery_browse_status	status,
				sw_uint32						interface_index,
				sw_const_string				name,
				sw_const_string				type,
				sw_const_string				domain,
				sw_opaque						extra)
{
	static sw_string		op			=	"browse_reply";
	static sw_uint32		op_len	=	13;
	sw_corby_object		client;
	sw_corby_buffer		buffer;
	sw_result				err;

	SW_UNUSED_PARAM(discovery);

	sw_debug(SW_LOG_VERBOSE, "%s %s %s\n",
				(name != NULL) ? name : "<null>",
				(type != NULL) ? type : "<null>",
				(domain != NULL) ? domain : "<null>");
	
	client	= (sw_corby_object) extra;

	err = sw_corby_object_start_request(client, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint8(buffer, (sw_uint8) status);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, name);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, type);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(client, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);
	
exit:

	return err;
}


static sw_result
sw_mdns_servant_resolve_reply(
				sw_discovery			discovery,
				sw_discovery_oid		oid,
				sw_uint32				interface_index,
				sw_const_string		name,
				sw_const_string		type,
				sw_const_string		domain,
				sw_ipv4_address		address,
				sw_port					port,
				sw_octets				text_record,
				sw_uint32				text_record_len,
				sw_opaque				extra)
{
	static sw_string		op			=	"resolve_reply";
	static sw_uint32		op_len	=	14;
	sw_corby_object		client;
	sw_corby_buffer		buffer;
	sw_result				err;

	SW_UNUSED_PARAM(discovery);

	sw_debug(SW_LOG_VERBOSE, "%s, %s, %s, %d\n",
				(name != NULL) ? name : "<null>",
				(type != NULL) ? type : "<null>",
				(domain != NULL) ? domain : "<null>",
				(int) port);
	
	client = (sw_corby_object) extra;

	err = sw_corby_object_start_request(client, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, name);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, type);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, domain);
	sw_check_okay(err, exit);
	
	err = sw_corby_buffer_put_uint32(buffer, sw_ipv4_address_saddr(address));
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, port);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_sized_octets(buffer, text_record, text_record_len);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(client, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	if (err != SW_OKAY)
	{
		sw_debug(SW_LOG_ERROR, "unable to communicate with client");
	}
	
	return err;
}


static sw_result
sw_mdns_servant_query_record_reply(
				sw_discovery								session,
				sw_discovery_oid							oid,
				sw_discovery_query_record_status		status,
				sw_uint32									interface_index,
				sw_const_string							fullname,
				sw_uint16									rrtype,
				sw_uint16									rrclass,
				sw_uint16									rrdatalen,	
				sw_const_octets							rrdata,
				sw_uint32									ttl,
				sw_opaque									extra)
{
	static sw_string		op			=	"query_record_reply";
	static sw_uint32		op_len	=	19;
	sw_corby_object		client;
	sw_corby_buffer		buffer;
	sw_result				err;

	SW_UNUSED_PARAM(session);
	
	client	= (sw_corby_object) extra;

	err = sw_corby_object_start_request(client, op, op_len, SW_FALSE, &buffer);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, oid);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, status);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, interface_index);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_cstring(buffer, fullname);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, rrtype);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint16(buffer, rrclass);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_sized_octets(buffer, rrdata, (sw_uint32) rrdatalen);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_uint32(buffer, ttl);
	sw_check_okay(err, exit);

	err = sw_corby_object_send(client, buffer, NULL, NULL, NULL);
	sw_check_okay(err, exit);

exit:

	if (err != SW_OKAY)
	{
		sw_debug(SW_LOG_ERROR, "unable to communicate with client");
	}
	
	return err;
}


sw_result
sw_mdns_servant_cleanup_client(
								sw_mdns_servant	self,
								sw_corby_channel	channel)
{
	sw_mdns_servant_client_node node = self->m_clients;

	while (node)
	{
		if (node->m_channel == channel)
		{
			sw_mdns_servant_client_node next = node->m_txen;
			sw_assert(node->m_cleanup_internal);
			(*node->m_cleanup_internal)(self, node);
			node = next;
		}
		else
		{
			node = node->m_txen;
		}
	}

	return SW_OKAY;
}


static void
sw_mdns_servant_publish_host_callback(
			void							*	inContext, 
			DNSHostRegistrationRef		inRef, 
			DNSStatus						inStatusCode, 
			void							*	inData )
{
	sw_mdns_servant_client_node node;

	SW_UNUSED_PARAM(inRef);
	SW_UNUSED_PARAM(inData);
	
	node = (sw_mdns_servant_client_node) inContext;
	
	(node->m_publish_reply)(NULL, node->m_oid, (inStatusCode == kDNSNoErr) ? SW_DISCOVERY_PUBLISH_STARTED : SW_DISCOVERY_PUBLISH_INVALID, node->m_extra);
}


static sw_result
sw_mdns_servant_publish_callback(
				void								*	inContext,
				DNSRegistrationRef            inRef,
				DNSStatus                  	inStatusCode,
				const DNSRegistrationEvent *  inEvent)
{
	sw_mdns_servant_client_node node;

	SW_UNUSED_PARAM(inRef);
	
	node = (sw_mdns_servant_client_node) inContext;
	
	if (inStatusCode != kDNSNoErr)
	{
		sw_debug(SW_LOG_ERROR, "inStatusCode is %d\n", inStatusCode);
		return SW_E_INIT;
	}

	switch (inEvent->type)
	{
		case kDNSRegistrationEventTypeInvalid: 
		{
			(node->m_publish_reply)(NULL, node->m_oid, SW_DISCOVERY_PUBLISH_INVALID, node->m_extra);
		}
		break;

		case kDNSRegistrationEventTypeRelease:
		{
			(node->m_publish_reply)(NULL, node->m_oid, SW_DISCOVERY_PUBLISH_STOPPED, node->m_extra);
		}
		break;

		case kDNSRegistrationEventTypeRegistered:
		{
			(node->m_publish_reply)(NULL, node->m_oid, SW_DISCOVERY_PUBLISH_STARTED, node->m_extra);
		}
		break;

		case kDNSRegistrationEventTypeNameCollision:
		{
			(node->m_publish_reply)(NULL, node->m_oid, SW_DISCOVERY_PUBLISH_NAME_COLLISION, node->m_extra);
		}
		break;
	}

	
	return SW_OKAY;
}



static sw_result
sw_mdns_servant_publish_callback2(
				void								*	inContext,
				DNSRegistrationRef            inRef,
				DNSStatus                  	inStatusCode,
				const DNSRegistrationEvent *  inEvent)
{
	SW_UNUSED_PARAM(inContext);
	SW_UNUSED_PARAM(inRef);
	SW_UNUSED_PARAM(inEvent);

	if (inStatusCode != kDNSNoErr)
	{
		sw_debug(SW_LOG_ERROR, "inStatusCode is %d\n", inStatusCode);
		return SW_E_INIT;
	}

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_browse_callback(
			void							*	inContext,
			DNSBrowserRef					inRef,
			DNSStatus						inStatusCode,
			const DNSBrowserEvent	*	inEvent)
{
	sw_mdns_servant_client_node node;

	SW_UNUSED_PARAM(inRef);
	
	node = (sw_mdns_servant_client_node) inContext;

	if (inStatusCode != kDNSNoErr)
	{
		sw_debug(SW_LOG_ERROR, "inStatusCode is %d\n", inStatusCode);
		return SW_E_INIT;
	}
	
	switch (inEvent->type)
	{
		case kDNSBrowserEventTypeInvalid:
		{	
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_INVALID, 0, NULL, NULL, NULL, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeRelease:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_RELEASE, 0, NULL, NULL, NULL, node->m_extra);

		}
		break;
		
		case kDNSBrowserEventTypeAddDomain:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_ADD_DOMAIN, 0, NULL, NULL, inEvent->data.addDomain.domain, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeAddDefaultDomain:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_ADD_DEFAULT_DOMAIN, 0, NULL, NULL, inEvent->data.addDomain.domain, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeRemoveDomain:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_REMOVE_DOMAIN, 0, NULL, NULL, inEvent->data.addDomain.domain, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeAddService:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_ADD_SERVICE, (sw_uint32) inEvent->data.addService.interfaceID, inEvent->data.addService.name, inEvent->data.addService.type, inEvent->data.addService.domain, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeRemoveService:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_REMOVE_SERVICE, (sw_uint32) inEvent->data.addService.interfaceID, inEvent->data.removeService.name, inEvent->data.removeService.type, inEvent->data.removeService.domain, node->m_extra);
		}
		break;
		
		case kDNSBrowserEventTypeResolved:
		{
			(node->m_browse_reply)(NULL, node->m_oid, SW_DISCOVERY_BROWSE_RESOLVED, (sw_uint32) inEvent->data.addService.interfaceID, inEvent->data.addService.name, inEvent->data.addService.type, inEvent->data.addDomain.domain, node->m_extra);
		}
		break;
	}
	return SW_OKAY;
}


static sw_result
sw_mdns_servant_resolve_callback(
				void								*	inContext,
				DNSResolverRef						inRef,
				DNSStatus                  	inStatusCode,
				const DNSResolverEvent		*	inEvent)
{
	sw_mdns_servant_client_node	node;
	sw_ipv4_address				address;
	char								name_buf[16];
	struct in_addr 				dummy;

	SW_UNUSED_PARAM(inRef);
	
	node = (sw_mdns_servant_client_node) inContext;

	if (inStatusCode != kDNSNoErr)
	{
		sw_debug(SW_LOG_ERROR, "inStatusCode is %d\n", inStatusCode);
		return SW_E_INIT;
	}
	
	/*
	 * ignore events that are not resolve events
	 */
	if (inEvent->type != kDNSResolverEventTypeResolved)
	{
		return SW_OKAY;
	}

	if (inEvent->data.resolved.address.addressType != kDNSNetworkAddressTypeIPv4)
	{
		return SW_OKAY;
	}

	sw_ipv4_address_init_from_saddr(&address, inEvent->data.resolved.address.u.ipv4.addr.v32);

	dummy.s_addr = inEvent->data.resolved.address.u.ipv4.addr.v32;

	sw_debug(SW_LOG_VERBOSE, "name %s, address %s %s, port = %d\n", inEvent->data.resolved.name, sw_ipv4_address_name(address, name_buf, 16), inet_ntoa(dummy), ntohs(inEvent->data.resolved.address.u.ipv4.port.v16));

	(node->m_resolve_reply)(NULL, node->m_oid, (sw_uint32) inEvent->data.resolved.interfaceID, inEvent->data.resolved.name, inEvent->data.resolved.type, inEvent->data.resolved.domain, address, ntohs(inEvent->data.resolved.address.u.ipv4.port.v16), (sw_octets) inEvent->data.resolved.textRecordRaw, inEvent->data.resolved.textRecordRawSize, node->m_extra);

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_query_record_callback(
				void					*	inContext, 
				DNSQueryRecordRef 	inRef, 
				DNSStatus 				inStatusCode,
				mDNSu32					inFlags,
				mDNSu32					interfaceIndex,
				const char			*	inFullName,
				mDNSu16					inRRClass,
				mDNSu16					inRRType,
				mDNSu16					inRDataLen,
				void					*	inRData,
				mDNSu32					inTTL )
{
	sw_mdns_servant_client_node node;

	SW_UNUSED_PARAM(inRef);
	SW_UNUSED_PARAM(inStatusCode);

	node = (sw_mdns_servant_client_node) inContext;

	(*node->m_query_record_reply)(NULL, node->m_oid, inFlags ? SW_DISCOVERY_QUERY_RECORD_ADD : 0, interfaceIndex, inFullName, inRRClass, inRRType, inRDataLen, inRData, inTTL, node->m_extra);

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_add_node(
					sw_mdns_servant					self,
					sw_mdns_servant_client_node	node)
{
	node->m_txen	=	NULL;
	node->m_verp	=	NULL;

	if (self->m_clients)
	{
		self->m_clients->m_verp = node;
	}
	
	node->m_txen		=	self->m_clients;
	self->m_clients	=	node;
	
	return SW_OKAY;
}


static sw_mdns_servant_client_node
sw_mdns_servant_lookup_node(
					sw_mdns_servant	self,
					sw_corby_channel	channel,
					sw_uint32				id)
{
	sw_mdns_servant_client_node node;
	
	for (node = self->m_clients; node; node = node->m_txen)
	{
		if ((node->m_channel == channel) && (node->m_oid == id))
		{
			return node;
		}
	}

	return NULL;
}


static sw_result
sw_mdns_servant_free_node(
					sw_mdns_servant					self,
					sw_mdns_servant_client_node	node)
{
	if (node->m_verp == NULL)
	{
		self->m_clients = node->m_txen;

		if (node->m_txen != NULL)
		{
			node->m_txen->m_verp = NULL;
		}
	}
	else if (node->m_txen == NULL)
	{
		node->m_verp->m_txen = NULL;
	}
	else
	{
		node->m_verp->m_txen = node->m_txen;
		node->m_txen->m_verp = node->m_verp;
	}

	sw_free(node->m_name);
	sw_free(node->m_reg_type);
	sw_free(node->m_domain);
	sw_free(node->m_attrs);
	sw_free(node);

	
	return SW_OKAY;
}


static sw_result
sw_mdns_servant_getline(
					sw_mdns_servant	self,
					FILE				*	f,
					sw_string			line,
					sw_size_t		*	len)
{
	const sw_size_t maxlen = *len;

	SW_UNUSED_PARAM(self);

	*len = 0;

	while (*len < maxlen)
	{
		int ch = fgetc(f);

		if (ch == EOF)
		{
			if (*len != 0)
			{
				return SW_OKAY;
			}
			else
			{
				return SW_E_EOF;
			}
		}
		else
		{
			line[*len] = (char) (ch & 0xff);
			(*len)++;

			if (ch == '\n')
			{
				return SW_OKAY;
			}
		}
	}

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_next_token(
					sw_mdns_servant	self,
					sw_string			line,
					sw_uint32		*	index,
					sw_size_t			len,
					sw_string			token)
{
	sw_uint32 tokenIndex = 0;

	SW_UNUSED_PARAM(self);

	while ((isspace(line[*index])) && ((*index) < len))
	{
		(*index)++;
	}

	if ((*index) == len)
	{
		return SW_E_UNKNOWN;
	}

	if (line[*index] == '"')
	{
		(*index)++;

		while ((line[*index] != '"') && ((*index) < len))
		{
			token[tokenIndex] = line[*index];
			tokenIndex++;
			(*index)++;
		}

		(*index)++;
	}
	else
	{
		while ((!isspace(line[*index])) && ((*index) < len))
		{
			token[tokenIndex] = line[*index];
			tokenIndex++;
			(*index)++;
		}
	}

	token[tokenIndex] = '\0';

	return SW_OKAY;
}


static sw_result
sw_mdns_servant_parse_conf_line(
					sw_mdns_servant	self,
					sw_string			line,
					sw_size_t			len,
					sw_string			name,
					sw_string			type,
					sw_string			domain,
					sw_uint16		*	port,
					sw_text_record	*	text_record,
					sw_const_string	file,
					int					lineno)
{
	sw_uint32	index = 0;
	sw_int8		temp[256];
	sw_result	err;

	err = sw_mdns_servant_next_token(self, line, &index, len, name);
	sw_check_okay( err, exit );

	err = sw_mdns_servant_next_token(self, line, &index, len, type);
	sw_check_okay( err, exit );

	err = sw_mdns_servant_next_token(self, line, &index, len, domain);
	sw_check_okay( err, exit );
	
	err = sw_mdns_servant_next_token(self, line, &index, len, temp);
	sw_check_okay( err, exit );

	(*port) = (sw_uint16) atoi(temp);

	err = sw_text_record_init(text_record);
	sw_check_okay( err, exit );

	while (sw_mdns_servant_next_token(self, line, &index, len, temp) == SW_OKAY)
	{
		sw_text_record_add_string(*text_record, temp);
	}

	err = SW_OKAY;

exit:

	if ( err )
	{
		sw_debug( SW_LOG_ERROR, "%s:%d: syntax error", file, lineno );
	}

	return err;
}

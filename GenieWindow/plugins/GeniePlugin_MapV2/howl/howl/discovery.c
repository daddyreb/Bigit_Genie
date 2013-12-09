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

#include "discovery_i.h"
#include <salt/debug.h>

extern sw_uint16 g_port;

sw_result
sw_discovery_init(
						sw_discovery	*	self)
{
	sw_result err;

	*self = (sw_discovery) sw_malloc(sizeof(struct _sw_discovery));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*self, 0, sizeof(struct _sw_discovery));

	/*
	 * initialize salt
	 */
	err = sw_salt_init(&(*self)->m_salt, 0, NULL);
	sw_check_okay(err, exit);

	//err = sw_mdns_stub_init(&(*self)->m_stub, (*self)->m_salt, (*self), 5335);
	err = sw_mdns_stub_init(&(*self)->m_stub, (*self)->m_salt, (*self), g_port);
	sw_check_okay(err, exit);

	err = sw_mdns_stub_check_version((*self)->m_stub);
	sw_check_okay(err, exit);

exit:

	if (err && *self)
	{
		sw_discovery_fina(*self);
		*self = NULL;
	}

	return err;
}


sw_result
sw_discovery_fina(
						sw_discovery		self)
{
	if (self->m_stub)
	{
		sw_mdns_stub_fina(self->m_stub);
	}

	if (self->m_servant && self->m_servant_fina_func)
	{
		(*self->m_servant_fina_func)(self->m_servant);
		// sw_mdns_servant_delete(self->m_servant);
	}

	if (self->m_salt)
	{
		sw_salt_fina(self->m_salt);
	}

	sw_free( self );

	return SW_OKAY;
}


sw_result
sw_discovery_publish_host(
						sw_discovery					self,
						sw_uint32						interface_index,
						sw_const_string				name,
						sw_const_string				domain,
						sw_ipv4_address				address,
						sw_discovery_publish_reply	reply,
						sw_opaque						extra,
						sw_discovery_oid			*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_publish_host(self->m_stub, interface_index, name, domain, address, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_publish(
						sw_discovery						self,
						sw_uint32							interface_index,
						sw_const_string					name,
						sw_const_string					type,
						sw_const_string					domain,
						sw_const_string					host,
						sw_port								port,
						sw_octets							text_record,
						sw_uint32							text_record_len,
						sw_discovery_publish_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_publish(self->m_stub, interface_index, name, type, domain, host, port, text_record, text_record_len, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}



sw_result
sw_discovery_publish_update(
						sw_discovery		self,
						sw_discovery_oid	oid,
						sw_octets			text_record,
						sw_uint32			text_record_len)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_publish_update(self->m_stub, oid, text_record, text_record_len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_browse_domains(
						sw_discovery						self,
						sw_uint32							interface_index,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_browse_domains(self->m_stub, interface_index, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_browse(
						sw_discovery						self,
						sw_uint32							interface_index,
						sw_const_string					type,
						sw_const_string					domain,
						sw_discovery_browse_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_browse_services(self->m_stub, interface_index, type, domain, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_resolve(
						sw_discovery						self,
						sw_uint32							interface_index,
						sw_const_string					name,
						sw_const_string					type,
						sw_const_string					domain,
						sw_discovery_resolve_reply		reply,
						sw_opaque							extra,
						sw_discovery_oid				*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_resolve(self->m_stub, interface_index, name, type, domain, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_query_record(
						sw_discovery								self,
						sw_uint32									interface_index,
						sw_uint32									flags,
						sw_const_string							fullname,
						sw_uint16									rrtype,
						sw_uint16									rrclass,
						sw_discovery_query_record_reply		reply,	
						sw_opaque									extra,
						sw_discovery_oid						*	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_query_record(self->m_stub, interface_index, flags, fullname, rrtype, rrclass, reply, extra, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_cancel(
					sw_discovery		self,
					sw_discovery_oid	oid)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_cancel(self->m_stub, oid);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_run(
						sw_discovery					self)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_salt_run(self->m_salt);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_stop_run(
						sw_discovery					self)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_salt_stop_run(self->m_salt);
	sw_check_okay(err, exit);

exit:

	return err;
}
						

int
sw_discovery_socket(
						sw_discovery self)
{
	sw_sockdesc_t	ret = 0;
	sw_result		err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	ret = sw_mdns_stub_socket(self->m_stub);

exit:

	return (int) ret;
}


sw_result
sw_discovery_read_socket(
						sw_discovery					self)
{
	sw_result err;

	sw_check(self->m_stub, exit, err = SW_E_UNKNOWN);
	err = sw_mdns_stub_read_socket(self->m_stub);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_discovery_salt(
						sw_discovery		self,
						sw_salt			*	salt)
{
	*salt = self->m_salt;
	return SW_OKAY;
}

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

#include "object_i.h"
#include "channel_i.h"
#include <salt/debug.h>


sw_result
sw_corby_object_init(
							sw_corby_object	*	self)
{
	sw_result err = SW_OKAY;

	*self = (sw_corby_object) sw_malloc(sizeof(struct _sw_corby_object));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*self, 0, sizeof(struct _sw_corby_object));

	(*self)->m_bufsize = 4192;

exit:
	
	return err;
}


sw_result
sw_corby_object_init_from_url(
							sw_corby_object		*	self,
							struct _sw_corby_orb	*	orb,
							sw_const_string			url,
							sw_socket_options			options,
							sw_uint32					bufsize)
{
   sw_string			port_spec;
   sw_string			host;
   sw_string			oid;
	sw_int8				buf[256];
   sw_string			protocol_spec;
   sw_string			oid_spec;
   sw_string			tmp;
	sw_corby_ior		ior;
	sw_corby_profile	profile;
	sw_result			err = SW_OKAY;

	/*
	 * initialize
	 */
	ior		= NULL;
	profile	= NULL;
	
	
	/*
	 * try and allocate 
	 */
	*self = (sw_corby_object) sw_malloc(sizeof(struct _sw_corby_object));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*self, 0, sizeof(struct _sw_corby_object));

	(*self)->m_orb = orb;

	err = sw_corby_ior_init(&ior);
	sw_check_okay(err, exit);

	err = sw_corby_profile_init(&profile);
	sw_check_okay(err, exit);

	sw_check(url, exit, err = SW_E_CORBY_BAD_URL);

   /*
      copy to temporary storage
   */
   sw_strcpy(buf, url);
   
	/*
		find protocol
	*/
	tmp = sw_strchr(buf, ':');
	sw_check(tmp, exit, err = SW_E_CORBY_BAD_URL);

   *tmp				=	'\0';
   protocol_spec	=	buf;
   if (sw_strcmp(protocol_spec, "swop") == 0)
   {
      profile->m_tag = SW_TAG_INTERNET_IOP;
   }
   else if (sw_strcmp(protocol_spec, "uiop") == 0)
   {
      profile->m_tag = SW_TAG_UIOP;
   }
   else if (sw_strcmp(protocol_spec, "miop") == 0)
   {
		profile->m_tag = SW_TAG_MIOP;
   }
   else
   {
		err = SW_E_CORBY_BAD_URL;
		goto exit;
   }
   
   /*
	 * make sure we have "//"
	 */
   sw_check(((*++tmp == '/') && (*++tmp == '/')), exit, err = SW_E_CORBY_BAD_URL);

	/*
	 * increment past the second slash
	 */
   ++tmp;
   
	/*
	 * check to see if we have a port spec
	 */
	if ((port_spec = sw_strchr(tmp, ':')) != NULL)
	{
		*port_spec	=	'\0';
		host			=	tmp;
		tmp			=  ++port_spec;
      
		oid_spec = sw_strchr(tmp, '/');
		sw_check(oid_spec, exit, err = SW_E_CORBY_BAD_URL);
      
		*oid_spec	=	'\0';
		port_spec	=	tmp;
		oid			=	++oid_spec;
   }
   else
   {
      port_spec	=	NULL;
      oid_spec		=	sw_strchr(tmp, '/');
		sw_check(oid_spec, exit, err = SW_E_CORBY_BAD_URL);

      *oid_spec	=	'\0';
      host			=	tmp;
      oid			=	++oid_spec;
   }
   
	err = sw_ipv4_address_init_from_name(&profile->m_address, host);
	sw_check_okay(err, exit);

	profile->m_port		=	(sw_uint16) atoi(port_spec);
	profile->m_oid_len	=	(sw_uint32) sw_strlen(oid);

	profile->m_oid = (sw_string) sw_malloc(profile->m_oid_len);
	err = sw_translate_error(profile->m_oid, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memcpy(profile->m_oid, oid, profile->m_oid_len);
	profile->m_txen		=	NULL;
	ior->m_txen				=	NULL;
	ior->m_num_profiles	=	1;
	ior->m_profiles		=	profile;
	(*self)->m_profile	=	NULL;
	(*self)->m_channel	=	NULL;
	(*self)->m_options	=	options;
	(*self)->m_iors		=	ior;
	(*self)->m_bufsize	=	bufsize;
	
exit:

	if (err != SW_OKAY)
	{
		sw_corby_object_fina(*self);
	}
	
	return err;
}


sw_result
sw_corby_object_fina(
							sw_corby_object	self)
{
	if (self != NULL)
	{
		if (self->m_channel != NULL)
		{
			sw_corby_channel_fina(self->m_channel);
			self->m_channel = NULL;
		}

		if (self->m_iors != NULL)
		{
			sw_corby_ior_fina(self->m_iors);
			self->m_iors = NULL;
		}

		sw_free(self);
	}

	return SW_OKAY;
}


sw_result
sw_corby_object_start_request(
							sw_corby_object 		self,
							sw_const_string		op,
							sw_uint32				op_len,
							sw_bool					reply_expected,
							sw_corby_buffer	*	buffer)
{
	sw_result err = SW_OKAY;

	sw_assert(self);
	sw_assert(self->m_iors);
	sw_assert(self->m_iors->m_profiles);

	/*
	 * if we don't have a channel yet then let's create one
	 */
	if (self->m_channel == NULL)
	{
		sw_check(self->m_orb, exit, err = SW_E_UNKNOWN);

		sw_assert(self->m_profile == NULL);

		self->m_profile = self->m_iors->m_profiles;
		err = sw_corby_channel_init_with_profile(&self->m_channel, self->m_orb, self->m_profile, self->m_options, self->m_bufsize);
		sw_check_okay(err, exit);
	}

	sw_assert(self->m_profile);
	sw_assert(self->m_channel);

	err = sw_corby_channel_start_request(self->m_channel, self->m_profile, buffer, op, op_len, reply_expected);
	sw_check_okay(err, exit);

exit:

	if (err && self->m_channel)
	{
		sw_corby_channel_fina(self->m_channel);
		self->m_profile	=	NULL;
		self->m_channel	=	NULL;
	}

	return err;
}


sw_result
sw_corby_object_send(
							sw_corby_object					self,
							sw_corby_buffer					buffer,
							sw_corby_buffer_observer		observer,
							sw_corby_buffer_written_func	func,
							sw_opaque							extra)
{
	sw_result result;

	sw_assert(self);
	sw_assert(self->m_iors);
	sw_assert(self->m_iors->m_profiles);
	sw_assert(self->m_channel);

	if ((result = sw_corby_channel_send(self->m_channel, buffer, observer, func, extra)) != SW_OKAY)
	{
		if (result != SW_E_INPROGRESS)
		{
			sw_corby_channel_fina(self->m_channel);
			self->m_profile	=	NULL;
			self->m_channel	=	NULL;
		}

		return result;
	}

	return result;
}


sw_result
sw_corby_object_recv(
							sw_corby_object		self,
							sw_corby_message	*	message,
                     sw_corby_buffer	*	buffer,
                     sw_uint8				*	endian,
                     sw_bool					block)
{
	sw_result result;

	sw_assert(self);
	sw_assert(self->m_iors);
	sw_assert(self->m_iors->m_profiles);
	sw_assert(self->m_channel);

	if ((result = sw_corby_channel_recv(self->m_channel, NULL, message, NULL, NULL, NULL, buffer, endian, block)) != SW_OKAY)
	{
		sw_corby_channel_fina(self->m_channel);
		self->m_profile	=	NULL;
		self->m_channel	=	NULL;
	}

	return result;
}


sw_result
sw_corby_object_channel(
							sw_corby_object		self,
							sw_corby_channel	*	channel)
{
	sw_result err = SW_OKAY;

	if (self->m_channel == NULL)
	{
		sw_check(self->m_orb, exit, err = SW_E_UNKNOWN);

		if (self->m_profile == NULL)
		{
			self->m_profile = self->m_iors->m_profiles;
		}

		err = sw_corby_channel_init_with_profile(&self->m_channel, self->m_orb, self->m_profile, self->m_options, self->m_bufsize);
		sw_check_okay(err, exit);
	}

	sw_assert(self->m_channel != NULL);
	*channel = self->m_channel;

exit:

	return err;
}


sw_result
sw_corby_object_set_channel(
							sw_corby_object		self,
							sw_corby_channel		channel)
{
	sw_result err = SW_OKAY;

	sw_assert(self->m_profile == NULL);

	err = sw_corby_channel_retain(channel);
	sw_check_okay(err, exit);

	self->m_profile = self->m_iors->m_profiles;
	self->m_channel = channel;

exit:

	return err;
}

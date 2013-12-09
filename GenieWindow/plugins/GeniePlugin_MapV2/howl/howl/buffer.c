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

#include "buffer_i.h"
#include "object_i.h"
#include <salt/debug.h>


#define SW_CORBY_BUFFER_PUT_OCTET(buffer, val)			\
if (buffer->m_eptr < buffer->m_end)							\
{																		\
	*buffer->m_eptr = val;										\
	buffer->m_eptr++;												\
	err = SW_OKAY;													\
}																		\
else																	\
{																		\
	err = sw_corby_buffer_overflow(buffer, val);			\
	sw_check_okay(err, exit);									\
}

#define SW_CORBY_BUFFER_GET_OCTET(buffer, val)			\
if (buffer->m_bptr < buffer->m_eptr)						\
{																		\
	*(sw_uint8*) val = *buffer->m_bptr;						\
	buffer->m_bptr++;												\
	err = SW_OKAY;													\
}																		\
else																	\
{																		\
	err = sw_corby_buffer_underflow(buffer, (sw_uint8*) val);		\
	sw_check_okay(err, exit);									\
}

/*
   private stuff
*/
static sw_result
sw_corby_buffer_overflow(
						sw_corby_buffer	buffer,
						sw_uint8			val);

static sw_result
sw_corby_buffer_underflow(
                  sw_corby_buffer	buffer,
                  sw_uint8		*	val);
                  

sw_result
sw_corby_buffer_init(
					sw_corby_buffer	*	buffer)
{
	sw_result err = SW_OKAY;

	*buffer = (sw_corby_buffer) sw_malloc(sizeof(struct _sw_corby_buffer));
	err = sw_translate_error(*buffer, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*buffer, 0, sizeof(struct _sw_corby_buffer));

exit:

	if (err != SW_OKAY)
	{
		*buffer = NULL;
	}

	return err;
}


sw_result
sw_corby_buffer_init_with_size(
               sw_corby_buffer	*	buffer,
               sw_size_t				size)
{
	sw_result err = SW_OKAY;

	*buffer = (sw_corby_buffer) sw_malloc(sizeof(struct _sw_corby_buffer));
	err = sw_translate_error(*buffer, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(*buffer, 0, sizeof(struct _sw_corby_buffer));

	(*buffer)->m_base = (sw_octets) sw_malloc(size);
	err = sw_translate_error((*buffer)->m_base, SW_E_MEM);
	sw_check_okay_log(err, exit);
	
   (*buffer)->m_end	=	(*buffer)->m_base + size;
   (*buffer)->m_bptr	=	(*buffer)->m_base;
	(*buffer)->m_eptr	=	(*buffer)->m_base;

exit:

	if (err != SW_OKAY)
	{
		sw_corby_buffer_fina(*buffer);
		*buffer = NULL;
	}

	return err;
}


sw_result
sw_corby_buffer_init_with_delegate(
					sw_corby_buffer					*	buffer,
					sw_corby_buffer_delegate			delegate,
					sw_corby_buffer_overflow_func		overflow,
					sw_corby_buffer_underflow_func	underflow,
					sw_opaque_t								extra)
{
	sw_result err;

	err = sw_corby_buffer_init(buffer);
	sw_check_okay(err, exit);
		
	(*buffer)->m_delegate			=	delegate;
	(*buffer)->m_overflow_func		=	overflow;
	(*buffer)->m_underflow_func	=	underflow;
	(*buffer)->m_delegate_extra	=	extra;

exit:

	return err;
}


sw_result
sw_corby_buffer_init_with_size_and_delegate(
					sw_corby_buffer					*	buffer,
					sw_size_t								size,
					sw_corby_buffer_delegate			delegate,
					sw_corby_buffer_overflow_func		overflow,
					sw_corby_buffer_underflow_func	underflow,
					sw_opaque_t								extra)
{
	sw_result err;

	err = sw_corby_buffer_init_with_size(buffer, size);
	sw_check_okay(err, exit);

	(*buffer)->m_delegate			=	delegate;
	(*buffer)->m_overflow_func		=	overflow;
	(*buffer)->m_underflow_func	=	underflow;
	(*buffer)->m_delegate_extra	=	extra;

exit:

	return err;
}


sw_result
sw_corby_buffer_fina(
               sw_corby_buffer	self)
{
	if (self)
	{
		if (self->m_base)
		{
   		sw_free(self->m_base);
		}

   	sw_free(self);
	}

   return SW_OKAY;
}


void
sw_corby_buffer_reset(
				sw_corby_buffer	self)
{
	self->m_bptr = self->m_eptr = self->m_base;
}


sw_result
sw_corby_buffer_set_octets(
				sw_corby_buffer	self,
				sw_octets			octets,
				sw_size_t			size)
{
	sw_assert(self);

	self->m_base 	=	octets;
	self->m_bptr 	= 	self->m_base;
	self->m_eptr 	= 	self->m_base + size;
	self->m_end		=	self->m_base + size;
	
	return SW_OKAY;
}
				

sw_octets
sw_corby_buffer_octets(
				sw_corby_buffer	self)
{
	sw_assert(self);

	return self->m_base;
}
				
				
sw_size_t
sw_corby_buffer_bytes_used(
				sw_corby_buffer	self)
{
	sw_assert(self);
	return (sw_size_t) (self->m_eptr - self->m_bptr);
}

				
sw_size_t
sw_corby_buffer_size(
				sw_corby_buffer	self)
{
	sw_assert(self);
	return (sw_size_t) (self->m_end - self->m_base);
}


sw_result
sw_corby_buffer_put_uint8(
                  sw_corby_buffer	buffer,
                  sw_uint8				val)
{
	sw_result err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, val);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_int8(
                  sw_corby_buffer	buffer,
                  sw_int8				val)
{
	sw_result err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, val);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_uint16(
               sw_corby_buffer	buffer,
               sw_uint16			val)
{
	sw_octets octets = (sw_octets) &val;
	sw_result	err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[0]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[1]);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_int16(
               sw_corby_buffer	buffer,
               sw_int16			val)
{
	sw_octets	octets = (sw_octets) &val;
	sw_result	err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[0]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[1]);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_uint32(
               sw_corby_buffer	buffer,
               sw_uint32			val)
{
	sw_octets	octets = (sw_octets) &val;
	sw_result	err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[0]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[1]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[2]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[3]);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_int32(
					sw_corby_buffer	buffer,
					sw_int32			val)
{
	sw_octets	octets = (sw_octets) &val;
	sw_result	err;

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[0]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[1]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[2]);
	sw_check_okay(err, exit);

	SW_CORBY_BUFFER_PUT_OCTET(buffer, octets[3]);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_octets(
						sw_corby_buffer	buffer,
						sw_const_octets	val,
						sw_size_t			len)
{
	sw_result err = SW_OKAY;

	while (len)
	{
		sw_size_t size_left = buffer->m_end - buffer->m_eptr;

		if (size_left > 0)
		{
			sw_size_t min_bytes = (size_left < len) ? size_left : len;

			sw_memcpy(buffer->m_eptr, val, min_bytes);
			buffer->m_eptr += min_bytes;
			val				+=	min_bytes;
			len				-= min_bytes;
		}
		else
		{
			err = sw_corby_buffer_overflow(buffer, *val);
			sw_check_okay(err, exit);

			val++;
			len--;
		}
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_put_sized_octets(
               sw_corby_buffer	buffer,
					sw_const_octets	val,
					sw_uint32			size)
{
	sw_result err = SW_OKAY;

	err = sw_corby_buffer_put_uint32(buffer, size);
	sw_check_okay(err, exit);
	
	err = sw_corby_buffer_put_octets(buffer, val, size);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_cstring(
               sw_corby_buffer	buffer,
               sw_const_string	val)
{
	sw_uint32	len = (sw_uint32) ((val) ? sw_strlen(val) + 1 : 0);
	sw_result	err;

	err = sw_corby_buffer_put_sized_octets(buffer, (sw_const_octets) val, len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_put_profile(
					sw_corby_buffer			self,
					const sw_corby_profile	profile)
{
	sw_uint32	offset;
	sw_uint32	encap_size;
	char	host[16];
	sw_result	err;
	
	/*
	 * first thing is write tag
	 */
	err = sw_corby_buffer_put_uint32(self, profile->m_tag);
	sw_check_okay(err, exit);
	
	/*
	 * stash the offset between our index ptr and the base of the buffer.
	 * we'll use this to figure out where to write the encapsulation
	 */
	offset = (sw_uint32) (self->m_eptr - self->m_base);
	self->m_eptr += sizeof(sw_uint32);
	
	/*
	 * write the endian
	 */
	err = sw_corby_buffer_put_uint8(self, SW_ENDIAN);
	sw_check_okay(err, exit);
	
	/*
	 * major
	 */
	err = sw_corby_buffer_put_int8(self, profile->m_major);
	sw_check_okay(err, exit);
	
	/*
	 * minor
	 */
	err = sw_corby_buffer_put_int8(self, profile->m_minor);
	sw_check_okay(err, exit);
	
	/*
	 * host
	 */
	err = sw_corby_buffer_put_cstring(self, sw_ipv4_address_name(profile->m_address, host, 16));
	sw_check_okay(err, exit);
	
	/*
	 * port
	 */
	err = sw_corby_buffer_put_uint16(self, profile->m_port);
	sw_check_okay(err, exit);
	
	/*
	 * oid
	 */
	err = sw_corby_buffer_put_sized_octets(self, (sw_const_octets) profile->m_oid, profile->m_oid_len);
	sw_check_okay(err, exit);
	
	/*
	 * now write the length of this encap
	 * hold on..this gets a little hairy
	 */
	encap_size = (sw_uint32) (self->m_eptr - self->m_base) - offset - sizeof(sw_uint32);
	sw_memcpy((self->m_base + offset), &encap_size, sizeof(sw_uint32));

exit:

	return err;
}
					

sw_result
sw_corby_buffer_put_ior(
					sw_corby_buffer		self,
					const sw_corby_ior	ior)
{
	sw_corby_profile	profile;
	sw_result			err;
	
	/*
	 * repository id
	 */
	err = sw_corby_buffer_put_cstring(self, ior->m_repository_id);
	sw_check_okay(err, exit);
	
	/*
	 * number of profiles
	 */
	err = sw_corby_buffer_put_uint32(self, ior->m_num_profiles);
	sw_check_okay(err, exit);
	
	/*
	 * the profiles
	 */
	for (profile = ior->m_profiles; profile; profile = profile->m_txen)
	{
		err = sw_corby_buffer_put_profile(self, profile);
		sw_check_okay(err, exit);
	}
	
exit:

	return err;
}


sw_result
sw_corby_buffer_put_object(
					sw_corby_buffer						self,
					const struct _sw_corby_object	*	object)
{
	sw_assert(self);
	sw_assert(object);

	return sw_corby_buffer_put_ior(self, object->m_iors);
}


sw_result
sw_corby_buffer_get_uint8(
                  sw_corby_buffer	buffer,
                  sw_uint8		*	val)
{
	sw_result err;

	SW_CORBY_BUFFER_GET_OCTET(buffer, val);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_get_int8(
               sw_corby_buffer	buffer,
               sw_int8			*	val)
{
	sw_result err;

	SW_CORBY_BUFFER_GET_OCTET(buffer, val);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_get_uint16(
               sw_corby_buffer	buffer,
               sw_uint16		*	val,
               sw_uint8			endian)
{
	sw_octets octets = (sw_octets) val;
	sw_result	err;

	if (endian == SW_ENDIAN)
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);
	}
	else
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_get_int16(
               sw_corby_buffer	buffer,
               sw_int16		*	val,
               sw_uint8				endian)
{
	sw_octets octets = (sw_octets) val;
	sw_result	err;

	if (endian == SW_ENDIAN)
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);
	}
	else
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_get_uint32(
               sw_corby_buffer	buffer,
               sw_uint32		*	val,
               sw_uint8				endian)
{
	sw_octets	octets = (sw_octets) val;
	sw_result	err;

	if (endian == SW_ENDIAN)
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 2));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 3));
		sw_check_okay(err, exit);
	}
	else
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 3));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 2));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_get_int32(
               sw_corby_buffer	buffer,
               sw_int32		*	val,
               sw_uint8			endian)
{
	sw_octets	octets = (sw_octets) val;
	sw_result	err;

	if (endian == SW_ENDIAN)
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 2));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 3));
		sw_check_okay(err, exit);
	}
	else
	{
		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 3));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 2));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, (octets + 1));
		sw_check_okay(err, exit);

		SW_CORBY_BUFFER_GET_OCTET(buffer, octets);
		sw_check_okay(err, exit);
	}

exit:
	
   return err;
}


sw_result
sw_corby_buffer_get_octets(
				sw_corby_buffer	buffer,
				sw_octets			buf,
				sw_size_t			len)
{
	sw_result err = SW_OKAY;

	while (len)
	{
		sw_size_t bytes_in_buf = (sw_size_t) (buffer->m_eptr - buffer->m_bptr);

		if (bytes_in_buf > 0)
		{
			sw_size_t min_bytes = (bytes_in_buf < len) ? bytes_in_buf : len;

			sw_memcpy(buf, buffer->m_bptr, min_bytes);
			buffer->m_bptr += min_bytes;
			buf				+=	min_bytes;
			len				-= min_bytes;
		}
		else
		{
			err = sw_corby_buffer_underflow(buffer, buf);
			sw_check_okay(err, exit);

			buf++;
			len--;
		}
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_allocate_and_get_sized_octets(
					sw_corby_buffer	self,
					sw_octets		*	val,
					sw_uint32		*	len,
					sw_uint8			endian)
{
	sw_result err;

	err = sw_corby_buffer_get_uint32(self, len, endian);
	sw_check_okay(err, exit);

	*val = (sw_octets) sw_malloc(*len);
	err = sw_translate_error(*val, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_corby_buffer_get_octets(self, *val, *len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_get_zerocopy_sized_octets(
					sw_corby_buffer	self,
					sw_octets		*	val,
					sw_uint32		*	len,
					sw_uint8			endian)
{
	sw_uint32	bytes_in_buf;
	sw_result	err;

	err = sw_corby_buffer_get_uint32(self, len, endian);
	sw_check_okay(err, exit);

	bytes_in_buf = (sw_uint32) (self->m_eptr - self->m_bptr);
	sw_check(*len <= bytes_in_buf, exit, err = SW_E_UNKNOWN);

	if (*len > 0)
	{
		*val = self->m_bptr;
		self->m_bptr += *len;
	}
	else
	{
		*val = NULL;
	}

exit:

	return err;
}
                                    

sw_result
sw_corby_buffer_get_sized_octets(
					sw_corby_buffer	buffer,
					sw_octets			val,
					sw_uint32		*	len,
					sw_uint8			endian)
{
	sw_uint32	max = *len;
	sw_result	err;

	err = sw_corby_buffer_get_uint32(buffer, len, endian);
	sw_check_okay(err, exit);
	sw_check(max >= *len, exit, err = SW_E_UNKNOWN);

	err = sw_corby_buffer_get_octets(buffer, val, *len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_allocate_and_get_cstring(
					sw_corby_buffer	self,
					sw_string		*	val,
					sw_uint32		*	len,
					sw_uint8			endian)
{
	sw_result err;

	err = sw_corby_buffer_get_uint32(self, len, endian);
	sw_check_okay(err, exit);

	*val = (sw_string) sw_malloc(*len);
	err = sw_translate_error(*val, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_corby_buffer_get_octets(self, (sw_octets) *val, *len);
	sw_check_okay(err, exit);

exit:

	return err;
}
				

sw_result
sw_corby_buffer_get_zerocopy_cstring(
					sw_corby_buffer	self,
					sw_string		*	val,
					sw_uint32		*	len,
					sw_uint8			endian)
{
	sw_uint32	bytes_in_buf;
	sw_result	err;

	err = sw_corby_buffer_get_uint32(self, len, endian);
	sw_check_okay(err, exit);

	bytes_in_buf = (sw_uint32) (self->m_eptr - self->m_bptr);
	sw_check(*len <= bytes_in_buf, exit, err = SW_E_UNKNOWN);

	if (*len > 0)
	{
		*val = (sw_string) self->m_bptr;
		self->m_bptr += *len;
	}
	else
	{
		*val = NULL;
	}

exit:

	return err;
}


sw_result
sw_corby_buffer_get_cstring(
               sw_corby_buffer	buffer,
               sw_string			val,
               sw_uint32		*	len,
               sw_uint8			endian)
{
	sw_uint32		max = *len;
	sw_result	err;

	err = sw_corby_buffer_get_uint32(buffer, len, endian);
	sw_check_okay(err, exit);
	sw_check(max >= *len, exit, err = SW_E_UNKNOWN);

	err = sw_corby_buffer_get_octets(buffer, (sw_octets) val, *len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_corby_buffer_get_profile(
					sw_corby_buffer		self,
					sw_corby_profile	*	profile,
					sw_uint8				endian)
{
	sw_uint32 encap_size;
	sw_uint8	encap_endian;
	char		host[16];
	sw_uint32	host_len;
	sw_result	err;
	
	/*
	 * allocate storage
	 */
	err = sw_corby_profile_init(profile);
	sw_check_okay(err, exit);
	
	/*
	 * tag
	 */
	err = sw_corby_buffer_get_uint32(self, &(*profile)->m_tag, endian);
	sw_check_okay(err, exit);
	
	/*
	 * size
	 */
	err = sw_corby_buffer_get_uint32(self, &encap_size, endian);
	sw_check_okay(err, exit);
	
	/*
	 * endian
	 */
	err = sw_corby_buffer_get_uint8(self, &encap_endian);
	sw_check_okay(err, exit);
	
	/*
	 * major
	 */
	err = sw_corby_buffer_get_uint8(self, &(*profile)->m_major);
	sw_check_okay(err, exit);
	
	/*
	 * minor
	 */
	err = sw_corby_buffer_get_uint8(self, &(*profile)->m_minor);
	sw_check_okay(err, exit);
	
	/*
	 * host
	 */
	host_len = 16;
	err = sw_corby_buffer_get_cstring(self, host, &host_len, encap_endian);
	sw_check_okay(err, exit);

	err = sw_ipv4_address_init_from_name(&(*profile)->m_address, host);
	sw_check_okay(err, exit);
	
	/*
	 * port
	 */
	err = sw_corby_buffer_get_uint16(self, &(*profile)->m_port, endian);
	sw_check_okay(err, exit);
	
	/*
	 * oid_len and oid
	 */
	err = sw_corby_buffer_allocate_and_get_sized_octets(self, (sw_octets*) &(*profile)->m_oid, &(*profile)->m_oid_len, encap_endian);
	sw_check_okay(err, exit);

exit:

	return err;
}
					
					
sw_result
sw_corby_buffer_get_ior(
					sw_corby_buffer	self,
					sw_corby_ior	*	ior,
					sw_uint8			endian)
{
	sw_corby_profile	profile;
	sw_corby_profile	last;
	sw_uint32			len;
	sw_uint32			i;
	sw_result			err;
	
	/*
	 * allocate
	 */
	err = sw_corby_ior_init(ior);
	sw_check_okay(err, exit);
	
	/*
	 * repository id
	 */
	err = sw_corby_buffer_allocate_and_get_cstring(self, &(*ior)->m_repository_id, &len, endian);
	sw_check_okay(err, exit);
	
	/*
	 * number of profiles
	 */
	err = sw_corby_buffer_get_uint32(self, &(*ior)->m_num_profiles, endian);
	sw_check_okay(err, exit);
	
	/*
	 * profiles...we want to store in the order they were read
	 */
	for (i = 0, last = NULL; i < (*ior)->m_num_profiles; i++)
	{		
		err = sw_corby_buffer_get_profile(self, &profile, endian);
		sw_check_okay(err, exit);
		
		if (last == NULL)
		{
			(*ior)->m_profiles = profile;
		}
		else
		{
			last->m_txen	=	profile;
		}

		last = profile;
	}
	
exit:

	return err;
}


sw_result
sw_corby_buffer_get_object(
					sw_corby_buffer		self,
					sw_corby_object	*	object,
					sw_uint8				endian)
{
	sw_result err;
	
	/*
	 * allocate
	 */
	err = sw_corby_object_init(object);
	sw_check_okay(err, exit);
	
	/*
	 * ior
	 */
	err = sw_corby_buffer_get_ior(self, &(*object)->m_iors, endian);
	sw_check_okay(err, exit);
	
exit:

	return err;
}


static sw_result
sw_corby_buffer_overflow(
						sw_corby_buffer	buffer,
						sw_uint8			val)
{
	sw_result err = SW_OKAY;

	if (buffer->m_overflow_func != NULL)
	{
		err = ((*buffer->m_overflow_func)(buffer->m_delegate, buffer, val, &buffer->m_base, &buffer->m_bptr, &buffer->m_eptr, &buffer->m_end, buffer->m_delegate_extra));
	}
	else
	{
 		sw_size_t newlen;
 		sw_size_t oldlen;
   	sw_size_t boff;
   	sw_size_t eoff;
   
   	/*
      	get all the relevant info
   	*/
   	oldlen	=	buffer->m_end - buffer->m_base;
   	boff		=	buffer->m_bptr - buffer->m_base;
   	eoff		=	buffer->m_eptr	- buffer->m_base;
   	newlen	=	oldlen * 2;
  	 
   	/*
      	new realloc
   	*/
		buffer->m_base = (sw_octets) sw_realloc(buffer->m_base, newlen);
		sw_check(buffer->m_base, exit, err = SW_E_MEM);

		/*
		 * reset the pointers
		 */
   	buffer->m_bptr = buffer->m_base + boff;
   	buffer->m_eptr = buffer->m_base + eoff;
   	buffer->m_end  = buffer->m_base + newlen;

		/*
		 * and store the guy
		 */
		*(buffer->m_eptr) = val;
		buffer->m_eptr++;
	}

exit:
   
	return err;
}


static sw_result
sw_corby_buffer_underflow(
                  sw_corby_buffer	buffer,
						sw_uint8		*	val)
{
	sw_result err;

	sw_check(buffer->m_underflow_func != NULL, exit, err = SW_E_CORBY_MARSHAL);

	err = (*buffer->m_underflow_func)(buffer->m_delegate, buffer, val, &buffer->m_base, &buffer->m_bptr, &buffer->m_eptr, &buffer->m_end, buffer->m_delegate_extra);
	sw_check_okay(err, exit);

exit:

	return err;
}

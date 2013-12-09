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

#include "text_record_i.h"
#include <salt/debug.h>

static const sw_uint32 MaxStringLen	=	255;

sw_result
sw_text_record_init(
			sw_text_record	*	self)
{
	sw_result err;

	*self = (sw_text_record) sw_malloc(sizeof(struct _sw_text_record));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_corby_buffer_init_with_size(&((*self)->m_buffer), 1024);
	sw_check_okay(err, exit);

exit:

	if (err && *self)
	{
		sw_text_record_fina(*self);
		*self = NULL;
	}

	return err;
}


sw_result
sw_text_record_fina(
			sw_text_record		self)
{
	sw_assert(self);
	sw_assert(self->m_buffer);

	sw_corby_buffer_fina(self->m_buffer);
	sw_free(self);

	return SW_OKAY;
}


sw_result
sw_text_record_add_string(
				sw_text_record		self,
				sw_const_string	string)
{
	sw_size_t	len;
	sw_result	err;

	sw_assert(string != NULL);

	len = sw_strlen(string);
	sw_check(len <= MaxStringLen, exit, err = SW_E_UNKNOWN);

	err = sw_corby_buffer_put_uint8(self->m_buffer, (sw_uint8) len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(self->m_buffer, (sw_const_octets) string, len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_text_record_add_key_and_string_value(
				sw_text_record		self,
				sw_const_string	key,
				sw_const_string	val)
{
	sw_size_t	len;
	sw_result	err;

	sw_assert(key != NULL);

	len = sw_strlen(key) + 1;

	if (val)
	{
		len += sw_strlen(val);
	}

	sw_check(len <= MaxStringLen, exit, err = SW_E_UNKNOWN);

	err = sw_corby_buffer_put_int8(self->m_buffer, (sw_uint8) len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(self->m_buffer, (sw_const_octets) key, sw_strlen(key));
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_int8(self->m_buffer, '=');
	sw_check_okay(err, exit);

	if (val)
	{
		err = sw_corby_buffer_put_octets(self->m_buffer, (sw_const_octets) val, sw_strlen(val));
		sw_check_okay(err, exit);
	}

exit:

	return err;
}


sw_result
sw_text_record_add_key_and_binary_value(
				sw_text_record		self,
				sw_const_string	key,
				sw_octets			val,
				sw_ulong			val_len)
{
	sw_size_t	len;
	sw_result	err;

	sw_assert(key != NULL);

	len = sw_strlen(key) + 1 + val_len;

	sw_check(len <= MaxStringLen, exit, err = SW_E_UNKNOWN);

	err = sw_corby_buffer_put_int8(self->m_buffer, (sw_uint8) len);
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(self->m_buffer, (sw_const_octets) key, sw_strlen(key));
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_int8(self->m_buffer, '=');
	sw_check_okay(err, exit);

	err = sw_corby_buffer_put_octets(self->m_buffer, (sw_const_octets) val, val_len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_octets
sw_text_record_bytes(
				sw_text_record		self)
{
	return sw_corby_buffer_octets(self->m_buffer);
}


sw_uint32
sw_text_record_len(
				sw_text_record		self)
{
	return (sw_uint32) sw_corby_buffer_bytes_used(self->m_buffer);
}


sw_result
sw_text_record_iterator_init(
				sw_text_record_iterator				*	self,
				sw_octets									text_record,
				sw_uint32										text_record_len)
{
	sw_result err;

	*self = (sw_text_record_iterator) sw_malloc(sizeof(struct _sw_text_record_iterator));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_corby_buffer_init(&((*self)->m_buffer));
	sw_check_okay(err, exit);

	err = sw_corby_buffer_set_octets((*self)->m_buffer, text_record, text_record_len);
	sw_check_okay(err, exit);

exit:

	return err;
}


sw_result
sw_text_record_iterator_fina(
				sw_text_record_iterator					self)
{
	sw_corby_buffer_set_octets(self->m_buffer, NULL, 0);
	sw_corby_buffer_fina(self->m_buffer);
	sw_free(self);

	return SW_OKAY;
}


sw_result
sw_text_record_iterator_next(
				sw_text_record_iterator					self,
				char											key[SW_TEXT_RECORD_MAX_LEN],
				sw_uint8										val[SW_TEXT_RECORD_MAX_LEN],
				sw_uint32								*	val_len)
{
	sw_bool		decodingValue;
	sw_uint8 	len;
	int			i;
	sw_result	err;

	sw_check(sw_corby_buffer_octets(self->m_buffer), exit, err = SW_E_UNKNOWN);
	sw_check(sw_corby_buffer_bytes_used(self->m_buffer), exit, err = SW_E_UNKNOWN);

	memset(key, 0, SW_TEXT_RECORD_MAX_LEN);
	memset(val, 0, SW_TEXT_RECORD_MAX_LEN);

	decodingValue	=	SW_FALSE;
	*val_len			=	0;

	err = sw_corby_buffer_get_uint8(self->m_buffer, &len);
	sw_check_okay(err, exit);

	sw_assert(len <= MaxStringLen);

	for (i = 0; i < len; i++)
	{
		sw_int8 c;

		err = sw_corby_buffer_get_int8(self->m_buffer, &c);
		sw_check_okay(err, exit);

		if (decodingValue == SW_FALSE)
		{
			if (c != '=')
			{
				key[i] = c;
			}
			else
			{
				decodingValue	=	SW_TRUE;
			}
		}
		else
		{
			val[*val_len] = c;
			(*val_len)++;
		}
	}

exit:

	return err;
}


sw_result
sw_text_record_string_iterator_init(
				sw_text_record_string_iterator	*	self,
				sw_const_string							text_record_string)
{
	sw_result err = SW_OKAY;

	*self = (sw_text_record_string_iterator) sw_malloc(sizeof(struct _sw_text_record_string_iterator));
	err = sw_translate_error(*self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	(*self)->m_string =	text_record_string;
	(*self)->m_index	=	0;

exit:

	return err;
}


sw_result
sw_text_record_string_iterator_fina(
				sw_text_record_string_iterator		self)
{
	sw_free(self);

	return SW_OKAY;
}


sw_result
sw_text_record_string_iterator_next(
				sw_text_record_string_iterator	self,
				char										key[SW_TEXT_RECORD_MAX_LEN],
				char										val[SW_TEXT_RECORD_MAX_LEN])
{
	sw_bool		decodingValue;
	sw_uint32	keyIndex;
	sw_uint32	valIndex;
	sw_result	err = SW_OKAY;

	sw_check(self->m_string, exit, err = SW_E_UNKNOWN);
	sw_check((self->m_string[self->m_index] != '\0'), exit, err = SW_E_UNKNOWN);

	if (self->m_string[self->m_index] == '\001')
	{
		self->m_index++;
	}

	memset(key, 0, SW_TEXT_RECORD_MAX_LEN);
	memset(val, 0, SW_TEXT_RECORD_MAX_LEN);
	decodingValue	=	SW_FALSE;
	keyIndex			=	0;
	valIndex			=	0;

	while ((self->m_string[self->m_index] != '\0') && (self->m_string[self->m_index] != '\001'))
	{
		char c = self->m_string[self->m_index++];

		if (decodingValue == SW_FALSE)
		{
			if (c != '=')
			{
				key[keyIndex++]	=	c;
			}
			else
			{
				decodingValue = SW_TRUE;
			}
		}
		else
		{
			val[valIndex++] = c;
		}
	}

exit:

	return err;
}

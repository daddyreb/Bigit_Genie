#ifndef _corby_buffer_i_h
#define _corby_buffer_i_h

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

#include <corby/buffer.h>
#include "ior.h"
#include "profile.h"


#ifdef __cplusplus
extern "C"
{
#endif 


#define SW_SWAP16(val) ((val << 8) | (val >> 8))

#define SW_SWAP32(l) ((((l) & 0xff000000) >> 24) |	\
  (((l) & 0x00ff0000) >> 8)  |							\
  (((l) & 0x0000ff00) << 8)  |							\
  (((l) & 0x000000ff) << 24))

#define SW_SWAP64(val)										\
{																	\
   sw_uint32 l1, l2, *v1, *v2;								\
   l1 = ((sw_uint32*)&val)[0];								\
   l2 = ((sw_uint32*)&val)[1];								\
   v1 = (sw_uint32*)&val;									\
   v2 = (sw_uint32*)&val + 1;								\
   *v1 = SW_SWAP32(l2);										\
   *v2 = SW_SWAP32(l1);										\
}


typedef struct _sw_cdr_alignment
{
	sw_uint32	m_align;
	sw_uint32	m_pad;
} sw_cdr_alignment;


struct _sw_corby_buffer
{
	sw_uint8							*	m_base;
	sw_uint8							*	m_bptr;
	sw_uint8							*	m_eptr;
	sw_uint8							*	m_end;

	sw_corby_buffer_delegate			m_delegate;
	sw_corby_buffer_overflow_func		m_overflow_func;
	sw_corby_buffer_underflow_func	m_underflow_func;
	sw_opaque_t								m_delegate_extra;
	sw_corby_buffer_observer			m_observer;
	sw_corby_buffer_written_func		m_written_func;
	sw_opaque_t								m_observer_extra;

	struct _sw_corby_buffer			*	m_prev;
	struct _sw_corby_buffer			*	m_next;
};


sw_result
sw_corby_buffer_put_profile(
					sw_corby_buffer			buffer,
					const sw_corby_profile	profile);
					

sw_result
sw_corby_buffer_put_ior(
					sw_corby_buffer		buffer,
					const sw_corby_ior	ior);


sw_result
sw_corby_buffer_get_profile(
					sw_corby_buffer		buffer,
					sw_corby_profile	*	profile,
					sw_uint8				endian);
					
					
sw_result
sw_corby_buffer_get_ior(
					sw_corby_buffer		buffer,
					sw_corby_ior		*	ior,
					sw_uint8				endian);
					

#ifdef __cplusplus
}
#endif


#endif

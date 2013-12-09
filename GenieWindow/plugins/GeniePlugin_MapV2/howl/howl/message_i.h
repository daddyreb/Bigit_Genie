#ifndef _sw_corby_message_i_h
#define _sw_corby_message_i_h

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

#include <corby/message.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define SW_SWOP_HEADER_SIZE	12
#define SW_SWOP_MAJOR			1
#define SW_SWOP_MINOR			0

typedef enum _sw_swop_message_type
{
	SW_SWOP_REQUEST				=	0,
	SW_SWOP_REPLY					=	1,
	SW_SWOP_CANCEL_REQUEST		=	2,
	SW_SWOP_LOCATE_REQUEST		=	3,
	SW_SWOP_LOCATE_REPLY		=	4,
	SW_SWOP_CLOSE_CONNECTION	=	5,
	SW_SWOP_MESSAGE_ERROR		=	6,
} sw_swop_message_type;


typedef enum _sw_swop_reply_status_type
{
	SW_SWOP_NO_EXCEPTION			=	0,
	SW_SWOP_USER_EXCEPTION			=	1,
	SW_SWOP_SYSTEM_EXCEPTION		=	2,
	SW_SWOP_LOCATION_FORWARD		=	3
} sw_swop_reply_status_type;


typedef enum _sw_swop_locate_status_type
{
	SW_SWOP_UNKNOWN_OBJECT			=	0,
	SW_SWOP_OBJECT_HERE				=	1,
	SW_SWOP_OBJECT_FORWARD			=	2
} sw_swop_locate_status_type;


typedef struct _sw_swop_message_header
{
	sw_int8	m_magic[4];
	sw_uint8	m_major;
	sw_uint8	m_minor;
	sw_uint8	m_endian;
	sw_uint8	m_msg_type;
	sw_uint32	m_msg_size;
} sw_swop_message_header;


typedef struct _sw_swop_request_header
{
	/*
		service context list
	*/

	sw_uint32	m_request_id;
	sw_uint8	m_reply_expected;
	sw_uint8	m_oid[64];
   sw_uint32	m_oid_len;
	sw_int8	m_op[64];
	sw_uint32	m_op_len;

	/*
		principal
	*/
	
} sw_swop_request_header;


typedef struct _sw_swop_reply_header
{
	/*
		service context list
	*/

	sw_uint32						m_request_id;
	sw_swop_reply_status_type	m_reply_status;
} sw_swop_reply_header;


typedef struct _sw_swop_cancel_request_header
{
	sw_uint32	m_request_id;
} sw_swop_cancel_request_header;


typedef struct _sw_swop_locate_request_header
{
	sw_uint32	m_request_id;
	sw_octets	m_object_key;
} sw_swop_locate_request_header;


typedef struct _sw_swop_locate_reply_header
{
	sw_uint32						m_request_id;
	sw_swop_locate_status_type	m_locate_status;
} sw_swop_locate_reply_header;


typedef union _sw_swop_message_body
{
	struct _sw_swop_request_header			m_request_header;
	struct _sw_swop_reply_header				m_reply_header;
	struct _sw_swop_cancel_request_header	m_cancel_request_header;
	struct _sw_swop_locate_request_header	m_locate_request_header;
	struct _sw_swop_locate_reply_header		m_locate_reply_header;
} sw_swop_message_body;


struct _sw_corby_message
{
   sw_swop_message_header	*	m_header;
	sw_swop_message_body			m_body;
};


#ifdef __cplusplus
}
#endif


#endif

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

#include "macosx_socket.h"
#include <salt/debug.h>


static sw_result
sw_macosx_socket_init(
				sw_macosx_socket		self);


static void
sw_macosx_socket_handler(
            CFSocketRef				cf_socket,
            CFSocketCallBackType	type,
            CFDataRef				address,
            const void			*	data,
            void					*	info);


sw_result
sw_tcp_socket_init(
			sw_socket * self)
{
	sw_macosx_socket	msocket;
	sw_result			err = SW_OKAY;
	
	msocket = (sw_macosx_socket) sw_malloc(sizeof(struct _sw_macosx_socket));
	err = sw_translate_error(msocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(msocket, 0, sizeof(struct _sw_macosx_socket));

	err = sw_tcp_socket_super_init(&msocket->m_super);
	sw_check_okay(err, exit);

	err = sw_macosx_socket_init(msocket);
	sw_check_okay(err, exit);

	*self = &msocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&msocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_tcp_socket_init_with_desc(
			sw_socket	*	self,
			sw_sockdesc_t	desc)
{
	sw_macosx_socket	msocket;
	sw_result			err = SW_OKAY;
	
	msocket = (sw_macosx_socket) sw_malloc(sizeof(struct _sw_macosx_socket));
	err = sw_translate_error(msocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(msocket, 0, sizeof(struct _sw_macosx_socket));

	err = sw_tcp_socket_super_init_with_desc(&msocket->m_super, desc);
	sw_check_okay(err, exit);

	err = sw_macosx_socket_init(msocket);
	sw_check_okay(err, exit);

	*self = &msocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&msocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_udp_socket_init(
			sw_socket * self)
{
	sw_macosx_socket	msocket;
	sw_result			err = SW_OKAY;
	
	msocket = (sw_macosx_socket) sw_malloc(sizeof(struct _sw_macosx_socket));
	err = sw_translate_error(msocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(msocket, 0, sizeof(struct _sw_macosx_socket));

	err = sw_udp_socket_super_init(&msocket->m_super);
	sw_check_okay(err, exit);

	err = sw_macosx_socket_init(msocket);
	sw_check_okay(err, exit);

	*self = &msocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&msocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_multicast_socket_init(
			sw_socket * self)
{
	sw_macosx_socket	msocket	=	NULL;
	sw_result			err		=	SW_OKAY;
	
	msocket = (sw_macosx_socket) sw_malloc(sizeof(struct _sw_macosx_socket));
	err = sw_translate_error(msocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(msocket, 0, sizeof(struct _sw_macosx_socket));

	err = sw_multicast_socket_super_init(&msocket->m_super);
	sw_check_okay(err, exit);

	err = sw_macosx_socket_init(msocket);
	sw_check_okay(err, exit);
	
	*self = &msocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&msocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_socket_fina(
			sw_socket self)
{
	sw_macosx_socket msocket = (sw_macosx_socket) self;

	if (msocket->m_super.m_registered)
	{
		sw_macosx_socket_disable_notifications(msocket);
	}

	sw_assert(msocket->m_super.m_registered == SW_FALSE);

	CFSocketInvalidate(msocket->m_cf_socket);
	CFRelease(msocket->m_cf_source);
	CFRelease(msocket->m_cf_socket);

	sw_socket_super_fina(self);

	sw_free(self);

	return SW_OKAY;
}


static sw_result
sw_macosx_socket_init(
			sw_macosx_socket self)
{
	CFSocketContext	cf_context;
	sw_result			err = SW_OKAY;

	self->m_cf_events				=	kCFSocketNoCallBack;

	cf_context.version			=	0;
	cf_context.info				=	(void*) self;
	cf_context.retain				=	NULL;
	cf_context.release			=	NULL;
	cf_context.copyDescription	=	NULL;

	self->m_cf_socket				=	CFSocketCreateWithNative(NULL, self->m_super.m_desc, kCFSocketWriteCallBack|kCFSocketReadCallBack, sw_macosx_socket_handler, &cf_context);
	sw_check_log(self->m_cf_socket, exit, err = SW_E_UNKNOWN);

	self->m_cf_source =	CFSocketCreateRunLoopSource(NULL, self->m_cf_socket, 0);
	sw_check_log(self->m_cf_source, exit, err = SW_E_UNKNOWN);

exit:

	return err;
}


sw_result
sw_macosx_socket_enable_notifications(
							sw_macosx_socket	self,
							sw_socket_event	events)
{
	self->m_cf_events	= kCFSocketNoCallBack;

	if (events & SW_SOCKET_WRITE)
	{
		self->m_cf_events |= kCFSocketWriteCallBack;
	}

	if (events & SW_SOCKET_READ)
	{
		self->m_cf_events |= kCFSocketReadCallBack;
	}

	if (!self->m_super.m_registered)
	{
		CFRunLoopAddSource(CFRunLoopGetCurrent(), self->m_cf_source, kCFRunLoopCommonModes);
		self->m_super.m_registered = SW_TRUE;
	}

	CFSocketEnableCallBacks(self->m_cf_socket, self->m_cf_events);

	return SW_OKAY;
}


sw_result
sw_macosx_socket_disable_notifications(
			sw_macosx_socket self)
{
	self->m_cf_events	=	0;

	sw_assert(self->m_super.m_registered = SW_TRUE);

	CFRunLoopRemoveSource(CFRunLoopGetCurrent(), self->m_cf_source, kCFRunLoopCommonModes);

	self->m_super.m_registered = SW_FALSE;

	return SW_OKAY;
}


static void
sw_macosx_socket_handler(
            CFSocketRef				cf_socket,
            CFSocketCallBackType	type,
            CFDataRef				address,
            const void			*	data,
            void					*	info)
{
	sw_macosx_socket msocket = (sw_macosx_socket) info;

	if (type & kCFSocketWriteCallBack)
	{
		if (msocket->m_cf_events & kCFSocketWriteCallBack)
		{
			(*msocket->m_super.m_func)(msocket->m_super.m_handler, msocket->m_super.m_salt, msocket, SW_SOCKET_WRITE, msocket->m_super.m_extra);
			CFSocketEnableCallBacks(msocket->m_cf_socket, msocket->m_cf_events);
		}
		else
		{
			fprintf(stderr, "**** ignoring write event ******\n");
		}
	}

	if (type & kCFSocketReadCallBack)
	{
		if (msocket->m_cf_events & kCFSocketReadCallBack)
		{
			(*msocket->m_super.m_func)(msocket->m_super.m_handler, msocket->m_super.m_salt, msocket, SW_SOCKET_READ, msocket->m_super.m_extra);
		}
		else
		{
			fprintf(stderr, "***** ignoring read event *****\n");
		}
	}
}

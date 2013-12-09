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

#include "macosx_salt.h"
#include <salt/address.h>
#include <salt/socket.h>
#include <salt/debug.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if_types.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <string.h>
#include <sys/signal.h>


static void
sw_salt_handle_socket(
            CFSocketRef				cf_socket,
            CFSocketCallBackType	type,
            CFDataRef				address,
            const void			*	data,
            void					*	info);


static void
sw_salt_handle_timer(
				CFRunLoopTimerRef	timer,
				void				*	info);


sw_result
sw_salt_init(
				sw_salt		*	salt,
				int				argc,
				char			**	argv)
{
	sw_result err = SW_OKAY;

	*salt = (sw_salt) sw_malloc(sizeof(struct _sw_salt));
	err = sw_translate_error(*salt, SW_E_MEM);
	sw_check_okay_log(err, exit);

	(*salt)->m_cf_run_loop		=	CFRunLoopGetCurrent();
	(*salt)->m_sockets.m_next	=	NULL;
	(*salt)->m_sockets.m_prev	=	NULL;
  	(*salt)->m_step				=	SW_FALSE;

	signal(SIGPIPE, SIG_IGN);

exit:

   return err;
}


sw_result
sw_salt_fina(
				sw_salt	self)
{
	sw_free(self);

   return SW_OKAY;
}


sw_result
sw_salt_register_socket(
               sw_salt						self,
               sw_socket					socket,
               sw_socket_event			events,
					sw_socket_handler			handler,
               sw_socket_handler_func	func,
               sw_opaque					extra)
{
	sw_macosx_socket	msocket = (sw_macosx_socket) socket;

	msocket->m_super.m_salt		=	self;
	msocket->m_super.m_events	=	events;
	msocket->m_super.m_handler	=	handler;
	msocket->m_super.m_func		=	func;
	msocket->m_super.m_extra	=	extra;

	return sw_macosx_socket_enable_notifications(msocket, events);
}


sw_result
sw_salt_unregister_socket(
					sw_salt		self,
					sw_socket	socket)
{
	sw_macosx_socket msocket = (sw_macosx_socket) socket;

	sw_macosx_socket_disable_notifications(msocket);

   return SW_OKAY;
}


sw_result
sw_salt_register_timer(
            sw_salt						self,
				sw_timer						timer,
            sw_time						timeout,
            sw_timer_handler			handler,
            sw_timer_handler_func	func,
				sw_opaque					extra)
{
	sw_macosx_timer	mtimer = (sw_macosx_timer) timer;
	sw_result			result;

	sw_assert(self);
	sw_assert(timer);

	mtimer->m_super.m_salt			=	self;
	mtimer->m_super.m_timeout		=	timeout;
	mtimer->m_super.m_handler		=	handler;
	mtimer->m_super.m_func			=	func;
	mtimer->m_super.m_extra			=	extra;

	mtimer->m_cf_run_loop			=	self->m_cf_run_loop;
	mtimer->m_interval				=	timeout.m_secs;

	result = sw_macosx_timer_enable_notifications(mtimer);

	if (result == SW_OKAY)
	{
		mtimer->m_super.m_registered = SW_TRUE;
	}

   return result;
}


sw_result
sw_salt_unregister_timer(
            sw_salt	salt,
            sw_timer	timer)
{
	sw_macosx_timer mtimer = (sw_macosx_timer) timer;

	if (mtimer->m_super.m_registered == SW_TRUE)
	{
		sw_macosx_timer_disable_notifications(mtimer);

		mtimer->m_super.m_registered = SW_FALSE;
	}

	return SW_OKAY;
}


sw_result
sw_salt_step(
				sw_salt		self,
				sw_uint32	*	msecs)
{
	self->m_step = SW_TRUE;

	if (msecs)
	{
		CFTimeInterval interval = (*msecs / 1000) ;
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, interval, FALSE) ;
	}
	else
	{
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, 99999999999, TRUE);
	}

	self->m_step = SW_FALSE;

	return SW_OKAY;
}


sw_result
sw_salt_run(
            sw_salt	self)
{
	CFRunLoopRun();

	return SW_OKAY;
}


sw_result
sw_salt_stop_run(
				sw_salt	self)
{
	CFRunLoopStop(self->m_cf_run_loop);

	return SW_OKAY;
}


int
sw_socket_error_code()
{
	return errno;
}

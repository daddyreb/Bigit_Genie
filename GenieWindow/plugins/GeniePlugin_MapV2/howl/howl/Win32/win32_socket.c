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

#include "win32_socket.h"
#include <salt/debug.h>
#include <winsock2.h>

static sw_result
sw_win32_socket_init(
			sw_win32_socket self);


sw_result
sw_tcp_socket_init(
			sw_socket * self)
{
	sw_win32_socket	wsocket;
	sw_result			err =	SW_OKAY;
	
	wsocket = (sw_win32_socket) sw_malloc(sizeof(struct _sw_win32_socket));
	err = sw_translate_error(wsocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(wsocket, 0, sizeof(struct _sw_win32_socket));

	err = sw_tcp_socket_super_init(&wsocket->m_super);
	sw_check_okay(err, exit);

	err = sw_win32_socket_init(wsocket);
	sw_check_okay(err, exit);

	*self = &wsocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&wsocket->m_super);
	}

	return err;
}


sw_result
sw_tcp_socket_init_with_desc(
			sw_socket 	*	self,
			sw_sockdesc_t	desc)
{
	sw_win32_socket	wsocket;
	sw_result			err = SW_OKAY;
	
	wsocket = (sw_win32_socket) sw_malloc(sizeof(struct _sw_win32_socket));
	err = sw_translate_error(wsocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(wsocket, 0, sizeof(struct _sw_win32_socket));

	err = sw_tcp_socket_super_init_with_desc(&wsocket->m_super, desc);
	sw_check_okay(err, exit);

	err = sw_win32_socket_init(wsocket);
	sw_check_okay(err, exit);

	*self = &wsocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&wsocket->m_super);
	}

	return err;
}


sw_result
sw_udp_socket_init(
			sw_socket * self)
{
	sw_win32_socket	wsocket;
	sw_result			err =	SW_OKAY;
	
	wsocket = (sw_win32_socket) sw_malloc(sizeof(struct _sw_win32_socket));
	err = sw_translate_error(wsocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(wsocket, 0, sizeof(struct _sw_win32_socket));

	err = sw_udp_socket_super_init(&wsocket->m_super);
	sw_check_okay(err, exit);

	err = sw_win32_socket_init(wsocket);
	sw_check_okay(err, exit);

	*self = &wsocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&wsocket->m_super);
	}

	return err;
}


sw_result
sw_multicast_socket_init(
			sw_socket * self)
{
	sw_win32_socket	wsocket	=	NULL;
	sw_result			err		=	SW_OKAY;
	
	wsocket = (sw_win32_socket) sw_malloc(sizeof(struct _sw_win32_socket));
	err = sw_translate_error(wsocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(wsocket, 0, sizeof(struct _sw_win32_socket));

	err = sw_multicast_socket_super_init(&wsocket->m_super);
	sw_check_okay(err, exit);

	err = sw_win32_socket_init(wsocket);
	sw_check_okay(err, exit);

	*self = &wsocket->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_socket_fina(&wsocket->m_super);
	}

	return err;
}


sw_result
sw_socket_fina(
			sw_socket self)
{
	sw_win32_socket wsocket = (sw_win32_socket) self;

	if (wsocket->m_inEvent == SW_TRUE)
	{
		wsocket->m_fina = SW_TRUE;
	}
	else
	{
		if (wsocket->m_handle != NULL)
		{
			BOOL ret = CloseHandle(wsocket->m_handle);
			sw_assert(ret == TRUE);
		}

		sw_socket_super_fina(self);

		sw_free(self);
	}

	return SW_OKAY;
}


sw_result
sw_win32_socket_enable_notifications(
			sw_win32_socket self)
{
	int			res;
	sw_result	err = SW_OKAY;

	res = WSAEventSelect(self->m_super.m_desc, self->m_handle, self->m_wsaEvents);
	err = sw_translate_error(res == 0, WSAGetLastError());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_win32_socket_disable_notifications(
			sw_win32_socket self)
{
	int			res;
	sw_result	err = SW_OKAY;

	res = WSAEventSelect(self->m_super.m_desc, self->m_handle, 0);
	err = sw_translate_error(res == 0, WSAGetLastError());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_win32_socket_has_window_event(
			sw_win32_socket	self,
			HWND					window,
			DWORD					event)
{
	SW_UNUSED_PARAM(event);

	sw_assert(self->m_inEvent == SW_FALSE);
	
	self->m_inEvent = SW_TRUE;
	
	WSAAsyncSelect(self->m_super.m_desc, window, 0, 0);

	return SW_OKAY;
}


sw_result
sw_win32_socket_had_window_event(
			sw_win32_socket	self,
			HWND					window,
			DWORD					event)
{
	sw_assert(self->m_inEvent == SW_TRUE);
	
	self->m_inEvent = SW_FALSE;

	//
	// do we still exist
	//
	if (self->m_fina == SW_FALSE)
	{
		WSAAsyncSelect(self->m_super.m_desc, window, event, self->m_wsaEvents);
	}
	else
	{
		sw_socket_fina(&self->m_super);
	}

	return SW_OKAY;
}



static sw_result
sw_win32_socket_init(
			sw_win32_socket self)
{
	sw_result err = SW_OKAY;

	self->m_wsaEvents	=	0;
	self->m_inEvent	=	SW_FALSE;
	self->m_fina		=	SW_FALSE;
	self->m_handle		=	CreateEvent(NULL, FALSE, FALSE, NULL);
	err = sw_translate_error(self->m_handle, GetLastError());
	sw_check_okay_log(err, exit);

exit:

	return err;
}

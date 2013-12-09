#ifndef _salt_win32_salt_h
#define _salt_win32_salt_h

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

#include "../salt_i.h"
#include "win32_interface.h"
#include "win32_socket.h"
#include "win32_time.h"

#include <winsock2.h>

#ifdef __cplusplus
extern "C"
{
#endif 


struct _sw_salt
{
	HANDLE										m_stop_run_event;
	HANDLE										m_stop_run_sync_event;
	sw_uint32										m_num_handles;
	struct _sw_win32_socket					m_sockets;
	struct _sw_win32_timer					m_timers;
	CRITICAL_SECTION							m_mutex;
	sw_bool										m_stepped;
	sw_bool										m_run;

	/*
	 * network interface management
	 */
	HANDLE										m_notify_addr_change_event;
	HANDLE										m_notify_addr_change_thread_handle;
	HANDLE										m_notify_addr_change_thread_stop;
	HANDLE										m_notify_addr_change_thread_sync;
	unsigned										m_notify_addr_change_thread_id;
	sw_bool										m_notify_addr_change;
	sw_network_interface_handler			m_handler;
	sw_network_interface_handler_func	m_func;
	sw_opaque									m_extra;

#if defined(HAVE_WSA_ASYNC_SELECT)

	sw_int8										m_window_class_name[256];
	HWND											m_select_window;

#endif
};


#ifdef __cplusplus
}
#endif 


#endif

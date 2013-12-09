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

#include "win32_salt.h"
#include "../tlist.h"
#include <iphlpapi.h>
#include <process.h>
#include <salt/address.h>
#include <salt/debug.h>
#include <stdlib.h>

#if !defined(_WIN32_WCE)
#	include <winsock2.h>
#	include <errno.h>
#	include <fcntl.h>
#	include <time.h>
#	include <io.h>
#else
#	include <winsock.h>
#endif

#include <stdio.h>

#define WM_SOCKET			(WM_USER + 47)
#define TICKS_PER_SEC	((LONGLONG) 10000000)
#define TICKS_PER_USEC	((LONGLONG) 10)

#if defined(HAVE_WSA_ASYNC_SELECT)
static LRESULT CALLBACK
sw_salt_window_handler(
		HWND		hwnd,
		UINT		uMsg,
		WPARAM	wParam,
		LPARAM	lParam);
#endif

static unsigned WINAPI
sw_salt_notify_addr_change(
		LPVOID	inParam);

sw_result
sw_salt_init(
				sw_salt		*	salt,
				int				argc,
				char			**	argv)
{
	WORD			wVersionRequested = MAKEWORD(2, 0);
	WSADATA		wsaData;
	WNDCLASS		wndclass;
	sw_salt		self;
	sw_result	err;

	SW_UNUSED_PARAM(argc);
	SW_UNUSED_PARAM(argv);

	/*
	 * initialize
	 */
	*salt = NULL;

	self = (sw_salt) sw_malloc(sizeof(struct _sw_salt));
	err = sw_translate_error(self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	self->m_notify_addr_change				=	SW_FALSE;
	self->m_sockets.m_prev					=	NULL;
	self->m_sockets.m_next					=	NULL;
	self->m_timers.m_prev					=	NULL;
	self->m_timers.m_next					=	NULL;
	InitializeCriticalSection(&self->m_mutex);
	self->m_stepped							=	SW_FALSE;
	self->m_run									=	SW_FALSE;

	err = WSAStartup(wVersionRequested, &wsaData);
	sw_check_okay_log(err, exit);

	self->m_stop_run_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	err = sw_translate_error(self->m_stop_run_event, GetLastError());
	sw_check_okay_log(err, exit);

	self->m_notify_addr_change_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	err = sw_translate_error(self->m_notify_addr_change_event, GetLastError());
	sw_check_okay_log(err, exit);

	self->m_num_handles = 2;

#if defined(HAVE_WSA_ASYNC_SELECT)
	
	wndclass.style				= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc		= (WNDPROC) sw_salt_window_handler;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= NULL;
	wndclass.hIcon				= NULL;
	wndclass.hCursor			= NULL;
	wndclass.hbrBackground	= NULL;
	wndclass.lpszMenuName	= NULL;
	sprintf(self->m_window_class_name, "SALT");
	wndclass.lpszClassName	= self->m_window_class_name;

	if (RegisterClass(&wndclass) == 0)
	{
		sw_check((GetLastError() == ERROR_CLASS_ALREADY_EXISTS) || (GetLastError() == ERROR_SUCCESS), exit, err = SW_E_UNKNOWN);
	}

	/*
	 * create a windows
	 */
	self->m_select_window = CreateWindow(self->m_window_class_name, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, NULL, self);
	err = sw_translate_error(self->m_select_window, GetLastError());
	sw_check_okay_log(err, exit);

#endif

	(*salt) = self;

exit:

	if (err && self)
	{
		sw_salt_fina(self);
	}

   return SW_OKAY;
}


sw_result
sw_salt_fina(
				sw_salt	self)
{
	if (self->m_notify_addr_change)
	{
		DWORD res;

		sw_assert(self->m_notify_addr_change_thread_stop != NULL);
		sw_assert(self->m_notify_addr_change_thread_sync != NULL);

		SetEvent(self->m_notify_addr_change_thread_stop);

		res = WaitForSingleObject(self->m_notify_addr_change_thread_sync, 1000 * 5);

		if (res != WAIT_OBJECT_0)
		{
			sw_debug(SW_LOG_ERROR, "unable to shutdown address change thread: %d %d\n", res, GetLastError());
			TerminateThread(self->m_notify_addr_change_thread_handle, 0);
		}

		CloseHandle(self->m_notify_addr_change_thread_stop);
		CloseHandle(self->m_notify_addr_change_thread_sync);

		self->m_notify_addr_change_thread_stop = NULL;
		self->m_notify_addr_change_thread_sync = NULL;
	}

#if defined(HAVE_WSA_ASYNC_SELECT)
	DestroyWindow(self->m_select_window);
	UnregisterClass(self->m_window_class_name, GetModuleHandle(NULL));
#endif

	DeleteCriticalSection(&self->m_mutex);

	CloseHandle(self->m_stop_run_event);
	CloseHandle(self->m_notify_addr_change_event);

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
	sw_win32_socket	wsocket	=	(sw_win32_socket) socket;
	int					ret;
	sw_result			err		=	SW_OKAY;

	wsocket->m_super.m_salt		=	self;
	wsocket->m_super.m_events	=	events;
	wsocket->m_super.m_handler	=	handler;
	wsocket->m_super.m_func		=	func;
	wsocket->m_super.m_extra	=	extra;
	wsocket->m_wsaEvents			=	0;

	if (events & SW_SOCKET_READ)
	{
		wsocket->m_wsaEvents |= (FD_ACCEPT|FD_READ|FD_CLOSE);
		// wsocket->m_wsaEvents |= FD_READ;
	}

	if (events & SW_SOCKET_WRITE)
	{
		wsocket->m_wsaEvents |= (FD_WRITE);
	}

	if (events & SW_SOCKET_OOB)
	{
		wsocket->m_wsaEvents |= (FD_OOB);
	}

#if defined(HAVE_WSA_ASYNC_SELECT)

	if (self->m_stepped == SW_TRUE)
	{
		err = sw_win32_socket_enable_notifications(wsocket);
		sw_check_okay(err, exit);
	}
	else
	{
		ret = WSAAsyncSelect(wsocket->m_super.m_desc, self->m_select_window, WM_SOCKET, wsocket->m_wsaEvents);
		err = sw_translate_error(ret == 0, WSAGetLastError());
		sw_check_okay_log(err, exit);
	}

#else

	err = sw_win32_socket_enable_notifications(wsocket);
	sw_check_okay(err, exit);

#endif

	/*
	 * if no errors, then push on socket stack
	 */
	if (wsocket->m_super.m_registered == SW_FALSE)
	{
		sw_tlist_push_front(self->m_sockets, wsocket);

		wsocket->m_super.m_registered = SW_TRUE;
		self->m_num_handles++;
	}

exit:
	
	/*
	 * done
	 */
	return err;
}


sw_result
sw_salt_unregister_socket(
				sw_salt		self,
				sw_socket	socket)
{
	sw_win32_socket wsocket = (sw_win32_socket) socket;

	sw_assert(wsocket->m_super.m_registered == SW_TRUE);
	sw_assert(wsocket->m_prev != NULL);

	sw_win32_socket_disable_notifications(wsocket);
	sw_tlist_remove(self->m_socket, wsocket);

	wsocket->m_super.m_registered = SW_FALSE;
	self->m_num_handles--;

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
	sw_win32_timer	wtimer = (sw_win32_timer) timer;
	UINT_PTR			ret;
	sw_result		err 	 =	SW_OKAY;
   
	wtimer->m_super.m_salt			=	self;
	wtimer->m_super.m_timeout		=	timeout;
	wtimer->m_super.m_handler		=	handler;
	wtimer->m_super.m_func			=	func;
	wtimer->m_super.m_extra			=	extra;
	wtimer->m_due_time.QuadPart	=	-(((LONGLONG) timeout.m_secs * TICKS_PER_SEC) + ((LONGLONG) timeout.m_usecs * TICKS_PER_USEC));

#if defined(HAVE_WSA_ASYNC_SELECT)

	if (self->m_stepped == SW_TRUE)
	{
		err = sw_win32_timer_enable_notifications(wtimer);
		sw_check_okay(err, exit);

		sw_tlist_push_front(self->m_timers, wtimer);
		self->m_num_handles++;
	}
	else
	{
		ret = SetTimer(self->m_select_window, (DWORD) wtimer, (timeout.m_secs * 1000) + (timeout.m_usecs / 1000), NULL);
		err = sw_translate_error(ret != 0, GetLastError());
		sw_check_okay_log(err, exit);
	}

#else

	err = sw_win32_timer_enable_notifications(wtimer);
	sw_check_okay(err, exit);

	sw_tlist_push_front(self->m_timers, wtimer);
	self->m_num_handles++;

#endif

	wtimer->m_super.m_registered = SW_TRUE;

exit:

   return err;
}


sw_result
sw_salt_unregister_timer(
            sw_salt	self,
            sw_timer	timer)
{
	sw_win32_timer wtimer = (sw_win32_timer) timer;

	sw_assert(wtimer->m_super.m_registered == SW_TRUE);

#if defined(HAVE_WSA_ASYNC_SELECT)

	if (self->m_stepped == SW_TRUE)
	{
		sw_assert(wtimer->m_prev != NULL);
		sw_win32_timer_disable_notifications(wtimer);
		sw_tlist_remove(self->m_timers, wtimer);
		self->m_num_handles--;
	}
	else
	{
		KillTimer(self->m_select_window, (DWORD) wtimer);
	}

#else

	sw_assert(wtimer->m_prev != NULL);
	sw_win32_timer_disable_notifications(wtimer);
	sw_tlist_remove(self->m_timers, wtimer);
	self->m_num_handles--;

#endif

	wtimer->m_super.m_registered = SW_FALSE;

	return SW_OKAY;
}


sw_result
sw_salt_register_network_interface(
				sw_salt										self,
				sw_network_interface						netif,
				sw_network_interface_handler			handler,
				sw_network_interface_handler_func	func,
				sw_opaque									extra)
{
	sw_result err = SW_OKAY;

	SW_UNUSED_PARAM(netif);

	if (self->m_notify_addr_change != SW_TRUE)
	{
		self->m_handler 	=	handler;
		self->m_func		=	func;
		self->m_extra		=	extra;

		self->m_notify_addr_change_thread_stop		=	CreateEvent(NULL, FALSE, FALSE, NULL);
		err = sw_translate_error(self->m_notify_addr_change_thread_stop, GetLastError());
		sw_check_okay_log(err, exit);

		self->m_notify_addr_change_thread_sync		=	CreateEvent(NULL, FALSE, FALSE, NULL);
		err = sw_translate_error(self->m_notify_addr_change_thread_sync, GetLastError());
		sw_check_okay_log(err, exit);

		self->m_notify_addr_change_thread_handle	=	(HANDLE) _beginthreadex( NULL, 0, sw_salt_notify_addr_change, self, 0, &self->m_notify_addr_change_thread_id );
		err = sw_translate_error(self->m_notify_addr_change_thread_handle, GetLastError());
		sw_check_okay_log(err, exit);

		self->m_notify_addr_change						=	SW_TRUE;
	}

exit:

	return err;
}


sw_result
sw_salt_unregister_network_interface_handler(
				sw_salt						self)
{
	self->m_handler	=	NULL;
	self->m_func		=	NULL;
	self->m_extra		=	NULL;

	return SW_OKAY;
}


sw_result
sw_salt_step(
				sw_salt		self,
				sw_uint32	*	msec)
{
	sw_win32_socket	wsocket;
	sw_win32_timer		wtimer;
	HANDLE			*	handles = NULL;
	sw_uint32			index;
	sw_bool				locked = SW_FALSE;
	DWORD					ret;
	sw_result			err = SW_OKAY;

	/*
	 * if the developer called run, then we will assume that we won't
	 * be using WSAAsyncSelect (on full blown windows).  therefore,
	 * we have to go through our sockets and timers and enable them.
	 *
	 * that also means that the first timer is screwed, as the time
	 * won't be correct.  but only for that first one
	 */
	if (self->m_stepped == SW_FALSE)
	{
		self->m_stepped = SW_TRUE;

		for (wsocket = self->m_sockets.m_next; wsocket; wsocket = wsocket->m_next)
		{
			sw_win32_socket_enable_notifications(wsocket);
		}

		for (wtimer = self->m_timers.m_next; wtimer; wtimer = wtimer->m_next)
		{
			sw_win32_timer_enable_notifications(wtimer);
		}
	}

	sw_salt_lock(self);
	locked = SW_TRUE;

	handles = (HANDLE*) sw_malloc(self->m_num_handles * sizeof(HANDLE));
	err = sw_translate_error(handles, SW_E_MEM);
	sw_check_okay_log(err, exit);
	
	index					=	0;
	handles[index++]	=	self->m_stop_run_event;
	handles[index++]	=	self->m_notify_addr_change_event;

	for (wsocket = self->m_sockets.m_next; wsocket; wsocket = wsocket->m_next)
	{
		handles[index++] = wsocket->m_handle;
	}

	for (wtimer = self->m_timers.m_next; wtimer; wtimer = wtimer->m_next)
	{
		handles[index++] = wtimer->m_handle;
	}

	sw_assert(index == self->m_num_handles);

	sw_salt_unlock(self);
	locked = SW_FALSE;

	ret = WaitForMultipleObjects(self->m_num_handles, handles, FALSE, (msec ? *msec : INFINITE));
	err = sw_translate_error(ret != WAIT_FAILED, GetLastError());
	sw_check_okay_log(err, exit);
	
	sw_salt_lock(self);
	locked = SW_TRUE;

	if (ret == WAIT_TIMEOUT)
	{
		//
		// nothing to do here
		//
	}
	else if (ret == WAIT_OBJECT_0)
	{
		//
		// not much to do here
		//
		self->m_run = SW_FALSE;
	}
	else if (ret == (WAIT_OBJECT_0 + 1))
	{
		sw_assert(self->m_notify_addr_change == SW_TRUE);

		(*self->m_func)(self->m_handler, self, NULL, self->m_extra);
	}
	else
	{
		sw_bool found = SW_FALSE;

		//
		// check for socket events
		//
		for (wsocket = self->m_sockets.m_next; wsocket; wsocket = wsocket->m_next)
		{
			if (handles[(ret - WAIT_OBJECT_0)] == wsocket->m_handle)
			{
				(wsocket->m_super.m_func)(wsocket->m_super.m_handler, self, &wsocket->m_super, wsocket->m_super.m_events, wsocket->m_super.m_extra);
				found = SW_TRUE;
				break;
			}
		}

		if (!found)
		{
			//
			// check for timer events
			//
			for (wtimer = self->m_timers.m_next; wtimer; wtimer = wtimer->m_next)
			{
				if (handles[(ret - WAIT_OBJECT_0)] == wtimer->m_handle)
				{
					sw_win32_timer_disable_notifications(wtimer);
					sw_salt_unregister_timer(self, &wtimer->m_super);
					(wtimer->m_super.m_func)(wtimer->m_super.m_handler, self, &wtimer->m_super, wtimer->m_super.m_timeout, wtimer->m_super.m_extra);

					break;
				}
			}
		}
	}

exit:

	if (handles != NULL)
	{
		sw_free(handles);
	}

	if (locked)
	{
		sw_salt_unlock(self);
	}

	return err;
}


sw_result
sw_salt_run(
            sw_salt	self)
{
	self->m_run = SW_TRUE;

   while (self->m_run == SW_TRUE)
	{
		sw_salt_step(self, NULL);
	}

	return SW_OKAY;
}
		

sw_result
sw_salt_stop_run(
				sw_salt	self)
{
	BOOL result;

	result = SetEvent(self->m_stop_run_event);

	return SW_OKAY;
}


sw_result
sw_salt_lock(
				sw_salt	self)
{
	EnterCriticalSection(&self->m_mutex);
	return SW_OKAY;
}


sw_result
sw_salt_unlock(
				sw_salt	self)
{
	LeaveCriticalSection(&self->m_mutex);
	return SW_OKAY;
}


#if defined(HAVE_WSA_ASYNC_SELECT)

static LRESULT CALLBACK
sw_salt_window_handler(
				HWND		hwnd,
				UINT		uMsg,
				WPARAM	wParam,
				LPARAM	lParam)
{
	sw_salt self;

	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT cs = (LPCREATESTRUCT) lParam;
		SetWindowLong(hwnd, GWL_USERDATA, (long) cs->lpCreateParams);
	}

	self = (sw_salt) GetWindowLong(hwnd, GWL_USERDATA);

	if (self != NULL)
	{
		if (uMsg == WM_SOCKET)
		{
			if (WSAGETSELECTERROR(lParam) && !(HIWORD(lParam)))
			{
				sw_debug(SW_LOG_ERROR, "socket error %d\n", GetLastError());
			}
			else
			{
				sw_win32_socket	wsocket;
				sw_bool				found;
				sw_sockdesc_t		fd;

				found =	SW_FALSE;
				fd		=	(sw_sockdesc_t) wParam;
	
				for (wsocket = self->m_sockets.m_next; wsocket; wsocket = (sw_win32_socket) wsocket->m_next)
				{
					if (wsocket->m_super.m_desc == fd)
					{
						sw_win32_socket_has_window_event(wsocket, self->m_select_window, WM_SOCKET);

						switch (WSAGETSELECTEVENT(lParam))
						{
							case FD_ACCEPT:
							{
								if (wsocket->m_super.m_func != NULL)
								{
									u_long cmd = 0;
						
									ioctlsocket(wsocket->m_super.m_desc, FIONBIO, &cmd);

               				(*wsocket->m_super.m_func)(wsocket->m_super.m_handler, self, &wsocket->m_super, SW_SOCKET_READ, wsocket->m_super.m_extra);
								}
							}
							break;

							case FD_READ:
							case FD_CLOSE:
							{
								if (wsocket->m_super.m_func != NULL)
								{	
               				(*wsocket->m_super.m_func)(wsocket->m_super.m_handler, self, &wsocket->m_super, SW_SOCKET_READ, wsocket->m_super.m_extra);	
								}
							}
							break;
						
       					case FD_WRITE:
							{
								if (wsocket->m_super.m_func != NULL)
								{
               				(*wsocket->m_super.m_func)(wsocket->m_super.m_handler, self, &wsocket->m_super, SW_SOCKET_WRITE, wsocket->m_super.m_extra);
								}
							}
							break;
	
							case FD_OOB:
							{
								if (wsocket->m_super.m_func != NULL)
								{
               				(*wsocket->m_super.m_func)(wsocket->m_super.m_handler, self, &wsocket->m_super, SW_SOCKET_OOB, wsocket->m_super.m_extra);
								}
							}
       					break;
						}
	
						sw_win32_socket_had_window_event(wsocket, self->m_select_window, WM_SOCKET);
						
						found = SW_TRUE;
						
						break;
					}
				}
	
				if (!found)
				{
					sw_debug(SW_LOG_ERROR, "internal error\n");
				}
			}
		}
		else if (uMsg == WM_TIMER)
		{
			sw_win32_timer	wtimer = (sw_win32_timer) wParam;

			if (wtimer->m_super.m_registered == SW_TRUE)
			{
				sw_salt_unregister_timer(self, &wtimer->m_super);

				(wtimer->m_super.m_func)(wtimer->m_super.m_handler, self, &wtimer->m_super, wtimer->m_super.m_timeout, wtimer->m_super.m_extra);
			}
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif


static unsigned WINAPI
sw_salt_notify_addr_change(
					LPVOID	inParam)
{
	sw_salt		self = (sw_salt) inParam;
	DWORD			res;
	sw_result	err = SW_OKAY;

	while (1)
	{
		HANDLE		inputs[2];
		HANDLE		handle;
		OVERLAPPED	o;

		handle	=	NULL;
		o.hEvent	=	handle;
		
		res = NotifyAddrChange(&handle, &o);
		err = sw_translate_error(res == ERROR_IO_PENDING, WSAGetLastError());
		sw_check_okay_log(err, exit);

		inputs[0] = handle;
		inputs[1] = self->m_notify_addr_change_thread_stop;

		res = WaitForMultipleObjects(2, inputs, FALSE, INFINITE);
		err = sw_translate_error(res != WAIT_FAILED, GetLastError());
		sw_check_okay_log(err, exit);

		if (res == WAIT_OBJECT_0)
		{
			SetEvent(self->m_notify_addr_change_event);
		}
		else if (res == (WAIT_OBJECT_0 + 1))
		{
			break;
		}
		else
		{
			sw_debug(SW_LOG_ERROR, "WaitForMultipleObjects() failed: %d\n", GetLastError());
			break;
		}
	}

exit:

	SetEvent(self->m_notify_addr_change_thread_sync);

	_endthreadex(0);

	return 0;
}

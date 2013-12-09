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

#define	WIN32_WINDOWS		0x0401		// Needed to use waitable timers.
#define	_WIN32_WINDOWS		0x0401		// Needed to use waitable timers.

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	<mDNSClientAPI.h>
#include	<mDNSPlatform.h>
#include <NotOSX/notosx_mdns_stub.h>
#include	"win32_mdns.h"
#include <salt/salt.h>
#include <salt/debug.h>

/*
 * Constants
 */
mDNSexport mDNSs32 		mDNSPlatformOneSecond	=	1000;
mDNSexport sw_string		g_iname						=	NULL;
mDNSexport sw_uint32		g_aname						=	0;
mDNSexport sw_string		g_aname_string				=	NULL;
extern mDNS				*	gMDNSPtr;


/*
 * Prototypes
 */
static DWORD
sw_win32_mdns_servant_thread(
				LPVOID	arg);


sw_result
sw_mdns_servant_new(
					sw_mdns_servant	*	servant,
					sw_uint16				port,
					sw_string			*	filters,
					sw_uint32					num_filters)
{
	sw_win32_mdns_servant	self;
	HANDLE						handles[2];
	DWORD							sig;
	sw_result					err = SW_OKAY;

	self = (sw_win32_mdns_servant) sw_malloc(sizeof(struct _sw_win32_mdns_servant));
	err = sw_translate_error(self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	self->m_super.m_salt				=	NULL;
	self->m_super.m_port				=	port;
	self->m_super.m_filters			=	filters;
	self->m_super.m_num_filters	=	num_filters;
	*servant								=	&self->m_super;

	self->m_start_handle	=	CreateEvent(NULL, 0, 0, NULL);
	err = sw_translate_error(self->m_start_handle, GetLastError());
	sw_check_okay_log(err, exit);

	self->m_stop_handle = CreateEvent(NULL, 0, 0, NULL);
	err = sw_translate_error(self->m_stop_handle, GetLastError());
	sw_check_okay_log(err, exit);

	self->m_thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) sw_win32_mdns_servant_thread, self, 0, &self->m_tid);
	err = sw_translate_error(self->m_thread_handle, GetLastError());
	sw_check_okay_log(err, exit);

	handles[0] = self->m_start_handle;
	handles[1] = self->m_stop_handle;

	sig = WaitForMultipleObjects(2, handles, FALSE, 10 * 1000);

	if (sig != WAIT_OBJECT_0)
	{
		sw_debug(SW_LOG_ERROR, "unable to start mDNSResponder thread\n");
		err = SW_E_UNKNOWN;
	}

exit:

	return err;
}


sw_result
sw_mdns_servant_delete(
				sw_mdns_servant	servant)
{
	sw_win32_mdns_servant	self	=	(sw_win32_mdns_servant) servant;
	DWORD							ret;

	mDNSPlatformStopRun(gMDNSPtr);
	sw_salt_stop_run(self->m_super.m_salt);

//	ret = WaitForSingleObject(self->m_stop_handle, 10 * 1000);

//	if (ret != WAIT_OBJECT_0)
//	{
//		sw_debug(SW_LOG_ERROR, "mDNSResponderThread failed to stop...terminating\n");
//		TerminateThread(self->m_thread_handle, 0);
//	}

        CloseHandle(self->m_thread_handle);
        CloseHandle(self->m_start_handle);
        CloseHandle(self->m_stop_handle);

	return S_OK;	
}


sw_uint16
sw_mdns_servant_port(
				sw_mdns_servant	servant)
{
	sw_win32_mdns_servant self = (sw_win32_mdns_servant) servant;

	return self->m_super.m_port;
}


static DWORD
sw_win32_mdns_servant_thread(
				LPVOID	arg)
{
	sw_win32_mdns_servant	self	=	(sw_win32_mdns_servant) arg;
	DWORD							ret	=	0;

	if (sw_mdns_servant_init(&self->m_super) != SW_OKAY)
	{
		sw_debug(SW_LOG_ERROR, "sw_mdns_servant_init_super failed\n");
		SetEvent(self->m_stop_handle);
		return 0;
	}

	SetEvent(self->m_start_handle);

	ret = mDNSPlatformRun(gMDNSPtr);

	sw_mdns_servant_fina(&self->m_super);

	SetEvent(self->m_stop_handle);

	ExitThread(0);

	return ret;
}

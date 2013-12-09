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
//#undef _WIN32_WINNT
//#define _WIN32_WINNT 0x0500

#include "win32_time.h"
#include <salt/debug.h>


sw_result
sw_timer_init(
			sw_timer * timer)
{
	sw_win32_timer wtimer;
	sw_result		err = SW_OKAY;

	wtimer = (sw_win32_timer) sw_malloc(sizeof(struct _sw_win32_timer));
	err = sw_translate_error(wtimer, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_timer_super_init(&wtimer->m_super);
	sw_check_okay(err, exit);

	wtimer->m_handle = CreateWaitableTimer(NULL, TRUE, NULL);
	err = sw_translate_error(wtimer->m_handle, GetLastError());
	sw_check_okay_log(err, exit);

	*timer = &wtimer->m_super;

exit:

	if (err != SW_OKAY)
	{
		sw_timer_fina(&wtimer->m_super);
	}

	return err;
}


sw_result
sw_timer_fina(
			sw_timer self)
{
	sw_win32_timer wtimer = (sw_win32_timer) self;
	BOOL				ret;

	ret = CloseHandle(wtimer->m_handle);
	sw_assert(ret == TRUE);

	sw_timer_super_fina(self);

	sw_free(wtimer);

	return SW_OKAY;
}


sw_result
sw_win32_timer_enable_notifications(
				sw_win32_timer timer)
{
	BOOL			ok;
	sw_result	err = SW_OKAY;

	ok = SetWaitableTimer(timer->m_handle, &timer->m_due_time, 0, NULL, NULL, 0);
	err = sw_translate_error(ok, GetLastError());
	sw_check_okay_log(err, exit);

exit:

	return err;
}


sw_result
sw_win32_timer_disable_notifications(
				sw_win32_timer timer)
{
	BOOL			ok;
	sw_result	err = SW_OKAY;

	ok = CancelWaitableTimer(timer->m_handle);
	err = sw_translate_error(ok, GetLastError());
	sw_check_okay_log(err, exit);

exit:

	return err;
}

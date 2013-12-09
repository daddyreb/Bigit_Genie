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

#include "macosx_time.h"
#include "macosx_salt.h"
#include <salt/debug.h>
#include <stdio.h>


static void
sw_macosx_timer_callback(
				CFRunLoopTimerRef	timer,
				void				*	info);


sw_result
sw_timer_init(
			sw_timer * timer)
{
	sw_macosx_timer	mtimer;
	sw_result			err = SW_OKAY;

	mtimer = (sw_macosx_timer) sw_malloc(sizeof(struct _sw_macosx_timer));
	err = sw_translate_error(mtimer, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_timer_super_init(&mtimer->m_super);
	sw_check_okay(err, exit);

	*timer = &mtimer->m_super;

exit:

	return err;
}


sw_result
sw_timer_fina(
			sw_timer self)
{
	sw_macosx_timer mtimer = (sw_macosx_timer) self;
	
	sw_timer_super_fina(self);

	sw_free(mtimer);

	return SW_OKAY;
}


sw_result
sw_macosx_timer_enable_notifications(
								sw_macosx_timer mtimer)
{
	CFRunLoopTimerContext	context;
	sw_result					err = SW_OKAY;

	sw_assert(mtimer);

	mtimer->m_cf_timer = NULL;

	context.version			=	0;
	context.info				=	mtimer;
	context.retain				=	NULL;
	context.release			=	NULL;
	context.copyDescription	=	NULL;

	mtimer->m_cf_timer = CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent() + mtimer->m_interval, 0, 0, 0, sw_macosx_timer_callback, &context);
	sw_check(mtimer->m_cf_timer, exit, err = SW_E_UNKNOWN);

	CFRunLoopAddTimer(mtimer->m_cf_run_loop, mtimer->m_cf_timer, kCFRunLoopCommonModes);

	CFRelease(mtimer->m_cf_timer);

exit:

	return err;
}


sw_result
sw_macosx_timer_disable_notifications(
								sw_macosx_timer mtimer)
{
	sw_result err = SW_OKAY;

	if (mtimer->m_super.m_registered == SW_TRUE)
	{
		CFRunLoopTimerInvalidate(mtimer->m_cf_timer);
	}

	return err;
}


static void
sw_macosx_timer_callback(
				CFRunLoopTimerRef	timer,
				void				*	info)
{
	sw_macosx_timer mtimer = (sw_macosx_timer) info;

	(mtimer->m_super.m_func)(mtimer->m_super.m_handler, mtimer->m_super.m_salt, &mtimer->m_super, mtimer->m_super.m_timeout, mtimer->m_super.m_extra);

	if (mtimer->m_super.m_salt->m_step == SW_TRUE)
	{
		CFRunLoopStop(mtimer->m_cf_run_loop);
	}
}

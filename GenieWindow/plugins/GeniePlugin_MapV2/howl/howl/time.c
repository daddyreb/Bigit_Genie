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

#include "time_i.h"
#include <salt/debug.h>
#include <stdio.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	include <sys/time.h>
#elif defined(WIN32)
#	include <time.h>
#elif defined(__VXWORKS__)
#	include <time.h>
#endif


#define MAXMICRO 1000000


sw_result
sw_timer_super_init(
			sw_timer timer)
{
	timer->m_registered	=	SW_FALSE;
	timer->m_handler		=	NULL;
	timer->m_func			=	NULL;
	timer->m_extra			=	NULL;

	return SW_OKAY;
}


sw_result
sw_timer_super_fina(
			sw_timer timer)
{
	SW_UNUSED_PARAM(timer);

	return SW_OKAY;
}
            

sw_result
sw_time_init_now(
				sw_time	*	self)
{
#if defined(WIN32)

	SYSTEMTIME	system_time;
	struct tm	local_time;
	time_t		now;
	GetLocalTime(&system_time);
	local_time.tm_sec   = system_time.wSecond;
	local_time.tm_min   = system_time.wMinute;
	local_time.tm_hour  = system_time.wHour;
	local_time.tm_mday  = system_time.wDay;
	local_time.tm_mon   = system_time.wMonth - 1;
	local_time.tm_year  = system_time.wYear - 1900;
	local_time.tm_wday  = 0;
	local_time.tm_yday  = 0;
	local_time.tm_isdst = -1;
	now = mktime(&local_time);
	self->m_secs	= (sw_uint32) now;
	self->m_usecs	= (system_time.wMilliseconds * 1000);   

#elif defined(__VXWORKS__)

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	self->m_secs	= ts.tv_sec;
	self->m_usecs	= ts.tv_nsec / 1000;

#else         

	struct timeval tv;
	gettimeofday(&tv, NULL);
	self->m_secs	=	tv.tv_sec;
	self->m_usecs	=	tv.tv_usec;

#endif

	return SW_OKAY;
}


sw_result
sw_time_init(
		sw_time	*	time)
{
	time->m_secs	=	0;
	time->m_usecs	=	0;

	return SW_OKAY;
}


sw_result
sw_time_fina(
		sw_time	time)
{
	SW_UNUSED_PARAM(time);

	return SW_OKAY;
}


sw_time
sw_time_add(
		sw_time	x,
		sw_time	y)
{
	sw_time sum;

	sum.m_usecs = x.m_usecs + y.m_usecs;
	sum.m_secs = x.m_secs + y.m_secs;

	/* normalize the sum */
	if (sum.m_usecs >= MAXMICRO)
	{
		sw_uint32 nsec;

		nsec			 = sum.m_usecs / MAXMICRO;
		sum.m_usecs	-= nsec * MAXMICRO;
		sum.m_secs	+= nsec;
	}

	return sum;
}


sw_time
sw_time_sub(
		sw_time	x,
		sw_time	y)
{
	sw_time diff;

	/*
		check for case: y > x (this assumes that the numbers have been normalized)
	*/
	if ((x.m_secs < y.m_secs) ||
	   ((x.m_secs == y.m_secs) && (x.m_usecs <= y.m_usecs)))
	{
		diff.m_secs		=	0;
		diff.m_usecs	=	0;
		return diff;
	}

	if (x.m_usecs < y.m_usecs) 
	{
		sw_int32 nsec = (y.m_usecs - x.m_usecs) / MAXMICRO + 1;
		y.m_usecs -= MAXMICRO * nsec;
		y.m_secs += nsec;
	}

	if (x.m_usecs - y.m_usecs > MAXMICRO) 
	{
		int nsec		= (x.m_usecs - y.m_usecs) / MAXMICRO;
		y.m_usecs	+= MAXMICRO * nsec;
		y.m_secs		-= nsec;
	} 

	diff.m_usecs = x.m_usecs - y.m_usecs;
	diff.m_secs = x.m_secs - y.m_secs;

   if (diff.m_usecs >= MAXMICRO)
	{
		sw_uint32 nsec;

		nsec				=	diff.m_usecs / MAXMICRO;
		diff.m_usecs	-= nsec * MAXMICRO;
		diff.m_secs		+= nsec;
	}

	return diff;
}


sw_int32
sw_time_cmp(
		sw_time x,
		sw_time y)
{
	sw_int32 dsecs;

	dsecs = x.m_secs - y.m_secs;

	return (dsecs == 0) ? x.m_usecs - y.m_usecs : dsecs;
}

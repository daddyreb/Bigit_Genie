#ifndef _salt_posix_salt_h
#define _salt_posix_salt_h

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

#include <salt/salt.h>
#include <salt/address.h>
#include "posix_socket.h"
#include "posix_time.h"
#include "posix_signal.h"
#include "posix_interface.h"
#include <pthread.h>


#ifdef __cplusplus
extern "C"
{
#endif 

#define SW_MAX_NETIF	10

struct _sw_salt
{
	struct _sw_posix_socket					m_sockets;
	struct _sw_posix_timer					m_timers;
	struct _sw_posix_signal					m_signals;
	sw_uint32										m_nifcount;
	struct _sw_posix_network_interface	m_nifs[SW_MAX_NETIF];
	struct _sw_posix_timer					m_niftimer;
	int											m_pipe[2];
	pthread_mutex_t							m_mutex;
	sw_bool										m_run;
};


#ifdef __cplusplus
}
#endif 


#endif

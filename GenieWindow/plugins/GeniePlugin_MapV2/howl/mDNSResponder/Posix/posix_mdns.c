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

#include <Posix/posix_mdns.h>
#include <mDNSClientAPI.h>
#include <mDNSPlatform.h>
#include <salt/signal.h>
#include <salt/debug.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>


mDNSs32			mDNSPlatformOneSecond	=	1000;
sw_string		g_iname						=	NULL;
extern mDNS	*	gMDNSPtr;


static void*
sw_mdns_servant_thread(
						void		*	arg);


sw_result
sw_mdns_daemon_is_running(
						sw_uint32	*	major,
						sw_uint32	*	minor,
						sw_uint32	*	micro)
{
	*major = 0;
	*minor = 9;
	*micro = 4;

	return SW_OKAY;
}


sw_result
sw_mdns_servant_new(
				sw_mdns_servant	*	servant,
				sw_uint16				port,
				sw_string			*	filters,
				sw_uint32					num_filters)
{
	sw_posix_mdns_servant	self;
	int							res;
	char							c;
	sw_result					err = SW_OKAY;

	self = (sw_posix_mdns_servant) sw_malloc(sizeof(struct _sw_posix_mdns_servant));
	err = sw_translate_error(self, SW_E_MEM);
	sw_check_okay_log(err, exit);

	self->m_super.m_salt				=	NULL;
	self->m_super.m_port				=	port;
	self->m_super.m_filters			=	filters;
	self->m_super.m_num_filters	=	num_filters;

	res = pipe(self->m_pipe);
	err = sw_translate_error(res == 0, sw_system_errno());
	sw_check_okay_log(err, exit);

	res = pthread_create(&self->m_tid, NULL, sw_mdns_servant_thread, self);
	err = sw_translate_error(res == 0, sw_system_errno());
	sw_check_okay_log(err, exit);

	res = read(self->m_pipe[0], &c, 1);
	err = sw_translate_error(res == 1, SW_E_UNKNOWN);
	sw_check_okay_log(err, exit);

	sw_check(c == 1, exit, err = SW_E_UNKNOWN);

	*servant = &self->m_super;

exit:

	if (err && self)
	{
		sw_mdns_servant_delete(&self->m_super);
		*servant = NULL;
	}

	return err;
}


sw_result
sw_mdns_servant_delete(
					sw_mdns_servant self)
{
	sw_posix_mdns_servant	servant = (sw_posix_mdns_servant) self;
	int							problems;
	fd_set						readFds;
	struct timeval				tv;
	int							ret;
	sw_result					err;

	mDNSPlatformStopRun(gMDNSPtr);
	sw_salt_stop_run(servant->m_super.m_salt);

	FD_ZERO(&readFds);
	FD_SET(servant->m_pipe[0], &readFds);
	tv.tv_sec	=	10;
	tv.tv_usec	=	0;
	problems		=	0;
	ret			=	select(servant->m_pipe[0] + 1, &readFds, NULL, NULL, &tv);

	if (ret == 1)
	{
		char c;

		ret = read(servant->m_pipe[0], &c, 1);

		if ((ret != 1) || (c != 0))
		{
			problems++;
		}
	}
	else if (ret == 0)
	{
		sw_debug(SW_LOG_ERROR, "timeout waiting for mdns_servant thread to exit\n");
		problems++;
	}
	else
	{
		sw_debug(SW_LOG_ERROR, "select error: %d\n", errno);
		problems++;
	}

	close(servant->m_pipe[0]);
	close(servant->m_pipe[1]);

	sw_free(servant);

	if (problems)
	{
		return SW_E_UNKNOWN;
	}
	
	return SW_OKAY;
}


sw_uint16
sw_mdns_servant_port(
					sw_mdns_servant	self)
{
	return self->m_port;
}


sw_result
sw_mdns_servant_refresh(
					sw_mdns_servant	self)
{
	refresh_interface_list(gMDNSPtr);
}


static void*
sw_mdns_servant_thread(
				void * arg)
{
	sw_posix_mdns_servant	self	=	(sw_posix_mdns_servant) arg;
	sw_result					err;
	void						*	ret	=	0;

	err = sw_mdns_servant_init(&self->m_super);
	sw_check_okay(err, exit);

	write(self->m_pipe[1], "\1", 1);

	mDNSPlatformRun(gMDNSPtr);

	sw_mdns_servant_fina(&self->m_super);

exit:

	write(self->m_pipe[1], "\0", 1);
	
	pthread_exit(NULL);

	return ret;
}

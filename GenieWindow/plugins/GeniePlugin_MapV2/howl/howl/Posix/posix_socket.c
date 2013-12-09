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

#include "posix_socket.h"
#include <salt/debug.h>


sw_result
sw_tcp_socket_init(
			sw_socket * self)
{
	sw_posix_socket	psocket;
	sw_result			err = SW_OKAY;
	
	psocket = (sw_posix_socket) sw_malloc(sizeof(struct _sw_posix_socket));
	err = sw_translate_error(psocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(psocket, 0, sizeof(struct _sw_posix_socket));

	err = sw_tcp_socket_super_init(&psocket->m_super);
	sw_check_okay(err, exit);

	*self = &psocket->m_super;

exit:

	if (err && psocket)
	{
		sw_socket_fina(&psocket->m_super);
	}

	return err;
}


sw_result
sw_tcp_socket_init_with_desc(
			sw_socket 	* self,
			sw_sockdesc_t	desc)
{
	sw_posix_socket	psocket;
	sw_result			err = SW_OKAY;
	
	psocket = (sw_posix_socket) sw_malloc(sizeof(struct _sw_posix_socket));
	err = sw_translate_error(psocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(psocket, 0, sizeof(struct _sw_posix_socket));

	err = sw_tcp_socket_super_init_with_desc(&psocket->m_super, desc);
	sw_check_okay(err, exit);

	*self = &psocket->m_super;

exit:

	if (err && psocket)
	{
		sw_socket_fina(&psocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_udp_socket_init(
			sw_socket * self)
{
	sw_posix_socket	psocket;
	sw_result			err = SW_OKAY;
	
	psocket = (sw_posix_socket) sw_malloc(sizeof(struct _sw_posix_socket));
	err = sw_translate_error(psocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(psocket, 0, sizeof(struct _sw_posix_socket));

	err = sw_udp_socket_super_init(&psocket->m_super);
	sw_check_okay(err, exit);

	*self = &psocket->m_super;

exit:

	if (err && psocket)
	{
		sw_socket_fina(&psocket->m_super);
		*self = NULL;
	}

	return err;
}


sw_result
sw_multicast_socket_init(
			sw_socket * self)
{
	sw_posix_socket	psocket;
	sw_result			err = SW_OKAY;
	
	psocket = (sw_posix_socket) sw_malloc(sizeof(struct _sw_posix_socket));
	err = sw_translate_error(psocket, SW_E_MEM);
	sw_check_okay_log(err, exit);

	sw_memset(psocket, 0, sizeof(struct _sw_posix_socket));

	err = sw_multicast_socket_super_init(&psocket->m_super);
	sw_check_okay(err, exit);

	*self = &psocket->m_super;

exit:

	if (err && psocket)
	{
		sw_socket_fina(&psocket->m_super);
	}

	return err;
}


sw_result
sw_socket_fina(
			sw_socket self)
{
	sw_posix_socket psocket = (sw_posix_socket) self;

	sw_socket_super_fina(self);

	sw_free(self);

	return SW_OKAY;
}

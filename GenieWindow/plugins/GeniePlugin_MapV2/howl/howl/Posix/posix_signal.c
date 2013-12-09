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

#include "posix_signal.h"
#include <salt/debug.h>
#include <stdio.h>


sw_result
sw_signal_init(
			sw_signal * signal,
			int			num)
{
	sw_posix_signal	psignal;
	sw_result			err = SW_OKAY;

	psignal = (sw_posix_signal) sw_malloc(sizeof(struct _sw_posix_signal));
	err = sw_translate_error(psignal, SW_E_MEM);
	sw_check_okay_log(err, exit);

	err = sw_signal_super_init(&psignal->m_super);
	sw_check_okay(err, exit);

	psignal->m_original	=	NULL;
	psignal->m_num			=	num;
	psignal->m_next		=	NULL;
	psignal->m_prev		=	NULL;

	*signal = &psignal->m_super;

exit:

	if (err && psignal)
	{
		sw_signal_fina(&psignal->m_super);
		*signal = NULL;
	}

	return err;
}


sw_result
sw_signal_fina(
			sw_signal self)
{
	sw_posix_signal ptimer = (sw_posix_signal) self;
	
	sw_signal_super_fina(self);

	sw_free(ptimer);

	return SW_OKAY;
}

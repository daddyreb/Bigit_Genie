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

#include "posix_salt.h"
#include "tlist.h"
#include <salt/address.h>
#include <salt/debug.h>
#define __USE_GNU
#include <pthread.h>
#include <errno.h>
#include <stdio.h>


static sw_result
sw_salt_insert_timer(
				sw_salt				self,
				sw_posix_timer		timer);


static sw_result
sw_salt_remove_timer(
				sw_salt				self,
				sw_posix_timer		timer);


static sw_result
sw_salt_peek_timer(
				sw_salt				self,
				sw_posix_timer	*	timer);


static sw_result
sw_salt_pop_timer(
				sw_salt				self);


#ifdef __DEBUG_PRIORITY_QUEUE__
static void 
sw_timer_queue_show(
				sw_posix_timer		root);
#endif


static void
sw_salt_signal_handler(
				int num);


static int g_write_pipe;


static sw_result
nif_timer_handler(
	sw_timer_handler	handler,
	sw_salt				salt,
	sw_timer				timer,
	sw_time				timeout,
	sw_opaque			extra);

sw_result
sw_salt_init(
				sw_salt		*	salt,
				int				argc,
				char			**	argv)
{
	pthread_mutexattr_t 	attrs;
	sw_result 				err = SW_OKAY;

	*salt = (sw_salt) sw_malloc(sizeof(struct _sw_salt));
	sw_check(*salt, exit, err = SW_E_MEM);

	(*salt)->m_sockets.m_next	=	NULL;
	(*salt)->m_sockets.m_prev	=	NULL;
	(*salt)->m_timers.m_next	=	NULL;
	(*salt)->m_timers.m_prev	=	NULL;
	(*salt)->m_signals.m_next	=	NULL;
	(*salt)->m_signals.m_prev	=	NULL;
	(*salt)->m_nifcount	= 0;

	err = sw_timer_init((sw_timer*)&((*salt)->m_niftimer));
	sw_check_okay(err, exit);

	pthread_mutexattr_init(&attrs);
#if defined(__linux__)
	pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE_NP);
#else
	pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
#endif
	pthread_mutex_init(&(*salt)->m_mutex, &attrs);
	(*salt)->m_run					=	SW_FALSE;

	/*
	 * create the command pipes
	 */
	err = pipe((*salt)->m_pipe);
	sw_check_okay(err, exit);

	g_write_pipe = (*salt)->m_pipe[1];

exit:

	if (err && *salt)
	{
		sw_salt_fina(*salt);
		*salt = NULL;
	}

   return err;
}


sw_result
sw_salt_fina(
				sw_salt	self)
{
   // sw_timer_queue_free(self->m_timer_queue);
	sw_timer_fina(*(sw_timer*)&self->m_niftimer);
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
	sw_posix_socket psocket = (sw_posix_socket) socket;

	sw_assert(psocket);

	sw_debug(SW_LOG_VERBOSE, "sw_salt_select() : fd %d with events %d\n", sw_socket_desc(socket), events);

	psocket->m_super.m_events		=	events;
	psocket->m_super.m_func			=	func;
	psocket->m_super.m_handler		=	handler;
	psocket->m_super.m_extra		=	extra;
	psocket->m_super.m_registered	=	SW_TRUE;

	sw_tlist_push_front(self->m_sockets, psocket);

	sw_assert(psocket->m_prev);

   return SW_OKAY;
}


sw_result
sw_salt_unregister_socket(
					sw_salt			self,
					sw_socket		socket)
{
	sw_posix_socket psocket = (sw_posix_socket) socket;

	sw_assert(psocket);
	sw_assert(psocket->m_super.m_registered == SW_TRUE);
	sw_assert(psocket->m_prev);

	sw_tlist_remove(self->m_sockets, psocket);

	psocket->m_super.m_registered = SW_FALSE;

	return SW_OKAY;
}


sw_result
sw_salt_register_timer(
            sw_salt						salt,
				sw_timer						timer,
            sw_time						timeout,
            sw_timer_handler			handler,
            sw_timer_handler_func	func,
				sw_opaque					extra)
{
	sw_posix_timer ptimer = (sw_posix_timer) timer;

	ptimer->m_rel_time			=	timeout;
	ptimer->m_super.m_func		=	func;
	ptimer->m_super.m_handler	=	handler;
	ptimer->m_super.m_extra		=	extra;

   sw_salt_insert_timer(salt, ptimer);

   return SW_OKAY;
}


sw_result
sw_salt_unregister_timer(
            sw_salt		salt,
            sw_timer		timer)
{
	sw_posix_timer ptimer = (sw_posix_timer) timer;

	sw_salt_remove_timer(salt, ptimer);

	return SW_OKAY;
}


sw_result
sw_salt_register_network_interface(
            sw_salt										salt,
				sw_network_interface						netif,
				sw_network_interface_handler			handler,
            sw_network_interface_handler_func	func,
				sw_opaque									extra)
{
	return SW_OKAY;
}


sw_result
sw_salt_unregister_network_interface(
            sw_salt					salt,
				sw_network_interface	netif)
{
	return SW_OKAY;
}


sw_result
sw_salt_register_signal(
				sw_salt						self,
				sw_signal					gsignal,
				sw_signal_handler			handler,
				sw_signal_handler_func	func,
				sw_opaque					extra)
{
	sw_posix_signal psignal = (sw_posix_signal) gsignal;

	psignal->m_super.m_handler	=	handler;
	psignal->m_super.m_func		=	func;
	psignal->m_super.m_extra	=	extra;
	psignal->m_original			=	signal(psignal->m_num, sw_salt_signal_handler);

	sw_tlist_push_front(self->m_signals, psignal);

	return SW_OKAY;
}


sw_result
sw_salt_unregister_signal(
				sw_salt		salt,
				sw_signal	gsignal)
{
	sw_posix_signal psignal = (sw_posix_signal) gsignal;

	signal(psignal->m_num, psignal->m_original);

	sw_tlist_remove(self->m_signals, psignal);

	return SW_OKAY;
}


sw_result
sw_salt_lock(
				sw_salt	self)
{
	sw_assert(self);
	pthread_mutex_lock(&self->m_mutex);
}


sw_result
sw_salt_unlock(
				sw_salt	self)
{
	sw_assert(self);
	pthread_mutex_unlock(&self->m_mutex);
}


sw_result
sw_salt_step(
				sw_salt		self,
				sw_uint32	*	msec)
{
	sw_bool				invoke_time_handler;
	fd_set				writefds;
	fd_set				readfds;
	fd_set				oobfds;
	sw_posix_socket	psocket;
	sw_posix_timer		ptimer;
	sw_sockdesc_t		max_fd;
	sw_int32				num;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&oobfds);
	max_fd = 0;

	/* sw_debug_memory_inuse(); */

	FD_SET(self->m_pipe[0], &readfds);
	max_fd = self->m_pipe[0] + 1;

   for (psocket = self->m_sockets.m_next; psocket; psocket = psocket->m_next)
	{
		if (psocket->m_super.m_events & SW_SOCKET_READ)
		{
			sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : adding %d to select read mask\n", psocket->m_super.m_desc);
			FD_SET(psocket->m_super.m_desc, &readfds);
		}
	
		if (psocket->m_super.m_events & SW_SOCKET_WRITE)
		{
			sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : adding %d to select write mask\n", psocket->m_super.m_desc);
			FD_SET(psocket->m_super.m_desc, &writefds);
		}
		
		if (psocket->m_super.m_events & SW_SOCKET_OOB)
		{
			sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : adding %d to select oob mask\n", psocket->m_super.m_desc);
			FD_SET(psocket->m_super.m_desc, &oobfds);
		}
	
		if (psocket->m_super.m_desc > max_fd)
		{
			max_fd = psocket->m_super.m_desc;
		}
	}

	/*
	 * check for timers
	 */
	sw_salt_peek_timer(self, &ptimer);

	if (msec || ptimer)
	{
		sw_time			before;
		struct timeval tv;

		/*
		 * if we have passed in both a timeout, and installed a timer handler, then we
		 * need to figure out which one will expire first
		 */
		if (msec && ptimer)
		{
			invoke_time_handler	=	SW_FALSE; 
			tv.tv_sec				=	(*msec / 1000);
			tv.tv_usec				=	(*msec % 1000) * 1000;

			if ((tv.tv_sec > ptimer->m_time_left.m_secs) ||
			    ((tv.tv_sec == ptimer->m_time_left.m_secs) && (tv.tv_usec >= ptimer->m_time_left.m_usecs)))
			{
				invoke_time_handler	=	SW_TRUE; 
				tv.tv_sec				=	ptimer->m_time_left.m_secs;
				tv.tv_usec				=	ptimer->m_time_left.m_usecs;
			}
		}
		else if (msec)
		{
			invoke_time_handler	=	SW_FALSE; 
			tv.tv_sec				=	(*msec / 1000);
			tv.tv_usec				=	(*msec % 1000) * 1000;
		}
		else
		{
			invoke_time_handler	=	SW_TRUE; 
			tv.tv_sec				=	ptimer->m_time_left.m_secs;
			tv.tv_usec				=	ptimer->m_time_left.m_usecs;
		}

      sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : calling select with max fd = %d, timeout = (%d, %d)\n", max_fd + 1, tv.tv_sec, tv.tv_usec);

		sw_time_init_now(&before);

		num = select(max_fd + 1, &readfds, &writefds, &oobfds, &tv);

      if ((num > 0) && ptimer)
      {
			sw_time		elapsed;
			sw_time		after;

			sw_time_init_now(&after);

			elapsed					= sw_time_sub(after, before);
			ptimer->m_time_left	= sw_time_sub(ptimer->m_time_left, elapsed);
		}
	}
	else
	{
		sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : calling select with max fd = %d, !timeout\n", max_fd + 1);
		num = select(max_fd + 1, &readfds, &writefds, &oobfds, NULL);
	}

	sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : select returns %d\n", num);
	if (num == -1)
	{
		sw_debug(SW_LOG_VERBOSE, "errno %d\n", errno);
	}

	if (num > 0)
	{
		if (FD_ISSET(self->m_pipe[0], &readfds))
		{
			sw_posix_signal	psignal;
			sw_uint8				sig;

			read(self->m_pipe[0], &sig, 1);

			num--;

			for (psignal = self->m_signals.m_next; psignal; psignal = psignal->m_next)
			{
				if (psignal->m_num == sig)
				{
					(*psignal->m_super.m_func)(psignal->m_super.m_handler, self, &psignal->m_super, psignal->m_super.m_extra);
					break;
				}
			}
		}

      for (psocket = self->m_sockets.m_next; psocket && (num > 0); psocket = psocket->m_next)
		{
			sw_socket_event events = 0;
		
			if (FD_ISSET(psocket->m_super.m_desc, &readfds))
			{
				sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : fd %d is readable\n", psocket->m_super.m_desc);
				FD_CLR(psocket->m_super.m_desc, &readfds);
				events |= SW_SOCKET_READ;
				num--;
			}
		
			if (FD_ISSET(psocket->m_super.m_desc, &writefds))
			{
				sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : fd %d is writable\n", psocket->m_super.m_desc);
				FD_CLR(psocket->m_super.m_desc, &writefds);
				events |= SW_SOCKET_WRITE;
				num--;
			}
		
			if (FD_ISSET(psocket->m_super.m_desc, &oobfds))
			{
				sw_debug(SW_LOG_VERBOSE, "sw_salt_run() : fd %d is oobable\n", psocket->m_super.m_desc);
				FD_CLR(psocket->m_super.m_desc, &oobfds);
				events |= SW_SOCKET_OOB;
				num--;
			}

			if (events)
			{
				(*psocket->m_super.m_func)(psocket->m_super.m_handler, self, &psocket->m_super, psocket->m_super.m_events, psocket->m_super.m_extra);

				/*
				 * reset to the beginning of list to make sure we don't
				 * get screwed by someone removing guys out from under
				 * us..not efficient but effective
				 */
				psocket = &self->m_sockets;
			}
		}
	}
	else if (num == 0)
	{
		if (invoke_time_handler)
		{
			sw_assert(ptimer != NULL);

			/*
			 * stash the id before we invoke.  if the handler removes the timer during the callback
			 * then we'll cool.  we know that it has been removed because it is no longer the head
			 * of the list.  we make sure this happens by clearing it's time left to zero.  if we try
			 * to insert another timer, even if the timer's time is zero, it won't go in the head
			 * of the list.
			 *
			 * we don't want to use "timer" after the callback, because it might have been free'd
			 */

			ptimer->m_time_left.m_secs		= 0;
			ptimer->m_time_left.m_usecs	= 0;
				
			sw_assert(ptimer->m_super.m_func != NULL);
			sw_salt_pop_timer(self);
			
			(ptimer->m_super.m_func)(ptimer->m_super.m_handler, self, &ptimer->m_super, ptimer->m_super.m_timeout, ptimer->m_super.m_extra);
		}
	}
	else if (errno != EINTR)
	{
		sw_debug(SW_LOG_ERROR, "select() failed: %d\n", errno);
			
		/*
		 * YO what do we do here?
		 *
		 * the EBADF is a bad one
		 */
	}
   
   return SW_OKAY;
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
	sw_uint8 c = 255;

	self->m_run = SW_FALSE;

	return (write(self->m_pipe[1], &c, 1) == 1) ? SW_OKAY : SW_E_UNKNOWN;
}


static sw_result
sw_salt_insert_timer(
				sw_salt			self,
				sw_posix_timer	timer)
{
	sw_assert(self != NULL);
	sw_assert(timer != NULL);

	timer->m_time_left = timer->m_rel_time;
	timer->m_next = NULL;

	if (self->m_timers.m_next == NULL)
	{
		sw_tlist_push_front(self->m_timers, timer);
	}
	else
	{
		struct _sw_posix_timer * 	pnode = self->m_timers.m_next;
		struct _sw_posix_timer *	pprev = NULL;
		sw_time 							sumTime = pnode->m_time_left;
		sw_time 							prevSum;

		/* find the right spot */
		while ( (pnode != NULL) && 
				  (sw_time_cmp(timer->m_time_left, sumTime) >= 0) )
		{
			pprev 	= pnode;
			pnode 	= pnode->m_next;
			prevSum 	= sumTime;

			if (pnode != NULL)
			{
				sumTime = sw_time_add(sumTime, pnode->m_time_left);
			}
		}

		/* insert the timer */
		if (pprev == NULL)	/* at head */
		{
			sw_tlist_push_front(self->m_timers, timer);
			timer->m_next->m_time_left	= sw_time_sub(timer->m_next->m_time_left, timer->m_time_left);
		}
		else						/* anywhere else */
		{
			timer->m_next = pprev->m_next;

			if (pprev->m_next != NULL)
			{
				pprev->m_next->m_prev = timer;
			}

			timer->m_prev = pprev;
			pprev->m_next = timer;

			/* adjust the time_left fields in the new node,
			 * and the next node */
			if (pnode)
			{
				pnode->m_time_left = sw_time_sub(sumTime, timer->m_time_left);
			}

			timer->m_time_left = sw_time_sub(timer->m_time_left, prevSum);
		}
	}

	return SW_OKAY;
}


static sw_result
sw_salt_remove_timer(
				sw_salt				self,
				sw_posix_timer		timer)
{
	struct _sw_posix_timer * pFound = self->m_timers.m_next;
	struct _sw_posix_timer * pTrail;

	if (pFound == NULL)
	{
		return SW_E_INIT;
	}

	/* if removing the head */
	if (pFound == timer)
	{
		sw_tlist_remove(self->m_timers, pFound);

		return SW_OKAY;
	}

	/* anywhere else */
	pTrail = pFound;
	pFound = pFound->m_next;
	while (pFound != NULL)
	{
		if (pFound == timer)
		{
			sw_tlist_remove(self->m_timers, pFound);

			return SW_OKAY;
		}

		pTrail = pFound;
		pFound = pFound->m_next;
	}

	return SW_E_INIT;
}


static sw_result
sw_salt_peek_timer(
				sw_salt				self,
				sw_posix_timer	*	timer)
{
	if (self->m_timers.m_next != NULL)
	{
		(*timer) = self->m_timers.m_next;
	}
	else
	{
		(*timer) = NULL;
	}

	return SW_OKAY;
}


static sw_result
sw_salt_pop_timer(
				sw_salt				self)
{
	struct _sw_posix_timer * popped = self->m_timers.m_next;

	if (popped == NULL)
	{
		return SW_OKAY;
	}

	sw_tlist_remove(self->m_timers, popped);

	return SW_OKAY;
}


static void
sw_salt_signal_handler(
				int num)
{
	sw_uint8 c = (sw_uint8) num;

	write(g_write_pipe, &c, 1);
}

int
sw_socket_error_code()
{
	return errno;
}

static sw_result
nif_timer_handler(
	sw_timer_handler	handler,
	sw_salt				salt,
	sw_timer				timer,
	sw_time				timeout,
	sw_opaque			extra)
{
	sw_result	result;
	sw_bool		islinked;
	sw_time		onesec;
	int			i;

	onesec.m_secs 	= 1;
	onesec.m_usecs	= 0;

	sw_debug(SW_LOG_VERBOSE, "***************checking link status, nifcount %d **************\n", salt->m_nifcount);
	/* check link connectivity for relevant nifs */
	for (i = 0; i < salt->m_nifcount; i++)
	{
		if ((result = sw_network_interface_link_status(&salt->m_nifs[i].m_super, &islinked)) != SW_OKAY)
		{
			return result;
		}

		/* invoke handler if any state has changed */
		if (islinked != salt->m_nifs[i].m_super.m_linked)
		{
			sw_network_interface nif = &salt->m_nifs[i].m_super;

			salt->m_nifs[i].m_super.m_linked = islinked;
			sw_debug(SW_LOG_VERBOSE, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@link state changed, invoking the handler@@@@@@@@@@@@@@@@@@@@\n");
			(*nif->m_func)(nif->m_handler, salt, nif, nif->m_extra);
		}
	}

	/* reinstall oneshot timer */
   return sw_salt_register_timer(salt, (struct _sw_timer*)&salt->m_niftimer, onesec, (sw_timer_handler) salt, nif_timer_handler, NULL);
}

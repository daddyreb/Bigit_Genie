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

#include "howl_config.h"
#include "posix_main.h"
#include <salt/debug.h>
#include <mDNSServant.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>


static int verbose_flag = 0;

/*
 * Solaris defined SIOCGIFCONF etc in <sys/sockio.h> but 
 * other platforms don't even have that include file.  So, 
 * if we haven't yet got a definition, let's try to find 
 * <sys/sockio.h>.
 */
static sw_result
write_pidfile(char * dname, int * pidfd)
{
   char        pidfile[64];
   char        str[16];
	int			res;
	sw_result	err;
                                                       
   umask(022);

#if defined(__sun)
	sprintf(pidfile, "/tmp/%s.pid", dname);
#else
   sprintf(pidfile, "/var/run/%s.pid", dname);
#endif
                                                       
   /* open the file */
	*pidfd = open(pidfile, O_RDWR|O_CREAT, 0644);
	err = sw_translate_error(*pidfd != -1, errno);
	sw_check_okay_log(err, exit);
	
   /* lock it */
#if !defined(__CYGWIN__)
   res = lockf(*pidfd, F_TLOCK,0);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);
#endif

   /* write our pid */
   sprintf(str, "%d\n", getpid());
   write(*pidfd, str, strlen(str)); // record pid to lockfile
                                                       
exit:
                                                       
   return err;
}


static sw_result
close_pidfile(int pidfd)
{
	if (pidfd != -1)
	{
		close(pidfd);
	}
}


#if defined(__sun)
static void
become_daemon(void)
{
	int i;

	if (fork())
	{
		_exit(0);
	}
		
#ifdef HAVE_SETSID
	setsif();
#else
#	ifdef TIOCNOTTY
	i = open("/dev/tty", O_RDWR);
	if(i>=0)
	{
		ioctl(i, (int)TIOCNOTTY, (char*)0);
		close(i);
	}
#	endif	/*TIOCNOTTY*/
#endif
	
	for (i=0; i<3; i++)
	{
		close(i);
		open("/dev/null", O_RDWR);
	}
}
#endif


static void
print_usage(const char * program)
{
	fprintf(stderr, "usage: %s [options]\n\n", "mDNSResponder" );
	fprintf(stderr, "Options:\n\n");
	fprintf(stderr, "-h              this help\n");
	fprintf(stderr, "-v              display version\n");
	fprintf(stderr, "-d              run in debug mode\n");
	fprintf(stderr, "-i ifname       run only on interface ifname\n");
	fprintf(stderr, "-a addr         run only on interface whose address is addr\n");
	fprintf(stderr, "-f config_file  load config_file\n");
	fprintf(stderr, "--verbose       verbose output\n\n");
	exit(0);
}


static void
sw_mdnsd_signal_handler(
						int sig)
{
	fprintf(stderr, "in signal handler: %d\n", sig);

}


int
main(int argc, char ** argv)
{
	sw_bool				make_daemon = SW_TRUE;
	sw_string			filters[5];
	sw_string			conf_file = NULL;
	sw_uint32			num_filters = 0;
	sw_mdns_servant	servant;
	sw_bool				done = SW_FALSE;
	int					option_index;
	int					pidfd;
	int					opt;
	sigset_t				signalSet;
	int					sig;

	static struct option long_opts[] =
	{
		{ "verbose", no_argument, &verbose_flag, 1 },
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long(argc, argv, "?hvdi:a:f:", long_opts, &option_index ) ) != EOF)
	{
		switch (opt)
		{
			case '?':
			case 'h':
			{
				print_usage(argv[0]);
			}
			break;

			case 'v':
			{
				fprintf(stderr, "mDNSResponder release " VERSION " Copyright(c) Porchdog Software 2003-2005.\n");
				exit(0);
			}
			break;

			case 'd':
			{
				make_daemon = SW_FALSE;
			}
			break;

			case 'i':
			{
				filters[num_filters++] = sw_strdup(optarg);
			}
			break;

			case 'a':
			{
				filters[num_filters++] = sw_strdup(optarg);
			}
			break;

			case 'f':
			{
				conf_file = sw_strdup(optarg);
			}
			break;
		}
	}

	if ( verbose_flag )
	{
		sw_debug_set_level( SW_LOG_VERBOSE );
	}

	if (make_daemon)
	{
#if defined(__sun)
		become_daemon();
#else
		if (daemon(SW_FALSE, SW_FALSE) < 0)
		{
			sw_debug(SW_LOG_ERROR, "unable to daemonize\n");
			exit(-1);
		}
#endif
	}
	
	signal(SIGINT, sw_mdnsd_signal_handler);
	signal(SIGHUP, sw_mdnsd_signal_handler);
	signal(SIGQUIT, sw_mdnsd_signal_handler);
	signal(SIGUSR1, sw_mdnsd_signal_handler);
	signal(SIGUSR2, sw_mdnsd_signal_handler);
	signal(SIGPIPE, sw_mdnsd_signal_handler);

	sigfillset(&signalSet);

	if (sw_mdns_servant_new(&servant, 5335, filters, num_filters) != SW_OKAY)
	{
		sw_debug(SW_LOG_ERROR, "error initializing mdnsd\n");
		exit(-1);
	}

	if (conf_file != NULL)
	{
		sw_mdns_servant_load_file(servant, conf_file);
	}
	else if (sw_mdns_servant_load_file(servant, "/etc/howl/mDNSResponder.conf") != SW_OKAY)
	{
		sw_mdns_servant_load_file(servant, "/usr/local/etc/howl/mDNSResponder.conf");
	}

	write_pidfile("mDNSResponder", &pidfd);

	while (!done)
	{
		#ifdef __sun
			sig=sigwait(&signalSet);
		#else
		sigwait(&signalSet, &sig);
		#endif
		switch (sig)
		{
			case SIGINT:
			{
				done = SW_TRUE;
			}
			break;

			case SIGQUIT:
			{
				done = SW_TRUE;
			}

			case SIGPIPE:
			{
			}
			break;

			default:
			{
				sw_mdns_servant_refresh(servant);
			}
			break;
		}
	}

	sw_mdns_servant_delete(servant);

	close_pidfile(pidfd);

	exit(0);
}

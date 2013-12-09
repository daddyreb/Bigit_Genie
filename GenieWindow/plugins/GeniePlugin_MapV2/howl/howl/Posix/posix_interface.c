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

#include "posix_interface.h"
#include <salt/debug.h>
#include <salt/interface.h>
#include <net/if.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>

#if defined(__linux__)
typedef unsigned long long u64;
typedef __uint32_t 	u32;
typedef __uint16_t 	u16;
typedef __uint8_t		u8;
#	include <linux/sockios.h>
#	include <linux/ethtool.h> 
#endif

#ifndef SIOCGIFCONF
#	include <sys/sockio.h>
#endif

#define SW_E_BAD_ARG 23

sw_result
sw_network_interface_link_status(
			sw_network_interface 	self, 
			sw_bool					*	islinked);

sw_result
sw_network_interfaces_fina(
	sw_uint32 					nifc,
	sw_network_interface * 	nifv);

static sw_result
sw_posix_inet_socket(int * fd);


#if defined(__CYGWIN__)

static int
if_nametoindex( const char * name )
{
	unsigned long index = 5381;
	int c;

	while ( c = *name++ )
	{
		index = ( ( index << 5 ) + index ) + c;
	}

	return index;
}

#endif

	
/*
	sockaddr_dl is only referenced if we're using IP_RECVIF, 
   so only include the header in that case.
*/

#ifdef  IP_RECVIF
#	include <net/if_dl.h>
#endif

#define max(a,b)	((a) > (b) ? (a) : (b))

static sw_result
sw_posix_inet_socket(int * fd)
{
	sw_result err;

	*fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	err = sw_translate_error(*fd >= 0, SW_E_UNKNOWN);
	sw_check_okay_log(err, exit);

exit:

	return err;
}

sw_string 
sw_mac_address_name(
				sw_mac_address	self,
				sw_string		name,
				sw_uint32			len)
{
	sprintf(name, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", self.m_id[0], self.m_id[1], self.m_id[2], self.m_id[3], self.m_id[4], self.m_id[5]);

	return name;
}

sw_result 
sw_network_interface_init(
            sw_network_interface   *  netif)
{
	sw_posix_network_interface	pnetif;
	sw_result 						err = SW_OKAY;

   /* allocate a posix network interface */
   pnetif = (sw_posix_network_interface) sw_malloc(sizeof(struct _sw_posix_network_interface));
	err = sw_translate_error(pnetif, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/* netif points to the first field we allocated */
	*netif = &pnetif->m_super;

exit:

	if (err && pnetif)
	{
		sw_network_interface_fina(&pnetif->m_super);
		*netif = NULL;
	}

   return err;
}


sw_result 
sw_network_interface_fina(
			sw_network_interface self)
{
	sw_posix_network_interface pnetif = (sw_posix_network_interface) self;
	
	sw_free(pnetif);

	return SW_OKAY;
}

sw_result
sw_network_interface_exists(
		sw_string name, 
		sw_bool * exists)
{
#if defined(__linux__)
	int 				s;
	struct ifreq	req;
	int				r;
	sw_result 		err = SW_OKAY;

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&s);
	sw_check_okay(err, exit);
	
	memset(&req, 0, sizeof(req));
	strncpy(req.ifr_name, name, IFNAMSIZ);

	if ((r = ioctl(s, SIOCGIFINDEX, &req)) < 0 && errno != ENODEV) 
	{
		sw_debug(SW_LOG_ERROR, "sw_network_interface_exists: %s\n", strerror(errno));
		err = SW_E_INIT;
		goto exit;
	}

	*exists = (r >= 0 && req.ifr_ifindex >= 0);

exit:

	close(s);

	return err;

#else
	return SW_E_UNKNOWN;
#endif
}

sw_result 
sw_network_interface_up(
					sw_network_interface self)
{
	int 				fd;
	struct ifreq 	ifr;
	int				res;
	sw_result		err = SW_OKAY;

#if !defined(__CYGWIN__)

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&fd);
	sw_check_okay(err, exit);

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, self->m_name, sizeof(ifr.ifr_name)-1);

	/* get the interface flags */
	res = ioctl(fd, SIOCGIFFLAGS, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);

	/* if it's already up, we're exit */
	if ((ifr.ifr_flags & IFF_UP) == IFF_UP)
	{
		err = SW_OKAY;
		goto exit;
	}

	/* get interface flags */
	res = ioctl(fd, SIOCGIFFLAGS, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);

	/* bring the interface up */
	ifr.ifr_flags |= IFF_UP;

	/* set interface flags */
	res = ioctl(fd, SIOCSIFFLAGS, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);

exit:

	close(fd);

#endif

	return err;
}


sw_result 
sw_network_interface_link_status(
			sw_network_interface 	self, 
			sw_bool					*	islinked)
{
	int 						fd;
	int						r;
	struct ifreq			ifr;
	int						res;
	sw_result 				err = SW_OKAY;
#if defined(__linux__)
	struct ethtool_value edata;
#endif

	sw_assert(self != NULL);		/* must be initialized */
	sw_assert(islinked != NULL);	/* must be allocated */

	*islinked = SW_TRUE;

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&fd);
	sw_check_okay(err, exit);

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, self->m_name, sizeof(ifr.ifr_name) - 1);

	/* get the interface flags */
	res = ioctl(fd, SIOCGIFFLAGS, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);

	/* if it's down, we can't have a link */
	if ((ifr.ifr_flags & IFF_UP) != IFF_UP)
	{
		*islinked = SW_FALSE;
		goto exit;
	}

#if defined(__linux__)
	edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t) &edata;

	res = ioctl(fd, SIOCETHTOOL, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);

	*islinked = edata.data ? SW_TRUE : SW_FALSE;
#endif

exit:

	close(fd);

	return err;
}


sw_result 
sw_network_interface_set_ipv4_address(
				sw_network_interface		self,
				sw_ipv4_address			addr)
{
	return sw_ipv4_address_init_from_address(&self->m_ipv4_address, addr);
}


static sw_result
sw_posix_network_interface_init_from_name(
	sw_posix_network_interface	nif,
	sw_string						ifname)
{
	int 				sock;
	char			*	cptr;
	sw_bool			islinked;
	struct ifreq	ifr;
	char				tmpname[18];
	int				res;
	sw_result		err = SW_OKAY;

	sw_assert(nif != NULL);			/* already allocated */
	sw_assert(ifname != NULL);		/* already initialized */

	/* make sure there won't be any problems */
	sw_memset(nif, 0, sizeof(struct _sw_posix_network_interface));
	if (sw_strlen(ifname) > IFNAMSIZ)
	{
		ifname[IFNAMSIZ - 1] = (char)0;
	}

	/* name */
	sw_strcpy(nif->m_super.m_name, ifname);

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&sock);
	sw_check_okay(err, exit);

	strcpy(ifr.ifr_name, ifname);
	ifr.ifr_addr.sa_family = AF_INET;

	/* ip address */
	if ((ioctl(sock, SIOCGIFADDR, &ifr)) < 0)
	{
		sw_saddr addrAny = 0;

		sw_debug(SW_LOG_ERROR, "couldn't get ip address for %s, setting ip to 0\n", ifr.ifr_name);
		err = sw_ipv4_address_init_from_saddr(&(nif->m_super.m_ipv4_address), addrAny);
		sw_check_okay(err, exit);
	}
	else
	{
		err = sw_ipv4_address_init_from_saddr(&(nif->m_super.m_ipv4_address), ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
		sw_check_okay(err, exit);
	}

	sw_ipv4_address_name(nif->m_super.m_ipv4_address, tmpname, 18);
	sw_debug(SW_LOG_VERBOSE, "got ip address: %s\n", tmpname);

	/* mac address */
#if defined(SIOCGIFHWADDR)
	res = ioctl(sock, SIOCGIFHWADDR, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);
	sw_memcpy(nif->m_super.m_mac_address.m_id, (sw_uint8*)(ifr.ifr_hwaddr.sa_data), sizeof(sw_mac_address)); 
#elif defined(SIOCGENADDR)
	res = ioctl(sock, SIOCGENADDR, &ifr);
	err = sw_translate_error(res == 0, errno);
	sw_check_okay_log(err, exit);
	sw_memcpy(nif->m_super.m_mac_address.m_id, (sw_uint8*)(ifr.ifr_ifru.ifru_enaddr), sizeof(sw_mac_address)); 
#endif

	/* index */
	nif->m_super.m_index = if_nametoindex(ifr.ifr_name);

	/* initialize link status field */
	sw_network_interface_link_status(&nif->m_super, &(nif->m_super.m_linked));

exit:

	close(sock);

	return err;
}


static sw_result
sw_posix_network_interface_init_from_ifreq(
	sw_posix_network_interface	nif,
	struct ifreq				*	ifr)
{
	int 				sock = SW_INVALID_SOCKET;
	char			*	cptr;
	sw_bool			islinked;
	int				res;
	sw_result		err = SW_E_INIT;

	sw_assert(nif != NULL);		/* already allocated */
	sw_assert(ifr != NULL);		/* already initialized */

	/* name */
	if ( (cptr = strchr(ifr->ifr_name, ':')) != NULL)
	{
		*cptr = 0;      
	}
	
	sw_memcpy(nif->m_super.m_name, ifr->ifr_name, IFNAMSIZ);
	nif->m_super.m_name[IFNAMSIZ-1] = '\0';

	/* index */
	nif->m_super.m_index = if_nametoindex(ifr->ifr_name);

	/* ip address */

	sw_ipv4_address_init_from_saddr(&(nif->m_super.m_ipv4_address), ((struct sockaddr_in *) &ifr->ifr_addr)->sin_addr.s_addr);

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&sock);
	sw_check_okay(err, exit);

#if defined(SIOCGIFNETMASK)
	// subnet mask

	res = ioctl( sock, SIOCGIFNETMASK, ifr );
	err = sw_translate_error( res != -1, errno );
	sw_check_okay_log( err, exit );
	sw_ipv4_address_init_from_saddr(&(nif->m_super.m_netmask), ((struct sockaddr_in*) &ifr->ifr_addr )->sin_addr.s_addr );
#endif

#if defined(SIOCGIFHWADDR)
	/* mac address */
	res = ioctl(sock, SIOCGIFHWADDR, ifr);
	err = sw_translate_error(res != -1, errno);
	sw_check_okay_log(err, exit);
	sw_memcpy(nif->m_super.m_mac_address.m_id, (sw_uint8*)(ifr->ifr_hwaddr.sa_data), sizeof(sw_mac_address)); 
#elif defined(SIOCGENADDR)
	res = ioctl(sock, SIOCGENADDR, ifr);
	err = sw_translate_error(res != -1, errno);
	sw_check_okay_log(err, exit);
	sw_memcpy(nif->m_super.m_mac_address.m_id, (sw_uint8*)(ifr->ifr_ifru.ifru_enaddr), sizeof(sw_mac_address)); 
#endif

	/* initialize link status field */
	sw_network_interface_link_status(&nif->m_super, &(nif->m_super.m_linked));

exit:

	close(sock);

	return err;
}


static sw_result
get_iflist_buffer(int sock, struct ifconf * ifc)
{
	int  lastlen = 0;
	sw_result err = SW_OKAY;

	sw_assert(ifc != NULL);

	/* we'll guess at the buflen until we grab enough */
	ifc->ifc_len = 10 * sizeof(struct ifreq);
	for ( ; ; )
	{
		/* allocate buffer big enough for if config data */
   	ifc->ifc_buf = (char*) sw_malloc(ifc->ifc_len);
		err = sw_translate_error(ifc->ifc_buf, SW_E_MEM);
		sw_check_okay_log(err, exit);

		/* get ifnet config */
		if (ioctl(sock, SIOCGIFCONF, ifc) < 0)
		{
			if (errno != EINVAL || lastlen != 0)
			{
				/* not enough memory */
				sw_free(ifc->ifc_buf);
				err = SW_E_INIT;
				goto exit;
			}
		}
		else
		{			
			/* if we get the same size twice, we're exit */
			if (ifc->ifc_len == lastlen)
			{
				err = SW_OKAY;   /* success */
				goto exit;
			}
				
			lastlen = ifc->ifc_len;
		}
		  
		/* bump up the size */
		ifc->ifc_len += 10 * sizeof(struct ifreq);   

		/* free the last guess */
		sw_free(ifc->ifc_buf);
	}

exit:

	return err;
}

#define _PATH_PROCNET_DEV "/proc/net/dev"

static int
procenetdev_version(char * buf)
{
	if (strstr(buf, "compressed"))
		return 3;
	if (strstr(buf, "bytes"))
		return 2;

	return 1;
}

static char * 
get_name(char * name, char *p)
{
    while (isspace(*p))
   p++;
    while (*p) {
   if (isspace(*p))
       break;
   if (*p == ':') {  /* could be an alias */
       char *dot = p, *dotname = name;
       *name++ = *p++;
       while (isdigit(*p))
      *name++ = *p++;
       if (*p != ':') { /* it wasn't, backup */
      p = dot;
      name = dotname;
       }
       if (*p == '\0')
      return NULL;
       p++;
       break;
   }
   *name++ = *p++;
    }
    *name++ = '\0';
    return p;

}


sw_result
sw_network_interfaces2(
		sw_uint32					* 	nifc,
		sw_network_interface	**	nifv)
{
	FILE * fh;
	char buf[512];
	int procnetdev_vsn;
	sw_result err = SW_OKAY;

	if ((fh = fopen(_PATH_PROCNET_DEV, "r")) == NULL)
	{
		sw_debug(SW_LOG_ERROR, "cannot open %s (%s).\n", _PATH_PROCNET_DEV, strerror(errno));
		goto exit;
	}

	fgets(buf, sizeof buf, fh); /* eat line */
	fgets(buf, sizeof buf, fh); /* eat line */

	/* TODO hard code 10 for now */
	/* allocate nifv  */
	*nifv = (sw_network_interface*) sw_malloc(10 * sizeof(sw_network_interface));
	err = sw_translate_error(*nifv, SW_E_MEM);
	sw_check_okay_log(err, exit);
	
	*nifc = 0;
	while (fgets(buf, sizeof(buf), fh))
	{
		char * s, name[IFNAMSIZ];
		sw_posix_network_interface nif;
		sw_ipv4_address ipaddr;

		s = get_name(name, buf);

		if (sw_strcmp(name, "lo") == 0)
		{
			continue;
		}

		/* create a new netif */
		err = sw_network_interface_init((sw_network_interface*)&nif); 
		sw_check_okay(err, exit);

		/* initialize fields */
		err = sw_posix_network_interface_init_from_name(nif, name);
		sw_check_okay(err, exit);

		err = sw_network_interface_ipv4_address((sw_network_interface)nif, &ipaddr);
		sw_check_okay(err, exit);

		(*nifv)[(*nifc)++] = (sw_network_interface)nif;
	}

	if (ferror(fh))
	{
		perror(_PATH_PROCNET_DEV);
		err = SW_E_INIT;
		goto exit;
	}

	err = SW_OKAY;

exit:

	if (err && *nifv)
	{
		sw_network_interfaces_fina(*nifc, *nifv);
	}

	if (fh != NULL)
	{
		fclose(fh);
	}

	return err;
}

sw_result
sw_network_interfaces(
		sw_uint32					* 	nifc,
		sw_network_interface	**	nifv)
{
	int         	sock;
	struct ifconf 	ifc;
	struct ifreq*	ifr;
	char 			*	ptr;
	int         	i = 0, n = 0, estimate = 0;
	sw_uint32			lb_saddr = sw_ipv4_address_saddr(sw_ipv4_address_loopback());
	sw_result		err = SW_E_INIT;

	*nifc = 0;
	ifc.ifc_buf = NULL;

	/* get a socket for ioctling */
	err = sw_posix_inet_socket(&sock);
	sw_check_okay(err, exit);

	err = get_iflist_buffer(sock, &ifc);
	sw_check_okay(err, exit);

	/* count netifs */
	estimate = ifc.ifc_len / sizeof(struct ifreq);

	/* allocate nifv  */
   *nifv = (sw_network_interface*) sw_malloc(estimate * sizeof(sw_network_interface));
	err = sw_translate_error(*nifv, SW_E_MEM);
	sw_check_okay_log(err, exit);

	/* creating netifs */
	ifr = ifc.ifc_req;

	for (ptr = ifc.ifc_buf, i = 0; ptr < ifc.ifc_buf + ifc.ifc_len; i < *nifc)
	{
		struct ifreq		*	ifr;
		sw_network_interface nif;
		sw_uint32				ip_saddr;

		ifr = (struct ifreq*) ptr;

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)

		ptr += sizeof(ifr->ifr_name) + max(sizeof(struct sockaddr), ifr->ifr_addr.sa_len);

#else

		ptr += sizeof(struct ifreq);

#endif

		ip_saddr = ((struct sockaddr_in *) &ifr->ifr_addr)->sin_addr.s_addr;

		if ((ifr->ifr_addr.sa_family != AF_INET) || (ip_saddr == lb_saddr) || !ip_saddr)
		{
			continue;   /* ignore loopback+non-inet */
		}

		/* create a new netif */
		err = sw_network_interface_init(&nif); 
		sw_check_okay(err, exit);

		/* initialize fields */
		err = sw_posix_network_interface_init_from_ifreq((sw_posix_network_interface)nif, ifr);
		sw_check_okay(err, exit);

		(*nifv)[(*nifc)++] = nif;
	}
    
	err = SW_OKAY;
    
exit:
    
	if (err && (*nifv > 0))
	{
		sw_network_interfaces_fina(*nifc, *nifv);
	}

	if (ifc.ifc_buf != NULL)
	{
		sw_free(ifc.ifc_buf);
	}
	
	if (sock != -1)
	{
		int bfd = close(sock);
		sw_assert(bfd == 0);
	}
	
	return err;    
}

sw_result
sw_network_interfaces_fina(
	sw_uint32 					nifc,
	sw_network_interface * 	nifv)
{
	int			i;
	sw_result	err = SW_OKAY;

	for (i = 0; i < nifc; i++)
	{
		err = sw_network_interface_fina(nifv[i]);
		sw_check_okay(err, exit);
	}

exit:

	sw_free(nifv);

	return err;
}


sw_result 
sw_network_interface_by_name(
            sw_string               name,
            sw_network_interface *  netif)
{
	sw_result err;

	err = sw_network_interface_init(netif);
	sw_check_okay(err, exit);

	err = sw_posix_network_interface_init_from_name((sw_posix_network_interface)*netif, name);

	if (err)
	{
		sw_network_interface_fina(*netif);
	}

exit:

	return err;
}

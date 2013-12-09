#include "common.h"
int	httpAddrLength(const http_addr_t *addr)
{
  if (!addr)
    return (0);

#ifdef AF_INET6
  if (addr->addr.sa_family == AF_INET6)
    return (sizeof(addr->ipv6));
  else
#endif
  if (addr->addr.sa_family == AF_INET)
    return (sizeof(addr->ipv4));
  else
    return (0);
}

void _httpAddrSetPort(http_addr_t *addr,int port)
{
  if (!addr || port <= 0)
    return;

#ifdef AF_INET6
  if (addr->addr.sa_family == AF_INET6)
    addr->ipv6.sin6_port = htons(port);
  else
#endif
  if (addr->addr.sa_family == AF_INET)
    addr->ipv4.sin_port = htons(port);
}

char *	httpAddrString(const http_addr_t *addr,char *s,int slen)
{
  if (!addr || !s || slen <= 2)
  {
    if (s && slen >= 1)
      *s = '\0';

    return (NULL);
  }

  if (addr->addr.sa_family == AF_INET)
  {
    unsigned temp;

    temp = ntohl(addr->ipv4.sin_addr.s_addr);

    snprintf(s, slen, "%d.%d.%d.%d", (temp >> 24) & 255,
             (temp >> 16) & 255, (temp >> 8) & 255, temp & 255);
  }
#ifdef AF_INET6
  else if (addr->addr.sa_family == AF_INET6)
  {
    char	*sptr,temps[64];

    snprintf(s, slen, "[v1.%s]", temps);
  }
#endif
  else
	  strncpy(s, "UNKNOWN", slen);
  return (s);
}

const char * httpGetHostname(http_t *http,char   *s,int  slen)
{
  if (!s || slen <= 1)
    return (NULL);

  if (http)
  {
    if (http->hostname[0] == '/')
      strncpy(s, "localhost", slen);
    else
      strncpy(s, http->hostname, slen);
  }
  else
  {
    if (gethostname(s, slen) < 0)
      strncpy(s, "localhost", slen);

    if (!strchr(s, '.'))
    {
      struct hostent	*host;
      if ((host = gethostbyname(s)) != NULL && host->h_name)
      {
		strncpy(s, host->h_name, slen);
      }
    }
  }
  return (s);
}

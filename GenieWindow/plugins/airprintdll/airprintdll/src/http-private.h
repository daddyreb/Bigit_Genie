#ifndef _HTTP_PRIVATE_H_
#  define _HTTP_PRIVATE_H_
#include <stdio.h>
#include <fcntl.h>
#  include <stddef.h>
#  include <stdlib.h>

#  include <limits.h>
#  include <io.h>
#  include <winsock2.h>
#  include <http.h>
#  include "ipp-private.h"

#define O_RDONLY	_O_RDONLY
#define O_WRONLY	_O_WRONLY
#define O_CREATE	_O_CREAT
#define O_TRUNC	_O_TRUNC
#define O_BINARY _O_BINARY

#define strtoll(nptr,endptr,base) strtol((nptr), (endptr), (base))
#define HTTP(con) &((con)->http)

#define snprintf 	_snprintf

typedef void *http_tls_t;
typedef void *http_tls_credentials_t;
struct _http_s
{
  int			fd;
  int			blocking;
  int			error;
  time_t		activity;
  http_state_t		state;
  http_status_t		status;
  http_version_t	version;
  http_keepalive_t	keep_alive;
  struct sockaddr_in	_hostaddr;
  char			hostname[HTTP_MAX_HOST],
				fields[HTTP_FIELD_MAX][HTTP_MAX_VALUE];
  char			*data;
  http_encoding_t	data_encoding;
  int			_data_remaining;
  int			used;
  char			buffer[HTTP_MAX_BUFFER];
  int			auth_type;
  char			nonce[HTTP_MAX_VALUE];
  int			nonce_count;
  fd_set		*input_set;
  http_status_t		expect;
  char			*cookie;
  char			_authstring[HTTP_MAX_VALUE],
				userpass[HTTP_MAX_VALUE];
  int			digest_tries;
  off_t			data_remaining;
  http_addr_t		*hostaddr;
  char			wbuffer[HTTP_MAX_BUFFER];
  int			wused;
  char			*field_authorization;
  char			*authstring;
  http_tls_credentials_t tls_credentials;
  http_timeout_cb_t	timeout_cb;
  void			*timeout_data;
  struct timeval	timeout_value;
};

extern char		*_httpDecodeURI(char *dst, const char *src,size_t dstsize);
extern void		_httpDisconnect(http_t *http);
extern ssize_t	_httpPeek(http_t *http, char *buffer, size_t length);
extern int		_httpUpdate(http_t *http, http_status_t *status);
extern int		_httpWait(http_t *http, int msec, int usessl);
extern void		_httpAddrSetPort(http_addr_t *addr, int port);
#endif

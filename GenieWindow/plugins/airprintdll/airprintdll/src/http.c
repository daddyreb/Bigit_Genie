#include "common.h"
#include <fcntl.h>
#include <tchar.h>
#include <ctype.h>

#ifdef DEBUG
static void		http_debug_hex(const char *prefix, const char *buffer,
			               int bytes);
#endif /* DEBUG */
static http_field_t	http_field(const char *name);
static int		http_write(http_t *http, const char *buffer,
			           int length);
static int		http_write_chunk(http_t *http, const char *buffer,
			                 int length);

static const char * const http_fields[] =
			{
			  "Accept-Language",
			  "Accept-Ranges",
			  "Authorization",
			  "Connection",
			  "Content-Encoding",
			  "Content-Language",
			  "Content-Length",
			  "Content-Location",
			  "Content-MD5",
			  "Content-Range",
			  "Content-Type",
			  "Content-Version",
			  "Date",
			  "Host",
			  "If-Modified-Since",
			  "If-Unmodified-since",
			  "Keep-Alive",
			  "Last-Modified",
			  "Link",
			  "Location",
			  "Range",
			  "Referer",
			  "Retry-After",
			  "Transfer-Encoding",
			  "Upgrade",
			  "User-Agent",
			  "WWW-Authenticate"
			};
#ifdef DEBUG
static const char * const http_states[] =
			{
			  "HTTP_WAITING",
			  "HTTP_OPTIONS",
			  "HTTP_GET",
			  "HTTP_GET_SEND",
			  "HTTP_HEAD",
			  "HTTP_POST",
			  "HTTP_POST_RECV",
			  "HTTP_POST_SEND",
			  "HTTP_PUT",
			  "HTTP_PUT_RECV",
			  "HTTP_DELETE",
			  "HTTP_TRACE",
			  "HTTP_CLOSE",
			  "HTTP_STATUS"
			};
#endif /* DEBUG */

void
httpClearCookie(http_t *http)
{
  if (!http)
    return;

  if (http->cookie)
  {
    free(http->cookie);
    http->cookie = NULL;
  }
}

void
httpClearFields(http_t *http)
{
  if (http)
  {
    memset(http->fields, 0, sizeof(http->fields));
    if (http->hostname[0] == '/')
      httpSetField(http, HTTP_FIELD_HOST, "localhost");
    else
      httpSetField(http, HTTP_FIELD_HOST, http->hostname);

    if (http->field_authorization)
    {
      free(http->field_authorization);
      http->field_authorization = NULL;
    }

    http->expect = (http_status_t)0;
  }
}

void
httpFlush(http_t *http)
{
  char		buffer[8192];		/* Junk buffer */
  int		blocking;		/* To block or not to block */
  http_state_t	oldstate;		/* Old state */

  blocking = http->blocking;
  http->blocking = 0;

  oldstate = http->state;
  while (httpRead2(http, buffer, sizeof(buffer)) > 0);

  http->blocking = blocking;

  if (http->state == oldstate && http->state != HTTP_WAITING && http->fd >= 0)
  {
    http->state = HTTP_WAITING;

    closesocket(http->fd);

    http->fd = -1;
  }
}

int	httpFlushWrite(http_t *http)
{
  int	bytes;				/* Bytes written */

  if (!http || !http->wused)
  {
    return (0);
  }

  if (http->data_encoding == HTTP_ENCODE_CHUNKED)
    bytes = http_write_chunk(http, http->wbuffer, http->wused);
  else
    bytes = http_write(http, http->wbuffer, http->wused);

  http->wused = 0;

  return (bytes);
}

char * httpGets(char *line, int length, http_t *http)
{
  char	*lineptr,			/* Pointer into line */
	*lineend,			/* End of line */
	*bufptr,			/* Pointer into input buffer */
	*bufend;			/* Pointer to end of buffer */
  int	bytes,				/* Number of bytes read */
	eol;				/* End-of-line? */

  if (http == NULL || line == NULL)
    return (NULL);

  http->error = 0;
  lineptr     = line;
  lineend     = line + length - 1;
  eol         = 0;

  while (lineptr < lineend)
  {
    WSASetLastError(0);

    while (http->used == 0)
    {
      while (!_httpWait(http, http->blocking ? 30000 : 10000, 1))
      {
		if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
			continue;
        http->error = WSAETIMEDOUT;
        return (NULL);
      }
        bytes = recv(http->fd, http->buffer + http->used,
	             HTTP_MAX_BUFFER - http->used, 0);

      if (bytes < 0)
      {
        if (WSAGetLastError() == WSAEINTR)
	  continue;
	else if (WSAGetLastError() == WSAEWOULDBLOCK)
	{
	  if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
	    continue;

	  http->error = WSAGetLastError();
	}
	else if (WSAGetLastError() != http->error)
	{
	  http->error = WSAGetLastError();
	  continue;
	}

        return (NULL);
      }
      else if (bytes == 0)
      {
		http->error = EPIPE;

        return (NULL);
      }

      http->used += bytes;
    }

    for (bufptr = http->buffer, bufend = http->buffer + http->used;
         lineptr < lineend && bufptr < bufend;)
    {
      if (*bufptr == 0x0a)
      {
        eol = 1;
		bufptr ++;
		break;
      }
      else if (*bufptr == 0x0d)
		bufptr ++;
      else
		*lineptr++ = *bufptr++;
    }

    http->used -= (int)(bufptr - http->buffer);
    if (http->used > 0)
      memmove(http->buffer, bufptr, http->used);

    if (eol)
    {
      http->activity = time(NULL);

      *lineptr = '\0';

      return (line);
    }
  }

  return (NULL);
}

int	httpPrintf(http_t     *http,const char *format,...)
{
  int		bytes;
  char		buf[16384];
  va_list	ap;

  va_start(ap, format);
  bytes = vsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  if (http->data_encoding == HTTP_ENCODE_FIELDS)
    return (httpWrite2(http, buf, bytes));
  else
  {
    if (http->wused)
    {
      if (httpFlushWrite(http) < 0)
	return (-1);
    }

    return (http_write(http, buf, bytes));
  }
}

ssize_t httpRead2(http_t *http, char   *buffer, size_t length)
{
  ssize_t	bytes;			/* Bytes read */
  char		len[32];		/* Length string */

  if (http == NULL || buffer == NULL)
    return (-1);

  http->activity = time(NULL);
  http->error    = 0;

  if (length <= 0)
    return (0);

  if (http->data_encoding == HTTP_ENCODE_CHUNKED &&
      http->data_remaining <= 0)
  {
    if (httpGets(len, sizeof(len), http) == NULL)
    {
      return (0);
    }

    http->data_remaining = strtoll(len, NULL, 16);
    if (http->data_remaining < 0)
    {
      return (0);
    }
  }

  if (http->data_remaining <= 0)
  {
    if (http->data_encoding == HTTP_ENCODE_CHUNKED)
      httpGets(len, sizeof(len), http);

    if (http->state == HTTP_POST_RECV)
      http->state = (http_state_t)(http->state+1);
    else
      http->state = HTTP_WAITING;

    http->data_encoding = HTTP_ENCODE_LENGTH;

    return (0);
  }
  else if (length > (size_t)http->data_remaining)
    length = (size_t)http->data_remaining;

  if (http->used == 0 && length <= 256)
  {
    if (!http->blocking)
    {
      while (!httpWait(http, 10000))
      {
		if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
		  continue;

		return (0);
      }
    }

    if (http->data_remaining > sizeof(http->buffer))
      bytes = sizeof(http->buffer);
    else
      bytes = http->data_remaining;
    {
      bytes = recv(http->fd, http->buffer, bytes, 0);
    }

    if (bytes > 0)
      http->used = bytes;
    else if (bytes < 0)
    {
      if (WSAGetLastError() != WSAEINTR)
      {
        http->error = WSAGetLastError();
        return (-1);
      }
      else if (WSAGetLastError() == WSAEWOULDBLOCK)
      {
        if (!http->timeout_cb || !(*http->timeout_cb)(http, http->timeout_data))
	{
	  http->error = WSAEWOULDBLOCK;
	  return (-1);
	}
      }
    }
    else
    {
      http->error = EPIPE;
      return (0);
    }
  }

  if (http->used > 0)
  {
    if (length > (size_t)http->used)
      length = (size_t)http->used;

    bytes = (ssize_t)length;

    memcpy(buffer, http->buffer, length);
    http->used -= (int)length;

    if (http->used > 0)
      memmove(http->buffer, http->buffer + length, http->used);
  }
  else
  {
    if (!http->blocking)
    {
      while (!httpWait(http, 10000))
      {
		if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
		  continue;

		return (0);
      }
    }

    while ((bytes = (ssize_t)recv(http->fd, buffer, (int)length, 0)) < 0)
    {
      if (WSAGetLastError() == WSAEWOULDBLOCK)
      {
        if (!http->timeout_cb || !(*http->timeout_cb)(http, http->timeout_data))
			break;
      }
      else if (WSAGetLastError() != WSAEINTR)
        break;
    }
  }

  if (bytes > 0)
  {
    http->data_remaining -= bytes;

    if (http->data_remaining <= INT_MAX)
      http->_data_remaining = (int)http->data_remaining;
    else
      http->_data_remaining = INT_MAX;
  }
  else if (bytes < 0)
  {
    if (WSAGetLastError() == WSAEINTR)
      bytes = 0;
    else
      http->error = WSAGetLastError();
  }
  else
  {
    http->error = EPIPE;
    return (0);
  }

  if (http->data_remaining == 0)
  {
    if (http->data_encoding == HTTP_ENCODE_CHUNKED)
      httpGets(len, sizeof(len), http);

    if (http->data_encoding != HTTP_ENCODE_CHUNKED)
    {
      if (http->state == HTTP_POST_RECV)
        http->state = (http_state_t)(http->state+1);
      else
	http->state = HTTP_WAITING;
    }
  }

#ifdef DEBUG
  http_debug_hex("httpRead2", buffer, (int)bytes);
#endif /* DEBUG */

  return (bytes);
}

void
httpSetCookie(http_t     *http,
              const char *cookie)
{
  if (!http)
    return;

  if (http->cookie)
    free(http->cookie);

  if (cookie)
    http->cookie = strdup(cookie);
  else
    http->cookie = NULL;
}

void
httpSetField(http_t *http, http_field_t field,const char   *value)
{
  if (http == NULL ||
      field < HTTP_FIELD_ACCEPT_LANGUAGE ||
      field > HTTP_FIELD_WWW_AUTHENTICATE ||
      value == NULL)
    return;

  strncpy(http->fields[field], value, HTTP_MAX_VALUE);

  if (field == HTTP_FIELD_AUTHORIZATION)
  {
    if (http->field_authorization)
      free(http->field_authorization);

    http->field_authorization = strdup(value);
  }
  else if (field == HTTP_FIELD_HOST)
  {
    char *ptr = strchr(value, ':');

    if (value[0] != '[' && ptr && strchr(ptr + 1, ':'))
    {

      snprintf(http->fields[HTTP_FIELD_HOST],
               sizeof(http->fields[HTTP_FIELD_HOST]), "[%s]", value);
    }
    else
    {
      ptr = http->fields[HTTP_FIELD_HOST];

      if (*ptr)
      {
	ptr += strlen(ptr) - 1;

	if (*ptr == '.')
	  *ptr = '\0';
      }
    }
  }
}

int _httpUpdate(http_t *http, http_status_t *status)
{
  char		line[32768],		/* Line from connection... */
		    *value;			/* Pointer to value on line */
  http_field_t	field;			/* Field index */
  int		major, minor;		/* HTTP version numbers */

  if (!httpGets(line, sizeof(line), http))
  {
    *status = HTTP_ERROR;
    return (0);
  }

  if (line[0] == '\0')
  {
    if (http->status == HTTP_CONTINUE)
    {
      *status = http->status;
      return (0);
    }

    if (http->status < HTTP_BAD_REQUEST)
      http->digest_tries = 0;

    httpGetLength2(http);

    switch (http->state)
    {
      case HTTP_GET :
      case HTTP_POST :
      case HTTP_POST_RECV :
      case HTTP_PUT :
          http->state = (http_state_t)(http->state+1);
      case HTTP_POST_SEND :
      case HTTP_HEAD :
	  break;

      default :
	  http->state = HTTP_WAITING;
	  break;
    }

    *status = http->status;
    return (0);
  }
  else if (!strncmp(line, "HTTP/", 5))
  {
    int	intstatus;

    if (sscanf(line, "HTTP/%d.%d%d", &major, &minor, &intstatus) != 3)
    {
      *status = http->status = HTTP_ERROR;
      return (0);
    }

    http->version = (http_version_t)(major * 100 + minor);
    *status       = http->status = (http_status_t)intstatus;
  }
  else if ((value = strchr(line, ':')) != NULL)
  {
    *value++ = '\0';
    while (isspace(*value))
      value ++;

	if (!strcaseinsensitivecmp(line, "expect"))
    {
      http->expect = (http_status_t)atoi(value);
    }
    else if (!strcaseinsensitivecmp(line, "cookie"))
    {
      httpSetCookie(http, value);
    }
    else if ((field = http_field(line)) != HTTP_FIELD_UNKNOWN)
      httpSetField(http, field, value);
#ifdef DEBUG
    else
      DEBUG_printf(("1_httpUpdate: unknown field %s seen!", line));
#endif /* DEBUG */
  }
  else
  {
    *status = http->status = HTTP_ERROR;
    return (0);
  }

  return (1);
}

http_status_t httpUpdate(http_t *http)
{
  http_status_t	status;			/* Request status */

  if (http->wused)
  {
    if (httpFlushWrite(http) < 0)
      return (HTTP_ERROR);
  }

  if (http->state == HTTP_WAITING)
    return (HTTP_CONTINUE);

  while (_httpUpdate(http, &status));

 /*
  * See if there was an error...
  */

  if (http->error == EPIPE && http->status > HTTP_CONTINUE)
  {
    return (http->status);
  }

  if (http->error)
  {
    http->status = HTTP_ERROR;
    return (HTTP_ERROR);
  }

 /*
  * Return the current status...
  */

  return (status);
}

int					/* O - 1 if data is available, 0 otherwise */
_httpWait(http_t *http,			/* I - Connection to server */
          int    msec,			/* I - Milliseconds to wait */
	  int    usessl)		/* I - Use SSL context? */
{
  fd_set		input_set;	/* select() input set */
  struct timeval	timeout;	/* Timeout */
  int			nfds;		/* Result from select()/poll() */

  if (http->fd < 0)
  {
    return (0);
  }

  do
  {
    FD_ZERO(&input_set);
    FD_SET(http->fd, &input_set);

    if (msec >= 0)
    {
      timeout.tv_sec  = msec / 1000;
      timeout.tv_usec = (msec % 1000) * 1000;

      nfds = select(http->fd + 1, &input_set, NULL, NULL, &timeout);
    }
    else
      nfds = select(http->fd + 1, &input_set, NULL, NULL, NULL);
  }
  while (nfds < 0 && (WSAGetLastError() == WSAEINTR || WSAGetLastError() == WSAEWOULDBLOCK));

  return (nfds > 0);
}

int	httpWait(http_t *http,int    msec)
{
  if (http == NULL)
    return (0);

  if (http->used)
  {
    return (1);
  }

  if (http->wused)
  {
    if (httpFlushWrite(http) < 0)
      return (0);
  }

  return (_httpWait(http, msec, 1));
}

ssize_t	 httpWrite2(http_t *http, const char *buffer, size_t length)
{
  ssize_t	bytes;

  if (http == NULL || buffer == NULL)
    return (-1);

  http->activity = time(NULL);

  if (length > 0)
  {
    if (http->wused && (length + http->wused) > sizeof(http->wbuffer))
    {
      httpFlushWrite(http);
    }

    if ((length + http->wused) <= sizeof(http->wbuffer) &&
        length < sizeof(http->wbuffer))
    {
      memcpy(http->wbuffer + http->wused, buffer, length);
      http->wused += (int)length;
      bytes = (ssize_t)length;
    }
    else
    {
      if (http->data_encoding == HTTP_ENCODE_CHUNKED)
		bytes = (ssize_t)http_write_chunk(http, buffer, (int)length);
      else
		bytes = (ssize_t)http_write(http, buffer, (int)length);
    }

    if (http->data_encoding == HTTP_ENCODE_LENGTH)
      http->data_remaining -= bytes;
  }
  else
    bytes = 0;

  if ((http->data_encoding == HTTP_ENCODE_CHUNKED && length == 0) ||
      (http->data_encoding == HTTP_ENCODE_LENGTH && http->data_remaining == 0))
  {
    if (http->wused)
      httpFlushWrite(http);

    if (http->data_encoding == HTTP_ENCODE_CHUNKED)
    {
      http_write(http, "0\r\n\r\n", 5);
      http->data_encoding  = HTTP_ENCODE_LENGTH;
      http->data_remaining = 0;
    }
  }

  return (bytes);
}

static int				/* O - Number of bytes written */
http_write(http_t     *http,		/* I - Connection to server */
           const char *buffer,		/* I - Buffer for data */
	   int        length)		/* I - Number of bytes to write */
{
  int	tbytes,				/* Total bytes sent */
	bytes;				/* Bytes sent */

  http->error = 0;
  tbytes      = 0;

  while (length > 0)
  {
    bytes = send(http->fd, buffer, length, 0);

    if (bytes < 0)
    {
#ifdef WIN32
      if (WSAGetLastError() == WSAEINTR)
        continue;
      else if (WSAGetLastError() == WSAEWOULDBLOCK)
      {
        if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
          continue;

        http->error = WSAGetLastError();
      }
      else if (WSAGetLastError() != http->error &&
               WSAGetLastError() != WSAECONNRESET)
      {
        http->error = WSAGetLastError();
	continue;
      }
#endif /* WIN32 */

      return (-1);
    }

    buffer += bytes;
    tbytes += bytes;
    length -= bytes;
  }

#ifdef DEBUG
  http_debug_hex("http_write", buffer - tbytes, tbytes);
#endif /* DEBUG */

  return (tbytes);
}

static int				/* O - Number bytes written */
http_write_chunk(http_t     *http,	/* I - Connection to server */
                 const char *buffer,	/* I - Buffer to write */
		 int        length)	/* I - Length of buffer */
{
  char	header[255];			/* Chunk header */
  int	bytes;				/* Bytes written */

  sprintf(header, "%x\r\n", length);
  if (http_write(http, header, (int)strlen(header)) < 0)
  {
    return (-1);
  }

  if ((bytes = http_write(http, buffer, length)) < 0)
  {
    return (-1);
  }

  if (http_write(http, "\r\n", 2) < 0)
  {
    return (-1);
  }

  return (bytes);
}


off_t httpGetLength2(http_t *http)
{

  if (!http)
    return (-1);

  if (!strcaseinsensitivecmp(http->fields[HTTP_FIELD_TRANSFER_ENCODING], "chunked"))
  {
    http->data_encoding  = HTTP_ENCODE_CHUNKED;
    http->data_remaining = 0;
  }
  else
  {
    http->data_encoding = HTTP_ENCODE_LENGTH;

    if (!http->fields[HTTP_FIELD_CONTENT_LENGTH][0])
    {
      if (http->status >= HTTP_MULTIPLE_CHOICES)
        http->data_remaining = 0;
      else
        http->data_remaining = 2147483647;
    }
    else
      http->data_remaining = strtoll(http->fields[HTTP_FIELD_CONTENT_LENGTH],
                                     NULL, 10);
  }

  if (http->data_remaining <= INT_MAX)
    http->_data_remaining = (int)http->data_remaining;
  else
    http->_data_remaining = INT_MAX;

  return (http->data_remaining);
}

static http_field_t			/* O - Field index */
http_field(const char *name)		/* I - String name */
{
  int	i;				/* Looping var */


  for (i = 0; i < HTTP_FIELD_MAX; i ++)
    if (strcaseinsensitivecmp(name, http_fields[i]) == 0)
      return ((http_field_t)i);

  return (HTTP_FIELD_UNKNOWN);
}

ssize_t					/* O - Number of bytes copied */
_httpPeek(http_t *http,			/* I - Connection to server */
          char   *buffer,		/* I - Buffer for data */
	  size_t length)		/* I - Maximum number of bytes */
{
  ssize_t	bytes;			/* Bytes read */
  char		len[32];		/* Length string */

  if (http == NULL || buffer == NULL)
    return (-1);

  http->activity = time(NULL);
  http->error    = 0;

  if (length <= 0)
    return (0);

  if (http->data_encoding == HTTP_ENCODE_CHUNKED &&
      http->data_remaining <= 0)
  {
    if (httpGets(len, sizeof(len), http) == NULL)
    {
      return (0);
    }

    http->data_remaining = strtoll(len, NULL, 16);
    if (http->data_remaining < 0)
    {
      return (0);
    }
  }

  if (http->data_remaining <= 0)
  {
    if (http->data_encoding == HTTP_ENCODE_CHUNKED)
      httpGets(len, sizeof(len), http);

    if (http->state == HTTP_POST_RECV)
      http->state = (http_state_t)(http->state+1);
    else
      http->state = HTTP_WAITING;

    http->data_encoding = HTTP_ENCODE_LENGTH;

    return (0);
  }
  else if (length > (size_t)http->data_remaining)
    length = (size_t)http->data_remaining;

  if (http->used == 0)
  {
    if (!http->blocking)
    {
      while (!httpWait(http, 10000))
      {
	if (http->timeout_cb && (*http->timeout_cb)(http, http->timeout_data))
	  continue;

	return (0);
      }
    }

    if (http->data_remaining > sizeof(http->buffer))
      bytes = sizeof(http->buffer);
    else
      bytes = http->data_remaining;

    bytes = recv(http->fd, http->buffer, bytes, 0);

    if (bytes > 0)
      http->used = bytes;
    else if (bytes < 0)
    {
      if (WSAGetLastError() != WSAEINTR && WSAGetLastError() != WSAEWOULDBLOCK)
      {
        http->error = WSAGetLastError();
        return (-1);
      }
    }
    else
    {
      http->error = EPIPE;
      return (0);
    }
  }

  if (http->used > 0)
  {
    if (length > (size_t)http->used)
      length = (size_t)http->used;

    bytes = (ssize_t)length;

    memcpy(buffer, http->buffer, length);
  }
  else
    bytes = 0;

  if (bytes < 0)
  {
#ifdef WIN32
    if (WSAGetLastError() == WSAEINTR || WSAGetLastError() == WSAEWOULDBLOCK)
      bytes = 0;
    else
      http->error = WSAGetLastError();
#else
    if (errno == EINTR || errno == EAGAIN)
      bytes = 0;
    else
      http->error = errno;
#endif /* WIN32 */
  }
  else if (bytes == 0)
  {
    http->error = EPIPE;
    return (0);
  }

#ifdef DEBUG
  http_debug_hex("_httpPeek", buffer, (int)bytes);
#endif /* DEBUG */

  return (bytes);
}

void
httpClose(http_t *http)
{
  if (!http)
    return;

  _httpDisconnect(http);

  if (http->cookie)
    free(http->cookie);

  httpClearFields(http);

  if (http->authstring && http->authstring != http->_authstring)
    free(http->authstring);

  free(http);
}

void
_httpDisconnect(http_t *http)
{
  closesocket(http->fd);

  http->fd = -1;
}

#ifndef _HTTP_H_
#define _HTTP_H_

#include "array.h"
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef off_t ssize_t;

#  define HTTP_MAX_URI		1024	
#  define HTTP_MAX_HOST		256	
#  define HTTP_MAX_BUFFER	2048	
#  define HTTP_MAX_VALUE	256	

typedef enum http_encoding_e		
{
  HTTP_ENCODE_LENGTH,			
  HTTP_ENCODE_CHUNKED,			
  HTTP_ENCODE_FIELDS			
} http_encoding_t;

typedef enum http_field_e		
{
  HTTP_FIELD_UNKNOWN = -1,		
  HTTP_FIELD_ACCEPT_LANGUAGE,		
  HTTP_FIELD_ACCEPT_RANGES,		
  HTTP_FIELD_AUTHORIZATION,		
  HTTP_FIELD_CONNECTION,		
  HTTP_FIELD_CONTENT_ENCODING,		
  HTTP_FIELD_CONTENT_LANGUAGE,		
  HTTP_FIELD_CONTENT_LENGTH,		
  HTTP_FIELD_CONTENT_LOCATION,		
  HTTP_FIELD_CONTENT_MD5,		
  HTTP_FIELD_CONTENT_RANGE,		
  HTTP_FIELD_CONTENT_TYPE,		
  HTTP_FIELD_CONTENT_VERSION,		
  HTTP_FIELD_DATE,			
  HTTP_FIELD_HOST,			
  HTTP_FIELD_IF_MODIFIED_SINCE,		
  HTTP_FIELD_IF_UNMODIFIED_SINCE,	
  HTTP_FIELD_KEEP_ALIVE,		
  HTTP_FIELD_LAST_MODIFIED,		
  HTTP_FIELD_LINK,			
  HTTP_FIELD_LOCATION,			
  HTTP_FIELD_RANGE,			
  HTTP_FIELD_REFERER,			
  HTTP_FIELD_RETRY_AFTER,		
  HTTP_FIELD_TRANSFER_ENCODING,		
  HTTP_FIELD_UPGRADE,			
  HTTP_FIELD_USER_AGENT,		
  HTTP_FIELD_WWW_AUTHENTICATE,		
  HTTP_FIELD_MAX			
} http_field_t;

typedef enum http_keepalive_e		
{
  HTTP_KEEPALIVE_OFF = 0,		
  HTTP_KEEPALIVE_ON			
} http_keepalive_t;

typedef enum http_state_e
{
  HTTP_WAITING,				
  HTTP_OPTIONS,				
  HTTP_GET,				
  HTTP_GET_SEND,			
  HTTP_HEAD,				
  HTTP_POST,				
  HTTP_POST_RECV,			
  HTTP_POST_SEND,			
  HTTP_PUT,				
  HTTP_PUT_RECV,			
  HTTP_DELETE,				
  HTTP_TRACE,				
  HTTP_CLOSE,				
  HTTP_STATUS				
} http_state_t;

typedef enum http_status_e		
{
  HTTP_ERROR = -1,			

  HTTP_CONTINUE = 100,			
  HTTP_SWITCHING_PROTOCOLS,		

  HTTP_OK = 200,			
  HTTP_CREATED,				
  HTTP_ACCEPTED,			
  HTTP_NOT_AUTHORITATIVE,		
  HTTP_NO_CONTENT,			
  HTTP_RESET_CONTENT,			
  HTTP_PARTIAL_CONTENT,			

  HTTP_MULTIPLE_CHOICES = 300,		
  HTTP_MOVED_PERMANENTLY,		
  HTTP_MOVED_TEMPORARILY,		
  HTTP_SEE_OTHER,			
  HTTP_NOT_MODIFIED,			
  HTTP_USE_PROXY,			

  HTTP_BAD_REQUEST = 400,		
  HTTP_UNAUTHORIZED,			
  HTTP_PAYMENT_REQUIRED,		
  HTTP_FORBIDDEN,			
  HTTP_NOT_FOUND,			
  HTTP_METHOD_NOT_ALLOWED,		
  HTTP_NOT_ACCEPTABLE,			
  HTTP_PROXY_AUTHENTICATION,		
  HTTP_REQUEST_TIMEOUT,			
  HTTP_CONFLICT,			
  HTTP_GONE,				
  HTTP_LENGTH_REQUIRED,			
  HTTP_PRECONDITION,			
  HTTP_REQUEST_TOO_LARGE,		
  HTTP_URI_TOO_LONG,			
  HTTP_UNSUPPORTED_MEDIATYPE,		
  HTTP_REQUESTED_RANGE,			
  HTTP_EXPECTATION_FAILED,		
  HTTP_UPGRADE_REQUIRED = 426,		

  HTTP_SERVER_ERROR = 500,		
  HTTP_NOT_IMPLEMENTED,			
  HTTP_BAD_GATEWAY,			
  HTTP_SERVICE_UNAVAILABLE,		
  HTTP_GATEWAY_TIMEOUT,			
  HTTP_NOT_SUPPORTED,			

  HTTP_AUTHORIZATION_CANCELED = 1000,	
  HTTP_PKI_ERROR,			
  HTTP_WEBIF_DISABLED			
} http_status_t;

typedef enum http_uri_status_e		
{
  HTTP_URI_OVERFLOW = -8,		
  HTTP_URI_BAD_ARGUMENTS = -7,		
  HTTP_URI_BAD_RESOURCE = -6,		
  HTTP_URI_BAD_PORT = -5,		
  HTTP_URI_BAD_HOSTNAME = -4,		
  HTTP_URI_BAD_USERNAME = -3,		
  HTTP_URI_BAD_SCHEME = -2,		
  HTTP_URI_BAD_URI = -1,		
  HTTP_URI_OK = 0,			
  HTTP_URI_MISSING_SCHEME,		
  HTTP_URI_UNKNOWN_SCHEME,		
  HTTP_URI_MISSING_RESOURCE		
} http_uri_status_t;

typedef enum http_uri_coding_e		
{
  HTTP_URI_CODING_NONE = 0,		
  HTTP_URI_CODING_USERNAME = 1,		
  HTTP_URI_CODING_HOSTNAME = 2,		
  HTTP_URI_CODING_RESOURCE = 4,		
  HTTP_URI_CODING_MOST = 7,		
  HTTP_URI_CODING_QUERY = 8,		
  HTTP_URI_CODING_ALL = 15		
} http_uri_coding_t;

typedef enum http_version_e		
{
  HTTP_0_9 = 9,				
  HTTP_1_0 = 100,			
  HTTP_1_1 = 101			
} http_version_t;

typedef union _http_addr_u
{
  struct sockaddr	addr;		
  struct sockaddr_in	ipv4;		
#ifdef AF_INET6
  struct sockaddr_in6	ipv6;		
#endif 
  char			pad[256];	
} http_addr_t;

typedef struct _http_s http_t;		

typedef int (*http_timeout_cb_t)(http_t *http, void *user_data);

extern ssize_t		httpWrite2(http_t *http, const char *buffer,size_t length);
extern const char	*httpGetHostname(http_t *http, char *s, int slen);
extern char		*httpAddrString(const http_addr_t *addr,char *s, int slen);
extern int		httpAddrLength(const http_addr_t *addr);
extern http_uri_status_t httpAssembleURI(http_uri_coding_t encoding,
			         char *uri, int urilen,
			         const char *scheme,
					 const char *username,
					 const char *host, int port,
					 const char *resource);
extern int	httpFlushWrite(http_t *http);
extern void		httpClearCookie(http_t *http);
extern void		httpClearFields(http_t *http);
extern void		httpFlush(http_t *http);
extern ssize_t	httpRead2(http_t *http, char *buffer, size_t length);
extern int		httpWait(http_t *http, int msec);
extern http_status_t	httpGetStatus(http_t *http);
extern http_uri_status_t httpSeparateURI(http_uri_coding_t decoding,
			                 const char *uri,
			        	 char *scheme, int schemelen,
			        	 char *username, int usernamelen,
					 char *host, int hostlen, int *port,
					 char *resource, int resourcelen);
extern http_status_t	httpUpdate(http_t *http);
extern int		httpPrintf(http_t *http, const char *format, ...);
extern const char	*httpStatus(http_status_t status);
extern http_status_t	httpGetStatus(http_t *http);
extern void		httpSetField(http_t *http, http_field_t field,
			             const char *value);
extern off_t		httpGetLength2(http_t *http);
extern http_status_t	httpGetStatus(http_t *http);
extern void httpGetDateString(time_t t, char *s, int slen);
#endif

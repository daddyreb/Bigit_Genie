#include "common.h"
static const char * const http_days[7] =
{
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};
static const char * const http_months[12] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};
static const char	*http_copy_decode(char *dst, const char *src,
									  int dstsize, const char *term,
									  int decode);
static char	*http_copy_encode(char *dst, const char *src,
							  char *dstend, const char *reserved,
							  const char *term, int encode);

http_uri_status_t httpSeparateURI(
								  http_uri_coding_t decoding,	 
								  const char        *uri, 
								  char              *scheme, 
								  int               schemelen, 
								  char              *username, 
								  int               usernamelen,
								  char              *host,
								  int               hostlen,	
								  int               *port,
								  char              *resource,
								  int               resourcelen)	
{
	char			*ptr,*end;
	const char		*sep;
	http_uri_status_t	status;

	if (scheme && schemelen > 0)
		*scheme = '\0';

	if (username && usernamelen > 0)
		*username = '\0';

	if (host && hostlen > 0)
		*host = '\0';

	if (port)
		*port = 0;

	if (resource && resourcelen > 0)
		*resource = '\0';

	if (!uri || !port || !scheme || schemelen <= 0 || !username ||
		usernamelen <= 0 || !host || hostlen <= 0 || !resource ||
		resourcelen <= 0)
		return (HTTP_URI_BAD_ARGUMENTS);

	if (!*uri)
		return (HTTP_URI_BAD_URI);

	status = HTTP_URI_OK;

	if (!strncmp(uri, "//", 2))
	{
		strncpy(scheme, "ipp", schemelen);
		status = HTTP_URI_MISSING_SCHEME;
	}
	else if (*uri == '/')
	{
		strncpy(scheme, "file", schemelen);
		status = HTTP_URI_MISSING_SCHEME;
	}
	else
	{
		for (ptr = scheme, end = scheme + schemelen - 1;
			*uri && *uri != ':' && ptr < end;)
			if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789-+.", *uri) != NULL)
				*ptr++ = *uri++;
			else
				break;

		*ptr = '\0';

		if (*uri != ':')
		{
			*scheme = '\0';
			return (HTTP_URI_BAD_SCHEME);
		}

		uri ++;
	}

	if (!strcmp(scheme, "http"))
		*port = 80;
	else if (!strcmp(scheme, "https"))
		*port = 443;
	else if (!strcmp(scheme, "ipp") || !strcmp(scheme, "ipps"))
		*port = 631;
	else if (!strncaseinsensitivecmp(scheme, "lpd",sizeof("lpd")))
		*port = 515;
	else if (!strcmp(scheme, "socket"))	
		*port = 9100;
	else if (strcmp(scheme, "file") && strcmp(scheme, "mailto"))
		status = HTTP_URI_UNKNOWN_SCHEME;

	if (!strncmp(uri, "//", 2))
	{
		uri += 2;

		if ((sep = strpbrk(uri, "@/")) != NULL && *sep == '@')
		{
			uri = http_copy_decode(username, uri, usernamelen, "@",
				decoding & HTTP_URI_CODING_USERNAME);

			if (!uri)
			{
				*username = '\0';
				return (HTTP_URI_BAD_USERNAME);
			}

			uri ++;
		}

		if (*uri == '[')
		{
			uri ++;
			if (!strncmp(uri, "v1.", 3))
				uri += 3;

			uri = http_copy_decode(host, uri, hostlen, "]",
				decoding & HTTP_URI_CODING_HOSTNAME);

			if (!uri)
			{
				*host = '\0';
				return (HTTP_URI_BAD_HOSTNAME);
			}

			if (*uri != ']')
			{
				*host = '\0';
				return (HTTP_URI_BAD_HOSTNAME);
			}

			uri ++;

			for (ptr = host; *ptr; ptr ++)
				if (*ptr == '+')
				{
					*ptr = '%';
					break;
				}
				else if (*ptr != ':' && *ptr != '.' && !isxdigit(*ptr & 255))
				{
					*host = '\0';
					return (HTTP_URI_BAD_HOSTNAME);
				}
		}
		else
		{
			for (ptr = (char *)uri; *ptr; ptr ++)
				if (strchr(":?/", *ptr))
					break;
				else if (!strchr("abcdefghijklmnopqrstuvwxyz"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"0123456789"
					"-._~"
					"%"
					"!$&'()*+,;=\\", *ptr))
				{
					*host = '\0';
					return (HTTP_URI_BAD_HOSTNAME);
				}

				uri = http_copy_decode(host, uri, hostlen, ":?/",
					decoding & HTTP_URI_CODING_HOSTNAME);

				if (!uri)
				{
					*host = '\0';
					return (HTTP_URI_BAD_HOSTNAME);
				}
		}

		if (!strcmp(scheme, "file") && strcmp(host, "localhost") && host[0])
		{
			*host = '\0';
			return (HTTP_URI_BAD_HOSTNAME);
		}
		if (*uri == ':')
		{
			if (!isdigit(uri[1] & 255))
			{
				*port = 0;
				return (HTTP_URI_BAD_PORT);
			}

			*port = strtol(uri + 1, (char **)&uri, 10);

			if (*uri != '/' && *uri)
			{
				*port = 0;
				return (HTTP_URI_BAD_PORT);
			}
		}
	}

	if (*uri == '?' || !*uri)
	{
		status    = HTTP_URI_MISSING_RESOURCE;
		*resource = '/';

		if (*uri == '?')
			uri = http_copy_decode(resource + 1, uri, resourcelen - 1, NULL,
			decoding & HTTP_URI_CODING_QUERY);
		else
			resource[1] = '\0';
	}
	else
	{
		uri = http_copy_decode(resource, uri, resourcelen, "?",
			decoding & HTTP_URI_CODING_RESOURCE);

		if (uri && *uri == '?')
		{
			char *resptr = resource + strlen(resource);

			uri = http_copy_decode(resptr, uri, resourcelen - (int)(resptr - resource),
				NULL, decoding & HTTP_URI_CODING_QUERY);
		}
	}

	if (!uri)
	{
		*resource = '\0';
		return (HTTP_URI_BAD_RESOURCE);
	}

	return (status);
}

char * _httpDecodeURI(char  *dst,const char *src,size_t dstsize)
{
	if (http_copy_decode(dst, src, (int)dstsize, NULL, 1))
		return (dst);
	else
		return (NULL);
}

static const char * http_copy_decode(
									 char       *dst,	
									 const char *src,	
									 int        dstsize,	
									 const char *term,	
									 int        decode)	
{
	char	*ptr,				
		*end;				
	int	quoted;				

	for (ptr = dst, end = dst + dstsize - 1;
		*src && (!term || !strchr(term, *src));
		src ++)
		if (ptr < end)
		{
			if (*src == '%' && decode)
			{
				if (isxdigit(src[1] & 255) && isxdigit(src[2] & 255))
				{
					src ++;
					if (isalpha(*src))
						quoted = (tolower(*src) - 'a' + 10) << 4;
					else
						quoted = (*src - '0') << 4;

					src ++;
					if (isalpha(*src))
						quoted |= tolower(*src) - 'a' + 10;
					else
						quoted |= *src - '0';

					*ptr++ = quoted;
				}
				else
				{
					*ptr = '\0';
					return (NULL);
				}
			}
			else
				*ptr++ = *src;
		}

		*ptr = '\0';

		return (src);
}

static char *				
http_copy_encode(char       *dst,	
				 const char *src,	
				 char       *dstend,	
				 const char *reserved,	
				 const char *term,	
				 int        encode)	
{
	static const char hex[] = "0123456789ABCDEF";


	while (*src && dst < dstend)
	{
		if (term && *src == *term)
			return (dst);

		if (encode && (*src == '%' || *src <= ' ' || *src & 128 ||
			(reserved && strchr(reserved, *src))))
		{
			if ((dst + 2) >= dstend)
				break;

			*dst++ = '%';
			*dst++ = hex[(*src >> 4) & 15];
			*dst++ = hex[*src & 15];

			src ++;
		}
		else
			*dst++ = *src++;
	}

	*dst = '\0';

	if (*src)
		return (NULL);
	else
		return (dst);
}


const char * httpStatus(http_status_t status)
{
	const char	*s;

	switch (status)
	{
	case HTTP_CONTINUE :
		s = "Continue";
		break;
	case HTTP_SWITCHING_PROTOCOLS :
		s = "Switching Protocols";
		break;
	case HTTP_OK :
		s = "OK";
		break;
	case HTTP_CREATED :
		s = "Created";
		break;
	case HTTP_ACCEPTED :
		s = "Accepted";
		break;
	case HTTP_NO_CONTENT :
		s = "No Content";
		break;
	case HTTP_MOVED_PERMANENTLY :
		s = "Moved Permanently";
		break;
	case HTTP_SEE_OTHER :
		s = "See Other";
		break;
	case HTTP_NOT_MODIFIED :
		s = "Not Modified";
		break;
	case HTTP_BAD_REQUEST :
		s = "Bad Request";
		break;
	case HTTP_UNAUTHORIZED :
	case HTTP_AUTHORIZATION_CANCELED :
		s = "Unauthorized";
		break;
	case HTTP_FORBIDDEN :
		s = "Forbidden";
		break;
	case HTTP_NOT_FOUND :
		s = "Not Found";
		break;
	case HTTP_REQUEST_TOO_LARGE :
		s = "Request Entity Too Large";
		break;
	case HTTP_URI_TOO_LONG :
		s = "URI Too Long";
		break;
	case HTTP_UPGRADE_REQUIRED :
		s = "Upgrade Required";
		break;
	case HTTP_NOT_IMPLEMENTED :
		s = "Not Implemented";
		break;
	case HTTP_NOT_SUPPORTED :
		s = "Not Supported";
		break;
	case HTTP_EXPECTATION_FAILED :
		s = "Expectation Failed";
		break;
	case HTTP_SERVICE_UNAVAILABLE :
		s = "Service Unavailable";
		break;
	case HTTP_SERVER_ERROR :
		s = "Internal Server Error";
		break;
	case HTTP_PKI_ERROR :
		s = "SSL/TLS Negotiation Error";
		break;
	case HTTP_WEBIF_DISABLED :
		s = "Web Interface is Disabled";
		break;

	default :
		s = "Unknown";
		break;
	}

	return s;
}

http_uri_status_t httpAssembleURI(
								  http_uri_coding_t encoding,		
								  char              *uri,		
								  int               urilen,		
								  const char        *scheme,		
								  const char        *username,	
								  const char        *host,		
								  int               port,		
								  const char        *resource)	
{
	char		*ptr,			
		*end;			

	if (!uri || urilen < 1 || !scheme || port < 0)
	{
		if (uri)
			*uri = '\0';

		return (HTTP_URI_BAD_ARGUMENTS);
	}

	end = uri + urilen - 1;
	ptr = http_copy_encode(uri, scheme, end, NULL, NULL, 0);

	if (!ptr)
		goto assemble_overflow;

	if (!strcmp(scheme, "mailto"))
	{
		if (ptr < end)
			*ptr++ = ':';
		else
			goto assemble_overflow;
	}
	else
	{
		if ((ptr + 2) < end)
		{
			*ptr++ = ':';
			*ptr++ = '/';
			*ptr++ = '/';
		}
		else
			goto assemble_overflow;
	}
	if (host)
	{
		if (username && *username)
		{
			ptr = http_copy_encode(ptr, username, end, "/?@", NULL,
				encoding & HTTP_URI_CODING_USERNAME);

			if (!ptr)
				goto assemble_overflow;

			if (ptr < end)
				*ptr++ = '@';
			else
				goto assemble_overflow;
		}

		if (host[0] != '[' && strchr(host, ':') && !strstr(host, "._tcp"))
		{
			if (strchr(host, '%'))
			{
				if ((ptr + 4) < end)
				{
					*ptr++ = '[';
					*ptr++ = 'v';
					*ptr++ = '1';
					*ptr++ = '.';
				}
				else
					goto assemble_overflow;
			}
			else
			{
				if (ptr < end)
					*ptr++ = '[';
				else
					goto assemble_overflow;
			}
			while (ptr < end && *host)
			{
				if (*host == '%')
				{
					*ptr++ = '+';			
					host ++;
				}
				else
					*ptr++ = *host++;
			}

			if (*host)
				goto assemble_overflow;

			if (ptr < end)
				*ptr++ = ']';
			else
				goto assemble_overflow;
		}
		else
		{
			ptr = http_copy_encode(ptr, host, end, ":/?#[]@\\\"", NULL,
				encoding & HTTP_URI_CODING_HOSTNAME);

			if (!ptr)
				goto assemble_overflow;
		}

		if (port > 0)
		{
			snprintf(ptr, end - ptr + 1, ":%d", port);
			ptr += strlen(ptr);

			if (ptr >= end)
				goto assemble_overflow;
		}
	}

	if (resource)
	{
		char	*query;	

		query = strchr(resource, '?');
		ptr   = http_copy_encode(ptr, resource, end, NULL, "?",
			encoding & HTTP_URI_CODING_RESOURCE);
		if (!ptr)
			goto assemble_overflow;

		if (query)
		{
			ptr = http_copy_encode(ptr, query, end, NULL, NULL,
				encoding & HTTP_URI_CODING_QUERY);
			if (!ptr)
				goto assemble_overflow;
		}
	}
	else if (ptr < end)
		*ptr++ = '/';
	else
		goto assemble_overflow;

	*ptr = '\0';

	return (HTTP_URI_OK);

assemble_overflow:

	*uri = '\0';
	return (HTTP_URI_OVERFLOW);
}

void httpGetDateString(time_t t,char *s,int slen)
{
	struct tm	*tdate;
	tdate = gmtime(&t);
	snprintf(s, slen, "%s, %02d %s %d %02d:%02d:%02d GMT",
		http_days[tdate->tm_wday], tdate->tm_mday,
		http_months[tdate->tm_mon], tdate->tm_year + 1900,
		tdate->tm_hour, tdate->tm_min, tdate->tm_sec);
	return (s);
}

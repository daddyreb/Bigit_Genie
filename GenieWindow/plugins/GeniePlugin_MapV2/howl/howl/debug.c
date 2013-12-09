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

#include <salt/debug.h>
#include <stdio.h>

#if defined(WIN32)
#	define snprintf _snprintf
#endif


static sw_int8		g_component[64] = { "howl" };
static int			g_debug_level = SW_LOG_NOTICE;

static sw_string sw_format_error_string(int, char *, size_t);

#if !defined(NDEBUG)

void
sw_print_assert(
		int					code,
		sw_const_string	assert_string,
		sw_const_string	file,
		sw_const_string	func,
		int					line)
{
	char message[1024];
	char string[1024];

	if (code)
	{
		snprintf(message, sizeof(message), "[assert] error: %d %s\n[assert] where: \"%s\", \"%s\", line: %d\n\n", code, sw_format_error_string(code, string, sizeof(string)), file, func, line);
	}
	else
	{
		snprintf(message, sizeof(message), "[assert] error: %s\n[assert] where: \"%s\", \"%s\", line: %d\n\n", assert_string, file, func, line);
	}

	fprintf(stderr, message);

#if defined(WIN32)
//	OutputDebugString(message);
#endif
}

#endif


void
sw_debug_init(
		int					level,
		sw_const_string	component)
{
	SW_UNUSED_PARAM(component);

	g_debug_level = level;
}


void
sw_debug_set_level(
		int					level)
{
	g_debug_level = level;
}


void
sw_print_debug(
		int					level,
		sw_const_string	format,
		...)
{
	if (level <= g_debug_level)
	{
		char		buffer1[1024];
		char		buffer2[1024];
		va_list	args;

		va_start(args, format);
	
		vsprintf(buffer1, format, args);
	
		va_end(args);

		if (buffer1[strlen(buffer1) - 1] == '\n')
		{
			buffer1[strlen(buffer1) - 1] = '\0';
		}

#if defined(WIN32)

		sprintf(buffer2, "[%s] %s (%d)\n", g_component, buffer1, GetCurrentThreadId());

#else

		sprintf(buffer2, "[%s] %s (%d)\n", g_component, buffer1, getpid());

#endif
	
		fprintf(stderr, buffer2);

#if defined(WIN32)

		//OutputDebugString(buffer2);

#endif
	}
}


static char*
sw_format_error_string(int code, char * string, size_t slen)
{
	char	temp[128];
	int	res;

	if (code == 0)
	{
		snprintf(string, slen, "(no error)");
		return string;
	}
	if (code & 0x80000000)
	{
		snprintf(string, slen, "(howl error)");
		return string;
	}
#if defined(WIN32)

	res = FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD) code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), temp, sizeof(temp), NULL );

	// Remove any trailing CR's or LF's since some messages have them.
	while((res > 0) && isspace(((unsigned char *) temp)[res - 1]))
		temp[ --res ] = '\0';
#else
	snprintf(temp, sizeof(temp), "%s", strerror(code));
#endif
	if (strlen(temp) > 0)
		snprintf(string, slen, "(%s)", temp);
	else
		snprintf(string, slen, "(unknown error)");

	return string;
}

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

#include "salt_i.h"
#include <salt/debug.h>
#include <stdio.h>


#if !defined(NDEBUG)

#	define MAX_MEMORY_BLOCKS 4192
#	define DEBUG_MEMORY

struct _sw_memory_node
{
	sw_opaque	m_mem;
	sw_size_t	m_size;
	sw_int8		m_function[128];
	sw_int8		m_file[128];
	sw_uint32	m_line;
};

static struct _sw_memory_node g_mem_nodes[MAX_MEMORY_BLOCKS] = { 0 };


static void
sw_memory_alloc(
			sw_opaque		mem,
			sw_size_t		size,
			const char	*	function,
			const char	*	file,
			sw_uint32		line);


static void
sw_memory_free(
			sw_opaque	mem);


void
sw_debug_memory_inuse()
{
	sw_size_t total;
	int		i;

	fprintf(stderr, "memory in-use\n{\n");
	for (i = 0, total = 0; i < 4192; i++)
	{
		if (g_mem_nodes[i].m_mem != NULL)
		{
			fprintf(stderr, "   block 0x%x: size = %d: owner = %s,%d\n", (sw_uint32) g_mem_nodes[i].m_mem, g_mem_nodes[i].m_size, g_mem_nodes[i].m_file, g_mem_nodes[i].m_line);
			total += g_mem_nodes[i].m_size;
		}
	}

	fprintf(stderr, "\n   total = %d\n}\n", total);
}


sw_opaque
_sw_debug_malloc(
			sw_size_t			size,
			sw_const_string	function,
			sw_const_string	file,
			sw_uint32			line)
{
	sw_opaque ret;
	
	ret = (sw_opaque) malloc(size);

#if defined(DEBUG_MEMORY)
	sw_memory_alloc(ret, size, function, file, line);
#endif

	return ret;
}


sw_opaque
_sw_debug_realloc(
			sw_opaque_t			mem,
			sw_size_t			size,
			sw_const_string	function,
			sw_const_string	file,
			sw_uint32			line)
{
	sw_opaque ret;

#if defined(DEBUG_MEMORY)
	sw_memory_free(mem);
#endif

	ret = (sw_opaque) realloc(mem, size);

#if defined(DEBUG_MEMORY)
	sw_memory_alloc(ret, size, function, file, line);
#endif

	return ret;
}


void
_sw_debug_free(
			sw_opaque_t			mem,
			sw_const_string	function,
			sw_const_string	file,
			sw_uint32			line)
{
	SW_UNUSED_PARAM(function);
	SW_UNUSED_PARAM(file);
	SW_UNUSED_PARAM(line);

#if defined(DEBUG_MEMORY)
	sw_memory_free(mem);
#endif

	free(mem);
}


static void
sw_memory_alloc(
			sw_opaque		mem,
			sw_size_t		size,
			const char	*	function,
			const char	*	file,
			sw_uint32		line)
{
	int i = 0;

	while ((g_mem_nodes[i].m_mem != NULL) && (i < MAX_MEMORY_BLOCKS))
	{
		i++;
	}

	if (i < MAX_MEMORY_BLOCKS)
	{
		g_mem_nodes[i].m_mem = mem;
		g_mem_nodes[i].m_size = size;
		sw_strcpy(g_mem_nodes[i].m_function, function);
		sw_strcpy(g_mem_nodes[i].m_file, file);
		g_mem_nodes[i].m_line = line;
	}
	else
	{
		sw_debug(SW_LOG_WARNING, "ran out of space");
	}
}


static void
sw_memory_free(
			sw_opaque	mem)
{
	int i = 0;

	while ((g_mem_nodes[i].m_mem != mem) && (i < MAX_MEMORY_BLOCKS))
	{
		i++;
	}

	if (i < MAX_MEMORY_BLOCKS)
	{
		g_mem_nodes[i].m_mem = NULL;
	}
	else
	{
		sw_debug(SW_LOG_WARNING, "can't find memory block\n");
	}
}

#endif

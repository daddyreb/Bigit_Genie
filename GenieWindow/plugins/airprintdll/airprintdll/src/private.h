#ifndef _PRIVATE_H
#define _PRIVATE_H
#include <ctype.h>
#include "http-private.h"
#include "ipp-private.h"

#define DEBUG_puts(x)
#define DEBUG_printf(x)
#define DEBUG_set(logfile,level,filter)

extern void strfree(const char *s);
extern char * stralloc(const char *s);

#endif //_PRIVATE_H

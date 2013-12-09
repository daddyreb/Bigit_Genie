#ifndef _STRING_PRIVATE_H_
#define _STRING_PRIVATE_H_
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
typedef struct
{
  unsigned int	ref_count;
  char		str[1];
} sp_item_t;
extern int	strcaseinsensitivecmp(const char *, const char *);
extern int	strncaseinsensitivecmp(const char *, const char *, size_t n);
extern char	*stralloc(const char *s);
extern void	strfree(const char *s);
#endif//_STRING_PRIVATE_H_

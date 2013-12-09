#include "IPFileMsg.h"

char *separate_token(char *buf, char separetor, char **handle)
{
	char	*_handle;

	if (handle == NULL)
		handle = &_handle;

	if (buf)
		*handle = buf;

	if (*handle == NULL || **handle == 0)
		return	NULL;

	while (**handle == separetor)
		(*handle)++;
	buf = *handle;

	if (**handle == 0)
		return	NULL;

	while (**handle && **handle != separetor)
		(*handle)++;

	if (**handle == separetor)
		*(*handle)++ = 0;

	return	buf;
}
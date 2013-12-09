#include "private.h"
void _cupsStrFree(const char *s)
{
    if(s)
    {
        delete s;
        s = NULL;
    }
}


char *					/* O - String pointer */
_cupsStrAlloc(const char *s)		/* I - String */
{
    if(!s)
    {
        return(NULL);
    }

    char * str;
    str = (char *)malloc(100);
    memset(str,0,100);
    strncpy(str,s,100);
    return str;
}

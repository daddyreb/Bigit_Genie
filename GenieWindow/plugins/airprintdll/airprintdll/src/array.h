#ifndef _ARRAY_H_
#define _ARRAY_H_
#include <stdlib.h>
typedef struct array_s array_t;
typedef int (*array_func_t)(void *first, void *second, void *data);
typedef int (*ahash_func_t)(void *element, void *data);
typedef void *(*acopy_func_t)(void *element, void *data);
typedef void (*afree_func_t)(void *element, void *data);
extern int		arrayAdd(array_t *a, void *e);
extern int		arrayCount(array_t *a);
extern void		arrayDelete(array_t *a);
extern void		*arrayFind(array_t *a, void *e);
extern void		*arrayFirst(array_t *a);
extern array_t	*arrayNew(array_func_t f, void *d);
extern void		*arrayNext(array_t *a);
extern int		arrayRemove(array_t *a, void *e);
#endif /*_ARRAY_H_ */

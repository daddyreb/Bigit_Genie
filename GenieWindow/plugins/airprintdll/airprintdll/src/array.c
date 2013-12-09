#include <string.h>
#include "string-private.h"
#include "array.h"

#define MAXSAVE	32

struct array_s
{
  int		num_elements,	
			alloc_elements,	
			current,	
			insert,		
			unique,		
			num_saved,	
			saved[MAXSAVE];
					
  void			**elements;	
  array_func_t	compare;	
  void			*data;		
  ahash_func_t	hashfunc;	
  int			hashsize,	
				*hash;		
  acopy_func_t	copyfunc;	
  afree_func_t	freefunc;	
};

static int	array_add(array_t *a, void *e, int insert);
static int	array_find(array_t *a, void *e, int prev, int *rdiff);

int					
arrayAdd(array_t *a,		
             void         *e)		
{
  if (!a || !e)
  {
    return (0);
  }
  return (array_add(a, e, 0));
}

int					
arrayCount(array_t *a)		
{
  if (!a)
    return (0);

  return (a->num_elements);
}

void
arrayDelete(array_t *a)	
{
  if (!a)
    return;

  if (a->freefunc)
  {
    int		i;			
    void	**e;			

    for (i = a->num_elements, e = a->elements; i > 0; i --, e ++)
      (a->freefunc)(*e, a->data);
  }

  if (a->alloc_elements)
    free(a->elements);

  if (a->hashsize)
    free(a->hash);

  free(a);
}

void *					
arrayFind(array_t *a,		
              void         *e)		
{
  int	current,			
	diff,				
	hash;				

  if (!a || !e)
    return (NULL);

  if (!a->num_elements)
    return (NULL);
  
  if (a->hash)
  {
    hash = (*(a->hashfunc))(e, a->data);

    if (hash < 0 || hash >= a->hashsize)
    {
      current = a->current;
      hash    = -1;
    }
    else
    {
      current = a->hash[hash];

      if (current < 0 || current >= a->num_elements)
        current = a->current;
    }
  }
  else
  {
    current = a->current;
    hash    = -1;
  }

  current = array_find(a, e, current, &diff);
  if (!diff)
  {
    if (!a->unique && a->compare)
    {
      while (current > 0 && !(*(a->compare))(e, a->elements[current - 1],
                                             a->data))
        current --;
    }

    a->current = current;

    if (hash >= 0)
      a->hash[hash] = current;

    return (a->elements[current]);
  }
  else
  {
    a->current = -1;

    return (NULL);
  }
}

void * arrayCurrent(array_t *a)
{
  if (!a)
    return (NULL);

  if (a->current >= 0 && a->current < a->num_elements)
    return (a->elements[a->current]);
  else
    return (NULL);
}

void *					
arrayFirst(array_t *a)		
{
  if (!a)
    return (NULL);
  a->current = 0;

  return (arrayCurrent(a));
}

array_t *				
arrayNew(array_func_t f,	
             void              *d)	
{  
  array_t	*a;			

  a = calloc(1, sizeof(array_t));
  if (!a)
    return (NULL);

  a->compare   = f;
  a->data      = d;
  a->current   = -1;
  a->insert    = -1;
  a->num_saved = 0;
  a->unique    = 1;
  a->hashfunc  = NULL;
  a->hashsize  = 0;
  a->hash      = NULL;
  a->copyfunc = NULL;
  a->freefunc = NULL;

  return (a);
}

void *					
arrayNext(array_t *a)		
{
  if (!a)
    return (NULL);

  if (a->current < a->num_elements)
    a->current ++;

  return (arrayCurrent(a));
}

int					
arrayRemove(array_t *a,	
                void         *e)	
{
  int	i,				
	current,			
	diff;				

  if (!a || !e)
    return (0);

  if (!a->num_elements)
    return (0);

  current = array_find(a, e, a->current, &diff);
  if (diff)
    return (0);

  a->num_elements --;

  if (a->freefunc)
    (a->freefunc)(a->elements[current], a->data);

  if (current < a->num_elements)
    memmove(a->elements + current, a->elements + current + 1,
            (a->num_elements - current) * sizeof(void *));

  if (current <= a->current)
    a->current --;

  if (current < a->insert)
    a->insert --;
  else if (current == a->insert)
    a->insert = -1;

  for (i = 0; i < a->num_saved; i ++)
    if (current <= a->saved[i])
      a->saved[i] --;

  if (a->num_elements <= 1)
    a->unique = 1;

  return (1);
}

static int array_add(array_t *a,		
               void         *e,		
	       int          insert)	
{
  int	i,				
	current,			
	diff;				

  if (a->num_elements >= a->alloc_elements)
  {
    void	**temp;			
    int		count;			


    if (a->alloc_elements == 0)
    {
      count = 16;
      temp  = malloc(count * sizeof(void *));
    }
    else
    {
      if (a->alloc_elements < 1024)
        count = a->alloc_elements * 2;
      else
        count = a->alloc_elements + 1024;

      temp = realloc(a->elements, count * sizeof(void *));
    }

    if (!temp)
    {
      return (0);
    }

    a->alloc_elements = count;
    a->elements       = temp;
  }
  if (!a->num_elements || !a->compare)
  {
    if (insert)
      current = 0;			
    else
      current = a->num_elements;	
  }
  else
  {
    current = array_find(a, e, a->insert, &diff);

    if (diff > 0)
    {
      current ++;
    }
    else if (!diff)
    {
      a->unique = 0;

      if (insert)
      {
		while (current > 0 && !(*(a->compare))(e, a->elements[current - 1],a->data))
			  current --;
      }
      else
      {
		do
		{
			  current ++;
		}
		while (current < a->num_elements && !(*(a->compare))(e, a->elements[current], a->data));
      }
    }
  }
  if (current < a->num_elements)
  {
    memmove(a->elements + current + 1, a->elements + current,
            (a->num_elements - current) * sizeof(void *));

    if (a->current >= current)
      a->current ++;

    for (i = 0; i < a->num_saved; i ++)
      if (a->saved[i] >= current)
	a->saved[i] ++;
  }
  if (a->copyfunc)
  {
    if ((a->elements[current] = (a->copyfunc)(e, a->data)) == NULL)
    {
      return (0);
    }
  }
  else
    a->elements[current] = e;

  a->num_elements ++;
  a->insert = current;

  return (1);
}

static int				
array_find(array_t *a,	
        	void         *e,	
		int          prev,	
		int          *rdiff)	
{
  int	left,				
	right,				
	current,			
	diff;				

  if (a->compare)
  {
    if (prev >= 0 && prev < a->num_elements)
    {
      if ((diff = (*(a->compare))(e, a->elements[prev], a->data)) == 0 ||
          (diff < 0 && prev == 0) ||
	  (diff > 0 && prev == (a->num_elements - 1)))
      {
	*rdiff = diff;

	return (prev);
      }
      else if (diff < 0)
      {
	left  = 0;
	right = prev;
      }
      else
      {
        left  = prev;
		right = a->num_elements - 1;
      }
    }
    else
    {
      left  = 0;
      right = a->num_elements - 1;
    }

    do
    {
      current = (left + right) / 2;
      diff    = (*(a->compare))(e, a->elements[current], a->data);

      if (diff == 0)
	break;
      else if (diff < 0)
	right = current;
      else
	left = current;
    }
    while ((right - left) > 1);

    if (diff != 0)
    {
      if ((diff = (*(a->compare))(e, a->elements[left], a->data)) <= 0)
        current = left;
      else
      {
        diff    = (*(a->compare))(e, a->elements[right], a->data);
        current = right;
      }
    }
  }
  else
  {
    diff = 1;

    for (current = 0; current < a->num_elements; current ++)
      if (a->elements[current] == e)
      {
        diff = 0;
        break;
      }
  }

  *rdiff = diff;

  return (current);
}

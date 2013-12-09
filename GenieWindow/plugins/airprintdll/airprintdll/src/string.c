#include "string-private.h"
#include "thread-private.h"
#include "array.h"
#include <stddef.h>

mutex_t	sp_mutex ;
array_t *stringpool = NULL;

static int	compare_sp_items(sp_item_t *a, sp_item_t *b);

char * stralloc(const char *s)
{
  sp_item_t	*item,*key;

  if (!s)
    return (NULL);

  mutexLock(&sp_mutex);

  if (!stringpool)
    stringpool = arrayNew((array_func_t)compare_sp_items, NULL);

  if (!stringpool)
  {
    mutexUnlock(&sp_mutex);

    return (NULL);
  }

  key = (sp_item_t *)(s - offsetof(sp_item_t, str));

  if ((item = (sp_item_t *)arrayFind(stringpool, key)) != NULL)
  {
    item->ref_count ++;
    mutexUnlock(&sp_mutex);
    return (item->str);
  }

  item = (sp_item_t *)calloc(1, sizeof(sp_item_t) + strlen(s));
  if (!item)
  {
    mutexUnlock(&sp_mutex);
    return (NULL);
  }
  item->ref_count = 1;
  strcpy(item->str, s);
  arrayAdd(stringpool, item);
  mutexUnlock(&sp_mutex);
  return (item->str);
}

void strfree(const char *s)
{
  sp_item_t	*item,*key;

  if (!s)
    return;

  if (!stringpool)
    return;

  mutexLock(&sp_mutex);

  key = (sp_item_t *)(s - offsetof(sp_item_t, str));

  if ((item = (sp_item_t *)arrayFind(stringpool, key)) != NULL &&
      item == key)
  {
    item->ref_count --;

    if (!item->ref_count)
    {
      arrayRemove(stringpool, item);

      free(item);
    }
  }

  mutexUnlock(&sp_mutex);
}

int strcaseinsensitivecmp(const char *s, const char *t)
{
  while (*s != '\0' && *t != '\0')
  {
    if (tolower(*s) < tolower(*t))
      return (-1);
    else if (tolower(*s) > tolower(*t))
      return (1);

    s ++;
    t ++;
  }

  if (*s == '\0' && *t == '\0')
    return (0);
  else if (*s != '\0')
    return (1);
  else
    return (-1);
}

int strncaseinsensitivecmp(const char *s, const char *t, size_t n)
{
  while (*s != '\0' && *t != '\0' && n > 0)
  {
    if (tolower(*s) < tolower(*t))
      return (-1);
    else if (tolower(*s) > tolower(*t))
      return (1);

    s ++;
    t ++;
    n --;
  }

  if (n == 0)
    return (0);
  else if (*s == '\0' && *t == '\0')
    return (0);
  else if (*s != '\0')
    return (1);
  else
    return (-1);
}

static int compare_sp_items(sp_item_t *a,sp_item_t *b)
{
  return (strcmp(a->str, b->str));
}

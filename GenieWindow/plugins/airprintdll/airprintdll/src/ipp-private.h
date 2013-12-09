#ifndef _IPP_PRIVATE_H_
#define _IPP_PRIVATE_H_
#include "ipp.h"

#define IPP_BUF_SIZE	(IPP_MAX_LENGTH + 2)

typedef struct _ipp_buffer_s		
{
  unsigned char		d[IPP_BUF_SIZE];
  struct _ipp_buffer_s	*next;		
  int			used;		
} _ipp_buffer_t;

typedef struct				
{
  int		multivalue;		
  const char	*name;			
  ipp_tag_t	value_tag;		
  ipp_tag_t	group_tag;		
} _ipp_option_t;

extern ipp_attribute_t	*_ippAddAttr(ipp_t *ipp, int num_values);
extern size_t		_ippAttrString(ipp_attribute_t *attr, char *buffer,
			               size_t bufsize);
extern _ipp_option_t	*_ippFindOption(const char *name);
extern void		_ippFreeAttr(ipp_attribute_t *attr);
#endif

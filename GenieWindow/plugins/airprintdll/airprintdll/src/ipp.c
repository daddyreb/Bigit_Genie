#include "common.h"

static unsigned char	*ipp_buffer_get(void);
static void		ipp_buffer_release(unsigned char *b);
static size_t		ipp_length(ipp_t *ipp, int collection);
static ssize_t		ipp_read_http(http_t *http, ipp_uchar_t *buffer,
			              size_t length);
static ssize_t		ipp_read_file(int *fd, ipp_uchar_t *buffer,
			              size_t length);
static ssize_t		ipp_write_file(int *fd, ipp_uchar_t *buffer,
			               size_t length);

_ipp_buffer_t * ipp_buffers = NULL;
mutex_t	ipp_mutex;

ipp_attribute_t *			
ippAddBoolean(ipp_t      *ipp,		
              ipp_tag_t  group,		
              const char *name,		
              char       value)		
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name              = stralloc(name);
  attr->group_tag         = group;
  attr->value_tag         = IPP_TAG_BOOLEAN;
  attr->values[0].boolean = value;

  return (attr);
}

ipp_attribute_t *			
ippAddBooleans(ipp_t      *ipp,		
               ipp_tag_t  group,	
	       const char *name,	
	       int        num_values,	
	       const char *values)	
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = IPP_TAG_BOOLEAN;

  if (values != NULL)
    for (i = 0, value = attr->values;
	 i < num_values;
	 i ++, value ++)
      value->boolean = values[i];

  return (attr);
}

ipp_attribute_t *			
ippAddCollection(ipp_t      *ipp,	
                 ipp_tag_t  group,	
		 const char *name,	
		 ipp_t      *value)	
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name                 = stralloc(name);
  attr->group_tag            = group;
  attr->value_tag            = IPP_TAG_BEGIN_COLLECTION;
  attr->values[0].collection = value;

  value->use ++;

  return (attr);
}

ipp_attribute_t *			
ippAddCollections(
    ipp_t       *ipp,			
    ipp_tag_t   group,			
    const char  *name,			
    int         num_values,		
    const ipp_t **values)		
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = IPP_TAG_BEGIN_COLLECTION;

  if (values != NULL)
  {
    for (i = 0, value = attr->values;
	 i < num_values;
	 i ++, value ++)
    {
      value->collection = (ipp_t *)values[i];
      value->collection->use ++;
    }
  }

  return (attr);
}

ipp_attribute_t *			
ippAddDate(ipp_t             *ipp,	
           ipp_tag_t         group,	
	   const char        *name,	
	   const ipp_uchar_t *value)	
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name || !value)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = IPP_TAG_DATE;
  memcpy(attr->values[0].date, value, 11);

  return (attr);
}

ipp_attribute_t *			
ippAddInteger(ipp_t      *ipp,		
              ipp_tag_t  group,		
	      ipp_tag_t  type,		
              const char *name,		
              int        value)		
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name              = stralloc(name);
  attr->group_tag         = group;
  attr->value_tag         = type;
  attr->values[0].integer = value;

  return (attr);
}

ipp_attribute_t *			
ippAddIntegers(ipp_t      *ipp,		
               ipp_tag_t  group,	
	       ipp_tag_t  type,		
	       const char *name,	
	       int        num_values,	
	       const int  *values)	
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = type;

  if (values != NULL)
    for (i = 0, value = attr->values;
	 i < num_values;
	 i ++, value ++)
      value->integer = values[i];

  return (attr);
}

ipp_attribute_t	*			
ippAddOctetString(ipp_t      *ipp,	
                  ipp_tag_t  group,	
                  const char *name,	
                  const void *data,	
		  int        datalen)	
{
  ipp_attribute_t	*attr;		


  if (ipp == NULL || name == NULL)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name                     = stralloc(name);
  attr->group_tag                = group;
  attr->value_tag                = IPP_TAG_STRING;
  attr->values[0].unknown.length = datalen;

  if (data)
  {
    if ((attr->values[0].unknown.data = malloc(datalen)) == NULL)
    {
      ippDeleteAttribute(ipp, attr);
      return (NULL);
    }

    memcpy(attr->values[0].unknown.data, data, datalen);
  }

  return (attr);
}

ipp_attribute_t *
ippAddString(ipp_t      *ipp,
             ipp_tag_t  group,
			 ipp_tag_t  type,
             const char *name,
             const char *charset,
             const char *value)
{
  ipp_attribute_t	*attr;		
  char		buffer[1024],	
			*bufptr;	

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  if (type == IPP_TAG_LANGUAGE && !strcaseinsensitivecmp(value, "C"))
    value = "en";

  if ((type == IPP_TAG_LANGUAGE || type == IPP_TAG_CHARSET) && value)
  {
    strncpy(buffer, value, sizeof(buffer));
    value = buffer;

    for (bufptr = buffer; *bufptr; bufptr ++)
      if (*bufptr == '_')
        *bufptr = '-';
      else
        *bufptr = tolower(*bufptr & 255);
  }

  attr->name                     = stralloc(name);
  attr->group_tag                = group;
  attr->value_tag                = type;
  attr->values[0].string.charset = ((int)type & IPP_TAG_COPY) ? (char *)charset :
                                   charset ? stralloc(charset) : NULL;
  attr->values[0].string.text    = ((int)type & IPP_TAG_COPY) ? (char *)value :
                                   value ? stralloc(value) : NULL;

  return (attr);
}

ipp_attribute_t *			
ippAddStrings(
    ipp_t              *ipp,		
    ipp_tag_t          group,		
    ipp_tag_t          type,		
    const char         *name,		
    int                num_values,	
    const char         *charset,	
    const char * const *values)		
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		
  char			buffer[1024],	
			*bufptr;	

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = type;

  for (i = 0, value = attr->values;
       i < num_values;
       i ++, value ++)
  {
    if (i == 0)
      value->string.charset = ((int)type & IPP_TAG_COPY) ? (char *)charset :
                                   charset ? stralloc(charset) : NULL;
    else
      value->string.charset = attr->values[0].string.charset;

    if (values != NULL)
    {
      if ((int)type & IPP_TAG_COPY)
        value->string.text = (char *)values[i];
      else if (type == IPP_TAG_LANGUAGE && !strcaseinsensitivecmp(values[i], "C"))
      {

	value->string.text = ((int)type & IPP_TAG_COPY) ? "en" :
                                      "en";
      }
      else if (type == IPP_TAG_LANGUAGE || type == IPP_TAG_CHARSET)
      {
	strncpy(buffer, values[i], sizeof(buffer));

	for (bufptr = buffer; *bufptr; bufptr ++)
	  if (*bufptr == '_')
	    *bufptr = '-';
	  else
	    *bufptr = tolower(*bufptr & 255);

	value->string.text = stralloc(buffer);
      }
      else
	value->string.text = stralloc(values[i]);

    }
  }

  return (attr);
}

ipp_attribute_t *			
ippAddRange(ipp_t      *ipp,		
            ipp_tag_t  group,		
	    const char *name,		
	    int        lower,		
	    int        upper)		
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name                  = stralloc(name);
  attr->group_tag             = group;
  attr->value_tag             = IPP_TAG_RANGE;
  attr->values[0].range.lower = lower;
  attr->values[0].range.upper = upper;

  return (attr);
}

ipp_attribute_t *			
ippAddRanges(ipp_t      *ipp,		
             ipp_tag_t  group,		
	     const char *name,		
	     int        num_values,	
	     const int  *lower,		
	     const int  *upper)		
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = IPP_TAG_RANGE;

  if (lower != NULL && upper != NULL)
    for (i = 0, value = attr->values;
	 i < num_values;
	 i ++, value ++)
    {
      value->range.lower = lower[i];
      value->range.upper = upper[i];
    }

  return (attr);
}

ipp_attribute_t *			
ippAddResolution(ipp_t      *ipp,	
        	 ipp_tag_t  group,	
		 const char *name,	
		 ipp_res_t  units,	
		 int        xres,	
		 int        yres)	
{
  ipp_attribute_t	*attr;		

  if (!ipp || !name)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 1)) == NULL)
    return (NULL);

  attr->name                       = stralloc(name);
  attr->group_tag                  = group;
  attr->value_tag                  = IPP_TAG_RESOLUTION;
  attr->values[0].resolution.xres  = xres;
  attr->values[0].resolution.yres  = yres;
  attr->values[0].resolution.units = units;

  return (attr);
}

ipp_attribute_t *			
ippAddResolutions(ipp_t      *ipp,	
        	  ipp_tag_t  group,	
		  const char *name,	
		  int        num_values,
		  ipp_res_t  units,	
		  const int  *xres,	
		  const int  *yres)	
{
  int			i;		
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!ipp || !name || num_values < 1)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, num_values)) == NULL)
    return (NULL);

  attr->name      = stralloc(name);
  attr->group_tag = group;
  attr->value_tag = IPP_TAG_RESOLUTION;

  if (xres != NULL && yres != NULL)
    for (i = 0, value = attr->values;
	 i < num_values;
	 i ++, value ++)
    {
      value->resolution.xres  = xres[i];
      value->resolution.yres  = yres[i];
      value->resolution.units = units;
    }

  return (attr);
}

ipp_attribute_t *			
ippAddSeparator(ipp_t *ipp)		
{
  ipp_attribute_t	*attr;		

  if (!ipp)
    return (NULL);

  if ((attr = _ippAddAttr(ipp, 0)) == NULL)
    return (NULL);

  attr->group_tag = IPP_TAG_ZERO;
  attr->value_tag = IPP_TAG_ZERO;

  return (attr);
}

time_t					
ippDateToTime(const ipp_uchar_t *date)	
{
  struct tm	unixdate;		
  time_t	t;			


  if (!date)
    return (0);

  memset(&unixdate, 0, sizeof(unixdate));

  unixdate.tm_year = ((date[0] << 8) | date[1]) - 1900;
  unixdate.tm_mon  = date[2] - 1;
  unixdate.tm_mday = date[3];
  unixdate.tm_hour = date[4];
  unixdate.tm_min  = date[5];
  unixdate.tm_sec  = date[6];

  t = mktime(&unixdate);

  if (date[8] == '-')
    t += date[9] * 3600 + date[10] * 60;
  else
    t -= date[9] * 3600 + date[10] * 60;

  return (t);
}

void
ippDelete(ipp_t *ipp)			
{
  ipp_attribute_t	*attr,		
			*next;		

  if (!ipp)
    return;

  ipp->use --;
  if (ipp->use > 0)
    return;

  for (attr = ipp->attrs; attr != NULL; attr = next)
  {
    next = attr->next;
    _ippFreeAttr(attr);
  }

  free(ipp);
}

void
ippDeleteAttribute(
    ipp_t           *ipp,		
    ipp_attribute_t *attr)		
{
  ipp_attribute_t	*current,	
			*prev;		

  for (current = ipp->attrs, prev = NULL;
       current != NULL && current != attr;
       prev = current, current = current->next);

  if (current)
  {
    if (prev)
      prev->next = current->next;
    else
      ipp->attrs = current->next;

    if (current == ipp->last)
      ipp->last = prev;

    _ippFreeAttr(current);
  }
}

ipp_attribute_t	* ippFindAttribute(ipp_t *ipp,const char *name,ipp_tag_t  type)
{
  if (!ipp || !name)
    return (NULL);

  ipp->current = NULL;

  return (ippFindNextAttribute(ipp, name, type));
}

ipp_attribute_t	* ippFindNextAttribute(ipp_t *ipp, const char *name, ipp_tag_t type) 
{
  ipp_attribute_t	*attr;		
  ipp_tag_t		value_tag;	

  if (!ipp || !name)
    return (NULL);

  if (ipp->current)
  {
    ipp->prev = ipp->current;
    attr      = ipp->current->next;
  }
  else
  {
    ipp->prev = NULL;
    attr      = ipp->attrs;
  }

  for (; attr != NULL; ipp->prev = attr, attr = attr->next)
  {
    value_tag = (ipp_tag_t)(attr->value_tag & IPP_TAG_MASK);

    if (attr->name != NULL && strcaseinsensitivecmp(attr->name, name) == 0 &&
        (value_tag == type || type == IPP_TAG_ZERO ||
	 (value_tag == IPP_TAG_TEXTLANG && type == IPP_TAG_TEXT) ||
	 (value_tag == IPP_TAG_NAMELANG && type == IPP_TAG_NAME)))
    {
      ipp->current = attr;

      return (attr);
    }
  }

  ipp->current = NULL;
  ipp->prev    = NULL;

  return (NULL);
}

size_t ippLength(ipp_t *ipp)
{
  return (ipp_length(ipp, 0));
}

ipp_t *					
ippNew(void)
{
  ipp_t	*temp;				

  if ((temp = (ipp_t *)calloc(1, sizeof(ipp_t))) != NULL)
  {
    temp->request.any.version[0] = 1;
    temp->request.any.version[1] = 1;
    temp->use                    = 1;
  }
  return (temp);
}

ipp_t *					
ippNewRequest(ipp_op_t op)		
{
  ipp_t		*request;		

  if ((request = ippNew()) == NULL)
    return (NULL);

  request->request.op.operation_id = op;
  request->request.op.request_id   = 1;

  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
               "attributes-charset", NULL, "utf-8");

  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
               "attributes-natural-language", NULL, "en-US");

  return (request);
}

ipp_state_t				
ippRead(http_t *http,			
        ipp_t  *ipp)			
{
  if (!http)
    return (IPP_ERROR);

  return (ippReadIO(http, (ipp_iocb_t)ipp_read_http, http->blocking, NULL,
                    ipp));
}

ipp_state_t				
ippReadFile(int   fd,			
            ipp_t *ipp)			
{
  return (ippReadIO(&fd, (ipp_iocb_t)ipp_read_file, 1, NULL, ipp));
}

ipp_state_t				
ippReadIO(void       *src,		
          ipp_iocb_t cb,		
	  int        blocking,		
	  ipp_t      *parent,		
          ipp_t      *ipp)		
{
  int			n;		
  unsigned char		*buffer,	
			string[IPP_MAX_NAME],
					
			*bufptr;	
  ipp_attribute_t	*attr;		
  ipp_tag_t		tag;		
  ipp_tag_t		value_tag;	
  ipp_value_t		*value;		

  if (!src || !ipp)
    return (IPP_ERROR);

  if ((buffer = ipp_buffer_get()) == NULL)
  {
    return (IPP_ERROR);
  }

  switch (ipp->state)
  {
    case IPP_IDLE :
        ipp->state = (ipp_state_t)(ipp->state+1); 

    case IPP_HEADER :
        if (parent == NULL)
	{
      if ((*cb)(src, buffer, 8) < 8)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }
          ipp->request.any.version[0]  = buffer[0];
          ipp->request.any.version[1]  = buffer[1];
          ipp->request.any.op_status   = (buffer[2] << 8) | buffer[3];
          ipp->request.any.request_id  = (((((buffer[4] << 8) | buffer[5]) << 8) |
	                        	 buffer[6]) << 8) | buffer[7];
        }

        ipp->state   = IPP_ATTRIBUTE;
	ipp->current = NULL;
	ipp->curtag  = IPP_TAG_ZERO;
	ipp->prev    = ipp->last;

        if (!blocking)
	  break;

    case IPP_ATTRIBUTE :
        for (;;)
	{
	  if ((*cb)(src, buffer, 1) < 1)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }
          tag = (ipp_tag_t)buffer[0];

	  if (tag == IPP_TAG_END)
	  {
	    ipp->state = IPP_DATA;
	    break;
	  }
          else if (tag < IPP_TAG_UNSUPPORTED_VALUE)
	  {
		  if (ipp->curtag == tag)
	      ipp->prev = ippAddSeparator(ipp);
            else if (ipp->current)
	      ipp->prev = ipp->current;

	    ipp->curtag  = tag;
	    ipp->current = NULL;
	    continue;
	  }

          if ((*cb)(src, buffer, 2) < 2)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }

          n = (buffer[0] << 8) | buffer[1];

          if (n >= IPP_BUF_SIZE)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }

          if (n == 0 && tag != IPP_TAG_MEMBERNAME &&
	      tag != IPP_TAG_END_COLLECTION)
	  {
        if (ipp->current == NULL)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }

            attr      = ipp->current;
	    value_tag = (ipp_tag_t)(attr->value_tag & IPP_TAG_MASK);

	    if (value_tag == IPP_TAG_ZERO)
	    {
	      attr->value_tag = tag;
	    }
	    else if (value_tag == IPP_TAG_TEXTLANG ||
	             value_tag == IPP_TAG_NAMELANG ||
		     (value_tag >= IPP_TAG_TEXT &&
		      value_tag <= IPP_TAG_MIMETYPE))
            {
	      if (tag != IPP_TAG_TEXTLANG && tag != IPP_TAG_NAMELANG &&
	          (tag < IPP_TAG_TEXT || tag > IPP_TAG_MIMETYPE) &&
		  tag != IPP_TAG_NOVALUE)
	      {
		ipp_buffer_release(buffer);
	        return (IPP_ERROR);
	      }
            }
	    else if (value_tag != tag)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
            }
	    if (attr->num_values == 1 ||
	        (attr->num_values > 0 &&
	         (attr->num_values & (IPP_MAX_VALUES - 1)) == 0))
	    {
	      ipp_attribute_t	*temp;	

              if ((temp = (ipp_attribute_t *)realloc(attr, sizeof(ipp_attribute_t) +
	                                (attr->num_values + IPP_MAX_VALUES - 1) *
					sizeof(ipp_value_t))) == NULL)
	      {
		ipp_buffer_release(buffer);
	        return (IPP_ERROR);
	      }

              if (temp != attr)
	      {
        	if (ipp->prev)
	          ipp->prev->next = temp;
		else
	          ipp->attrs = temp;

        	attr = ipp->current = ipp->last = temp;
	      }
	    }
	  }
	  else if (tag == IPP_TAG_MEMBERNAME)
	  {
	    if (n)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }

            if (ipp->current)
	      ipp->prev = ipp->current;

	    attr = ipp->current = _ippAddAttr(ipp, 1);

	    attr->group_tag  = ipp->curtag;
	    attr->value_tag  = IPP_TAG_ZERO;
	    attr->num_values = 0;
	  }
	  else if (tag != IPP_TAG_END_COLLECTION)
	  {
		if ((*cb)(src, buffer, n) < n)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }

	    buffer[n] = '\0';

            if (ipp->current)
	      ipp->prev = ipp->current;

	    if ((attr = ipp->current = _ippAddAttr(ipp, 1)) == NULL)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }

	    attr->group_tag  = ipp->curtag;
	    attr->value_tag  = tag;
	    attr->name       = stralloc((char *)buffer);
	    attr->num_values = 0;
	  }
	  else
	    attr = NULL;

          if (tag != IPP_TAG_END_COLLECTION)
            value = attr->values + attr->num_values;
	  else
	    value = NULL;

	  if ((*cb)(src, buffer, 2) < 2)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }

	  n = (buffer[0] << 8) | buffer[1];

	  switch (tag)
	  {
	    case IPP_TAG_INTEGER :
	    case IPP_TAG_ENUM :
		if (n != 4)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, 4) < 4)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		n = (((((buffer[0] << 8) | buffer[1]) << 8) | buffer[2]) << 8) |
		    buffer[3];

                value->integer = n;
	        break;

	    case IPP_TAG_BOOLEAN :
		if (n != 1)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, 1) < 1)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

                value->boolean = buffer[0];
	        break;

            case IPP_TAG_NOVALUE :
	    case IPP_TAG_NOTSETTABLE :
	    case IPP_TAG_DELETEATTR :
	    case IPP_TAG_ADMINDEFINE :
	    if (attr->value_tag == tag)
		{
		  if (n == 0)
		    break;

		  attr->value_tag = IPP_TAG_TEXT;
		}

	    case IPP_TAG_TEXT :
	    case IPP_TAG_NAME :
	    case IPP_TAG_KEYWORD :
	    case IPP_TAG_URI :
	    case IPP_TAG_URISCHEME :
	    case IPP_TAG_CHARSET :
	    case IPP_TAG_LANGUAGE :
	    case IPP_TAG_MIMETYPE :
		if (n >= IPP_BUF_SIZE)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		if ((*cb)(src, buffer, n) < n)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		buffer[n] = '\0';
		value->string.text = stralloc((char *)buffer);
	        break;

	    case IPP_TAG_DATE :
		if (n != 11)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, value->date, 11) < 11)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}
	        break;

	    case IPP_TAG_RESOLUTION :
		if (n != 9)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, 9) < 9)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

                value->resolution.xres =
		    (((((buffer[0] << 8) | buffer[1]) << 8) | buffer[2]) << 8) |
		    buffer[3];
                value->resolution.yres =
		    (((((buffer[4] << 8) | buffer[5]) << 8) | buffer[6]) << 8) |
		    buffer[7];
                value->resolution.units =
		    (ipp_res_t)buffer[8];
	        break;

	    case IPP_TAG_RANGE :
		if (n != 8)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, 8) < 8)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

                value->range.lower =
		    (((((buffer[0] << 8) | buffer[1]) << 8) | buffer[2]) << 8) |
		    buffer[3];
                value->range.upper =
		    (((((buffer[4] << 8) | buffer[5]) << 8) | buffer[6]) << 8) |
		    buffer[7];
	        break;

	    case IPP_TAG_TEXTLANG :
	    case IPP_TAG_NAMELANG :
	        if (n >= IPP_BUF_SIZE || n < 4)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, n) < n)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

        bufptr = buffer;

		n = (bufptr[0] << 8) | bufptr[1];

		if ((bufptr + 2 + n) >= (buffer + IPP_BUF_SIZE) ||
		    n >= sizeof(string))
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		memcpy(string, bufptr + 2, n);
		string[n] = '\0';

		value->string.charset = stralloc((char *)string);

                bufptr += 2 + n;
		n = (bufptr[0] << 8) | bufptr[1];

		if ((bufptr + 2 + n) >= (buffer + IPP_BUF_SIZE))
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		bufptr[2 + n] = '\0';
                value->string.text = stralloc((char *)bufptr + 2);
	        break;

            case IPP_TAG_BEGIN_COLLECTION :

            value->collection = ippNew();

                if (n > 0)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		if (ippReadIO(src, cb, 1, ipp, value->collection) == IPP_ERROR)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}
                break;

            case IPP_TAG_END_COLLECTION :
		ipp_buffer_release(buffer);

                if (n > 0)
		{
		  return (IPP_ERROR);
		}
		return (ipp->state = IPP_DATA);

            case IPP_TAG_MEMBERNAME :

		if (n >= IPP_BUF_SIZE)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

	        if ((*cb)(src, buffer, n) < n)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		buffer[n] = '\0';
		attr->name = stralloc((char *)buffer);

                attr->num_values --;
		break;

            default : 
		if (n > IPP_MAX_LENGTH)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

		if (!value)
		{
		  ipp_buffer_release(buffer);
		  return (IPP_ERROR);
		}

                value->unknown.length = n;
	        if (n > 0)
		{
		  if ((value->unknown.data = malloc(n)) == NULL)
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
		  }

                  if ((*cb)(src,(ipp_uchar_t *)value->unknown.data, n) < n)
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
		  }
		}
		else
		  value->unknown.data = NULL;
	        break;
	  }

          attr->num_values ++;

          if (!blocking)
	    break;
	}
        break;

    case IPP_DATA :
        break;

    default :
        break; 
  }
  ipp_buffer_release(buffer);

  return (ipp->state);
}

ipp_state_t ippWrite(http_t *http, ipp_t  *ipp)
{
  if (!http)
    return (IPP_ERROR);

  return (ippWriteIO(http, (ipp_iocb_t)httpWrite2, http->blocking, NULL, ipp));
}

ipp_state_t	 ippWriteIO(void *dst, ipp_iocb_t cb, int blocking,ipp_t *parent, ipp_t *ipp)
{
  int			i;		
  int			n;		
  unsigned char		*buffer,	
			*bufptr;	
  ipp_attribute_t	*attr;		
  ipp_value_t		*value;		

  if (!dst || !ipp)
    return (IPP_ERROR);

  if ((buffer = ipp_buffer_get()) == NULL)
  {
    return (IPP_ERROR);
  }

  switch (ipp->state)
  {
    case IPP_IDLE :
        ipp->state = (ipp_state_t)(ipp->state+1);

    case IPP_HEADER :
        if (parent == NULL)
	{
      bufptr = buffer;

	  *bufptr++ = ipp->request.any.version[0];
	  *bufptr++ = ipp->request.any.version[1];
	  *bufptr++ = ipp->request.any.op_status >> 8;
	  *bufptr++ = ipp->request.any.op_status;
	  *bufptr++ = ipp->request.any.request_id >> 24;
	  *bufptr++ = ipp->request.any.request_id >> 16;
	  *bufptr++ = ipp->request.any.request_id >> 8;
	  *bufptr++ = ipp->request.any.request_id;

      if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }
	}

    ipp->state   = IPP_ATTRIBUTE;
	ipp->current = ipp->attrs;
	ipp->curtag  = IPP_TAG_ZERO;

   // if (!blocking)
	  //break;

    case IPP_ATTRIBUTE :
        while (ipp->current != NULL)
	{
	  bufptr = buffer;
	  attr   = ipp->current;

	  ipp->current = ipp->current->next;

          if (!parent)
	  {
	    if (ipp->curtag != attr->group_tag)
	    {
	      ipp->curtag = attr->group_tag;

	      if (attr->group_tag == IPP_TAG_ZERO)
		continue;
	      *bufptr++ = attr->group_tag;
	    }
	    else if (attr->group_tag == IPP_TAG_ZERO)
	      continue;
	  }

          if (parent == NULL)
	  {
		   if ((n = (int)strlen(attr->name)) > (IPP_BUF_SIZE - 4))
			{
			  ipp_buffer_release(buffer);
			  return (IPP_ERROR);
			}
			*bufptr++ = attr->value_tag;
			*bufptr++ = n >> 8;
			*bufptr++ = n;
			memcpy(bufptr, attr->name, n);
			bufptr += n;
          }
	  else
	  {
        if ((n = (int)strlen(attr->name)) > (IPP_BUF_SIZE - 7))
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }

            *bufptr++ = IPP_TAG_MEMBERNAME;
			*bufptr++ = 0;
			*bufptr++ = 0;
			*bufptr++ = n >> 8;
			*bufptr++ = n;
			memcpy(bufptr, attr->name, n);
			bufptr += n;

            *bufptr++ = attr->value_tag;
            *bufptr++ = 0;
            *bufptr++ = 0;
	  }

	  switch (attr->value_tag & ~IPP_TAG_COPY)
	  {
	    case IPP_TAG_INTEGER :
	    case IPP_TAG_ENUM :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 9)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
            *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
	      *bufptr++ = 0;
		  *bufptr++ = 4;
		  *bufptr++ = value->integer >> 24;
		  *bufptr++ = value->integer >> 16;
		  *bufptr++ = value->integer >> 8;
		  *bufptr++ = value->integer;
		}
		break;

	    case IPP_TAG_BOOLEAN :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 6)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
			*bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
	      *bufptr++ = 0;
		  *bufptr++ = 1;
		  *bufptr++ = value->boolean;
		}
		break;

	    case IPP_TAG_TEXT :
	    case IPP_TAG_NAME :
	    case IPP_TAG_KEYWORD :
	    case IPP_TAG_URI :
	    case IPP_TAG_URISCHEME :
	    case IPP_TAG_CHARSET :
	    case IPP_TAG_LANGUAGE :
	    case IPP_TAG_MIMETYPE :
	    for (i = 0, value = attr->values;i < attr->num_values;i ++,value ++)
		{
		  if (i)
		  {
                    if ((IPP_BUF_SIZE - (bufptr - buffer)) < 3)
		    {
                      if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	              {
			ipp_buffer_release(buffer);
	        	return (IPP_ERROR);
	              }

		      bufptr = buffer;
		    }

                    *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }

                  if (value->string.text != NULL)
                    n = (int)strlen(value->string.text);
		  else
		    n = 0;

                  if (n > (IPP_BUF_SIZE - 2))
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
		  }
                  if ((int)(IPP_BUF_SIZE - (bufptr - buffer)) < (n + 2))
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }
	      *bufptr++ = n >> 8;
		  *bufptr++ = n;

		  if (n > 0)
		  {
		    memcpy(bufptr, value->string.text, n);
		    bufptr += n;
		  }
		}
		break;

	    case IPP_TAG_DATE :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 16)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
			*bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
	      *bufptr++ = 0;
		  *bufptr++ = 11;
		  memcpy(bufptr, value->date, 11);
		  bufptr += 11;
		}
		break;

	    case IPP_TAG_RESOLUTION :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 14)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
		      return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
            *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
	      *bufptr++ = 0;
		  *bufptr++ = 9;
		  *bufptr++ = value->resolution.xres >> 24;
		  *bufptr++ = value->resolution.xres >> 16;
		  *bufptr++ = value->resolution.xres >> 8;
		  *bufptr++ = value->resolution.xres;
		  *bufptr++ = value->resolution.yres >> 24;
		  *bufptr++ = value->resolution.yres >> 16;
		  *bufptr++ = value->resolution.yres >> 8;
		  *bufptr++ = value->resolution.yres;
		  *bufptr++ = value->resolution.units;
		}
		break;

	    case IPP_TAG_RANGE :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 13)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
            *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
	      *bufptr++ = 0;
		  *bufptr++ = 8;
		  *bufptr++ = value->range.lower >> 24;
		  *bufptr++ = value->range.lower >> 16;
		  *bufptr++ = value->range.lower >> 8;
		  *bufptr++ = value->range.lower;
		  *bufptr++ = value->range.upper >> 24;
		  *bufptr++ = value->range.upper >> 16;
		  *bufptr++ = value->range.upper >> 8;
		  *bufptr++ = value->range.upper;
		}
		break;

	    case IPP_TAG_TEXTLANG :
	    case IPP_TAG_NAMELANG :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
		  if (i)
		  {
            if ((IPP_BUF_SIZE - (bufptr - buffer)) < 3)
		    {
               if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	              {
						ipp_buffer_release(buffer);
	        			return (IPP_ERROR);
	              }
		      bufptr = buffer;
		    }

            *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
          n = 4;

		  if (value->string.charset != NULL)
                    n += (int)strlen(value->string.charset);

		  if (value->string.text != NULL)
                    n += (int)strlen(value->string.text);

          if (n > (IPP_BUF_SIZE - 2))
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
           }

                  if ((int)(IPP_BUF_SIZE - (bufptr - buffer)) < (n + 2))
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

                 
	          *bufptr++ = n >> 8;
		  *bufptr++ = n;

                 
		  if (value->string.charset != NULL)
		    n = (int)strlen(value->string.charset);
		  else
		    n = 0;

	          *bufptr++ = n >> 8;
		  *bufptr++ = n;

                 
		  if (n > 0)
		  {
		    memcpy(bufptr, value->string.charset, n);
		    bufptr += n;
		  }

                 
                  if (value->string.text != NULL)
		    n = (int)strlen(value->string.text);
		  else
		    n = 0;

	          *bufptr++ = n >> 8;
		  *bufptr++ = n;

                 
		  if (n > 0)
		  {
		    memcpy(bufptr, value->string.text, n);
		    bufptr += n;
		  }
		}
		break;

            case IPP_TAG_BEGIN_COLLECTION :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{

                  if ((IPP_BUF_SIZE - (bufptr - buffer)) < 5)
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

		  if (i)
		  {
            *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }

	          *bufptr++ = 0;
		  *bufptr++ = 0;

                  if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	          {
		    ipp_buffer_release(buffer);
	            return (IPP_ERROR);
	          }

		  bufptr = buffer;

           value->collection->state = IPP_IDLE;

		  if (ippWriteIO(dst, cb, 1, ipp,
		                 value->collection) == IPP_ERROR)
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
		  }
		}
		break;

            default :
	        for (i = 0, value = attr->values;
		     i < attr->num_values;
		     i ++, value ++)
		{
		  if (i)
		  {
                    if ((IPP_BUF_SIZE - (bufptr - buffer)) < 3)
		    {
                      if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	              {
			ipp_buffer_release(buffer);
	        	return (IPP_ERROR);
	              }

		      bufptr = buffer;
		    }

                    *bufptr++ = attr->value_tag;
		    *bufptr++ = 0;
		    *bufptr++ = 0;
		  }
                  n = value->unknown.length;

                  if (n > (IPP_BUF_SIZE - 2))
		  {
		    ipp_buffer_release(buffer);
		    return (IPP_ERROR);
		  }

                  if ((int)(IPP_BUF_SIZE - (bufptr - buffer)) < (n + 2))
		  {
                    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	            {
		      ipp_buffer_release(buffer);
	              return (IPP_ERROR);
	            }

		    bufptr = buffer;
		  }

                 
	          *bufptr++ = n >> 8;
		  *bufptr++ = n;

                 
		  if (n > 0)
		  {
		    memcpy(bufptr, value->unknown.data, n);
		    bufptr += n;
		  }
		}
		break;
	  }

	  if (bufptr > buffer)
	  {
	    if ((*cb)(dst, buffer, (int)(bufptr - buffer)) < 0)
	    {
	      ipp_buffer_release(buffer);
	      return (IPP_ERROR);
	    }
	  }

    //      if (!blocking)
			 //break;
	}

	if (ipp->current == NULL)
	{
	if (parent == NULL)
	  {
            buffer[0] = IPP_TAG_END;
	    n         = 1;
	  }
	  else
	  {
            buffer[0] = IPP_TAG_END_COLLECTION;
	    buffer[1] = 0; 
	    buffer[2] = 0;
	    buffer[3] = 0; 
	    buffer[4] = 0;
	    n         = 5;
	  }

	  if ((*cb)(dst, buffer, n) < 0)
	  {
	    ipp_buffer_release(buffer);
	    return (IPP_ERROR);
	  }

	  ipp->state = IPP_DATA;
	}
        break;

    case IPP_DATA :
        break;

    default :
        break; 
  }

  ipp_buffer_release(buffer);

  return (ipp->state);
}

ipp_attribute_t *			
_ippAddAttr(ipp_t *ipp,			
            int   num_values)		
{
  ipp_attribute_t	*attr;		

  if (!ipp || num_values < 0)
    return (NULL);

  attr = (ipp_attribute_t *)calloc(sizeof(ipp_attribute_t) +
                (num_values - 1) * sizeof(ipp_value_t), 1);

  if (attr != NULL)
  {
    attr->num_values = num_values;

    if (ipp->last == NULL)
      ipp->attrs = attr;
    else
      ipp->last->next = attr;

    ipp->last = attr;
  }
  return (attr);
}

void
_ippFreeAttr(ipp_attribute_t *attr)	
{
  int		i;			
  ipp_value_t	*value;			

  switch (attr->value_tag)
  {
    case IPP_TAG_TEXT :
    case IPP_TAG_NAME :
    case IPP_TAG_RESERVED_STRING :
    case IPP_TAG_KEYWORD :
    case IPP_TAG_URI :
    case IPP_TAG_URISCHEME :
    case IPP_TAG_CHARSET :
    case IPP_TAG_LANGUAGE :
    case IPP_TAG_MIMETYPE :
	for (i = 0, value = attr->values;
	     i < attr->num_values;
	     i ++, value ++)
	  strfree(value->string.text);
	break;

    case IPP_TAG_TEXTLANG :
    case IPP_TAG_NAMELANG :
	for (i = 0, value = attr->values;
	     i < attr->num_values;
	     i ++, value ++)
	{
	  if (value->string.charset && i == 0)
	    strfree(value->string.charset);
	  strfree(value->string.text);
	}
	break;

    case IPP_TAG_INTEGER :
    case IPP_TAG_ENUM :
    case IPP_TAG_BOOLEAN :
    case IPP_TAG_DATE :
    case IPP_TAG_RESOLUTION :
    case IPP_TAG_RANGE :
	break;

    case IPP_TAG_BEGIN_COLLECTION :
	for (i = 0, value = attr->values;
	     i < attr->num_values;
	     i ++, value ++)
          ippDelete(value->collection);
	break;

    case IPP_TAG_STRING :
	for (i = 0, value = attr->values;
	     i < attr->num_values;
	     i ++, value ++)
	  free(value->unknown.data);
        break;

    default :
        if (!((int)attr->value_tag & IPP_TAG_COPY))
	{
	  for (i = 0, value = attr->values;
	       i < attr->num_values;
	       i ++, value ++)
            if (value->unknown.data)
	      free(value->unknown.data);
        }
	break;
  }

  if (attr->name)
    strfree(attr->name);

  free(attr);
}

static unsigned char *			
ipp_buffer_get(void)
{
  _ipp_buffer_t		*buffer;	

  mutexLock(&ipp_mutex);
  for (buffer = ipp_buffers; buffer; buffer = buffer->next)
    if (!buffer->used)
    {
      buffer->used = 1;
	  mutexUnlock(&ipp_mutex);
      return (buffer->d);
    }

  if ((buffer = (_ipp_buffer_t *)malloc(sizeof(_ipp_buffer_t))) == NULL)
  {
	mutexUnlock(&ipp_mutex);
    return (NULL);
  }

  buffer->used    = 1;
  buffer->next    = ipp_buffers;
  ipp_buffers = buffer;

  mutexUnlock(&ipp_mutex);
  return (buffer->d);
}

static void
ipp_buffer_release(unsigned char *b)	
{
  mutexLock(&ipp_mutex);
  ((_ipp_buffer_t *)b)->used = 0;
  mutexUnlock(&ipp_mutex);
}

static size_t ipp_length(ipp_t *ipp,int   collection)
{
  int			i;		
  int			bytes;		
  ipp_attribute_t	*attr;		
  ipp_tag_t		group;		
  ipp_value_t		*value;		

  if (ipp == NULL)
    return (0);

  bytes = collection ? 0 : 8;

  group = IPP_TAG_ZERO;

  for (attr = ipp->attrs; attr != NULL; attr = attr->next)
  {
    if (attr->group_tag != group && !collection)
    {
      group = attr->group_tag;
      if (group == IPP_TAG_ZERO)
	continue;
      bytes ++;	
    }

    if (!attr->name)
      continue;

    bytes += (int)strlen(attr->name);	
    bytes += attr->num_values;		
    bytes += 2 * attr->num_values;	
    bytes += 2 * attr->num_values;	

    if (collection)
      bytes += 5;			

    switch (attr->value_tag & ~IPP_TAG_COPY)
    {
      case IPP_TAG_INTEGER :
      case IPP_TAG_ENUM :
          bytes += 4 * attr->num_values;
	  break;

      case IPP_TAG_BOOLEAN :
          bytes += attr->num_values;
	  break;

      case IPP_TAG_TEXT :
      case IPP_TAG_NAME :
      case IPP_TAG_KEYWORD :
      case IPP_TAG_URI :
      case IPP_TAG_URISCHEME :
      case IPP_TAG_CHARSET :
      case IPP_TAG_LANGUAGE :
      case IPP_TAG_MIMETYPE :
	  for (i = 0, value = attr->values;
	       i < attr->num_values;
	       i ++, value ++)
	    if (value->string.text != NULL)
	      bytes += (int)strlen(value->string.text);
	  break;

      case IPP_TAG_DATE :
          bytes += 11 * attr->num_values;
	  break;

      case IPP_TAG_RESOLUTION :
          bytes += 9 * attr->num_values;
	  break;

      case IPP_TAG_RANGE :
          bytes += 8 * attr->num_values;
	  break;

      case IPP_TAG_TEXTLANG :
      case IPP_TAG_NAMELANG :
          bytes += 4 * attr->num_values;

	  for (i = 0, value = attr->values;
	       i < attr->num_values;
	       i ++, value ++)
	  {
	    if (value->string.charset != NULL)
	      bytes += (int)strlen(value->string.charset);

	    if (value->string.text != NULL)
	      bytes += (int)strlen(value->string.text);
	  }
	  break;

      case IPP_TAG_BEGIN_COLLECTION :
	  for (i = 0, value = attr->values;
	       i < attr->num_values;
	       i ++, value ++)
            bytes += (int)ipp_length(value->collection, 1);
	  break;

      default :
	  for (i = 0, value = attr->values;
	       i < attr->num_values;
	       i ++, value ++)
            bytes += value->unknown.length;
	  break;
    }
  }

  if (collection)
    bytes += 5;
  else
    bytes ++;

  return (bytes);
}

static ssize_t				
ipp_read_http(http_t      *http,	
              ipp_uchar_t *buffer,	
	      size_t      length)	
{
  int		tbytes,			
		bytes;			
  char		len[32];		

  for (tbytes = 0, bytes = 0;
       tbytes < (int)length;
       tbytes += bytes, buffer += bytes)
  {
    if (http->state == HTTP_WAITING)
      break;

    if (http->used > 0 && http->data_encoding == HTTP_ENCODE_LENGTH)
    {
      if (http->used > (int)(length - tbytes))
        bytes = (int)(length - tbytes);
      else
        bytes = http->used;

      if (bytes == 1)
	buffer[0] = http->buffer[0];
      else
	memcpy(buffer, http->buffer, bytes);

      http->used           -= bytes;
      http->data_remaining -= bytes;

      if (http->data_remaining <= INT_MAX)
	http->_data_remaining = (int)http->data_remaining;
      else
	http->_data_remaining = INT_MAX;

      if (http->used > 0)
	memmove(http->buffer, http->buffer + bytes, http->used);

      if (http->data_remaining == 0)
      {
	if (http->data_encoding == HTTP_ENCODE_CHUNKED)
	{
	  if (!httpGets(len, sizeof(len), http))
	    return (-1);
	}

	if (http->data_encoding != HTTP_ENCODE_CHUNKED)
	{
	  if (http->state == HTTP_POST_RECV)
            http->state = (http_state_t)(http->state+1);
	  else
	    http->state = HTTP_WAITING;
	}
      }
    }
    else
    {
      if (!http->blocking)
      {
	if (!httpWait(http, 10000))
	{
		bytes = -1;
	  break;
	}
      }

      if ((bytes = httpRead2(http, (char *)buffer, length - tbytes)) < 0)
      {
#ifdef WIN32
        break;
#else
        if (errno != EAGAIN && errno != EINTR)
	  break;

	bytes = 0;
#endif 
      }
      else if (bytes == 0)
        break;
    }
  }

  if (tbytes == 0 && bytes < 0)
    tbytes = -1;

  return (tbytes);
}

static ssize_t				
ipp_read_file(int         *fd,		
              ipp_uchar_t *buffer,	
	      size_t      length)	
{
  return ((ssize_t)read(*fd, buffer, (unsigned)length));
}

static ssize_t				
ipp_write_file(int         *fd,		
               ipp_uchar_t *buffer,	
               size_t      length)	
{
  return ((ssize_t)write(*fd, buffer, (unsigned)length));
}

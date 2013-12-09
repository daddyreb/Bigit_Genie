#include "common.h"
static const char * const ipp_status_oks[] =	
		{
		  "successful-ok",
		  "successful-ok-ignored-or-substituted-attributes",
		  "successful-ok-conflicting-attributes",
		  "successful-ok-ignored-subscriptions",
		  "successful-ok-ignored-notifications",
		  "successful-ok-too-many-events",
		  "successful-ok-but-cancel-subscription",
		  "successful-ok-events-complete"
		},
		* const ipp_status_400s[] =	
		{
		  "client-error-bad-request",
		  "client-error-forbidden",
		  "client-error-not-authenticated",
		  "client-error-not-authorized",
		  "client-error-not-possible",
		  "client-error-timeout",
		  "client-error-not-found",
		  "client-error-gone",
		  "client-error-request-entity-too-large",
		  "client-error-request-value-too-long",
		  "client-error-document-format-not-supported",
		  "client-error-attributes-or-values-not-supported",
		  "client-error-uri-scheme-not-supported",
		  "client-error-charset-not-supported",
		  "client-error-conflicting-attributes",
		  "client-error-compression-not-supported",
		  "client-error-compression-error",
		  "client-error-document-format-error",
		  "client-error-document-access-error",
		  "client-error-attributes-not-settable",
		  "client-error-ignored-all-subscriptions",
		  "client-error-too-many-subscriptions",
		  "client-error-ignored-all-notifications",
		  "client-error-print-support-file-not-found"
		},
		* const ipp_status_500s[] =		
		{
		  "server-error-internal-error",
		  "server-error-operation-not-supported",
		  "server-error-service-unavailable",
		  "server-error-version-not-supported",
		  "server-error-device-error",
		  "server-error-temporary-error",
		  "server-error-not-accepting-jobs",
		  "server-error-busy",
		  "server-error-job-canceled",
		  "server-error-multiple-document-jobs-not-supported",
		  "server-error-printer-is-deactivated"
		};
static char	* const ipp_std_ops[] =
		{
		  
		  "unknown-00",
		  "unknown-01",
		  "Print-Job",
		  "Print-URI",
		  "Validate-Job",
		  "Create-Job",
		  "Send-Document",
		  "Send-URI",
		  "Cancel-Job",
		  "Get-Job-Attributes",
		  "Get-Jobs",
		  "Get-Printer-Attributes",
		  "Hold-Job",
		  "Release-Job",
		  "Restart-Job",
		  "unknown-0f",

		  "Pause-Printer",
		  "Resume-Printer",
		  "Purge-Jobs",
		  "Set-Printer-Attributes",
		  "Set-Job-Attributes",
		  "Get-Printer-Supported-Values",
		  "Create-Printer-Subscription",
		  "Create-Job-Subscription",
		  "Get-Subscription-Attributes",
		  "Get-Subscriptions",
		  "Renew-Subscription",
		  "Cancel-Subscription",
		  "Get-Notifications",
		  "Send-Notifications",
		  "unknown-1e",
		  "unknown-1f",

		  
		  "unknown-20",
		  "Get-Printer-Support-Files",
		  "Enable-Printer",
		  "Disable-Printer",
		  "Pause-Printer-After-Current-Job",
		  "Hold-New-Jobs",
		  "Release-Held-New-Jobs",
		  "Deactivate-Printer",
		  "Activate-Printer",
		  "Restart-Printer",
		  "Shutdown-Printer",
		  "Startup-Printer",
		  "Reprocess-Job",
		  "Cancel-Current-Job",
		  "Suspend-Current-Job",
		  "Resume-Job",

		  
		  "Promote-Job",
		  "Schedule-Job-After",
		  "unknown-32",
		  "Cancel-Document",
		  "Get-Document-Attributes",
		  "Get-Documents",
		  "Delete-Document",
		  "Set-Document-Attributes",
		  "Cancel-Jobs",
		  "Cancel-My-Jobs",
		  "Resubmit-Job",
		  "Close-Job"
		},
		* const ipp_tag_names[] =
		{			
		  "zero",		
		  "operation-attributes-tag",
					
		  "job-attributes-tag",	
		  "end-of-attributes-tag",
					
		  "printer-attributes-tag",
					
		  "unsupported-attributes-tag",
					
		  "subscription-attributes-tag",
					
		  "event-notification-attributes-tag",
					
		  "unknown-08",		
		  "unknown-09",		
		  "unknown-0a",		
		  "unknown-0b",		
		  "unknown-0c",		
		  "unknown-0d",		
		  "unknown-0e",		
		  "unknown-0f",		
		  "unsupported",	
		  "default",		
		  "unknown",		
		  "no-value",		
		  "unknown-14",		
		  "not-settable",	
		  "delete-attribute",	
		  "admin-define",	
		  "unknown-18",		
		  "unknown-19",		
		  "unknown-1a",		
		  "unknown-1b",		
		  "unknown-1c",		
		  "unknown-1d",		
		  "unknown-1e",		
		  "unknown-1f",		
		  "unknown-20",		
		  "integer",		
		  "boolean",		
		  "enum",		
		  "unknown-24",		
		  "unknown-25",		
		  "unknown-26",		
		  "unknown-27",		
		  "unknown-28",		
		  "unknown-29",		
		  "unknown-2a",		
		  "unknown-2b",		
		  "unknown-2c",		
		  "unknown-2d",		
		  "unknown-2e",		
		  "unknown-2f",		
		  "octetString",	
		  "dateTime",		
		  "resolution",		
		  "rangeOfInteger",	
		  "collection",		
		  "textWithLanguage",	
		  "nameWithLanguage",	
		  "endCollection",	
		  "unknown-38",		
		  "unknown-39",		
		  "unknown-3a",		
		  "unknown-3b",		
		  "unknown-3c",		
		  "unknown-3d",		
		  "unknown-3e",		
		  "unknown-3f",		
		  "unknown-40",		
		  "textWithoutLanguage",
		  "nameWithoutLanguage",
		  "unknown-43",		
		  "keyword",		
		  "uri",		
		  "uriScheme",		
		  "charset",		
		  "naturalLanguage",	
		  "mimeMediaType",	
		  "memberAttrName"	
		};
static const char * const job_states[] =
{					
  "pending",
  "pending-held",
  "processing",
  "processing-stopped",
  "canceled",
  "aborted",
  "completed"
};
static const char * const printer_states[] =
{					
  "idle",
  "processing",
  "stopped",
};

static size_t	ipp_col_string(ipp_t *col, char *buffer, size_t bufsize);

size_t					
_ippAttrString(ipp_attribute_t *attr,	
               char            *buffer,	
               size_t          bufsize)	
{
  int		i;			
  char		*bufptr,		
		*bufend,		
		temp[256];		
  const char	*ptr;			
  ipp_value_t	*val;			


  if (!attr || !attr->name)
  {
    if (buffer)
      *buffer = '\0';

    return (0);
  }

  bufptr = buffer;
  if (buffer)
    bufend = buffer + bufsize - 1;
  else
    bufend = NULL;

  for (i = attr->num_values, val = attr->values; i > 0; i --, val ++)
  {
    if (val > attr->values)
    {
      if (buffer && bufptr < bufend)
        *bufptr++ = ',';
      else
        bufptr ++;
    }

    switch (attr->value_tag & ~IPP_TAG_COPY)
    {
      case IPP_TAG_ENUM :
          if (!strcmp(attr->name, "printer-state") &&
              val->integer >= IPP_PRINTER_IDLE &&
              val->integer <= IPP_PRINTER_STOPPED)
          {
            ptr = printer_states[val->integer - IPP_PRINTER_IDLE];

            if (buffer && bufptr < bufend)
              strncpy(bufptr, ptr, bufend - bufptr + 1);

            bufptr += strlen(ptr);
            break;
          }
          else if (!strcmp(attr->name, "job-state") &&
		   val->integer >= IPP_JOB_PENDING &&
		   val->integer <= IPP_JOB_COMPLETED)
          {
            ptr = job_states[val->integer - IPP_JOB_PENDING];

            if (buffer && bufptr < bufend)
              strncpy(bufptr, ptr, bufend - bufptr + 1);

            bufptr += strlen(ptr);
            break;
          }

      case IPP_TAG_INTEGER :
          if (buffer && bufptr < bufend)
            bufptr += snprintf(bufptr, bufend - bufptr + 1, "%d", val->integer);
          else
            bufptr += snprintf(temp, sizeof(temp), "%d", val->integer);
          break;

      case IPP_TAG_BOOLEAN :
          if (buffer && bufptr < bufend)
            strncpy(bufptr, val->boolean ? "true" : "false",
                    bufend - bufptr + 1);

          bufptr += val->boolean ? 4 : 5;
          break;

      case IPP_TAG_RANGE :
          if (buffer && bufptr < bufend)
            bufptr += snprintf(bufptr, bufend - bufptr + 1, "%d-%d",
                               val->range.lower, val->range.upper);
          else
            bufptr += snprintf(temp, sizeof(temp), "%d-%d", val->range.lower,
                               val->range.upper);
          break;

      case IPP_TAG_RESOLUTION :
          if (buffer && bufptr < bufend)
            bufptr += snprintf(bufptr, bufend - bufptr + 1, "%dx%d%s",
                               val->resolution.xres, val->resolution.yres,
                               val->resolution.units == IPP_RES_PER_INCH ?
                                   "dpi" : "dpc");
          else
            bufptr += snprintf(temp, sizeof(temp), "%dx%d%s",
                               val->resolution.xres, val->resolution.yres,
                               val->resolution.units == IPP_RES_PER_INCH ?
                                   "dpi" : "dpc");
          break;

      case IPP_TAG_DATE :
          {
            unsigned year;		

            year = (val->date[0] << 8) + val->date[1];

	    if (val->date[9] == 0 && val->date[10] == 0)
	      snprintf(temp, sizeof(temp), "%04u-%02u-%02uT%02u:%02u:%02uZ",
		       year, val->date[2], val->date[3], val->date[4],
		       val->date[5], val->date[6]);
	    else
	      snprintf(temp, sizeof(temp),
	               "%04u-%02u-%02uT%02u:%02u:%02u%c%02u%02u",
		       year, val->date[2], val->date[3], val->date[4],
		       val->date[5], val->date[6], val->date[8], val->date[9],
		       val->date[10]);

            if (buffer && bufptr < bufend)
              strncpy(bufptr, temp, bufend - bufptr + 1);

            bufptr += strlen(temp);
          }
          break;

      case IPP_TAG_TEXT :
      case IPP_TAG_NAME :
      case IPP_TAG_KEYWORD :
      case IPP_TAG_CHARSET :
      case IPP_TAG_URI :
      case IPP_TAG_MIMETYPE :
      case IPP_TAG_LANGUAGE :
      case IPP_TAG_TEXTLANG :
      case IPP_TAG_NAMELANG :
	  if (!val->string.text)
	    break;

          for (ptr = val->string.text; *ptr; ptr ++)
          {
            if (*ptr == '\\' || *ptr == '\"')
            {
              if (buffer && bufptr < bufend)
                *bufptr = '\\';
              bufptr ++;
            }

            if (buffer && bufptr < bufend)
              *bufptr = *ptr;
            bufptr ++;
          }
          break;

      case IPP_TAG_BEGIN_COLLECTION :
          if (buffer && bufptr < bufend)
            bufptr += ipp_col_string(val->collection, bufptr,
                                     bufend - bufptr + 1);
          else
            bufptr += ipp_col_string(val->collection, NULL, 0);
          break;

      case IPP_TAG_STRING :
          for (ptr = val->string.text; *ptr; ptr ++)
          {
            if (*ptr == '\\' || isspace(*ptr))
            {
              if (buffer && bufptr < bufend)
                *bufptr = '\\';
              bufptr ++;

              if (buffer && bufptr < bufend)
                *bufptr = *ptr;
              bufptr ++;
            }
            else if (!isprint(*ptr & 255))
            {
              if (buffer && bufptr < bufend)
                bufptr += snprintf(bufptr, bufend - bufptr + 1, "\\%03o",
                                   *ptr & 255);
              else
                bufptr += snprintf(temp, sizeof(temp), "\\%03o",
                                   *ptr & 255);
            }
            else
            {
              if (buffer && bufptr < bufend)
                *bufptr = *ptr;
              bufptr ++;
            }
          }
          break;

      default :
          ptr = ippTagString(attr->value_tag);
          if (buffer && bufptr < bufend)
            strncpy(bufptr, ptr, bufend - bufptr + 1);
          bufptr += strlen(ptr);
          break;
    }
  }

  if (buffer && bufptr < bufend)
    *bufptr = '\0';
  else if (bufend)
    *bufend = '\0';

  return (bufptr - buffer);
}

const char *				
ippErrorString(ipp_status_t error)	
{
  char ipp_unknown[255] = {0};

  if (error >= IPP_OK && error <= IPP_OK_EVENTS_COMPLETE)
    return (ipp_status_oks[error]);
  else if (error == IPP_REDIRECTION_OTHER_SITE)
    return ("redirection-other-site");
  else if (error >= IPP_BAD_REQUEST && error <= IPP_PRINT_SUPPORT_FILE_NOT_FOUND)
    return (ipp_status_400s[error - IPP_BAD_REQUEST]);
  else if (error >= IPP_INTERNAL_ERROR && error <= IPP_PRINTER_IS_DEACTIVATED)
    return (ipp_status_500s[error - IPP_INTERNAL_ERROR]);

  sprintf(ipp_unknown, "unknown-%04x", error);

  return (ipp_unknown);
}

ipp_status_t				
ippErrorValue(const char *name)		
{
  int		i;

  for (i = 0; i < (sizeof(ipp_status_oks) / sizeof(ipp_status_oks[0])); i ++)
    if (!strcaseinsensitivecmp(name, ipp_status_oks[i]))
      return ((ipp_status_t)i);

  if (!strcaseinsensitivecmp(name, "redirection-other-site"))
    return (IPP_REDIRECTION_OTHER_SITE);

  for (i = 0; i < (sizeof(ipp_status_400s) / sizeof(ipp_status_400s[0])); i ++)
    if (!strcaseinsensitivecmp(name, ipp_status_400s[i]))
      return ((ipp_status_t)(i + 0x400));

  for (i = 0; i < (sizeof(ipp_status_500s) / sizeof(ipp_status_500s[0])); i ++)
    if (!strcaseinsensitivecmp(name, ipp_status_500s[i]))
      return ((ipp_status_t)(i + 0x500));

  return ((ipp_status_t)-1);
}

const char *				
ippOpString(ipp_op_t op)		
{
	char ipp_unknown[255] = {0};

  if (op >= IPP_PRINT_JOB && op <= IPP_CLOSE_JOB)
    return (ipp_std_ops[op]);

  sprintf(ipp_unknown, "unknown-%04x", op);

  return (ipp_unknown);
}

ipp_op_t				
ippOpValue(const char *name)		
{
  int		i;

  for (i = 0; i < (sizeof(ipp_std_ops) / sizeof(ipp_std_ops[0])); i ++)
    if (!strcaseinsensitivecmp(name, ipp_std_ops[i]))
      return ((ipp_op_t)i);

  return ((ipp_op_t)-1);
}

const char * ippTagString(ipp_tag_t tag)
{
  tag = (ipp_tag_t)(tag & IPP_TAG_MASK);

  if (tag < (ipp_tag_t)(sizeof(ipp_tag_names) / sizeof(ipp_tag_names[0])))
    return (ipp_tag_names[tag]);
  else
    return ("UNKNOWN");
}

ipp_tag_t				
ippTagValue(const char *name)		
{
  int	i;				


  for (i = 0; i < (sizeof(ipp_tag_names) / sizeof(ipp_tag_names[0])); i ++)
    if (!strcaseinsensitivecmp(name, ipp_tag_names[i]))
      return ((ipp_tag_t)i);

  if (!strcaseinsensitivecmp(name, "operation"))
    return (IPP_TAG_OPERATION);
  else if (!strcaseinsensitivecmp(name, "job"))
    return (IPP_TAG_JOB);
  else if (!strcaseinsensitivecmp(name, "printer"))
    return (IPP_TAG_PRINTER);
  else if (!strcaseinsensitivecmp(name, "unsupported"))
    return (IPP_TAG_UNSUPPORTED_GROUP);
  else if (!strcaseinsensitivecmp(name, "subscription"))
    return (IPP_TAG_SUBSCRIPTION);
  else if (!strcaseinsensitivecmp(name, "event"))
    return (IPP_TAG_EVENT_NOTIFICATION);
  else if (!strcaseinsensitivecmp(name, "language"))
    return (IPP_TAG_LANGUAGE);
  else if (!strcaseinsensitivecmp(name, "mimetype"))
    return (IPP_TAG_MIMETYPE);
  else if (!strcaseinsensitivecmp(name, "name"))
    return (IPP_TAG_NAME);
  else if (!strcaseinsensitivecmp(name, "text"))
    return (IPP_TAG_TEXT);
  else if (!strcaseinsensitivecmp(name, "begCollection"))
    return (IPP_TAG_BEGIN_COLLECTION);
  else
    return (IPP_TAG_ZERO);
}

static size_t				
ipp_col_string(ipp_t  *col,		
               char   *buffer,		
               size_t bufsize)		
{
  char			*bufptr,	
			*bufend,	
			temp[256];	
  ipp_attribute_t	*attr;		


  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  if (buffer && bufptr < bufend)
    *bufptr = '{';
  bufptr ++;

  for (attr = col->attrs; attr; attr = attr->next)
  {
    if (!attr->name)
      continue;

    if (buffer && bufptr < bufend)
      bufptr += snprintf(bufptr, bufend - bufptr + 1, "%s=", attr->name);
    else
      bufptr += strlen(attr->name) + 1;

    if (buffer && bufptr < bufend)
      bufptr += _ippAttrString(attr, bufptr, bufend - bufptr + 1);
    else
      bufptr += _ippAttrString(attr, temp, sizeof(temp));
  }

  if (buffer && bufptr < bufend)
    *bufptr = '}';
  bufptr ++;

  return (bufptr - buffer);
}

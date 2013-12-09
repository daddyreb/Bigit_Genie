#ifndef _IPP_H_
#define _IPP_H_
#include "http.h"
#define IPP_VERSION		"\002\001"
#define IPP_PORT 631

#  define IPP_MAX_LENGTH	32767	
#  define IPP_MAX_NAME		256	
#  define IPP_MAX_VALUES	8	

typedef enum ipp_tag_e			
{
  IPP_TAG_ZERO = 0x00,			
  IPP_TAG_OPERATION,			
  IPP_TAG_JOB,				
  IPP_TAG_END,				
  IPP_TAG_PRINTER,			
  IPP_TAG_UNSUPPORTED_GROUP,		
  IPP_TAG_SUBSCRIPTION,			
  IPP_TAG_EVENT_NOTIFICATION,		
  IPP_TAG_UNSUPPORTED_VALUE = 0x10,	
  IPP_TAG_DEFAULT,			
  IPP_TAG_UNKNOWN,			
  IPP_TAG_NOVALUE,			
  IPP_TAG_NOTSETTABLE = 0x15,		
  IPP_TAG_DELETEATTR,			
  IPP_TAG_ADMINDEFINE,			
  IPP_TAG_INTEGER = 0x21,		
  IPP_TAG_BOOLEAN,			
  IPP_TAG_ENUM,				
  IPP_TAG_STRING = 0x30,		
  IPP_TAG_DATE,				
  IPP_TAG_RESOLUTION,			
  IPP_TAG_RANGE,			
  IPP_TAG_BEGIN_COLLECTION,		
  IPP_TAG_TEXTLANG,			
  IPP_TAG_NAMELANG,			
  IPP_TAG_END_COLLECTION,		
  IPP_TAG_TEXT = 0x41,			
  IPP_TAG_NAME,				
  IPP_TAG_RESERVED_STRING,		
  IPP_TAG_KEYWORD,			
  IPP_TAG_URI,				
  IPP_TAG_URISCHEME,			
  IPP_TAG_CHARSET,			
  IPP_TAG_LANGUAGE,			
  IPP_TAG_MIMETYPE,			
  IPP_TAG_MEMBERNAME,			
  IPP_TAG_MASK = 0x7fffffff,		
  IPP_TAG_COPY = -0x7fffffff-1		
} ipp_tag_t;

typedef enum ipp_res_e			
{
  IPP_RES_PER_INCH = 3,			
  IPP_RES_PER_CM			
} ipp_res_t;

typedef enum ipp_finish_e		
{
  IPP_FINISHINGS_NONE = 3,		
  IPP_FINISHINGS_STAPLE,		
  IPP_FINISHINGS_PUNCH,			
  IPP_FINISHINGS_COVER,			
  IPP_FINISHINGS_BIND,			
  IPP_FINISHINGS_SADDLE_STITCH,		
  IPP_FINISHINGS_EDGE_STITCH,		
  IPP_FINISHINGS_FOLD,			
  IPP_FINISHINGS_TRIM,			
  IPP_FINISHINGS_BALE,			
  IPP_FINISHINGS_BOOKLET_MAKER,		
  IPP_FINISHINGS_JOB_OFFSET,		
  IPP_FINISHINGS_STAPLE_TOP_LEFT = 20,	
  IPP_FINISHINGS_STAPLE_BOTTOM_LEFT,	
  IPP_FINISHINGS_STAPLE_TOP_RIGHT,	
  IPP_FINISHINGS_STAPLE_BOTTOM_RIGHT,	
  IPP_FINISHINGS_EDGE_STITCH_LEFT,	
  IPP_FINISHINGS_EDGE_STITCH_TOP,	
  IPP_FINISHINGS_EDGE_STITCH_RIGHT,	
  IPP_FINISHINGS_EDGE_STITCH_BOTTOM,	
  IPP_FINISHINGS_STAPLE_DUAL_LEFT,	
  IPP_FINISHINGS_STAPLE_DUAL_TOP,	
  IPP_FINISHINGS_STAPLE_DUAL_RIGHT,	
  IPP_FINISHINGS_STAPLE_DUAL_BOTTOM,	
  IPP_FINISHINGS_BIND_LEFT = 50,	
  IPP_FINISHINGS_BIND_TOP,		
  IPP_FINISHINGS_BIND_RIGHT,		
  IPP_FINISHINGS_BIND_BOTTOM		
} ipp_finish_t;

typedef enum ipp_orient_e		
{
  IPP_PORTRAIT = 3,			
  IPP_LANDSCAPE,			
  IPP_REVERSE_LANDSCAPE,		
  IPP_REVERSE_PORTRAIT			
} ipp_orient_t;

typedef enum ipp_quality_e		
{
  IPP_QUALITY_DRAFT = 3,		
  IPP_QUALITY_NORMAL,			
  IPP_QUALITY_HIGH			
} ipp_quality_t;

typedef enum ipp_jstate_e		
{
  IPP_JOB_PENDING = 3,			
  IPP_JOB_HELD,				
  IPP_JOB_PROCESSING,			
  IPP_JOB_STOPPED,			
  IPP_JOB_CANCELED,			
  IPP_JOB_ABORTED,			
  IPP_JOB_COMPLETED			
} ipp_jstate_t;
#define IPP_JOB_CANCELLED IPP_JOB_CANCELED

typedef enum ipp_pstate_e		
{
  IPP_PRINTER_IDLE = 3,			
  IPP_PRINTER_PROCESSING,		
  IPP_PRINTER_STOPPED			
} ipp_pstate_t;

typedef enum ipp_state_e		
{
  IPP_ERROR = -1,			
  IPP_IDLE,				
  IPP_HEADER,				
  IPP_ATTRIBUTE,			
  IPP_DATA				
} ipp_state_t;

typedef enum ipp_op_e			
{
  IPP_PRINT_JOB = 0x0002,		
  IPP_PRINT_URI,			
  IPP_VALIDATE_JOB,			
  IPP_CREATE_JOB,			
  IPP_SEND_DOCUMENT,			
  IPP_SEND_URI,				
  IPP_CANCEL_JOB,			
  IPP_GET_JOB_ATTRIBUTES,		
  IPP_GET_JOBS,				
  IPP_GET_PRINTER_ATTRIBUTES,		
  IPP_HOLD_JOB,				
  IPP_RELEASE_JOB,			
  IPP_RESTART_JOB,			
  IPP_PAUSE_PRINTER = 0x0010,		
  IPP_RESUME_PRINTER,			
  IPP_PURGE_JOBS,			
  IPP_SET_PRINTER_ATTRIBUTES,		
  IPP_SET_JOB_ATTRIBUTES,		
  IPP_GET_PRINTER_SUPPORTED_VALUES,	
  IPP_CREATE_PRINTER_SUBSCRIPTION,	
  IPP_CREATE_JOB_SUBSCRIPTION,		
  IPP_GET_SUBSCRIPTION_ATTRIBUTES,	
  IPP_GET_SUBSCRIPTIONS,		
  IPP_RENEW_SUBSCRIPTION,		
  IPP_CANCEL_SUBSCRIPTION,		
  IPP_GET_NOTIFICATIONS,		
  IPP_SEND_NOTIFICATIONS,		
  IPP_GET_PRINT_SUPPORT_FILES = 0x0021,	
  IPP_ENABLE_PRINTER,			
  IPP_DISABLE_PRINTER,			
  IPP_PAUSE_PRINTER_AFTER_CURRENT_JOB,	
  IPP_HOLD_NEW_JOBS,			
  IPP_RELEASE_HELD_NEW_JOBS,		
  IPP_DEACTIVATE_PRINTER,		
  IPP_ACTIVATE_PRINTER,			
  IPP_RESTART_PRINTER,			
  IPP_SHUTDOWN_PRINTER,			
  IPP_STARTUP_PRINTER,			
  IPP_REPROCESS_JOB,			
  IPP_CANCEL_CURRENT_JOB,		
  IPP_SUSPEND_CURRENT_JOB,		
  IPP_RESUME_JOB,			
  IPP_PROMOTE_JOB,			
  IPP_SCHEDULE_JOB_AFTER,		
  IPP_CANCEL_DOCUMENT = 0x0033,		
  IPP_GET_DOCUMENT_ATTRIBUTES,		
  IPP_GET_DOCUMENTS,			
  IPP_DELETE_DOCUMENT,			
  IPP_SET_DOCUMENT_ATTRIBUTES,		
  IPP_CANCEL_JOBS,			
  IPP_CANCEL_MY_JOBS,			
  IPP_RESUBMIT_JOB,			
  IPP_CLOSE_JOB		
} ipp_op_t;

typedef enum ipp_status_e		
{
  IPP_OK = 0x0000,			
  IPP_OK_SUBST,				
  IPP_OK_CONFLICT,			
  IPP_OK_IGNORED_SUBSCRIPTIONS,		
  IPP_OK_IGNORED_NOTIFICATIONS,		
  IPP_OK_TOO_MANY_EVENTS,		
  IPP_OK_BUT_CANCEL_SUBSCRIPTION,	
  IPP_OK_EVENTS_COMPLETE,		
  IPP_REDIRECTION_OTHER_SITE = 0x200,		
  IPP_BAD_REQUEST = 0x0400,		
  IPP_FORBIDDEN,			
  IPP_NOT_AUTHENTICATED,		
  IPP_NOT_AUTHORIZED,			
  IPP_NOT_POSSIBLE,			
  IPP_TIMEOUT,				
  IPP_NOT_FOUND,			
  IPP_GONE,				
  IPP_REQUEST_ENTITY,			
  IPP_REQUEST_VALUE,			
  IPP_DOCUMENT_FORMAT,			
  IPP_ATTRIBUTES,			
  IPP_URI_SCHEME,			
  IPP_CHARSET,				
  IPP_CONFLICT,				
  IPP_COMPRESSION_NOT_SUPPORTED,	
  IPP_COMPRESSION_ERROR,		
  IPP_DOCUMENT_FORMAT_ERROR,		
  IPP_DOCUMENT_ACCESS_ERROR,		
  IPP_ATTRIBUTES_NOT_SETTABLE,		
  IPP_IGNORED_ALL_SUBSCRIPTIONS,	
  IPP_TOO_MANY_SUBSCRIPTIONS,		
  IPP_IGNORED_ALL_NOTIFICATIONS,	
  IPP_PRINT_SUPPORT_FILE_NOT_FOUND,	

  IPP_INTERNAL_ERROR = 0x0500,		
  IPP_OPERATION_NOT_SUPPORTED,		
  IPP_SERVICE_UNAVAILABLE,		
  IPP_VERSION_NOT_SUPPORTED,		
  IPP_DEVICE_ERROR,			
  IPP_TEMPORARY_ERROR,			
  IPP_NOT_ACCEPTING,			
  IPP_PRINTER_BUSY,			
  IPP_ERROR_JOB_CANCELED,		
  IPP_MULTIPLE_JOBS_NOT_SUPPORTED,	
  IPP_PRINTER_IS_DEACTIVATED			
} ipp_status_t;

typedef unsigned char ipp_uchar_t;	
typedef ssize_t	(*ipp_iocb_t)(void *, ipp_uchar_t *, size_t);
					
typedef union ipp_request_u		
{
  struct				
  {
    ipp_uchar_t	version[2];		
    int		op_status;		
    int		request_id;		
  }		any;

  struct				
  {
    ipp_uchar_t	version[2];		
    ipp_op_t	operation_id;		
    int		request_id;		
  }		op;

  struct				
  {
    ipp_uchar_t	version[2];		
    ipp_status_t status_code;		
    int		request_id;		
  }		status;

  
  struct				
  {
    ipp_uchar_t	version[2];		
    ipp_status_t status_code;		
    int		request_id;		
  }		event;
} ipp_request_t;


typedef struct ipp_s ipp_t;

typedef union ipp_value_u		
{
  int		integer;		

  char		boolean;		

  ipp_uchar_t	date[11];		

  struct
  {
    int		xres,			
		yres;			
    ipp_res_t	units;			
  }		resolution;		

  struct
  {
    int		lower,			
		upper;			
  }		range;			

  struct
  {
    char	*charset;		
    char	*text;			
  }		string;			

  struct
  {
    int		length;			
    void	*data;			
  }		unknown;		


  ipp_t		*collection;		
} ipp_value_t;

typedef struct ipp_attribute_s		
{
  struct ipp_attribute_s *next;		
  ipp_tag_t	group_tag,		
		value_tag;		
  char		*name;			
  int		num_values;		
  ipp_value_t	values[1];		
} ipp_attribute_t;

struct ipp_s				
{
  ipp_state_t	state;			
  ipp_request_t	request;		
  ipp_attribute_t *attrs;		
  ipp_attribute_t *last;		
  ipp_attribute_t *current;		
  ipp_tag_t	curtag;			


  ipp_attribute_t *prev;		


  int		use;			
};


/*
 * Prototypes...
 */

extern ipp_attribute_t	*ippAddBoolean(ipp_t *ipp, ipp_tag_t group,
			               const char *name, char value);
extern ipp_attribute_t	*ippAddBooleans(ipp_t *ipp, ipp_tag_t group,
			                const char *name, int num_values,
					const char *values);
extern ipp_attribute_t	*ippAddDate(ipp_t *ipp, ipp_tag_t group,
			            const char *name, const ipp_uchar_t *value);
extern ipp_attribute_t	*ippAddInteger(ipp_t *ipp, ipp_tag_t group,
			               ipp_tag_t type, const char *name,
				       int value);
extern ipp_attribute_t	*ippAddIntegers(ipp_t *ipp, ipp_tag_t group,
			                ipp_tag_t type, const char *name,
					int num_values, const int *values);
extern ipp_attribute_t	*ippAddRange(ipp_t *ipp, ipp_tag_t group,
			             const char *name, int lower, int upper);
extern ipp_attribute_t	*ippAddRanges(ipp_t *ipp, ipp_tag_t group,
			              const char *name, int num_values,
				      const int *lower, const int *upper);
extern ipp_attribute_t	*ippAddResolution(ipp_t *ipp, ipp_tag_t group,
			                  const char *name, ipp_res_t units,
					  int xres, int yres);
extern ipp_attribute_t	*ippAddResolutions(ipp_t *ipp, ipp_tag_t group,
			                   const char *name, int num_values,
					   ipp_res_t units, const int *xres,
					   const int *yres);
extern ipp_attribute_t	*ippAddSeparator(ipp_t *ipp);
extern ipp_attribute_t	*ippAddString(ipp_t *ipp, ipp_tag_t group,
			              ipp_tag_t type, const char *name,
				      const char *charset, const char *value);
extern ipp_attribute_t	*ippAddStrings(ipp_t *ipp, ipp_tag_t group,
			               ipp_tag_t type, const char *name,
				       int num_values, const char *charset,
				       const char * const *values);
extern time_t		ippDateToTime(const ipp_uchar_t *date);
extern void		ippDelete(ipp_t *ipp);
extern const char	*ippErrorString(ipp_status_t error);
extern ipp_attribute_t	*ippFindAttribute(ipp_t *ipp, const char *name,
			                  ipp_tag_t type);
extern ipp_attribute_t	*ippFindNextAttribute(ipp_t *ipp, const char *name,
			                      ipp_tag_t type);
extern size_t		ippLength(ipp_t *ipp);
extern ipp_t		*ippNew(void);
extern ipp_state_t	ippRead(http_t *http, ipp_t *ipp);
extern const ipp_uchar_t *ippTimeToDate(time_t t);
extern ipp_state_t	ippWrite(http_t *http, ipp_t *ipp);
extern int		ippPort(void);
extern void		ippSetPort(int p);

extern ipp_attribute_t	*ippAddCollection(ipp_t *ipp, ipp_tag_t group,
			                  const char *name, ipp_t *value) ;
extern ipp_attribute_t	*ippAddCollections(ipp_t *ipp, ipp_tag_t group,
			                   const char *name, int num_values,
					   const ipp_t **values) ;
extern void		ippDeleteAttribute(ipp_t *ipp, ipp_attribute_t *attr) ;
extern ipp_state_t	ippReadFile(int fd, ipp_t *ipp) ;
extern ipp_state_t	ippWriteFile(int fd, ipp_t *ipp) ;

extern ipp_attribute_t	*ippAddOctetString(ipp_t *ipp, ipp_tag_t group,
			                   const char *name,
					   const void *data, int datalen) ;
extern ipp_status_t	ippErrorValue(const char *name) ;
extern ipp_t		*ippNewRequest(ipp_op_t op) ;
extern const char	*ippOpString(ipp_op_t op) ;
extern ipp_op_t		ippOpValue(const char *name) ;
extern ipp_state_t	ippReadIO(void *src, ipp_iocb_t cb, int blocking,
			          ipp_t *parent, ipp_t *ipp) ;
extern ipp_state_t	ippWriteIO(void *dst, ipp_iocb_t cb, int blocking,
			           ipp_t *parent, ipp_t *ipp) ;

extern const char	*ippTagString(ipp_tag_t tag) ;
extern ipp_tag_t	ippTagValue(const char *name) ;

#endif

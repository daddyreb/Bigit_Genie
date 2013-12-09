#ifndef _IPPSERVER_H_
#define _IPPSERVER_H_
#include <winsock2.h>
#include <sys/stat.h>
#include <Winspool.h>
#include "dns_sd.h"
#include "ierrors.h"
#include "iapi.h"
#include "common.h"

enum _ipp_preasons_e			
{
  _IPP_PRINTER_NONE = 0x0000,		
  _IPP_PRINTER_OTHER = 0x0001,		
  _IPP_PRINTER_COVER_OPEN = 0x0002,	
  _IPP_PRINTER_INPUT_TRAY_MISSING = 0x0004,
					
  _IPP_PRINTER_MARKER_SUPPLY_EMPTY = 0x0008,
					
  _IPP_PRINTER_MARKER_SUPPLY_LOW = 0x0010,
					
  _IPP_PRINTER_MARKER_WASTE_ALMOST_FULL = 0x0020,
					
  _IPP_PRINTER_MARKER_WASTE_FULL = 0x0040,
					
  _IPP_PRINTER_MEDIA_EMPTY = 0x0080,	
  _IPP_PRINTER_MEDIA_JAM = 0x0100,	
  _IPP_PRINTER_MEDIA_LOW = 0x0200,	
  _IPP_PRINTER_MEDIA_NEEDED = 0x0400,	
  _IPP_PRINTER_MOVING_TO_PAUSED = 0x0800,
					
  _IPP_PRINTER_PAUSED = 0x1000,		
  _IPP_PRINTER_SPOOL_AREA_FULL = 0x2000,
  _IPP_PRINTER_TONER_EMPTY = 0x4000,	
  _IPP_PRINTER_TONER_LOW = 0x8000	
};
typedef unsigned int _ipp_preasons_t;	

typedef enum _ipp_media_class_e
{
  _IPP_GENERAL,				
  _IPP_PHOTO_ONLY,			
  _IPP_ENV_ONLY				
} _ipp_media_class_t;

const char * const media_supported[] =
{					
  "iso_a4_210x297mm",			
  "iso_a5_148x210mm",			
  "iso_a6_105x148mm",			
  "iso_dl_110x220mm",			
  "na_legal_8.5x14in",			
  "na_letter_8.5x11in",			
  "na_number-10_4.125x9.5in",		
  "na_index-3x5_3x5in",			
  "oe_photo-l_3.5x5in",			
  "na_index-4x6_4x6in",			
  "na_5x7_5x7in"			
};
const int media_col_sizes[][3] =
{					
  { 21000, 29700, _IPP_GENERAL },	
  { 14800, 21000, _IPP_PHOTO_ONLY },	
  { 10500, 14800, _IPP_PHOTO_ONLY },	
  { 11000, 22000, _IPP_ENV_ONLY },	
  { 21590, 35560, _IPP_GENERAL },	
  { 21590, 27940, _IPP_GENERAL },	
  { 10477, 24130, _IPP_ENV_ONLY },	
  {  7630, 12700, _IPP_PHOTO_ONLY },	
  {  8890, 12700, _IPP_PHOTO_ONLY },	
  { 10160, 15240, _IPP_PHOTO_ONLY },	
  { 12700, 17780, _IPP_PHOTO_ONLY }	
};

const char * const media_type_supported[] =
				      
{
  "auto",
  "cardstock",
  "envelope",
  "labels",
  "other",
  "photographic-glossy",
  "photographic-high-gloss",
  "photographic-matte",
  "photographic-satin",
  "photographic-semi-gloss",
  "stationery",
  "stationery-letterhead",
  "transparency"
};

typedef struct _ipp_job_s _ipp_job_t;

typedef struct _ipp_printer_s
{
  char		*name,		
			*directory,	
			*hostname,	
			*uri;		
  array_t		*uris;
  int			port;		
  size_t		urilen;		
  ipp_t			*attrs;		
  ipp_pstate_t		state;		
  _ipp_preasons_t	state_reasons;	
  array_t		*jobs;		
  _ipp_job_t		*active_job;	
  int			next_job_id;	
  rwlock_t	rwlock;		
} _ipp_printer_t;

struct _ipp_job_s			
{
  int			id;		
  char			*name,		
			*username,	
			*format;	
  ipp_jstate_t		state;		
  time_t		processing,	
			completed;	
  ipp_t			*attrs;		
  int			cancel;		
  char			*filename;	
  int			fd;		
  _ipp_printer_t	*printer;	
};

typedef struct _ipp_client_s		
{
  http_t		http;		
  ipp_t			*request,	
			*response;	
  time_t		start;		
  http_state_t		operation;	
  ipp_op_t		operation_id;	
  char			uri[1024];	
  http_addr_t		addr;		
  _ipp_printer_t	*printer;	
  _ipp_job_t		*job;		
} _ipp_client_t;


typedef struct
{
    char  name[450];
}PrinterAttr;

typedef struct
{
    PrinterAttr attr[40];
    int count;
}PrinterList;

typedef struct _register_printer_info_s
{
	PrinterList printerlist;
	DNSServiceRef	ipp_ref[40];
	int flag[40];
}register_printer_info_t;

void ippserver();
_ipp_printer_t	*createprinter();
void		clean_jobs(_ipp_printer_t *printer);
int		compare_jobs(_ipp_job_t *a, _ipp_job_t *b);
ipp_attribute_t	*copy_attribute(ipp_t *to, ipp_attribute_t *attr,
		                        ipp_tag_t group_tag, int quickcopy);
void		copy_attributes(ipp_t *to, ipp_t *from, array_t *ra,
			                ipp_tag_t group_tag, int quickcopy);
void		copy_job_attributes(_ipp_client_t *client,
			                    _ipp_job_t *job, array_t *ra);
_ipp_client_t	*create_client(_ipp_printer_t *printer, int sock);
_ipp_job_t	*create_job(_ipp_client_t *client);
int		create_listener(int family, int *port);
ipp_t		*create_media_col(const char *media, const char *type,
					  int width, int length, int margins);
_ipp_printer_t	*create_printer(const char *servername,
								const char *name, const char *location,
								const char *make, const char *model,
								const char *docformats, int ppm,
								int ppm_color, int duplex, int port,
								const char *directory);
array_t	*create_requested_array(_ipp_client_t *client);
void		debug_attributes(const char *title, ipp_t *ipp);
void		delete_client(_ipp_client_t *client);
void		delete_job(_ipp_job_t *job);
void		delete_printer(_ipp_printer_t *printer);
_ipp_job_t	*find_job(_ipp_client_t *client);
void		html_escape(_ipp_client_t *client, const char *s,size_t slen);
void		html_printf(_ipp_client_t *client, const char *format, ...);
void		ipp_cancel_job(_ipp_client_t *client);
void		ipp_get_job_attributes(_ipp_client_t *client);
void		ipp_get_jobs(_ipp_client_t *client);
void		ipp_get_printer_attributes(_ipp_client_t *client);
void		ipp_print_job(_ipp_client_t *client);
void		ipp_validate_job(_ipp_client_t *client);
void		*process_client(_ipp_client_t *client);
int		process_http(_ipp_client_t *client);
int		process_ipp(_ipp_client_t *client);
void		*process_job(_ipp_job_t *job);
int		respond_http(_ipp_client_t *client, http_status_t code,const char *type, size_t length);
void		respond_ipp(_ipp_client_t *client, ipp_status_t status,const char *message, ...);
void		run_printer(_ipp_printer_t *printer);
int		valid_job_attributes(_ipp_client_t *client);
#endif 

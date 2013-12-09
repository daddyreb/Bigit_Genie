#ifndef _COMMON_H_
#define _COMMON_H_

#include "string-private.h"
#include "http-private.h"
#include "ipp-private.h"
#include "thread-private.h"

#define access		_access
#define close		_close
#define fileno		_fileno
#define lseek		_lseek
#define mkdir(d,p)	_mkdir(d)
#define open		_open
#define read	        _read
#define rmdir		_rmdir
#define snprintf 	_snprintf
#define strdup		_strdup
#define unlink		_unlink
#define vsnprintf 	_vsnprintf
#define write		_write
#define getpid		_getpid

#define F_OK		00
#define W_OK		02
#define R_OK		04
#define O_RDONLY	_O_RDONLY
#define O_WRONLY	_O_WRONLY
#define O_CREATE	_O_CREAT
#define O_TRUNC	_O_TRUNC
#define O_BINARY _O_BINARY

#define DEBUG_puts(x)
#define DEBUG_printf(x)
#endif //_COMMON_H_
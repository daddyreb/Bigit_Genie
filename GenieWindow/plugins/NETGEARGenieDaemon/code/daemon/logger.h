#ifndef __logger_h__
#define __logger_h__

#if defined(GEINE_DAEMON_ENABLE_LOGGER)
#define LOGGER_INIT()         GDL_Init()
#define LOGGER_DEBUG(_x_)     GDL_Debug _x_
#define LOGGER_TERM()         GDL_Term()
#else
#define LOGGER_INIT()         (void)0
#define LOGGER_DEBUG(_x_)     (void)0
#define LOGGER_TERM()         (void)0
#endif

void GDL_Init();
void GDL_Debug(const char *fmt, ...);
void GDL_Term();

#endif // __logger_h__

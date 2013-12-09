#ifndef __logger_h__
#define __logger_h__

#include <QString>

#define LOG_DEBUG       0
#define LOG_INFO        1
#define LOG_ERROR       2

#ifdef LPCMANAGER_ENABLE_LOGGER
#define ENABLE_LOGGER 1
#endif

#ifdef ENABLE_LOGGER

void __LoggerInit();
void __LoggerTerm();

#define DBGLOGINIT        __LoggerInit
#define DBGLOGTERM        __LoggerTerm

#if defined(Q_CC_MSVC) || defined(Q_CC_GNU)
#define USE_LOGGER_AGENT
#endif

#ifdef USE_LOGGER_AGENT

class LoggerAgentImpl;

class LoggerAgent
{
public:
	LoggerAgent(const LoggerAgent& other);
	LoggerAgent(const char *func, const char *file, int line);
	~LoggerAgent();
	void operator()(int category, int level, const QString& text);

private:
	LoggerAgent();

	friend class LoggerAgentImpl;
	LoggerAgentImpl *m_impl;
};

LoggerAgent __LoggerOutput(const char *func, const char *file, int line);

//#ifdef Q_CC_GNU
//#define DBGLOG            __LoggerOutput(__PRETTY_FUNCTION__, __FILE__, __LINE__)
//#else
#define DBGLOG            __LoggerOutput(__FUNCTION__, __FILE__, __LINE__)
//#endif

#else

void __LoggerOutput(int category, int level, const QString& text);

#define DBGLOG            __LoggerOutput

#endif // USE_LOGGER_AGENT

#else

#define DBGLOGINIT()
#define DBGLOGTERM()
#define DBGLOG(a, b, c)

#endif // ENABLE_LOGGER

#endif // __logger_h__

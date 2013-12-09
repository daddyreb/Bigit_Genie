#include "logger.h"
#include <stdio.h>
#include <QDebug>

#ifdef ENABLE_LOGGER

const int CATEGORY_MAX = 10;
static int g_categoryLevel[CATEGORY_MAX];
const char *g_categoryName[] = {
	"DBG", "INF", "ERR"
};

FILE *g_loggerFP = NULL;

#if defined(Q_OS_WIN32)

#include <time.h>
#include <windows.h>
#include <shlobj.h>

static QString platformPrefix(int category, int level)
{
	QString prefix;

#ifdef Q_CC_MSVC
	char tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_s(&tm1, &t1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", &tm1);

	char buf2[512];
	if (category < ARRAYSIZE(g_categoryName)) {
		sprintf_s(buf2, "%s %s[0x%04lx][0x%04lx]", tmbuf, g_categoryName[category], GetCurrentProcessId(), GetCurrentThreadId());
	} else {
		sprintf_s(buf2, "%s C%02d[0x%04lx][0x%04lx]", tmbuf, level, GetCurrentProcessId(), GetCurrentThreadId());
	}

	prefix = QString::fromUtf8(buf2);

#else
	char tmbuf[256];
	tm *tm1;
	time_t t1;
	time(&t1);
	tm1 = localtime(&t1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", tm1);

	char buf2[512];
	if (category < static_cast<int>(sizeof(g_categoryName) / sizeof(g_categoryName[0]))) {
		sprintf(buf2, "%s %s[0x%04lx][0x%04lx]", tmbuf, g_categoryName[category], GetCurrentProcessId(), GetCurrentThreadId());
	} else {
		sprintf(buf2, "%s C%02d[0x%04lx][0x%04lx]", tmbuf, level, GetCurrentProcessId(), GetCurrentThreadId());
	}

	prefix = QString::fromUtf8(buf2);

#endif
	return prefix;
}

static void platformOutput(const QString& text)
{
	if (g_loggerFP) {
		QString t = text;
		t.append(QString::fromUtf8("\r\n"));

#if defined(Q_CC_MSVC) && !defined(NDEBUG)
		OutputDebugStringW(t.utf16());
#endif

		QByteArray tb = t.toUtf8();
		fwrite(tb.data(), 1, tb.size(), g_loggerFP);
		fflush(g_loggerFP);
	}
}

static FILE *platformFileOpen(const char *name)
{
#ifndef SHGFP_TYPE_DEFAULT
#define SHGFP_TYPE_DEFAULT 1
#endif // SHGFP_TYPE_DEFAULT

	HRESULT hr;
	WCHAR szLoggerPath[MAX_PATH];
	hr = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szLoggerPath);
	if (FAILED(hr)) {
		return NULL;
	}

	QString path = QString::fromUtf16(reinterpret_cast<ushort*>(szLoggerPath));
	path.append(QString::fromUtf8("\\"));
	path.append(QString::fromUtf8(name));
	path.append(QString::fromUtf8(".log"));

	FILE *fp = NULL;

#ifdef Q_CC_MSVC
	fp = _wfsopen(path.utf16(), L"ab", _SH_DENYNO);
#else
	fp = fopen(path.toUtf8().data(), "ab");
#endif

	return fp;
}

#elif defined(Q_OS_UNIX)

#include <unistd.h>
#include <pthread.h>

static QString platformPrefix(int category, int level)
{
	QString prefix;

	char tmbuf[256];
	tm *tm1;
	time_t t1;
	time(&t1);
	tm1 = localtime(&t1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", tm1);

	char buf2[512];
	if (category < static_cast<int>(sizeof(g_categoryName) / sizeof(g_categoryName[0]))) {
		sprintf(buf2, "%s %s[0x%08x][%p]", tmbuf, g_categoryName[category], getpid(), pthread_self());
	} else {
		sprintf(buf2, "%s C%02d[0x%08x][%p]", tmbuf, level, getpid(), pthread_self());
	}

	prefix = QString::fromUtf8(buf2);
	return prefix;
}

static void platformOutput(const QString& text)
{
	if (g_loggerFP) {
		QByteArray t = text.toUtf8();
		t.push_back('\n');
		fwrite(t.data(), 1, t.size(), g_loggerFP);
		fflush(g_loggerFP);
	}
}

static FILE *platformFileOpen(const char *name)
{
	QByteArray path("/var/tmp/");
	path.append(name);
	path.append(".log");
	return fopen(path.data(), "ab");
}

#endif

void __LoggerInit()
{
	for (int i = 0; i < CATEGORY_MAX; i++) {
		g_categoryLevel[i] = 4096;
	}

	if (!g_loggerFP) {
		g_loggerFP = platformFileOpen("LPCManager");
	}
}

void __LoggerTerm()
{
	if (g_loggerFP) {
		fclose(g_loggerFP);
		g_loggerFP = NULL;
	}
}

#ifdef USE_LOGGER_AGENT

class LoggerAgentImpl
{
public:
	LoggerAgentImpl(const char *func, const char *file, int line)
	: m_func(QString::fromAscii(func)), m_file(QString::fromAscii(file)), m_line(line)
	{
	}

	QString m_func;
	QString m_file;
	int m_line;
};

LoggerAgent::LoggerAgent(const char *func, const char *file, int line)
{
	m_impl = new LoggerAgentImpl(func, file, line);
}

LoggerAgent::~LoggerAgent()
{
	delete m_impl;
}

void LoggerAgent::operator()(int category, int level, const QString& text)
{
	if (category >= CATEGORY_MAX) {
		return;
	}

	if (g_categoryLevel[category] < level) {
		return;
	}

	QString prefix = platformPrefix(category, level);
	QString output = QString::fromUtf8("%1[%2][%4:%5] %3");
	platformOutput(output.arg(prefix).arg(m_impl->m_func.rightJustified(60)).arg(text).arg(m_impl->m_file).arg(m_impl->m_line));
}

LoggerAgent __LoggerOutput(const char *func, const char *file, int line)
{
	return LoggerAgent(func, file, line);
}

#else

void __LoggerOutput(int category, int level, const QString& text)
{
	if (category >= CATEGORY_MAX) {
		return;
	}

	if (g_categoryLevel[category] < level) {
		return;
	}

	QString prefix = platformPrefix(category, level);
	QString output = QString::fromUtf8("%1 %2");
	platformOutput(output.arg(prefix).arg(text));
}

#endif // USE_LOGGER_AGENT

#endif // ENABLE_LOGGER

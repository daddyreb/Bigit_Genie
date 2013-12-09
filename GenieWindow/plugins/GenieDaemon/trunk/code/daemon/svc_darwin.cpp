#include "svc_darwin.h"
#include "logger.h"
#include "thread.h"
#include "daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void signal_handler(int sig);

int console_main(int argc, char **argv);
int ak_main(int argc, char **argv);
int daemon_main(int argc, char **argv);

//pthread_cond_t g_condStop;
//pthread_mutex_t g_mutexStop;

int main(int argc, char *argv[])
{
#if !defined(GENIE_DAEMON_TEST_CONSOLE)
	pid_t pid1 = fork();
	if (pid1 == 0) {
		setsid();

		pid_t pid2 = fork();
		if (pid2 == 0) {
			return daemon_main(argc, argv);
		} else if (pid2 > 0) {
			// parent
			exit(0);
		} else {
			// error
			return 3;
		}
	} else if (pid1 > 0) {
		// parent
		exit(0);
	} else {
		// error
		return 2;
	}

	// never
	return 0;
#else
	return daemon_main(argc, argv);
#endif
}

int daemon_main(int argc, char **argv)
{
	LOGGER_INIT();
	int retval = ak_main(argc, argv);
	LOGGER_DEBUG(("ak_main return: %d", retval));
	LOGGER_TERM();
	return retval;
}

bool g_quit_flag = false;

int ak_main(int argc, char **argv)
{
//	pthread_mutex_init(&g_mutexStop, NULL);
//	pthread_cond_init(&g_condStop, NULL);

	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGPIPE, signal_handler);

	GlobalLock glock("NETGEARGenieDaemon");
	if (!glock.acquire()) {
		LOGGER_DEBUG(("instance already running!"));
		return 11;
	}

	GenieDaemon *daemon = GenieDaemon::create();
	if (!daemon) {
		LOGGER_DEBUG(("out of memory!"));
		return 2;
	}

	if (!daemon->start()) {
		LOGGER_DEBUG(("failed to start daemon!"));
		return 3;
	}

	// stupid
	int cnt = 0;
	while (!g_quit_flag) {
		if (++cnt == 5) {
			cnt = 0;
			daemon->check();
		}
		GTF::sleep(1000);
	}

	daemon->stop();
	delete daemon;

//	pthread_mutex_lock(&g_mutexStop);
//	LOGGER_DEBUG(("wait in"));
//	int n = pthread_cond_wait(&g_condStop, &g_mutexStop);
//	LOGGER_DEBUG(("wait out %d %d", n, errno));
//	pthread_mutex_unlock(&g_mutexStop);

//	pthread_mutex_destroy(&g_mutexStop);
//	pthread_cond_destroy(&g_condStop);
	return 0;
}

void signal_handler(int sig)
{
	LOGGER_DEBUG(("signal_handler: %d", sig));
	if (sig == SIGTERM) {
		g_quit_flag = true;
//		pthread_cond_signal(&g_condStop);
	}
}

static FILE *g_gdl_fp = NULL;

void GDL_Init()
{
	if (g_gdl_fp) {
		return;
	}

	if ((g_gdl_fp = fopen("/tmp/NETGEARGenieDaemon.log", "ab")) == NULL) {
		return;
	}

	char tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_r(&t1, &tm1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", &tm1);
	fprintf(g_gdl_fp, "=================================\n geniedaemon %s\n=================================\n", tmbuf);
	fflush(g_gdl_fp);

}

void GDL_Debug(const char *fmt, ...)
{
	if (!g_gdl_fp) {
		return;
	}

	char tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_r(&t1, &tm1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", &tm1);

	va_list args;
	va_start(args, fmt);

	char linebuf[2048];
	int len = vsprintf(linebuf, fmt, args);
	linebuf[len] = 0;

	fprintf(g_gdl_fp, "%s DBG[%d][%p] %s\n", tmbuf, getpid(), pthread_self(), linebuf);
	fflush(g_gdl_fp);

	va_end(args);
}

void GDL_Term()
{
	if (g_gdl_fp) {
		fprintf(g_gdl_fp, "\n\n\n");
		fclose(g_gdl_fp);
		g_gdl_fp = NULL;
	}
}

GlobalLock::GlobalLock(const char *name)
: m_name(name), m_fd(-1)
{
}

GlobalLock::~GlobalLock()
{
	release();
}

bool GlobalLock::acquire()
{
	LOGGER_DEBUG(("acquiring lock [%s]", m_name.c_str()));
	if (acquireImpl()) {
		LOGGER_DEBUG(("acauired"));
		return true;
	}
	LOGGER_DEBUG(("lock already acquired!"));
	return false;
}

void GlobalLock::release()
{
	if (m_fd != -1) {
		LOGGER_DEBUG(("releasing lock [%s]", m_name.c_str()));
		releaseImpl();
		LOGGER_DEBUG(("lock released"));
	}
}

bool GlobalLock::acquireImpl()
{
	m_fd = open(fileName().c_str(), O_RDWR | O_CREAT | O_EXLOCK | O_NONBLOCK);
	if (m_fd == -1) {
		return false;
	}

	return true;
}

void GlobalLock::releaseImpl()
{
	if (m_fd != -1) {
		close(m_fd);
		m_fd = 0;
		unlink(fileName().c_str());
	}
}

std::string GlobalLock::fileName() const
{
	std::string path("/tmp/");
	path.append(m_name);
	path.append(".pid");
	return path;
}

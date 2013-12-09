#include "SingleInstance.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *lockfile_name)
{
    assert ( lockfile_name != NULL );

    int     fd;
    char    buf[16];

    fd = open(lockfile_name, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        syslog(LOG_ERR, "can't open %s: %s", lockfile_name, strerror(errno));
        exit(1);
    }

    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "can't lock %s: %s", lockfile_name, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);

    return(0);
}



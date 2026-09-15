#include "apue.h"
#include <errno.h>        /* for definition of errno */
#include <stdarg.h>       /* ISO C variable arguments */

static void err_doit(int , int, const char *, va_list);

/*
 * Nonfatal error related to a system call
 * Print a message and return.
 */
void err_ret(const char *fmt, ...)
{
    va_list    ap;
    err_doit(1, errno, fmt, ap);
    va_end(ap);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate
 */
void
err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}


/*
 * Fatel error unrelated to a system call
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void
err_exit(int error, const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, error, fmt, ap);
     va_end(ap);
     exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void
err_dump(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, errno, fmt, ap);
     va_end(ap);
     abort();
     exit(1);
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
err_msg(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(0, 0, fmt, ap);
     va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */

void
err_quit(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(0, 0, fmt, ap);
     va_end(ap);
     exit(1);
}

/*
 * Print a message and return to caller.
 * Caller specified "errnoflag"
 */
static void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
    char buf[MAXLINE];
    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag)
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",
                strerror(error));
    strcat(buf, "\n");
    fflush(stdout);  /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(NULL);    /* flushes all stdio output streams */
}


void
set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int        val;

    if ( (val = fcntl(fd, F_GETFL, 0)) < 0 )
    {
        printf("fcntl F_GETFL error");
        exit(1);

    }

    val |= flags;        /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0)
    {
        printf("fcntl F_SETFL error");
        exit(1);

    }

}

void
clr_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) == -1)
    {
        printf("fcntl F_CLRFL error");
        //syslog(LOG_ERR, __FILE__, __LINE__,"fcntl() error : %s", strerror(errno));
        exit(1);

    }
    val &= ~flags; /* turn flags off */

    if (fcntl(fd, F_SETFL, val) == -1)
    {
        printf("fcntl F_CLR_FL error");
        //syslog(LOG_ERR, __FILE__, __LINE__,"fcntl() error : %s", strerror(errno));
        exit(1);

    }
    return;

}


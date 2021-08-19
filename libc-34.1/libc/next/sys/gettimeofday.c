/*
 * gettimeofday.c - because it couldn't be done
 * (c) 1990 NeXT, Inc.
 */

#include <syscall.h>
#include <time.h>
#include <tzfile.h>
#include <sys/time.h>
#include <errno.h>

gettimeofday (struct timeval *tp, struct timezone *tzp)
{
	extern int errno;
	static int validtz = 0;
	static struct timezone cached_tz;
	struct tm *localtm;

	if (syscall (SYS_gettimeofday, tp, tzp) < 0) {
		return (-1);
	}
	if (validtz == 0)  {
		localtm = localtime ((time_t *)&tp->tv_sec);
		cached_tz.tz_dsttime = localtm->tm_isdst;
		cached_tz.tz_minuteswest = 
			(-localtm->tm_gmtoff / SECS_PER_MIN) +
			(localtm->tm_isdst * MINS_PER_HOUR);
		validtz = 1;
	}
	if (tzp) {
	  tzp->tz_dsttime = cached_tz.tz_dsttime;
	  tzp->tz_minuteswest = cached_tz.tz_minuteswest;
	}
	return (0);
}

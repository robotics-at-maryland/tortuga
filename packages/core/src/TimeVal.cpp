/*  XFC: Xfce Foundation Classes (Core Library)
 *  Copyright (C) 2004 The XFC Development Team.
 *
 *  timeval.cc - GTimeVal C++ wrapper implentation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

// STD Includes
#include <cstdlib>
#include <cassert>

// Project Includes
#include "core/include/TimeVal.h"

// On windows we need our own gettimeofday
#ifdef RAM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

#define EPOCHFILETIME (116444736000000000LL)
#else 
#include <unistd.h> // for sleep
#endif // RAM_WINDOWS

namespace ram {
namespace core {
  
#ifdef RAM_WINDOWS  
__inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME        ft;
    LARGE_INTEGER   li;
    __int64         t;
    static int      tzflag;

    if (tv)
    {
        GetSystemTimeAsFileTime(&ft);
        li.LowPart  = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart;       /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;     /* Offset to the Epoch time */
        t /= 10;                /* In microseconds */
        tv->tv_sec  = (long)(t / 1000000);
        tv->tv_usec = (long)(t % 1000000);
    }

    if (tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        long timezone = 0;
        _get_timezone(&timezone);
        tz->tz_minuteswest = (int)(timezone / 60);
         _get_daylight(&tz->tz_dsttime);
    }

    return 0;
}
#endif // RAM_WINDOWS
    
// Adapted from the avahi library, under LPGL (replace g_time_val_add)
struct timeval* time_val_add(struct timeval *a, long usec) {
    long u;
    assert(a);

    u = usec + a->tv_usec;

    if (u < 0) {
        a->tv_usec = (long) (1000000 + (u % 1000000));
        a->tv_sec += (long) (-1 + (u / 1000000));
    } else {
        a->tv_usec = (long) (u % 1000000);
        a->tv_sec += (long) (u / 1000000);
    }

    return a;
}
    
TimeVal::TimeVal()
{
	timeval_.tv_sec = 0;
	timeval_.tv_usec = 0;
}

TimeVal::TimeVal(long seconds, long microseconds)
{
	timeval_.tv_sec = seconds;
	timeval_.tv_usec = 0;
	time_val_add(time_val(), microseconds);
}

TimeVal::TimeVal(const double& timeval)
{
	timeval_.tv_sec = (long)timeval;
	timeval_.tv_usec = (long)((timeval - ((double)timeval_.tv_sec)) * USEC_PER_SEC);
	if (timeval_.tv_usec >= USEC_PER_SEC)
	{
		timeval_.tv_sec += timeval_.tv_usec / USEC_PER_SEC;
		timeval_.tv_usec %= USEC_PER_SEC;
	}
}

TimeVal::TimeVal(const struct timeval& timeval)
{
	timeval_.tv_sec = timeval.tv_sec;
	timeval_.tv_usec = timeval.tv_usec;
}

TimeVal TimeVal::timeOfDay()
{
    TimeVal current;
    current.now();
    return current;
}
    
void TimeVal::now()
{
    gettimeofday(time_val(), NULL);
}
    
void TimeVal::sleep(long seconds)
{
#ifdef RAM_POSIX
    sleep(seconds);
#else
    Sleep(seconds * 1000);
#endif
}
    
inline bool
TimeVal::equal(const TimeVal& other) const
{
	return timeval_.tv_sec == other.seconds() && timeval_.tv_usec == other.microseconds();
}

bool
TimeVal::operator<(const TimeVal& other)
{
	if (timeval_.tv_sec < other.seconds())
		return true;
	else if (timeval_.tv_sec == other.seconds() && timeval_.tv_usec < other.microseconds())
		return true;
	else
		return false;
}

bool
TimeVal::operator>(const TimeVal& other)
{
	if (timeval_.tv_sec > other.seconds())
		return true;
	else if (timeval_.tv_sec == other.seconds() && timeval_.tv_usec > other.microseconds())
		return true;
	else
		return false;
}

inline void
TimeVal::add(long microseconds)
{
	time_val_add(const_cast<struct timeval*>(&timeval_), microseconds);
}

void
TimeVal::add(long seconds, long microseconds)
{
	timeval_.tv_sec += seconds;
	time_val_add(time_val(), microseconds);
}

void
TimeVal::add(const TimeVal& other)
{
	timeval_.tv_sec += other.seconds();
	time_val_add(time_val(), other.microseconds());
}

void
TimeVal::subtract(const TimeVal& other)
{
	timeval_.tv_sec -= other.seconds();
	if (timeval_.tv_usec < other.microseconds())
	{
		timeval_.tv_sec--;
		timeval_.tv_usec += USEC_PER_SEC;
	}
	timeval_.tv_usec -= other.microseconds();
}

long
TimeVal::difference(const TimeVal& other)
{
	return ((timeval_.tv_sec - other.seconds()) * USEC_PER_SEC) + (timeval_.tv_usec - other.microseconds());
}

TimeVal&
TimeVal::operator+=(long microseconds)
{
	add(microseconds);
	return *this;
}

TimeVal&
TimeVal::operator+=(const TimeVal& other)
{
	add(other);
	return *this;
}

TimeVal&
TimeVal::operator-=(long microseconds)
{
	add(-microseconds);
	return *this;
}

TimeVal&
TimeVal::operator-=(const TimeVal& other)
{
	subtract(other);
	return *this;
}

} // namespace core
} // namespace ram

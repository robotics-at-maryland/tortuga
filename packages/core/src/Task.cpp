
// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/Task.h"

// System Includes
#ifdef RAM_POSIX
#include <unistd.h>
#else
#include <windows.h> // For Sleep()
#endif // RAM_POSIX

#include <iostream>

const static long USEC_PER_MILLISEC = 1000;
const static long NSEC_PER_MILLISEC = 1000000;
const static long NSEC_PER_USEC = 1000;

// How close do we try to get to actual sleep time (in usec)
const static long SLEEP_THRESHOLD = 500;

namespace ram {
namespace core {


typedef boost::int64_t Usec;

// Adapted from the avahi library, under LPGL


Usec timeval_diff(const struct timeval *a, const struct timeval *b) {
    assert(a);
    assert(b);

    if (timeval_compare(a, b) < 0)
        return - timeval_diff(b, a);

    return ((Usec) a->tv_sec - b->tv_sec)*1000000 + a->tv_usec - b->tv_usec;
}

struct timeval* timeval_add(struct timeval *a, Usec usec) {
    Usec u;
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

Usec age(const struct timeval *a) {
    struct timeval now;
    
    assert(a);

    gettimeofday(&now, NULL);

    return timeval_diff(&now, a);
}

struct timeval *elapse_time(struct timeval *tv, unsigned msec) {
    assert(tv);

    gettimeofday(tv, NULL);

    if (msec)
        timeval_add(tv, (Usec) msec*1000);
        
    return tv;
}

    
Task::Task(long updateRate, int priority, bool repeat) :
    m_updateRate(updateRate),
	m_priority(priority),
	m_repeat(repeat),
	m_enabled(true),
	m_totalRunTime(0),
	m_totalOffTime(0),
	m_runs(0)
{
    gettimeofday(&m_nextRunTime, NULL);
	//timeval_add(&m_nextRunTime, m_updateRate);
	gettimeofday(&m_lastRunEnd, NULL);
}

Task::~Task()
{
}

void Task::startUpdate()
{
	gettimeofday(&m_lastRunStart, NULL);
	m_totalOffTime += timeval_diff(&m_lastRunStart, &m_lastRunEnd);
}

void Task::endUpdate()
{
	gettimeofday(&m_lastRunEnd, NULL);
	m_totalRunTime += timeval_diff(&m_lastRunEnd, &m_lastRunStart);
	m_nextRunTime = m_lastRunEnd;
	timeval_add(&m_nextRunTime, m_updateRate);
	m_runs ++;
}



timeval Task::getNextRunTime()
{
	return m_nextRunTime;
}
    
long Task::getTotalRunTime()
{
	return m_totalRunTime;
}

long Task::getTotalOffTime()
{
	return m_totalOffTime;
}

long Task::getAverageRunTime()
{
	if (m_runs == 0)
		return 0;
	else
		return m_totalRunTime / m_runs;
}

long Task::getAverageOffTime()
{
	if (m_runs == 0)
		return 0;
	else
		return m_totalOffTime / m_runs;
}
    
} // namespace core     
} // namespace ram

// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/Process.h"

namespace ram {
namespace core {

int timeval_compare(const struct timeval *a, const struct timeval *b) {
    assert(a);
    assert(b);

    if (a->tv_sec < b->tv_sec)
        return -1;

    if (a->tv_sec > b->tv_sec)
        return 1;

    if (a->tv_usec < b->tv_usec)
        return -1;

    if (a->tv_usec > b->tv_usec)
        return 1;

    return 0;
}

Process::Process(int core) : m_core(core), m_started(false)
{
}

Process::~Process()
{
	stop();
}

void Process::addTask(Task *t)
{
	m_queue.push(t);
}

void Process::start()
{
	if (!m_started)
	{
		m_started = true;
		m_thread = new boost::thread(boost::bind(&Process::run,this));
	}
}

void Process::stop()
{
	m_started = false;
}

void Process::run()
{
	while (m_started)
	{
		if (!m_queue.empty())
		{
			Task *nextTask = m_queue.top();
			m_queue.pop();
			
			while (1)
			{
				timeval now;
				gettimeofday(&now, NULL);
				if (timeval_compare(now, nextTask->getNextRunTime()) >= 0)
					break;
			}
			
			nextTask->update();
			
			if (nextTask->getRepeat())
				m_queue.push(nextTask);
		}
	}
}


}
}
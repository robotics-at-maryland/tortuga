// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <iostream>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/Process.h"

namespace ram {
namespace core {


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
				timeval next = nextTask->getNextRunTime();
				gettimeofday(&now, NULL);
				if (timeval_compare(&now, &next) >= 0)
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
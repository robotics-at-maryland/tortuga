#ifndef RAM_CORE_PROCESS
#define RAM_CORE_PROCESS

// Library Includes
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <queue>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/Task.h"

// System Includes
#ifdef RAM_POSIX
#include <unistd.h>
#else
#include <windows.h> // For Sleep()
#endif // RAM_POSIX

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class RAM_EXPORT Process
{
public:

	Process(int core);
	~Process();
	
	void addTask(Task *t);
	
	void start();
	void stop();
	
protected:

	int m_core;
	bool m_started;

	std::priority_queue<Task*, std::vector<Task*>, std::less<std::vector<Task*>::value_type> > m_queue;
	
	boost::thread* m_thread;
	
	void run();

};

}
}

#endif
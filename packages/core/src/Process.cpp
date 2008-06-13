
#include "core/include/Process.h"

namespace ram {
namespace core {

Process::Process(int core) : m_core(core)
{
	
}

Process::~Process()
{
}

void Process::addTask(Task &t)
{
	m_queue.push(t);
}


}
}
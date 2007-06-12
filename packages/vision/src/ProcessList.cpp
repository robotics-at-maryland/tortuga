#include "vision/include/ProcessList.h"

ProcessList::ProcessList()
{

}

void ProcessList::addStep(std::string s)
{
	toCall.push_back(s);
}


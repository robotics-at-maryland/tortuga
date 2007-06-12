#ifndef process_list_h
#define process_list_h

#include <iostream>
#include <string>
#include <list>

typedef std::list<std::string> StringList;
typedef StringList::iterator StringListIterator;
class ProcessList
{
	public:
			ProcessList();
	void	addStep(std::string);
	void	run();
//	void	lookup(std::string);
	StringList toCall;
	private:
};
#endif
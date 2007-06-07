/*!
$Id$
 
    (c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains the delcation for the SubjectManager class.
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
    03-Apr-2006 J Lisee     Created.
*/

#ifndef __MOCKSUBJECT_H
#define __MOCKSUBJECT_H

#include "rtsx/foundation/pattern/h/subject.h"

namespace ssl {
namespace foundation {
namespace pattern {
    
// Mock class for testing
class MockSubject : public Subject 
{
public:
    int m_callCount;
    void *data;
    long flag;
    
    MockSubject() :
        m_callCount(0),
        data(0),
        flag(0)
    {
    }
    
    void notifyObservers(void* data = 0, long flag = 0)
    {
        m_callCount++;
        this->data = data;
        this->flag = flag;
    }
};

} // namespace pattern
} // namespace foundation
} // namespace ssl

#endif // __MOCKSUBJECT_H

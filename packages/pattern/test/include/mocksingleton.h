/*!
$Id: mocksingleton.h 1142 2006-09-22 21:54:02Z jlisee $
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains a test/example class using the Singleton template
 
	HISTORY
    28-Aug-2006 J Lisee     Created
*/

#ifndef __MOCKSINGLETON_H
#define __MOCKSINGLETON_H

// Foundation Includes
#include "rtsx/foundation/pattern/h/singleton.h"

namespace ssl {
namespace foundation {
namespace pattern {

class MockSingleton : public Singleton<MockSingleton>
{
    int data;
public:
    MockSingleton()
    {
        data = 5;
    };
    int getData()
    {
        return data;
    }
    void setData(int newData)
    {
        data = newData;
    }
};

} // namespace pattern
} // namespace foundation
} // namespace ssl
    
#endif // __MOCKSINGLETON_H

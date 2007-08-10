/*!
$Id: testsingleton.cxx 1194 2006-09-26 13:16:14Z roderick $
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains a the test cases for Singleton template
 
	HISTORY
    28-Aug-2006 J Lisee     Created
*/


// CxxTest Includes
#include <cxxtest/TestSuite.h>

// Foundation Includes
#include "rtsx/foundation/pattern/test/h/mocksingleton.h"

namespace ssl {
namespace foundation {
namespace pattern {
    
    template<> MockSingleton* Singleton<MockSingleton>::ms_Singleton = 0;
    
} // namespace pattern
} // namespace foundation
} // namespace ssl


using namespace ssl::foundation::pattern;

class TestSubject : public CxxTest::TestSuite
{
public:
    void setUp()
    {
       new MockSingleton();
    }
    
    void tearDown()
    {
        delete MockSingleton::getSingletonPtr();
    }
    
    void testSingleton()
    {
        TSM_ASSERT("Singleton not created", MockSingleton::getSingletonPtr());
        TSM_ASSERT_EQUALS("Singleton data not properly created", 5,
                          MockSingleton::getSingleton().getData());
        
        // use the singleton somewhere else and confirm the results
        useSingleton(11);
        TSM_ASSERT_EQUALS("Singleton not working", 11, 
                          MockSingleton::getSingleton().getData());
    }
    
    void useSingleton(int num)
    {
        MockSingleton::getSingleton().setData(num);
    }
};

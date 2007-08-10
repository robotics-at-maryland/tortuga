/*!
$Id$
 
    (c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        The tests for the Subject class.
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
*/

// CxxTest Includes
#include <cxxtest/TestSuite.h>

// Figure out some way to make this nasty macro better
#define protected public
#include "rtsx/foundation/pattern/h/subject.h"
#undef protected

#include "rtsx/foundation/pattern/test/h/mockobserver.h"

using namespace ssl::foundation::pattern;

class TestSubject : public CxxTest::TestSuite
{
public:
    Subject* subject;
    MockObserver* mockObs;
    
    void setUp() 
    {
        subject = new Subject();
    }
    
    void tearDown()
    {
        delete subject;
        subject = 0;
        delete[] mockObs;
        mockObs = 0;
    }
    
    void testAddObserver() 
    {
        fillSubject(subject, 2);
        
        TS_ASSERT_EQUALS(2, subject->countObservers());
    }
    
    void testCountObservers()
    {
        fillSubject(subject, 4);
        
        TS_ASSERT_EQUALS(4, subject->countObservers());
    }
    
    void testRemoveObserver()
    {
        // Add some observers
        MockObserver mock, mock2, mock3;
        subject->addObserver(&mock2);
        subject->addObserver(&mock);
        subject->addObserver(&mock3);
        
        // Remove our observer
        subject->removeObserver(&mock);
        
        // Notify the remaining observers
        subject->setChanged();
        subject->notifyObservers();
        
        TSM_ASSERT_EQUALS("Observer not removed", 0, mock.callCount);
    }
    
    void testRemoveObservers()
    {
        fillSubject(subject, 4);
        
        subject->removeObservers();
        
        TSM_ASSERT_EQUALS("Observers not removed", 0, 
                          subject->countObservers());
    }
    
    void testHasChanged()
    {
        subject->setChanged();
        
        TSM_ASSERT("Observer has not changed", subject->hasChanged());
    }
    
    void testNotifyObservers()
    {
        fillSubject(subject, 3);
        
        // Test to make sure all observers our called
        subject->setChanged();
        subject->notifyObservers();
        testMocks(mockObs, 3, 1, 0, 0);
        
        // Test to make sure they won't get called again
        subject->notifyObservers();
        testMocks(mockObs, 3, 1, 0 , 0);
        
        // Test to make sure they got the proper data
        subject->setChanged();
        subject->notifyObservers(const_cast<char*>("My Test"), 10);
        testMocks(mockObs, 3, 2, const_cast<char*>("My Test"), 10);
    }
    
    // Checks a list of MockObservers to make sure they have the proper data
    void testMocks(MockObserver *mocks, int size, int callcount, char* data, 
                   long flag)
    {
        for(int i = 0; i < size; ++i)
        {
            TSM_ASSERT_EQUALS("Obsever not called", callcount, 
                              mocks[i].callCount);
            TSM_ASSERT_EQUALS("Wrong data passed", data, (char*)mocks[i].data);
            TSM_ASSERT_EQUALS("Wrong flag passed", flag, mocks[i].flag);
        }
    }
    
    // Help function
    void fillSubject(Subject* sub, int count)
    {
        mockObs = new MockObserver[count];
        for (int i = 0; i < count; ++i)
        {
            sub->addObserver(&mockObs[i]);
        }
    }
};

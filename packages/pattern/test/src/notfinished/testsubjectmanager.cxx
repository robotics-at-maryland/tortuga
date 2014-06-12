/*!
$Id$
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains the unit tests for SubjectManager.
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
    27-Mar-2006 J Lisee     File Created
*/

// CxxTest Includes
#include <cxxtest/TestSuite.h>

// Project Includes
#include "rtsx/foundation/pattern/h/subjectmanager.h"
#include "rtsx/foundation/pattern/h/subject.h"
#include "rtsx/foundation/pattern/test/h/mocksubject.h"


using namespace ssl::foundation::pattern;

class TestSubjectManager : public CxxTest::TestSuite
{
public:
    SubjectManager* manager;
    MockSubject* mockSubs;
    
    void setUp() 
    {
        manager = new SubjectManager();
    }
    
    void tearDown() {
        delete manager;
        manager = 0;
        delete[] mockSubs;
        mockSubs = 0;
    }
    
    void testAddSubject() 
    {
        fillManager(manager, 2);

        TS_ASSERT_EQUALS(2, manager->countSubjects());
    }
    
    void testCountObservers()
    {        
        fillManager(manager, 4);
         
         TS_ASSERT_EQUALS(4, manager->countSubjects());
    }
    
    void testRemoveSubject()
    {
        // Add some Subjects
        fillManager(manager, 3);
        
        // Remove our observer
        manager->removeSubject(&mockSubs[0]);
        
        // Notify the remaining observers
        manager->notifySubjects();
        
        TSM_ASSERT_EQUALS("Subject not removed", 0, 
                          mockSubs[0].m_callCount);
        TSM_ASSERT_EQUALS("Subject not called", 1,
                          mockSubs[1].m_callCount);
        TSM_ASSERT_EQUALS("Subject not called", 1, 
                          mockSubs[2].m_callCount); 
    }
    
    void testRemoveSubjects()
    {
        fillManager(manager, 4);
        
        manager->removeSubjects();
        
        TSM_ASSERT_EQUALS("Subjects not removed", 0, 
                          manager->countSubjects()); 
    }
    
    
    void testNotifyObservers()
    {
        fillManager(manager, 3);
        
        manager->notifySubjects();
        testMocks(mockSubs, 3, 1, "", 0); 
    }
    
    // Checks a list of MockSubjects to make sure they have the proper data
    void testMocks(MockSubject *mocks, int size, int callcount, char* data, 
                   long flag)
    {
        for(int i = 0; i < size; ++i)
        {
            TSM_ASSERT_EQUALS("Obsevable not called", callcount, 
                              mocks[i].m_callCount);
            TSM_ASSERT_EQUALS("Wrong flag passed", flag, mocks[i].flag);
        } 
    }
    
    // Help function
    void fillManager(SubjectManager* manager, int count)
    {
        mockSubs = new MockSubject[count];
        for (int i = 0; i < count; ++i)
        {
            manager->addSubject(&mockSubs[i]);
        }
    }
};

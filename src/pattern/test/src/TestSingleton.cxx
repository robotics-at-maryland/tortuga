/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/pattern/test/src/TestSingleton.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Test Includes
#include "pattern/include/Singleton.h"

using namespace ram;

// Standard singleton
class BaseSingleton : public pattern::Singleton<BaseSingleton>
{
public:
    virtual ~BaseSingleton() {}
    static BaseSingleton& getSingleton();
    static BaseSingleton* getSingletonPtr();
    
    virtual int bar() { return 3; }
};

// This normall goes in a Cpp file
template<> BaseSingleton* 
pattern::Singleton<BaseSingleton>::ms_Singleton = 0;

BaseSingleton& BaseSingleton::getSingleton()
{  
    assert(ms_Singleton && 
           "Singleton instance or BaseSingleton not created");  
    return *ms_Singleton;
}
BaseSingleton* BaseSingleton::getSingletonPtr()
{
    return ms_Singleton;
}  

// Subclass of a singleton
class SubclassSingleton : public BaseSingleton
{
public:
    virtual ~SubclassSingleton() {}

    // Overrides base singleton
    virtual int bar() { return 8; }

    // Subclass specific method
    virtual int foo() { return 10; }

    // Override BaseSingleton grabing methods to return this class
    static SubclassSingleton& getSingleton()
    {
        return static_cast<SubclassSingleton&>(BaseSingleton::getSingleton());
    }

    static SubclassSingleton* getSingletonPtr()
    {
        return static_cast<SubclassSingleton*>(BaseSingleton::getSingletonPtr());
    }
};

SUITE(Singleton)
{

// Make sure the normal singleton works
TEST(Base)
{
    new BaseSingleton();
    CHECK_EQUAL(3, BaseSingleton::getSingleton().bar());

    // Test cleanup
    delete BaseSingleton::getSingletonPtr();
    CHECK(0 == BaseSingleton::getSingletonPtr());
}

// Make sure the normal subclass of a singleton works
TEST(Specific)
{
    new SubclassSingleton();

    // Call super class method
    CHECK_EQUAL(8, SubclassSingleton::getSingleton().bar());

    // Now subclasses method
    CHECK_EQUAL(10, SubclassSingleton::getSingleton().foo());
    CHECK_EQUAL(10, SubclassSingleton::getSingletonPtr()->foo());

    // Test cleanup
    delete SubclassSingleton::getSingletonPtr();
    CHECK(0 == SubclassSingleton::getSingletonPtr());
    CHECK(0 == BaseSingleton::getSingletonPtr());
}

TEST(lateBinding)
{
    new SubclassSingleton();

    // Make sure we get the overridden method
    CHECK_EQUAL(8, BaseSingleton::getSingleton().bar());

    // Test cleanup
    delete BaseSingleton::getSingletonPtr();
    CHECK(0 == SubclassSingleton::getSingletonPtr());
    CHECK(0 == BaseSingleton::getSingletonPtr());
}
    
} // SUITE(Singleton)

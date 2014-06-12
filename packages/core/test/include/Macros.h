/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/include/Macros.h
 */

#ifndef RAM_CORE_TEST_MACROS_03_11_2009
#define RAM_CORE_TEST_MACROS_03_11_2009

// Library Includes
#include "UnitTest++/TestResults.h"
#include "UnitTest++/TestDetails.h"
#include "UnitTest++/TestMacros.h"

#define TEST_UTILITY_FWD(Name, ArgList) \
class TestUtility##Name                                             \
{                                                                   \
public:                                                             \
    TestUtility##Name(UnitTest::TestResults& testResults,           \
                     const UnitTest::TestDetails &details)          \
        : testResults_(testResults), m_details(details)             \
    {                                                               \
    }                                                               \
                                                                    \
    void Name ArgList;                                              \
    UnitTest::TestResults& testResults_;                            \
    const UnitTest::TestDetails &m_details;                         \
private:                                                            \
    void operator =(const TestUtility##Name&);                      \
};                                                    

#define TEST_UTILITY_IMP(Name, ArgList) \
void TestUtility##Name::Name ArgList

#endif // RAM_CORE_TEST_MACROS_03_11_2009

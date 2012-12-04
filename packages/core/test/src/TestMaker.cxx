/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestMaker.cxx
 */

// STD Includes
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>

// Test Includes
#include "core/include/Maker.h"
#include "core/test/include/TestMaker.h"

// Needed force creation of the static objects which register the makers
IntMaker IntMaker::registerThis;
DoubleMaker DoubleMaker::registerThis;

IntMakerVer2 IntMakerVer2::registerThis;
DoubleMakerVer2 DoubleMakerVer2::registerThis;

// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

// Needed to prevent ea chclient from have there own static registry
namespace ram {
namespace core {

template<>
NumberMaker::MakerMap* NumberMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static NumberMaker::MakerMap* reg = new NumberMaker::MakerMap();
    return reg;
}

} // namespace core
} // namespace ram

// See TestMaker.h for an example of how to create classes which use the maker
// pattern
TEST(basics)
{
    std::stringstream test("Double 28.3 Int 10");
    
    Number* newNum = NumberMaker::newObject(test);
    Double* dbl = dynamic_cast<Double*>(newNum);
    // Wrong object created
    CHECK(dbl);
    // Double not converted properly
    CHECK_EQUAL(28.3, dbl->value);

    newNum = NumberMaker::newObject(test);
    Int* integer = dynamic_cast<Int*>(newNum);

    // Wrong object created
    CHECK(integer);
    // Integer not converted properly
    CHECK_EQUAL(10, integer->value);
}

TEST(basicsVer2)
{
    std::stringstream test("DoubleVer2 28.3 IntVer2 10");
    
    Number* newNum = NumberMaker::newObject(test);
    Double* dbl = dynamic_cast<Double*>(newNum);
    // Wrong object created
    CHECK(dbl);
    // Double not converted properly
    CHECK_EQUAL(28.3, dbl->value);

    newNum = NumberMaker::newObject(test);
    Int* integer = dynamic_cast<Int*>(newNum);

    // Wrong object created
    CHECK(integer);
    // Integer not converted properly
    CHECK_EQUAL(10, integer->value);
}

TEST(getRegisteredKeys)
{
    std::vector<std::string> expected = boost::assign::list_of
        ("Double")("DoubleVer2")("Int")("IntVer2");

    std::vector<std::string> result = NumberMaker::getRegisteredKeys();
    CHECK(expected == result);
}

TEST(isKeyRegistered)
{
    CHECK(NumberMaker::isKeyRegistered("Int"));
    CHECK(false == NumberMaker::isKeyRegistered("Bob"));
}

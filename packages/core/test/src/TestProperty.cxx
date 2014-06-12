/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestProperty.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/Property.h"

using namespace ram;

struct PropertyFixture {

    PropertyFixture()
    {}
};

SUITE(Property) {

TEST_FIXTURE(PropertyFixture, variableProperty)
{
    // Create the property
    int value;
    core::VariableProperty<int> prop("val", "A test property", 5, &value);

    // Set the property
    int expectedVal = 10;
    prop.set(expectedVal);

    // Check the variable to make sure it changed
    CHECK_EQUAL(expectedVal, value);

    // Check to make the property now reports the new value
    CHECK_EQUAL(expectedVal, prop.getAsInt());

    // Test min and max
    core::VariableProperty<int> prop2("val2", "A test property", 5, &value, 2, 
				      13);
    CHECK_EQUAL(2, boost::any_cast<int>(prop2.getMinValue()));
    CHECK_EQUAL(13, boost::any_cast<int>(prop2.getMaxValue()));
}


void setter(int* ref, int val)
{
    *ref = val;
}

int getter(int* ref)
{
    return *ref;
}

TEST_FIXTURE(PropertyFixture, functionProperty)
{
    // Create the property
    int value;
    core::FunctionProperty<int> prop("val", "A test property", 5,
                                     boost::bind(getter, &value),
                                     boost::bind(setter, &value, _1));
                                    

    // Set the property
    int expectedVal = 10;
    prop.set(expectedVal);

    // Check the variable to make sure it changed
    CHECK_EQUAL(expectedVal, value);

    // Check to make the property now reports the new value
    CHECK_EQUAL(expectedVal, prop.getAsInt());    

    // Ensure it doesn't have a min/max
    CHECK_EQUAL(false, prop.hasMinMax());

     // Test min and max
    core::FunctionProperty<int> prop2("val2", "A test property", 5,
				      boost::bind(getter, &value),
				      boost::bind(setter, &value, _1),
				      0, 10);
    CHECK(prop2.hasMinMax());
    CHECK_EQUAL(0, boost::any_cast<int>(prop2.getMinValue()));
    CHECK_EQUAL(10, boost::any_cast<int>(prop2.getMaxValue()));
}

TEST_FIXTURE(PropertyFixture, toString)
{
    // Create the property
    int value = 5;
    core::VariableProperty<int> prop("val", "A test property", 5, &value);

    // Set the property
    std::string expected("5");
    CHECK_EQUAL(expected, prop.toString());
}


} // SUITE(PROPERTY)

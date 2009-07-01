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
#include <boost/assign/list_of.hpp>

// Project Includes
#include "core/include/PropertySet.h"

using namespace ram;

SUITE(PropertySet) {

struct Fixture {

    Fixture()
    {
        prop = core::PropertyPtr(
            new core::VariableProperty<int>("val", "A test property", 5,
                                            &value));
    }

    core::PropertySet propSet;
    core::PropertyPtr prop;
    int value;
};

    
TEST_FIXTURE(Fixture, addProperty)
{
    // Add the property
    propSet.addProperty(prop);
}

void setter(int* ref, int val)
{
    *ref = val;
}

int getter(int* ref)
{
    return *ref;
}


TEST_FIXTURE(Fixture, addPropertySpecial)
{
    // Add the property
    propSet.addProperty(prop);

    // Have it build a variable property for us
    int value2;
    propSet.addProperty("val2", "A test auto var", 5, &value2);

    // Have it build a function property for us
    int value3;
    propSet.addProperty("val3", "A test auto func", 5,
                        boost::bind(getter, &value3),
                        boost::bind(setter, &value3, _1));
}

TEST_FIXTURE(Fixture, addPropertyConfig)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'boolVal' : 0,"
            "  'intVal' : 56,"
            "  'intVal2' : 0,"
            "  'doubleVal' : 123.4,"
            "  'vector2Val' : [2, 3],"
            "  'vector3Val' : [5, 6, 7],"
            "  'quatVal' : [0, 1, 0, 0] }"));
    
    // Test bool loading
    bool boolVal = true;
    propSet.addProperty(config, true, "boolVal", "", true, &boolVal);
    CHECK_EQUAL(false, boolVal);

    // Test int loading
    int intVal = 8;
    propSet.addProperty(config, true, "intVal", "", 8, &intVal);
    CHECK_EQUAL(56, intVal);

    // Test double loading
    double doubleVal = 9.6;
    propSet.addProperty(config, true, "doubleVal", "", 9.6, &doubleVal);
    CHECK_EQUAL(123.4, doubleVal);

    // Test int with min/max (just make sure we can call it)
    propSet.addProperty(config, true, "intVal2", "", 8, &intVal, 0, 28);
    core::PropertyPtr prop2 = propSet.getProperty("intVal2");
    CHECK_EQUAL(0, boost::any_cast<int>(prop2->getMinValue()));
    CHECK_EQUAL(28, boost::any_cast<int>(prop2->getMaxValue()));

#ifdef RAM_WITH_MATH
    // Test Vector2 loading
    math::Vector2 vector2Val(math::Vector2::ZERO);
    propSet.addProperty(config, true, "vector2Val", "", math::Vector2(1, 2),
                        &vector2Val);
    CHECK_EQUAL(math::Vector2(2, 3), vector2Val);

    // Test Vector3 loading
    math::Vector3 vector3Val(math::Vector3::ZERO);
    propSet.addProperty(config, true, "vector3Val", "", math::Vector3(1, 2, 3),
                        &vector3Val);
    CHECK_EQUAL(math::Vector3(5, 6, 7), vector3Val);

    // Test Quaternion loading
    math::Quaternion quatVal(math::Quaternion::IDENTITY);
    propSet.addProperty(config, true, "quatVal", "", math::Quaternion(1,2,3,4),
                        &quatVal);
    CHECK_EQUAL(math::Quaternion(0, 1, 0, 0), quatVal);
#endif // RAM_WITH_MATH
}

TEST_FIXTURE(Fixture, getProperty)
{
    // Add the property
    propSet.addProperty(prop);

    // Get it back and make sure its the right one
    core::PropertyPtr prop2 = propSet.getProperty("val");
    CHECK_EQUAL(prop.get(), prop2.get());
}

TEST_FIXTURE(Fixture, hasProperty)
{
    // Make sure its not there
    CHECK_EQUAL(false, propSet.hasProperty("val"));
    
    // Add the property
    propSet.addProperty(prop);

    // Now make sure its there
    CHECK(propSet.hasProperty("val"));
}

TEST_FIXTURE(Fixture, getPropertyNames)
{
    core::PropertyPtr prop2(
        new core::VariableProperty<int>("val 2", "property", 6, &value));

    // Add the properties
    propSet.addProperty(prop);
    propSet.addProperty(prop2);

    std::vector<std::string> propNames = boost::assign::list_of("val")("val 2");
    CHECK_ARRAY_EQUAL(propNames, propSet.getPropertyNames(), propNames.size());
}

TEST_FIXTURE(Fixture, addPropertiesFromSet)
{
    // Our starter property set
    propSet.addProperty(prop);

    // Create our set we wish to add
    core::PropertySet propSet2;

    core::PropertyPtr prop2(
        new core::VariableProperty<int>("val 2", "property", 6, &value));
    core::PropertyPtr prop3(
        new core::VariableProperty<int>("val 3", "property", 8, &value));

    propSet2.addProperty(prop2);
    propSet2.addProperty(prop3);

    // Add the prop set to the original to the original
    propSet.addPropertiesFromSet(&propSet2);
    
    std::vector<std::string> propNames =
        boost::assign::list_of("val")("val 2")("val 3");
    CHECK_ARRAY_EQUAL(propNames, propSet.getPropertyNames(), propNames.size());
}




} // SUITE(PropertySet)

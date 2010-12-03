/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestPythonConfigNodeImp.cpp
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS // turn off warning about getenv
#endif
 
// STD Includes
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/ConfigNode.h"

namespace ram {
namespace core {

static std::string getConfigFile()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return (root / "packages" / "core" / "test" / "data" /
            "testInclude.yml").file_string();
}

    
const std::string BASIC_CFG(
    "{'TestInt': 10, "
    " 'TestDouble': 23.5,"
    " 'TestStr' : 'Str',"
    " 'Array' : [4,5,6],"
    " 'Map' :"
    "     {'A' : 'D', 'B' : 'E', 'C' : 'F'} }");
    
struct TestPythonConfigNode
{
    TestPythonConfigNode()
        : configNode(ConfigNode::fromString(BASIC_CFG))
    {
        
    }

    ConfigNode configNode;
};

TEST_FIXTURE(TestPythonConfigNode, toString)
{
    std::string config("{'Test': 10}");
    ConfigNode node(ConfigNode::fromString(config));
    CHECK_EQUAL(config, node.toString());
}
    
TEST_FIXTURE(TestPythonConfigNode, asString)
{
    CHECK_EQUAL("Str", configNode["TestStr"].asString());
    CHECK_EQUAL("Def", configNode["NotThere"].asString("Def"));
}
    
TEST_FIXTURE(TestPythonConfigNode, asInt)
{
    CHECK_EQUAL(10, configNode["TestInt"].asInt());
    CHECK_EQUAL(2, configNode["NotThere"].asInt(2));
    
    // Fix this to throw a standard exception
    // CHECK_THROW(configNode["Bob"].asInt(), py::error_already_set);
}

TEST_FIXTURE(TestPythonConfigNode, asDouble)
{
    CHECK_EQUAL(23.5, configNode["TestDouble"].asDouble());
    CHECK_EQUAL(17.6, configNode["NotThere"].asDouble(17.6));
}

TEST_FIXTURE(TestPythonConfigNode, subNodes)
{
    NodeNameList subnodes = configNode["Map"].subNodes();
    NodeNameListIter result;

    // Ensure we got the right size
    CHECK_EQUAL(3u, subnodes.size());
    
    CHECK(subnodes.end() != subnodes.find("A"));
    CHECK(subnodes.end() != subnodes.find("B"));
    CHECK(subnodes.end() != subnodes.find("C"));
}

TEST_FIXTURE(TestPythonConfigNode, size)
{
    CHECK_EQUAL(3u, configNode["Array"].size());
    CHECK_EQUAL(3u, configNode["Map"].size());
}
    
TEST_FIXTURE(TestPythonConfigNode, exists)
{
    CHECK(false == configNode.exists("Bob"));
    CHECK(configNode.exists("Map"));
}

TEST_FIXTURE(TestPythonConfigNode, index)
{
    CHECK_EQUAL(4, configNode["Array"][0].asInt());
    CHECK_EQUAL(5, configNode["Array"][1].asInt());
    CHECK_EQUAL(6, configNode["Array"][2].asInt());
}

TEST_FIXTURE(TestPythonConfigNode, map)
{
    CHECK_EQUAL("D", configNode["Map"]["A"].asString());
    CHECK_EQUAL("E", configNode["Map"]["B"].asString());
    CHECK_EQUAL("F", configNode["Map"]["C"].asString());
}

TEST_FIXTURE(TestPythonConfigNode, set)
{
    // Ensure value is not there
    CHECK_EQUAL("NotHere", configNode["Map"]["TestSet"].asString("NotHere"));

    // Set value and make sure it stuck
    configNode["Map"].set("TestSet", "MyVal");
    CHECK_EQUAL("MyVal", configNode["Map"]["TestSet"].asString());
}

TEST_FIXTURE(TestPythonConfigNode, include)
{
    configNode = ConfigNode::fromFile(getConfigFile());
    
    // Make sure original values are there
    CHECK(configNode.exists("Base"));
    ConfigNode base = configNode["Base"];
    CHECK(base.exists("Sub"));
    ConfigNode sub = base["Sub"];
    
    CHECK_EQUAL(100, sub["count"].asInt());
    CHECK_EQUAL("Test.Good", sub["type"].asString());

    // Test the base level import
    CHECK(configNode.exists("Other"));
    CHECK_EQUAL(1, configNode["Other"]["key"].asInt());
    
    // Test import of mid-level import
    CHECK(base.exists("Sys2"));
    CHECK_EQUAL("Sonar", base["Sys2"]["type"].asString());
    
    // Test deep import
    CHECK_EQUAL(57.6, sub["setting"].asDouble());
    CHECK_EQUAL("Bob", sub["mode"].asString());

    // Test recursive import
    CHECK_EQUAL(67, configNode["Other"]["recVal"].asInt());

    // Test to make sure that subnodes works
    NodeNameList subnodes = configNode["Base"].subNodes();
    NodeNameListIter result;

    // Ensure we got the right size
    CHECK_EQUAL(2u, subnodes.size());
    
    CHECK(subnodes.end() != subnodes.find("Sub"));
    CHECK(subnodes.end() != subnodes.find("Sys2"));
}


#ifndef RAM_WINDOWS // Can't seem to get the file path right on windows
TEST_FIXTURE(TestPythonConfigNode, fromFile)
{
    std::string base = std::string(getenv("RAM_SVN_DIR")) + "/packages/core/";
    ConfigNode node(ConfigNode::fromFile(base + "test/data/test.yml"));

    CHECK_EQUAL(100, node["Node"]["count"].asInt());
    CHECK_EQUAL("Test.Good", node["Node"]["type"].asString());
}
#endif

TEST_FIXTURE(TestPythonConfigNode, writeToFile)
{
    // How the hell do I do this?
    std::string base = "{'This is a test yaml file' : 'do not use'}";
    ConfigNode node(ConfigNode::fromString(base));

    // Test yaml file location
    std::string filepath = std::string(getenv("RAM_SVN_DIR"))
        + "/build/packages/core/test.yml";
    boost::filesystem::path path(filepath);

    if (boost::filesystem::exists(path))
        remove(filepath.c_str());

    CHECK(!boost::filesystem::exists(path));

    node.writeToFile(filepath);

    CHECK(boost::filesystem::exists(path));

    // Cleanup
    remove(filepath.c_str());
}

} // namespace core
} // namespace ram
    
int main(int argc, char* argv[])
{
    return UnitTest::RunAllTests();
}

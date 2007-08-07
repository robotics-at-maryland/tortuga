// STD Includes
#include <string>
#include <cstdlib>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/ConfigNode.h"

namespace ram {
namespace core {

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

TEST_FIXTURE(TestPythonConfigNode, fromFile)
{
    std::string base = std::string(getenv("RAM_SVN_DIR")) + "/packages/core/";
    ConfigNode node(ConfigNode::fromFile(base + "test/data/test.yml"));

    CHECK_EQUAL(100, node["Node"]["count"].asInt());
    CHECK_EQUAL("Test.Good", node["Node"]["type"].asString());
}

} // namespace core
} // namespace ram
    
int main()
{
    return UnitTest::RunAllTests();
}

    

#include <utility>

#include "include/Config.h"


ConfigNode ConfigNode::operator[](int index)
{
    return ConfigNode(m_impl->idx(index));
}

ConfigNode ConfigNode::operator[](std::string key)
{
    return ConfigNode(m_impl->map(key));
}

std::string ConfigNode::asString()
{
    return m_impl->asString();
}

double ConfigNode::asDouble()
{
    return m_impl->asDouble();
}

int ConfigNode::asInt()
{
    return m_impl->asInt();
}

ConfigNode ConfigNode::construct(std::string type)
{
    if (type == "Test")
    {
        return ConfigNode(ConfigNodeImpPtr(new TestConfigNodeImp()));
    }

    assert(false && "Wrong type of config node");
    return ConfigNode(ConfigNodeImpPtr());
}


ConfigNode::ConfigNode(ConfigNodeImpPtr impl) :
    m_impl(impl)
{
}

ConfigNode::ConfigNode()
{
    assert(false && "ConfigNode() Should not be called");
}

ConfigNode::ConfigNode(const ConfigNode& configNode)
{
    m_impl = configNode.m_impl;
//    assert(false && "ConfigNode(const ConfigNode* configNode)");
}

ConfigNode::ConfigNode& ConfigNode::operator=(const ConfigNode& that)
{
    // make sure not same object
    if (this != &that)
    {  
        m_impl = that.m_impl;
    }
    return *this;    // Return ref for multiple assignment
}

// ------------------------------------------------------------------------- //
//                T E S T   C O N F I G   N O D E   I M P                    //
// ------------------------------------------------------------------------- //

TestConfigNodeImp::TestConfigNodeImp()
{
    tmp_int = 5;
    tmp_double = 26.3;
    tmp_string = "Bob";

    int_list.push_back(2);
    int_list.push_back(3);
    int_list.push_back(4);

    str_int_map["John"] =  10;
    str_int_map["Larry"] =  20;
    str_int_map["Joe"] =  30;
}


ConfigNodeImpPtr TestConfigNodeImp::idx(int index)
{
    TestConfigNodeImp*  newNode = new TestConfigNodeImp();
    newNode->tmp_int = int_list[index];
    
    return ConfigNodeImpPtr(newNode);
}

ConfigNodeImpPtr TestConfigNodeImp::map(std::string key)
{
    TestConfigNodeImp*  newNode = new TestConfigNodeImp();
    newNode->tmp_int = str_int_map[key];
    
    return ConfigNodeImpPtr(newNode);
}

std::string TestConfigNodeImp::asString()
{
    return tmp_string;
}

double TestConfigNodeImp::asDouble()
{
    return tmp_double;
}

int TestConfigNodeImp::asInt()
{
    return tmp_int;
}

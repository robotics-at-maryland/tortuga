/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNode.cpp
 */

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/ConfigNodeImp.h"
#include "core/include/PythonConfigNodeImp.h"

namespace ram {
namespace core {

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

std::string ConfigNode::asString(const std::string& def)
{
    return m_impl->asString(def);
}

double ConfigNode::asDouble()
{
    return m_impl->asDouble();
}

double ConfigNode::asDouble(const double def)
{
    return m_impl->asDouble(def);
}
    
int ConfigNode::asInt()
{
    return m_impl->asInt();
}

int ConfigNode::asInt(const int def)
{
    return m_impl->asInt(def);
}

NodeNameList ConfigNode::subNodes()
{
    return m_impl->subNodes();
}

size_t ConfigNode::size()
{
    return m_impl->size();
}

bool ConfigNode::exists(std::string name)
{
	NodeNameList nodes = subNodes();
	NodeNameListIter result = nodes.find(name);
	return (nodes.end() != result);
}

void ConfigNode::set(std::string key, std::string str)
{
    m_impl->set(key, str);
}

ConfigNode ConfigNode::fromString(std::string data)
{
    return ConfigNode(ConfigNodeImpPtr(new PythonConfigNodeImp(data)));
}

ConfigNode ConfigNode::fromFile(std::string data)
{
    return ConfigNode(PythonConfigNodeImp::fromYamlFile(data));
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

ConfigNode& ConfigNode::operator=(const ConfigNode& that)
{
    // make sure not same object
    if (this != &that)
    {  
        m_impl = that.m_impl;
    }
    return *this;    // Return ref for multiple assignment
}

} // namespace core
} // namespace ram

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNode.cpp
 */

// Library Includes
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/ConfigNodeImp.h"
#include "core/include/PythonConfigNodeImp.h"

namespace ram {
namespace core {

ConfigNode ConfigNode::operator[](int index)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return ConfigNode(m_impl->idx(index));
}

ConfigNode ConfigNode::operator[](std::string key)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return ConfigNode(m_impl->map(key));
}

std::string ConfigNode::asString()
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asString();
}

std::string ConfigNode::asString(const std::string& def)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asString(def);
}

double ConfigNode::asDouble()
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asDouble();
}

double ConfigNode::asDouble(const double def)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asDouble(def);
}
    
int ConfigNode::asInt()
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asInt();
}

int ConfigNode::asInt(const int def)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->asInt(def);
}

NodeNameList ConfigNode::subNodes()
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->subNodes();
}

size_t ConfigNode::size()
{
    assert(m_impl.get() && "No ConfigNode impl found");
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
    assert(m_impl.get() && "No ConfigNode impl found");
    m_impl->set(key, str);
}

void ConfigNode::set(std::string key, int value)
{
    assert(m_impl.get() && "No ConfigNode impl found");
    m_impl->set(key, value);
}
    
ConfigNode ConfigNode::fromString(std::string data)
{
    return ConfigNode(ConfigNodeImpPtr(new PythonConfigNodeImp(data)));
}

std::string ConfigNode::toString()
{
    assert(m_impl.get() && "No ConfigNode impl found");
    return m_impl->toString();
}

ConfigNode ConfigNode::fromFile(std::string configPath)
{
    boost::filesystem::path path(configPath);
    if (path.extension() == ".yml" || path.extension() == ".sml") {
        return ConfigNode(PythonConfigNodeImp::fromYamlFile(path.string()));
    } else {
        assert(false && "Invalid configuration type!");
    }
}
    
ConfigNode::ConfigNode(ConfigNodeImpPtr impl) :
    m_impl(impl)
{
    assert(m_impl.get() && "No ConfigNode impl found");
}

ConfigNode::ConfigNode()
{
    assert(false && "ConfigNode() Should not be called");
}

ConfigNode::ConfigNode(const ConfigNode& configNode)
{
    assert(configNode.m_impl.get() && "No ConfigNode impl found");
    m_impl = configNode.m_impl;
//    assert(false && "ConfigNode(const ConfigNode* configNode)");
}

ConfigNode& ConfigNode::operator=(const ConfigNode& that)
{
    assert(that.m_impl.get() && "No ConfigNode impl found");
    
    // make sure not same object
    if (this != &that)
    {  
        m_impl = that.m_impl;
    }
    return *this;    // Return ref for multiple assignment
}

void ConfigNode::writeToFile(std::string fileName, bool silent)
{
    m_impl->writeToFile(fileName, silent);
}

} // namespace core
} // namespace ram

#include <utility>

#include "include/Config.h"

ConfigNode::ConfigNode()
//ConfigNode::ConfigNode(py::object pyobj)
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

ConfigNode ConfigNode::operator[](int index)
{
    ConfigNode newNode;
    newNode.tmp_int = int_list[index];
    
    return newNode;
}

ConfigNode ConfigNode::operator[](std::string val)
{
    ConfigNode newNode;
    newNode.tmp_int = str_int_map[val];
    
    return newNode;
}


std::string ConfigNode::asString()
{
    return tmp_string;
}


double ConfigNode::asDouble()
{
    return tmp_double;
}


int ConfigNode::asInt()
{
    return tmp_int;
}                       

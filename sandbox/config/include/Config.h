

#ifndef RAM_CORE_CONFIG_06_27_2006
#define RAM_CORE_CONFIG_06_27_2006

// STD Includes
#include <string>
#include <vector>
#include <map>

// Library Includes
//#include <boost/python.hpp>
//namespace py = boost::python;

class ConfigNode
{
public:
    ConfigNode();
//    ConfigNode(bp::object pyobj);

    /** Grab a section of the config like an array */
    ConfigNode operator[](int index);

    /** Grab a sub node with the same name */
    ConfigNode operator[](std::string map);

    /** Convert the node to a string value */
    std::string asString();

    /** Convert the node to a double */
    double asDouble();

    /** Convert the node to an int */
    int asInt();

private:
    //   enum MyType{
    //  INT, DOUBLE, STRING, NODE
//    };

    //  ConfigNode::MyType type;
        
    int tmp_int;
    double tmp_double;
    std::string tmp_string;
    
    std::vector<int> int_list;
    std::map<std::string, int> str_int_map;
};

#endif // RAM_CORE_CONFIG_06_27_2006



#ifndef RAM_CORE_CONFIG_06_27_2006
#define RAM_CORE_CONFIG_06_27_2006

// STD Includes
#include <string>
#include <vector>
#include <map>

// Library Includes
#include <boost/shared_ptr.hpp>
//#include <boost/python.hpp>
///namespace py = boost::python;

class IConfigNode;
typedef boost::shared_ptr<IConfigNode> IConfigNodePtr;

class IConfigNode
{
public:
    /** Grab a section of the config like an array */
    virtual IConfigNodePtr idx(int index) = 0;

    /** Grab a sub node with the same name */
    virtual IConfigNodePtr map(std::string map) = 0;

    /** Convert the node to a string value */
    virtual std::string asString() = 0;

    /** Convert the node to a double */
    virtual double asDouble() = 0;

    /** Convert the node to an int */
    virtual int asInt() = 0;
};



class ConfigNode : public IConfigNode
{
public:
    ConfigNode();
//    ConfigNode(by::object pyobj);

    /** Grab a section of the config like an array */
    virtual IConfigNodePtr idx(int index);

    /** Grab a sub node with the same name */
    virtual IConfigNodePtr map(std::string map);

    /** Convert the node to a string value */
    virtual std::string asString();

    /** Convert the node to a double */
    virtual double asDouble();

    /** Convert the node to an int */
    virtual int asInt();

private:
    //   enum MyType{
    //  INT, DOUBLE, STRING, NODE
//    };

    //  ConfigNode::MyType type;


//    py::object m_node;
    int tmp_int;
    double tmp_double;
    std::string tmp_string;
    
    std::vector<int> int_list;
    std::map<std::string, int> str_int_map;
};

#endif // RAM_CORE_CONFIG_06_27_2006

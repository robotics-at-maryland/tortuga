

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


class ConfigNodeImp;
typedef boost::shared_ptr<ConfigNodeImp> ConfigNodeImpPtr;

class ConfigNode
{
public:
    ConfigNode(const ConfigNode& configNode);
    
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

    static ConfigNode construct(std::string type);
private:
    ConfigNode();
    ConfigNode& operator=(const ConfigNode& that);
    
    ConfigNode(ConfigNodeImpPtr impl);

    
    ConfigNodeImpPtr m_impl;
};



class ConfigNodeImp
{
public:
//    ConfigNode();
//    ConfigNode(by::object pyobj);

    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index) = 0;

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key) = 0;

    /** Convert the node to a string value */
    virtual std::string asString() = 0;

    /** Convert the node to a double */
    virtual double asDouble() = 0;

    /** Convert the node to an int */
    virtual int asInt() = 0;
};

class TestConfigNodeImp : public ConfigNodeImp
{
public:
    TestConfigNodeImp();
    
    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index);

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key);

    /** Convert the node to a string value */
    virtual std::string asString();

    /** Convert the node to a double */
    virtual double asDouble();

    /** Convert the node to an int */
    virtual int asInt();
private:
    int tmp_int;
    double tmp_double;
    std::string tmp_string;
    
    std::vector<int> int_list;
    std::map<std::string, int> str_int_map;
};

#endif // RAM_CORE_CONFIG_06_27_2006

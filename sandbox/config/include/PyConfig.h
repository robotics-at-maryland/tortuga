


#ifndef RAM_CORE_PYCONFIG_06_27_2006
#define RAM_CORE_PYCONFIG_06_27_2006

#include <boost/python.hpp>
namespace py = boost::python;

class PythonConfigNodeImp : public ConfigNodeImp
{
public:
    PythonConfigNodeImp(py::object m_pyobj);
    
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
    py::object m_pyobj;
};

#endif // RAM_CORE_PYCONFIG_06_27_2006

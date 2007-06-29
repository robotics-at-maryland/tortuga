#include <utility>

#include "include/Config.h"
#include "include/PyConfig.h"

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

ConfigNode ConfigNode::construct(std::string type, boost::any values)
{
    if (type == "Test")
    {
        return ConfigNode(ConfigNodeImpPtr(new TestConfigNodeImp()));
    }
    if (type == "Python")
    {
//        py::object dict = 
        return ConfigNode(ConfigNodeImpPtr(new PythonConfigNodeImp(boost::any_cast<py::dict>(values))));
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

// ------------------------------------------------------------------------- //
//               P Y T H O N   C O N F I G   N O D E   I M P                 //
// ------------------------------------------------------------------------- //

PythonConfigNodeImp::PythonConfigNodeImp(py::object pyobj) :
    m_pyobj(pyobj)
{
}


ConfigNodeImpPtr PythonConfigNodeImp::idx(int index)
{
    return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[index]));
}

ConfigNodeImpPtr PythonConfigNodeImp::map(std::string key)
{   
    return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[key]));
}

std::string PythonConfigNodeImp::asString()
{
    return std::string(py::extract<char*>(m_pyobj));
}

double PythonConfigNodeImp::asDouble()
{
    return py::extract<double>(m_pyobj);
}

int PythonConfigNodeImp::asInt()
{
    return py::extract<int>(m_pyobj);
}

boost::any testDict()
{
    Py_Initialize();
    
    py::dict Controller;
//    py::long_ test(5l);
//    Controller["Param"] = py::object(py::handle<>(PyInt_FromLong(10)));
    Controller["Param"] = 5;
    Controller["Param2"] = 10;

    py::list params;
    params.append(1);
    params.append(2);
    params.append(3);

    py::dict Thrusters;
    Thrusters["params"] = params;
    Thrusters["name"] = "Port";

    py::dict Modules;
    Modules["Controller"] = Controller;
    Modules["Thrusters"] = Thrusters;

    py::dict base;
    base["Modules"]= Modules;
    
    return boost::any(base);
}

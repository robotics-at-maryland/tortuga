#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

namespace bo = boost;
namespace ba = boost::algorithm;
namespace py = boost::python;
using namespace std;


py::object grabObject(py::object& loc_namespace, string path)
{
    vector<string> nodes;
    ba::split(nodes, path, bo::is_any_of("."));
    py::object obj = loc_namespace[nodes[0]];

    for(size_t i = 1; i < nodes.size(); ++i)
    {
        try {
            obj = obj.attr(nodes[i].c_str());
        } catch (py::error_already_set) {
            PyErr_Clear();
            return py::object();
        }
    }

    return obj;
}

void setObject(py::object& loc_namespace, string path, py::object val)
{
    // Split up the path by the "."
    vector<string> nodes;
    ba::split(nodes, path, bo::is_any_of("."));
    py::object obj = loc_namespace[nodes[0]];

    for(size_t i = 1; i < nodes.size(); ++i)
    {
        const char* name = nodes[i].c_str();


        try {
            if(nodes.size() == (i + 1))
                obj.attr(name) = val;
            else
                obj = obj.attr(name);

        // That sub class dosen't exist yet
        } catch (py::error_already_set) {
            PyErr_Clear();

            // Hack to generate a new Class, can't find Boost.Python way
            ostringstream os;
            os << "new_type = type('" << name << "', (object,), {})";
            py::handle<> ignore((PyRun_String(os.str().c_str(),
                                                  Py_single_input,
                                                  loc_namespace.ptr(),
                                                  loc_namespace.ptr())));
            obj.attr(name) = loc_namespace["new_type"];
            obj = obj.attr(name);
        }
    }
}

void do_serialize(py::object obj, fstream& outfile, const string& name = "",
                  int depth = 0)
{
    try
    {
        // Print out the class
        char* classname = py::extract<char*>(obj.attr("__name__"));
        for (int i = 0; i < depth; ++i)
            outfile << '\t';
        outfile << "class " << classname << "(object):" << endl;

        // Get all the properties of the class
        py::list items =
            py::extract<py::list>(obj.attr("__dict__").attr("keys")());
        for (int i = 0; i < py::len(items); ++i)
        {
            string item = py::extract<string>(items[i]);
            if (!ba::starts_with(item, "__"))
                do_serialize(obj.attr(item.c_str()), outfile, item, depth + 1);
        }

    // We couldn't get the name attribute, so its must be a basic type
    } catch (py::error_already_set) {
        PyErr_Clear();

        for(int i = 0; i < depth; ++i)
            outfile << '\t';
        outfile << name << " = ";

        // If its a string we need to escape it
        if (obj.attr("__class__").attr("__name__") == py::str("str"))
            outfile <<  py::extract<char*>("'" + py::str(obj) + "'");
        else
            outfile << py::extract<char*>(py::str(obj));
        outfile << endl;
    }
}

void serialize(py::object obj, string filename)
{
    fstream outfile(filename.c_str(), ios::out);

    if(outfile.bad())
    {
        cerr << "Error opening: " << filename << endl;
    }
    else
    {
        do_serialize(obj, outfile);
        outfile.close();
    }
}

BOOST_PYTHON_MODULE(cpyconfig)
{
    def("grabVal", grabObject);
    def("setVal", setObject);
    def("serialize", serialize);
}

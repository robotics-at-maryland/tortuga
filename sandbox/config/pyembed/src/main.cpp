#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>

#include <string>
#include <iostream>

namespace bo = boost;
namespace py = boost::python;
using namespace std;

py::object grabObject(py::object& loc_namespace, string);
void setObject(py::object& loc_namespace, string path, py::object val);
void serialize(py::object obj, string filename);

int main( int argc, char ** argv ) {
  try {
    Py_Initialize();

    // Import the __main__ module and grab its namespace
    py::object main_module((
      py::handle<>(py::borrowed(PyImport_AddModule("__main__")))));
    py::object main_namespace = main_module.attr("__dict__");


    py::handle<> ignored2(( PyRun_String(
    "class Test(object):\n"
    "    str = 'value'\n"
    "    class sub(object):\n"
    "        arm = 10",
                                     Py_file_input,
                                     main_namespace.ptr(),
                                     main_namespace.ptr() ) ));

    cout << "Begining tests" << endl;
    cout << "Test.arm.arm: " << py::extract<char*>(py::str(grabObject(main_namespace, "Test.arm.arm"))) << endl;
    setObject(main_namespace, "Test.str", py::str("Hello"));
    cout << "Test.str: " << py::extract<char*>(py::str(grabObject(main_namespace, "Test.str"))) << endl;
    setObject(main_namespace, "Test.arm.arm", py::str("here"));
    cout << "Test.arm.arm: " << py::extract<char*>(py::str(grabObject(main_namespace, "Test.arm.arm"))) << endl;
    setObject(main_namespace, "Test.Extra.Loc.special", py::str("here2"));
    cout << "Test.arm.arm: " << py::extract<char*>(py::str(grabObject(main_namespace, "Test.Extra.Loc.special"))) << endl;

    serialize(main_namespace["Test"], "cpptest.py");

  } catch( py::error_already_set ) {
    PyErr_Print();
  }
}


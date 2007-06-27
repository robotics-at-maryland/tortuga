#include <boost/python.hpp>

namespace py = boost::python;

struct World
{
    void set(std::string msg) { this->msg = msg; }
    std::string greet() { return msg; }
    std::string msg;
};

BOOST_PYTHON_MODULE(myhello)
{
    py::class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
    ;
}

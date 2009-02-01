// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/python.hpp>
#include "enums_to_be_exported.hpp"

BOOST_PYTHON_MODULE(pyenums){
    //exporting members of namespace "::enums"
    boost::python::enum_<enums::color>("Color")
        .value("RED", enums::red)
        .value("BLUE", enums::blue)
        .export_values()
        .value("green", enums::green)
        ;
}
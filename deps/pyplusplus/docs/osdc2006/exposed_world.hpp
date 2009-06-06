// std directories: []
// user defined directories: ['/home/roman/pygccxml_sources/source/pyplusplus/docs/ConferenceIsrael2006']

#include "boost/python.hpp"

#include "world.hpp"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(pyplusplus){
    bp::class_< world >( "world"
                         , bp::init< std::string const & >(( bp::arg("msg") ))
                         /*[ undefined call policies ]*/ )    
        .def( "set"
              , (void ( ::world::* )( ::std::string const & ) )(&world::set)
              , ( bp::arg("msg") )
              , bp::default_call_policies() )    
        .def( "greet"
              , (::std::string const & ( ::world::* )(  ) const)
                (&world::greet)
              , bp::return_value_policy< bp::copy_const_reference, bp::default_call_policies >() );
}
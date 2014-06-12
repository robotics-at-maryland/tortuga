# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators

TRANSLATE_CODE = \
"""
void translate(boost::python::object py_class, cppexceptions::custom_exception_t const& e){
    std::cout << "I am here";
    boost::python::object py_custom_exception = py_class(e);
    boost::python::object type = py_custom_exception.attr( "__class__" );
    PyErr_SetObject( type.ptr(), py_custom_exception.ptr() );
}

"""

REGISTER_CODE = \
"""
    boost::python::object py_class = custom_exception_t_exposer;
    boost::python::register_exception_translator<cppexceptions::custom_exception_t>( 
            boost::bind( &translate, py_class, _2 ) );

"""

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'cppexceptions'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        #mb.class_( 'custom_exception_t' ).add_code( REGISTER_CODE, False)
        #mb.build_code_creator( tester_t.EXTENSION_NAME )

        #mb.code_creator.add_include( "boost/bind.hpp" )
        #mb.code_creator.add_include( "iostream" )
        #translate = code_creators.custom_text_t( TRANSLATE_CODE )
        #mb.code_creator.adopt_creator( translate, -1 )
        custom_exception = mb.class_( 'custom_exception_t' )        
        custom_exception.translate_exception_to_string( 'PyExc_RuntimeError', 'exc.what().c_str()' )
        
    def run_tests( self, module):
        try:
            module.throw_custom_exception()
        except RuntimeError, error:
            self.failUnless( "profe of concept" in str( error ) )
        #custom_exception_t = module.custom_exception_t
        #bases = list( custom_exception_t.__bases__ ) + [RuntimeError]
        #custom_exception_t.__bases__ = tuple( bases )
        #custom_exception_t.__str__ = custom_exception_t.what
        #try:
            #module.throw_custom_exception()
        #except RuntimeError, error:
            #self.failUnless( str(error) == "profe of concept" )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
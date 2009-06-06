# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

auto_convert_code = \
"""
namespace bp = boost::python;

struct PyStringToUTF16{

    typedef custom_strings::utf16_t string_type;

    static void* convertible(PyObject* obj){
        if( PyString_Check(obj) || PyUnicode_Check(obj) ){
            return obj;
        }
        else{
            return 0;
        }
    }

    static void
    construct( PyObject* obj, bp::converter::rvalue_from_python_stage1_data* data){
        typedef bp::converter::rvalue_from_python_storage<string_type> string_storage_t;
        string_storage_t* the_storage = reinterpret_cast<string_storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;

        bp::object py_str( bp::handle<>( bp::borrowed( obj ) ) );                
        if( PyString_Check(obj) ){        
            std::string c_str = bp::extract<std::string>( py_str );
            new (memory_chunk) string_type(c_str);
        }
        else{ //unicode
            std::wstring c_str = bp::extract<std::wstring>( py_str );
            new (memory_chunk) string_type(c_str);
        }
        data->convertible = memory_chunk;        
    }
};
"""

register_auto_convert_code = \
"""
bp::converter::registry::push_back( &PyStringToUTF16::convertible
                                    , &PyStringToUTF16::construct
                                    , bp::type_id<PyStringToUTF16::string_type>() );

"""

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'custom_string'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )
            
    def customize( self, mb ):
        mb.add_declaration_code( auto_convert_code )
        mb.add_registration_code( register_auto_convert_code )
        mb.constructors().allow_implicit_conversion = False
        mb.variable( 'm_name' ).use_make_functions = True
        mb.variable( 'm_123' ).use_make_functions = True
        mb.class_( 'name_t' ).add_properties();
        
    def run_tests( self, module):
        self.failUnless( "1" == module.utf16_to_string( "1" ) )
        self.failUnless( "22" == module.utf16_to_wstring( u"22" ) )
        n = module.name_t()
        n.m_name = '456'
        self.failUnless( '456' == module.utf16_to_string( n.m_name ) )
        self.failUnless( '123' == module.utf16_to_string( n.m_123 ) )
        
        n.name = '11'
        self.failUnless( '11' == module.utf16_to_string( n.m_name ) )
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus.module_builder import call_policies

get_size_code = """
struct raw_data_size_t{
    ssize_t
    operator()( boost::python::object self ){
        boost::python::object raw_data = self.attr( "raw_data" );
        return boost::python::len( raw_data );
    }
};
"""

get_size_code = """
struct raw_data_size_t{
    ssize_t
    operator()( boost::python::tuple args ){
        boost::python::object self = args[0];
        call_policies::return_range_image_t& image 
            = boost::python::extract<call_policies::return_range_image_t&>( self );
        return image.raw_data.size();
    }
};
"""

get_create_images_size = """
struct get_create_images_size_t{
    ssize_t
    operator()( boost::python::object self ){
        return 3;
    }
};
"""

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'call_policies'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.calldef( 'return_second_arg' ).call_policies = call_policies.return_arg( 2 )
        mb.calldef( 'return_self' ).call_policies = call_policies.return_self()

        mb.class_( 'impl_details_t' ).opaque = True

        mb.calldef( 'get_opaque' ).call_policies \
            = call_policies.return_value_policy( call_policies.return_opaque_pointer )

        mb.calldef( 'get_fundamental_ptr_value' ).call_policies \
            = call_policies.return_value_policy( call_policies.return_pointee_value )

        mb.calldef( 'get_fundamental_ptr_value_null' ).call_policies \
            = call_policies.return_value_policy( call_policies.return_pointee_value )

        mb.calldef( 'create_arr_3' ).call_policies \
            = call_policies.convert_array_to_tuple( 3, call_policies.memory_managers.delete_ )

        image = mb.class_('return_range_image_t')
        image.add_declaration_code( get_size_code )
        image.add_declaration_code( get_create_images_size )
        get_raw_data = image.mem_fun( 'get_raw_data' )
        get_raw_data.call_policies \
            = call_policies.return_range( get_raw_data, 'raw_data_size_t' )
        get_raw_data_const = image.mem_fun( 'get_raw_data_const' )
        get_raw_data_const.call_policies \
            = call_policies.return_range( get_raw_data_const, 'raw_data_size_t' )
        create_images = image.mem_fun( 'create_images' )
        create_images.call_policies \
            = call_policies.return_range( create_images
                                          , 'get_create_images_size_t'
                                          , call_policies.return_value_policy(call_policies.reference_existing_object) )

    def run_tests(self, module):
        self.failUnless( module.compare( module.my_address() ) )

        x = module.return_second_arg( 1, 2, 3)
        self.failUnless( x == 2 )

        x = module.dummy()
        y = module.return_self( x, 0 )
        self.failUnless( x.id() == y.id() )

        y = module.copy_const_reference( x )
        self.failUnless( x.id() != y.id() )

        cont = module.container()
        self.failUnless( 1977 == cont[1977] )

        self.failUnless( 0.5 == module.get_fundamental_ptr_value() )
        
        self.failUnless( None is module.get_fundamental_ptr_value_null() )
        
        module.get_impl_details()

        module.get_opaque()
        
        x = module.arrays()
        for i in range( 4 ):
            arr3 = x.create_arr_3()
            self.failUnless( arr3 == (0,1,2) )
            
        image = module.return_range_image_t()
        raw_data = image.get_raw_data()
        self.failUnless( ['1', '\0', '2']==list( raw_data ) )
        raw_data[1] = 'x'            
        self.failUnless( raw_data[1] == image.raw_data[1] )
        for index, img in enumerate( image.create_images() ):
            print index, img

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

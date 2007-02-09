# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import math
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies


class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'function_transformations'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        
        mb.global_ns.calldefs().create_with_signature = True
        
        hello_world = mb.free_fun( 'hello_world' )
        hello_world.add_transformation( ft.output(0) )
        
        calc = mb.class_('calculator_t' )
        assign_funs = calc.mem_funs( lambda decl: decl.name.startswith( 'assign' ) )
        assign_funs.add_transformation( ft.output(0), ft.output(1) )
        
        clone_and_assign_5 = calc.mem_fun( 'clone_and_assign_5' )
        clone_and_assign_5.add_transformation( ft.output(0) )
        clone_and_assign_5.call_policies = call_policies.return_value_policy( call_policies.manage_new_object )
            
        window = mb.class_( 'window_t' )
        window.mem_fun( 'resize' ).add_transformation( ft.input(0), ft.input(1) )
        window.mem_fun( 'resize_in_out' ).add_transformation( ft.inout(0), ft.inout(1) )
        
        point3d = mb.class_( 'point3d_t' )
        point3d.add_wrapper_code( '' )
        point3d.mem_fun( 'initialize' ).add_transformation( ft.input_static_array(0, size=3) )
        point3d.mem_fun( 'position' ).add_transformation( ft.output_static_array(0, size=3) )
        point3d.mem_fun( 'distance' ).add_transformation( ft.output(1) )
        
        image = mb.class_( "image_t" )
        image.always_expose_using_scope = True
        image.member_function( "get_size" )
        image.member_function( "get_size" ).add_transformation( ft.output(0), ft.output(1) )
        image.member_function( "get_one_value" ).add_transformation( ft.output(0) )
        image.member_function( "get_size2" ).add_transformation( ft.output(0), ft.output(1) )
        image.member_function( "input_arg" ).add_transformation( ft.input(0) )
        image.member_function( "fixed_input_array" ).add_transformation( ft.input_static_array(0,3) )
        image.member_function( "fixed_output_array" ).add_transformation( ft.output_static_array(0,3) )
        mb.free_function("get_cpp_instance").call_policies \
            = call_policies.return_value_policy(call_policies.reference_existing_object)
        mb.variable( "cpp_instance" ).exclude()

        cls = mb.class_("no_virtual_members_t")
        cls.member_function("member").add_transformation( ft.output(0) )

        cls = mb.class_("ft_private_destructor_t")
        cls.member_function("get_value").add_transformation( ft.output(0) )

        mb.decls(lambda decl: decl.name.startswith("_")).exclude()

        cls = mb.class_("bug_render_target_t")
        cls.mem_fun("get_statistics", arg_types=['float &']*2).add_transformation( ft.output(0), ft.output(1) )
        
        cls = mb.class_( 'modify_type_tester_t' )
        do_nothing = cls.mem_fun( 'do_nothing' )
        do_nothing.add_transformation( ft.modify_type(0, declarations.remove_reference ) )

        clone = cls.mem_fun( 'clone' )
        clone.call_policies = call_policies.return_value_policy( call_policies.manage_new_object )
        clone.add_transformation( ft.modify_type(0, declarations.remove_reference ) )
        
        cls = mb.class_( 'input_c_buffer_tester_t')
        write_mf = cls.mem_fun( 'write' )
        write_mf.add_transformation( ft.input_c_buffer( 'buffer', 'size' ) )
        write_s = cls.mem_fun( 'write_s' )
        write_s.add_transformation( ft.input_c_buffer( 'buffer', 'size' ) )
        
    def run_tests(self, module):
        """Run the actual unit tests.
        """

        ####### Do the tests directly on the wrapper C++ class ########
        calc = module.calculator_t()
        self.failUnless( ( 0, 1, 2 ) == calc.assign_0_1_2() )
        self.failUnless( ( 1, 2 ) == calc.assign_1_2() )
        calc2, five = calc.clone_and_assign_5()
        self.failUnless( five == 5 )
        self.failUnless( calc2 )
        #test make_object function
        self.failUnless( sys.getrefcount( calc ) == sys.getrefcount( calc2 ) )

        window = module.window_t()
        window.height = 0
        window.width = 0
        window.resize( 1, 2 )
        self.failUnless( window.height==1 and window.width==2 )
        square, h, w = window.resize_in_out( 3, 7 )
        self.failUnless( square == 1*3*2*7 and h==3 and w==2*7 )
        self.failUnless( window.height==3 and window.width==2*7 )
        
        point3d = module.point3d_t()
        result = point3d.initialize( [ 1,2,3 ] )
        self.failUnless( result== 1*2*3 and point3d.x == 1 and point3d.y==2 and point3d.z==3 )
        self.failUnless( [1,2,3] == point3d.position() )
        self.failUnless( module.point3d_t.distance( point3d ) == math.sqrt( 1*1 + 2*2 + 3*3 ) )
        
        self.failUnless( module.hello_world() == "hello world!" )
        img = module.image_t( 2, 6)

        # Check a method that returns two values by reference
        self.assertEqual(img.get_size(), (2,6))

        # Check a method that only returns one value by reference
        self.assertEqual(img.get_one_value(), 2)

        # Check if the C++ class can also be passed back to C++
        self.assertEqual(module.get_image_one_value(img), 2)

        # Check get_size2()
        self.assertEqual(img.get_size2(), (0,2,6))
        self.assertEqual(img.get_size2(1), (1,2,6))

        # Check the input_arg method
        self.assertEqual(img.input_arg(5), 5)

        # Check the fixed_input_array method
        self.assertEqual(img.fixed_input_array([1,2,3]), 6)
        self.assertEqual(img.fixed_input_array((1,2,3)), 6)

        # Check the fixed_output_array method
        self.assertEqual(img.fixed_output_array(), [1,2,3])

        self.assertEqual(module.ft_private_destructor_t.get_value(), 21)

        ####### Do the tests on a class derived in Python ########

        class py_image1_t(module.image_t):
            def __init__(self, h, w):
                module.image_t.__init__(self, h, w)
                self.fixed_output_array_mode = 0

            # Override a virtual method
            def get_one_value(self):
                return self.m_height+1

            def fixed_output_array(self):
                from named_tuple import named_tuple
                # Produce a correct return value
                if self.fixed_output_array_mode==0:
                    return named_tuple( ('v', (2,5,7)) )
                # Produce the wrong type
                elif self.fixed_output_array_mode==1:
                    return 5
                # Produce a sequence with the wrong number of items
                elif self.fixed_output_array_mode==2:
                    return named_tuple( ('v', (2,5)) )

        pyimg1 = py_image1_t(3,7)

        # Check a method that returns two values by reference
        self.assertEqual(pyimg1.get_size(), (3,7))

        # Check a method that only returns one value by reference
        self.assertEqual(pyimg1.get_one_value(), 4)

        # Check if the Python class can also be passed back to C++
        self.assertEqual(module.get_image_one_value(pyimg1), 4)

        # Check if fixed_output_array() is correctly called from C++
        self.assertEqual(module.image_fixed_output_array(pyimg1), 14)
        pyimg1.fixed_output_array_mode = 1
        self.assertRaises(TypeError, lambda : module.image_fixed_output_array(pyimg1))
        pyimg1.fixed_output_array_mode = 2
        self.assertRaises(ValueError, lambda : module.image_fixed_output_array(pyimg1))

        class py_image2_t(module.image_t):
            def __init__(self, h, w):
                module.image_t.__init__(self, h, w)

            # Override a virtual method and invoke the inherited method
            def get_one_value(self):
                return module.image_t.get_one_value(self)+2

        pyimg2 = py_image2_t(4,8)

        # Check the derived get_one_value() method
        self.assertEqual(pyimg2.get_one_value(), 6)

        # Check if the Python class can also be passed back to C++
        self.assertEqual(module.get_image_one_value(pyimg2), 6)

        ####### Do the tests on a class instantiated in C++ ########

        cppimg = module.get_cpp_instance()

        # Check a method that returns two values by reference
        self.assertEqual(cppimg.get_size(), (12,13))

        # Check a method that only returns one value by reference
        self.assertEqual(cppimg.get_one_value(), 12)

        # Check if the C++ class can also be passed back to C++
        self.assertEqual(module.get_image_one_value(cppimg), 12)

        ######### Test no_virtual_members_t ########

        cls = module.no_virtual_members_t()
        self.assertEqual(cls.member(), (True, 17))

        class py_bug_render_target_t( module.bug_render_target_t ):
            def __init__( self ):
                module.bug_render_target_t.__init__( self )
                
            def get_statistics( self ):
                from named_tuple import named_tuple
                return named_tuple( ( 'x', 2.0 ), ( 'y', 3.0 ) )
        tmp = py_bug_render_target_t()
        
        tmp.get_statistics()
        self.failUnless( 2.0 + 3.0 == module.bug_render_target_t.get_static_statistics( tmp ) )
        
        tmp = module.modify_type_tester_t()
        self.failUnless( 123 == tmp.do_nothing(123) )
        self.failUnless( tmp != tmp.clone(123) )

        tmp = module.input_c_buffer_tester_t()
        hw = 'hello world'
        dummy = 11
        self.failUnless( 'hello world' == tmp.write( list( hw ), dummy ) )
        self.failUnless( 'hello world' == tmp.write_s( dummy, tuple( list( hw ) ) ) )
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pygccxml import declarations

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'member_functions'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        names = [
            'protected_protected_derived_t'
            , 'protected_public_derived_t'
            , 'protected_base_t'
            , 'public_derived_t'
            , 'public_base_t'
            , 'private_derived_t'
            , 'private_base_t'
        ]

        mb.classes( lambda decl: decl.name in names ).always_expose_using_scope = True
        #will reporoduce bug
        mb.class_('callable_t').always_expose_using_scope = True
        mb.BOOST_PYTHON_MAX_ARITY = 1
        mb.free_function( 'instantiate_mem_fun_environment' ).exclude()
        
        env = mb.class_( 'mem_fun_environment_t' )
        get_value = env.member_function( 'get_value', return_type='int' )
        get_value.alias = 'get_value_int' 
        get_value.name = get_value.demangled_name
        get_value.create_with_signature = True
        
        mb.run_query_optimizer()
        
        mb.class_('mem_fun_constness_t' ).always_expose_using_scope = True
        
        mb.mem_funs().add_override_precall_code( '/*override precall code*/' )
        mb.mem_funs().add_default_precall_code( '/*default precall code*/' )
        
    def create_py_immutable_by_ref( self, module ):
        class py_immutable_by_ref( module.immutable_by_ref_t ):
            def __init__( self ):
                module.immutable_by_ref_t.__init__( self )

            def identity( self, x ):
                return x*2

        return py_immutable_by_ref()

    def create_test_class_inst(self, class_ ):
        class tester_impl_t( class_ ):
            def __init__(self):
                class_.__init__( self )

            def pure_virtual(self, x):
                return x + x

            def pure_virtual_overloaded( self, *args ):
                assert 1 <= len(args) <= 2
                if 1 == len( args ):
                    return args[0] + args[0]
                else:
                    return args[0] + args[1]

            def pure_virtual_const( self, x ):
                return x + x

            def pure_virtual_const_overloaded( self, *args ):
                assert 1 <= len(args) <= 2
                if 1 == len( args ):
                    return args[0] + args[0]
                else:
                    return args[0] + args[1]

        return tester_impl_t()

    def _test_instance( self, inst, class_defined_in_cpp):
        self.failUnless( 23 == inst.regular( 23 ) )
        self.failUnless( 9 == inst.regular_overloaded( 3 ) )
        self.failUnless( 15 == inst.regular_overloaded( 3, 5 ) )

        self.failUnless( 23 == inst.regular_const( 23 ) )
        self.failUnless( 9 == inst.regular_const_overloaded( 3 ) )
        self.failUnless( 15 == inst.regular_const_overloaded( 3, 5 ) )

        self.failUnless( 23 == inst.virtual_( 23 ) )
        self.failUnless( 9 == inst.virtual_overloaded( 3 ) )
        self.failUnless( 15 == inst.virtual_overloaded( 3, 5 ) )

        self.failUnless( 23 == inst.virtual_const( 23 ) )
        self.failUnless( 9 == inst.virtual_const_overloaded( 3 ) )
        self.failUnless( 15 == inst.virtual_const_overloaded( 3, 5 ) )

        if class_defined_in_cpp:
            self.failUnless( 23 == inst.pure_virtual( 23 ) )
            self.failUnless( 9 == inst.pure_virtual_overloaded( 3 ) )
            self.failUnless( 15 == inst.pure_virtual_overloaded( 3, 5 ) )

            self.failUnless( 23 == inst.pure_virtual_const( 23 ) )
            self.failUnless( 9 == inst.pure_virtual_const_overloaded( 3 ) )
            self.failUnless( 15 == inst.pure_virtual_const_overloaded( 3, 5 ) )
        else:
            self.failUnless( 46 == inst.pure_virtual( 23 ) )
            self.failUnless( 6 == inst.pure_virtual_overloaded( 3 ) )
            self.failUnless( 8 == inst.pure_virtual_overloaded( 3, 5 ) )

            self.failUnless( 46 == inst.pure_virtual_const( 23 ) )
            self.failUnless( 6 == inst.pure_virtual_const_overloaded( 3 ) )
            self.failUnless( 8 == inst.pure_virtual_const_overloaded( 3, 5 ) )

    def run_tests(self, module):
        derived = module.protected_public_derived_t()
        self._test_instance( derived, True )
        derived = module.protected_protected_derived_t()
        self._test_instance( derived, True )
        derived = self.create_test_class_inst( module.protected_base_t )
        self._test_instance( derived, False )
        derived = self.create_test_class_inst( module.public_derived_t )
        self._test_instance( derived, False )
        derived = self.create_test_class_inst( module.public_base_t )
        self._test_instance( derived, False )

        x = self.create_py_immutable_by_ref(module)
        self.failUnless( x.identity( '11' ) == '1111' )
        self.failUnless( module.immutable_by_ref_t.call_identity(x, '11') == '1111' )
        
        env = module.mem_fun_environment_t()
        env.get_value_int("xxx")
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

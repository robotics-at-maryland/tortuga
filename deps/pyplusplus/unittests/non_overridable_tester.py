# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import ctypes
import unittest
import fundamental_tester_base
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'non_overridable'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
    
    def customize( self, mb ):
        mb.mem_funs( 'string_ref' ).call_policies \
            = call_policies.return_value_policy( call_policies.copy_non_const_reference )
        
        non_overridable_pv_t = mb.class_( 'non_overridable_pv_t' )
        non_overridable_pv_t.mem_fun( 'void_ptr' ).mark_as_non_overridable( reason='xxx void*' )
        
    def create_pv_derived(self, module):
        class py_pv_t( module.non_overridable_pv_t ):
            def __init__( self ):
                module.non_overridable_pv_t.__init__( self )
            
            def void_ptr( self ):
                return ctypes.c_void_p( id(self) )
        return py_pv_t()
    
    def create_v_derived(self, module):
        class py_v_t( module.non_overridable_v_t ):
            def __init__( self ):
                module.non_overridable_v_t.__init__( self )
            
            def void_ptr( self ):
                return ctypes.c_void_p( id(self) )
        return py_v_t()
    
    def run_tests( self, module):
        x = self.create_pv_derived(module)
        self.failUnlessRaises( RuntimeError, module.non_overridable_pv_t.test, x )

        y = self.create_v_derived(module)
        self.failUnlessRaises( ReferenceError, module.non_overridable_v_t.test, y )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

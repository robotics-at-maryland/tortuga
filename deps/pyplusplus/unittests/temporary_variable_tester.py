# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators 
from pyplusplus import module_builder

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'temporary_variable'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb):
        functions = mb.calldefs( lambda decl: not decl.call_policies 
                                             and 'name' in decl.alias )
        functions.call_policies \
            = module_builder.call_policies.return_value_policy( module_builder.call_policies.copy_non_const_reference )

    def _create_derived_from_named_item(self, extmodule):
        class apple_t( extmodule.named_item_t ):
            def __init__( self ):
                extmodule.named_item_t.__init__( self )
            def name( self ): #this function will be never called
                return "apple"
        return apple_t()
    
    def _create_derived_from_pure_virtual(self, extmodule):
        class python_pure_virtual_t( extmodule.pure_virtual_t ):
            def __init__( self ):
                extmodule.pure_virtual_t.__init__( self )
            def name( self ): #this function will be never called
                return "name"
            def name_ref( self ): #this function will be never called
                return "name_ref"
        return python_pure_virtual_t()

    def _create_derived_from_virtual(self, extmodule):
        class python_virtual_t( extmodule.virtual_t ):
            def __init__( self ):
                extmodule.virtual_t.__init__( self )
            def name( self ): #this function will be never called
                return "name"
            def name_ref( self ): #this function will be never called
                return "name_ref"
        return python_virtual_t()
        
    def run_tests(self, extmodule):   
        apple = self._create_derived_from_named_item( extmodule )
        self.failUnless( extmodule.get_name( apple ) == 'no name' )
        pv = self._create_derived_from_pure_virtual( extmodule )

        self.failIfNotRaisesAny( extmodule.get_name, pv )
        self.failIfNotRaisesAny( extmodule.get_name_ref, pv )
        
        virtual = self._create_derived_from_virtual( extmodule )
        self.failUnless( extmodule.get_name( virtual ) == 'no name' )
        self.failUnless( extmodule.get_name_ref( virtual ) == 'no name' )
        
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import module_builder

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'regression1'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize( self, mb ):
        set_updater = mb.calldef( 'set_updater' )
        set_updater.call_policies = module_builder.call_policies.with_custodian_and_ward( 1, 2 )
   
    def create_other_updater(self, module):
        class other_updater( module.updater_i ):
            def __init__( self ):
                module.updater_i.__init__( self )
            
            def update_by_ref( self, d ):
                d.a = -1
                
            def update_by_ptr( self, d ):
                d.a = -101
                
        return other_updater()
        
    def run_tests( self, module):
        update_holder = module.updater_holder()
        updater = module.some_updater()
        update_holder.set_updater( updater )
        update_holder.update_by_ref()
        self.failUnless( 10 == update_holder.get_data().a )
        update_holder.update_by_ptr()
        self.failUnless( 101 == update_holder.get_data().a )

        other_updater = self.create_other_updater( module )
        update_holder.set_updater( other_updater )
        update_holder.update_by_ref()
        self.failUnless( -1 == update_holder.get_data().a )
        update_holder.update_by_ptr()
        self.failUnless( -101 == update_holder.get_data().a )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
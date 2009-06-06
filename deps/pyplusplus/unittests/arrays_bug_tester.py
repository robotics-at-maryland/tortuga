# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'arrays_bug'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.add_registration_code( 'pyplusplus::containers::static_sized::register_array_1< int, 10 >( "X1" );' )
        mb.add_registration_code( 'pyplusplus::containers::static_sized::register_array_1< int, 10 >( "X2" );' )
            
    def run_tests( self, module):
        m = module.arrays_bug
        c = m.container()
        x = c.items[0].values[0]
        c.items[0].values[0] = 1
        y = c.items[0]
        c.items[0] = m.item()
        
        self.failUnless( id(module.X1) == id(module.X2) == id( c.items[0].values.__class__ ) )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'enums'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        mb.enum( 'Chisla' ).include()
        
        color = mb.enumeration( 'color' )
        color.alias = 'Color'
        color.value_aliases['red'] = 'RED'
        color.value_aliases['blue']= 'BLUE'
        color.export_values = ['red', 'blue']

    def get_green( self, module):
        module.green

    def run_tests(self, module):        
        self.failUnless( module.Color.RED == 1 )
        self.failUnless( module.Color.green == 2 )
        self.failUnless( module.Color.BLUE == 4 )
        self.failUnless( module.RED )
        self.failUnless( module.BLUE )
        self.failIfNotRaisesAny( lambda: self.get_green( module ) )
        self.failUnless( 1 == module.to_int( 1 ) )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

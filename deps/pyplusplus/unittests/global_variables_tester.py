# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'global_variables'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):        
        mb.variable('non_const_var').alias = 'NonConstVar'

    def run_tests(self, module):        
        self.failUnless( module.NonConstVar == module.color.blue )
        self.failUnless( module.const_var == module.color.red )
        module.init_garray()
        self.failUnless( 10 == len( module.garray ) )
        for index in range( 10 ):
            self.failUnless( -index == module.garray[index].value )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
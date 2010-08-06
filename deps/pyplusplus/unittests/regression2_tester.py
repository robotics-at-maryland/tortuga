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
    EXTENSION_NAME = 'regression2'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize( self, mb ):
        matcher = ~declarations.namespace_matcher_t()
        matcher = matcher & declarations.declaration_matcher_t( name='get_a' )
        decls = mb.decls( matcher )
        decls.ignore = True
    
    def run_tests(self, module):        
        x_inst = module.x()
        self.failIf( hasattr( x_inst, 'get_a' ) )
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
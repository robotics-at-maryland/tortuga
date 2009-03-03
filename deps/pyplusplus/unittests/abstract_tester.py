# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'abstract'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def create_circle(self, module):
        class circle( module.shape ):
            def __init__(self):
                module.shape.__init__( self )
            
            def color(self):
                return module.colors.green
            
        return circle()
    
    def run_tests( self, module):
        sh = module.shape()
        sq = module.square()
        cr = self.create_circle(module)
        self.failIfNotRaisesAny( sh.color )
        self.failUnless( sq.color() == module.colors.red)
        self.failUnless( module.call(sq) == module.colors.red)
        self.failUnless( cr.color() == module.colors.green )
        self.failUnless( module.call(cr) == module.colors.green )         

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
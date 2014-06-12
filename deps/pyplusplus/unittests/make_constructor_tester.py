# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'make_constructor'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mc = mb.namespace( 'mc' )
        numbers = mc.class_( 'numbers_t' )
        numbers.add_fake_constructors( mc.calldefs( 'create' ) )
        numbers.add_fake_constructors( mc.calldefs(lambda d: d.name.startswith('~') ) )
        mc.calldef( 'create', arg_types=[None]*5 ).exclude()

    def run_tests(self, module):
        n = module.numbers_t( 1,2,3,4)
        self.failUnless( n.x == 1+2 and n.y == 3+4)
        n = module.numbers_t( 10, 18)
        self.failUnless( n.x == 10 and n.y == 18)

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

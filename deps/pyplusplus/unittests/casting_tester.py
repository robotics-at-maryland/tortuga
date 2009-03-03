# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'casting'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )
    def customize( self, mb ):
        mb.class_("float_vector").add_registration_code("def( bp::init< const casting::float_vector& >() )")

    def run_tests( self, module):
        x_inst = module.x()
        x_inst.value = 25
        self.failUnless( 25 == module.identity(x_inst) )
        self.failUnless( 25 == module.x_value(25) )
        self.failUnless( 1 == module.x_value(True) )
        self.failUnless( 0 == module.x_value(False) )
        fv = module.float_vector( 5.0 )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

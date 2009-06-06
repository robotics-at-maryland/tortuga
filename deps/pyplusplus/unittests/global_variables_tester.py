# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import ctypes
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
        mb.variable( 'some_value' ).expose_address = True

    def run_tests(self, module):
        self.failUnless( module.NonConstVar == module.color.blue )
        self.failUnless( module.const_var == module.color.red )
        module.init_garray()
        self.failUnless( 10 == len( module.garray ) )
        for index in range( 10 ):
            self.failUnless( -index == module.garray[index].value )
        self.failUnless( 3 == len( module.someSin )
                         and module.someSin[0] == 'A'
                         and module.someSin[1] == 'B'
                         and module.someSin[2] == '\0' )

        self.failUnless( module.some_value == module.get_some_value_address() )
        self.failUnless( -7 == module.get_some_value() )
        x = ctypes.c_int.from_address( module.some_value )
        self.failUnless( x.value == module.get_some_value() )
        x.value = 9
        self.failUnless( x.value == module.get_some_value() == 9)

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

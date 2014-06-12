# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import pdb
import ctypes
import unittest
import fundamental_tester_base

class actual_data_t( ctypes.Union ):
    _fields_ = [( "i", ctypes.c_int ), ( 'd', ctypes.c_double )]

#this is compilation test
class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'unions'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb):
        mb.add_declaration_code('extern "C"{ int mmm( int i, int j ){ return i + j; } }' )

    def run_tests(self, module):
        obj = module.data_t()
        actual_data = actual_data_t.from_address( obj.data )
        obj.set_d( 4.0 )
        self.failUnless( actual_data.d == 4.0 )
        obj.set_i( 1977 )
        self.failUnless( actual_data.i == 1977 )
        actual_data.i = 18
        self.failUnless( obj.get_i() == 18 )
        actual_data.d = 12.12
        self.failUnless( obj.get_d() == 12.12 )

        obj2 = module.data2_t()
        obj2.set_d( 4.0 )
        self.failUnless( obj2.d == 4.0 )
        obj2.set_i( 1977 )
        self.failUnless( obj2.i == 1977 )

        if 'win' not in sys.platform:
            mdll = ctypes.cdll.LoadLibrary( module.__file__ )
            self.failUnless( 4 == mdll.mmm( 1, 3 ) )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

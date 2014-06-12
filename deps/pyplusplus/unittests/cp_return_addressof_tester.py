# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import ctypes
import unittest
import fundamental_tester_base
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'cp_return_addressof'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.classes().always_expose_using_scope = True
        mb.calldef( 'get_buffer' ).call_policies \
            = call_policies.return_value_policy( call_policies.return_addressof )

    def run_tests(self, module):
        image = module.image_t()
        buffer_type = ctypes.c_int * 5
        buffer = buffer_type.from_address( image.get_buffer() )
        self.failUnless( [0,1,2,3,4] == list( buffer ) )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

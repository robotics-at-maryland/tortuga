# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import math
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies


class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'bool_by_ref'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):        
        set_flag = mb.mem_fun( '::tests::listener::listen' )
        set_flag.add_transformation( ft.inout(2) )
        
       
    def run_tests(self, module):
        class list1 ( module.listener ):
            def __init__ ( self ):
                module.listener.__init__(self)
            def listen( self, id, name,  skip):
                print "Python: listen called with", id, name, skip
                return 11## Test always returns True...
        
        c = list1()
        ret = module.callListener( c )
        print ret
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

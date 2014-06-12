# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'overloads_macro'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        calc = mb.class_( 'calculator_t' )
        calc.always_expose_using_scope = True
        calc.member_functions( 'add' ).set_use_overload_macro( True )
        add_doubles = calc.member_function( 'add', arg_types=['double', 'double'] )
        add_doubles.set_use_overload_macro( False )

        mb.free_functions( 'add' ).set_use_overload_macro( True )
        add_doubles = mb.free_function( 'add', arg_types=['double', 'double'] )
        add_doubles.set_use_overload_macro( False )


    def run_tests(self, module):
        calc = module.calculator_t()
        self.failUnless( 3 == calc.add( 1, 2 ) )
        self.failUnless( 9 == calc.add( 3, 2, 3 ) )
        self.failUnless( 3 == calc.add( 1.5, 1.5 ) )
        self.failUnless( 3 == module.add( 1, 2 ) )
        self.failUnless( 9 == module.add( 3, 2, 3 ) )
        self.failUnless( 3 == module.add( 1.5, 1.5 ) )


def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

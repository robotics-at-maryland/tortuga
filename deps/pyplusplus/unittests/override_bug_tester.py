# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import function_transformers as FT

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'override_bug'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        mb.class_("Derived2").member_functions("eval_c").exclude()
        mb.class_( 'BB' ).include()
        do_smth = mb.mem_fun( '::override_bug::AA::do_smth' )
        do_smth.add_transformation( FT.output(0), alias='do_smth_a' )
        do_smth = mb.mem_fun( '::override_bug::BB::do_smth' )
        do_smth.add_transformation( FT.output(0), FT.output(1), alias='do_smth_b' )
        mb.class_( 'XX' ).mem_fun( 'do_smth' ).exclude()

    def run_tests(self, module):
        class C( module.B ):
            def __init__( self ):
                module.B.__init__( self )
            def foo( self ):
                return ord( 'c' )
        self.failUnless( ord('c') == module.invoke_foo( C() ) )

        class Derived4(module.Derived3):
            def __init__( self ):
                module.Derived3.__init__( self )
            def eval_a(self):
                return 3
            def eval_c(self):
                return 300 # ignored because eval_c excluded

        self.failUnless( 22223 == module.eval( Derived4() ) )

        bb = module.BB()
        print dir( bb )
        x = bb.do_smth_b()
        self.failUnless( x[0] == x[1] == ord( 'b' ) )

        # Notes:
        # would return 22222 before any patch, since Derived3 wouldn't have a wrapper
        # would return 22123 with my original "ignore" handling and a list
        # instead of a set, because eval_c would be excluded naively
        # would return 22323 if ignore flag wouldn't be considered
        # would return ????3 (1s in some locations and 2s in others because of
        # hashing) if set wouldn't be replaced by a list
        # would return 11113 if protected virtual methods wouldn't be included

        b = module.B()
        self.failUnless( 7 == b.foo( 3,4))
        
        d = module.D()
        self.failUnless( 12 == d.foo(3,4) )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

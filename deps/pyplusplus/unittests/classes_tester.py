# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'classes'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.classes().expose_this = True
        mb.classes().expose_sizeof = True
        mb.class_( 'fundamental2' ).alias = 'FUNDAMENTAL2'
        apple = mb.class_( 'apple' )
        self.failUnless( apple.alias == 'the_tastest_fruit' )
        apple.alias = 'apple'
        apple.add_registration_code( "/*head*/", works_on_instance=False, tail=False )
        apple.add_registration_code( "/*tail*/", works_on_instance=False, tail=True )

        protected_static_t = mb.class_( 'protected_static_t' )
        self.failUnless( 'PROTECTED_STATIC' in protected_static_t.alias)
        protected_static_t.alias = 'protected_static_t'

    def run_tests(self, module):
        self.failIfRaisesAny( module.fundamental1 )
        self.failIfRaisesAny( module.FUNDAMENTAL2 )

        self.failIfRaisesAny( module.apple )
        self.failUnless( isinstance( module.apple(), module.fruit ) )

        self.failIfRaisesAny( module.noncopyable1 )

        self.failIfRaisesAny( module.abstract )

        self.failIfRaisesAny( module.constructor1 )
        self.failIfRaisesAny( module.constructor1, 1, 2 )
        self.failIfRaisesAny( module.constructor1, 3, module.constructor1() )

        self.failIfRaisesAny( module.scope_based_exposer )
        self.failIfRaisesAny( lambda: module.scope_based_exposer.EColor.red )

        self.failUnless( 24 == module.protected_static_t.identity(24) )
        self.failUnless( 29 == module.protected_static_t.identity(29) )
        self.failUnless( -24 == module.protected_static_t().invert_sign(24) )
        self.failUnless( 67 == module.protected_static_t().invert_sign(-67) )
        self.failUnless( module.protected_static_t().this )
        self.failUnless( module.protected_static_t().sizeof )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

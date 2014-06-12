# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'no_init'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        cls = mb.class_( 'value_plus_x_t' )
        cls.add_registration_code( 'bp::register_ptr_to_python< boost::shared_ptr< no_init_ns::value_plus_x_t > >();', False )

    def create_identity_value( self, module, v ):
        class identity_value_t( module.value_i ):
            def __init__( self, v ):
                module.value_i.__init__( self )
                self.value = v

            def get_value(self):
                return self.value

        return identity_value_t(v)

    def create_plus_5_value( self, module, v ):
        class plus_5_value_t( module.value_plus_x_t ):
            def __init__( self, v ):
                module.value_plus_x_t.__init__( self, v )

            def get_plus_value(self):
                print "I am here"
                return 5
        return plus_5_value_t( v )

    def run_tests(self, module):
        identity_value = self.create_identity_value( module, 23 )
        self.failUnless( 23 == module.get_value( identity_value ) )
        self.failUnless( -1 == module.get_value( None ) )
        plus_5_value = self.create_plus_5_value( module, 23 )
        print plus_5_value.get_value()
        print module.get_value( plus_5_value )
        self.failUnless( 28 == module.get_value( plus_5_value ) )



def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

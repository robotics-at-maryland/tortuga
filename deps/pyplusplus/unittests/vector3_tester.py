# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'vector3'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        vec3 = mb.class_( 'Vector3' )
        vec3.always_expose_using_scope = True
        #mb.calldefs().use_keywords = False
        vec3.add_registration_code( 'add_property( "val", &vector3::Vector3::val)' )
        vec3.add_registration_code( 'add_property( "x", &vector3::Vector3::x)' )
        vec3.add_registration_code( 'add_property( "y", &vector3::Vector3::y)' )
        vec3.add_registration_code( 'add_property( "z", &vector3::Vector3::z)' )

        
    def run_tests( self, module):
        v3 = module.Vector3()
        self.failUnless( v3.ZERO == v3.do_smth() )
        #test copy constructor
        self.failUnless( module.Vector3(v3.ZERO) == v3.do_smth() )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'module_body'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb):
        item = mb.class_( 'item_t' )
        item.include()
        item.add_declaration_code( "int get11( const mb::item_t& item ){ return 11;}" )
        item.add_registration_code( 'def( "get11", &get11 )' )
        item.add_wrapper_code( '//this is wrapper code' )

        mb.add_declaration_code( "int get1(){ return 1;} ")
        mb.add_declaration_code( "//this is a comment", False )
        mb.add_registration_code( 'bp::def( "get1", &get1 );' )
        mb.add_registration_code( '//this is another comment', False )

        mb.build_code_creator( self.EXTENSION_NAME )

        mb.add_constants( version='"0.0.0"')

    def run_tests(self, module):
        self.failUnless( 1 == module.get1() )
        self.failUnless( 11 == module.item_t().get11() )
        self.failUnless( "0.0.0" == module.version )
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

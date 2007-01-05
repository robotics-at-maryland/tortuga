# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus.module_builder import call_policies
from pyplusplus.module_creator import sort_algorithms

class tester_base_t(fundamental_tester_base.fundamental_tester_base_t):
    def __init__( self, name, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( self, name, *args )
    
    def customize( self, mb ):
        sort_algorithms.USE_CALLDEF_ORGANIZER = self.USE_CALLDEF_ORGANIZER
        mb.build_code_creator( self.EXTENSION_NAME )
        
    def tearDown( self ):
        sort_algorithms.USE_CALLDEF_ORGANIZER = False

class tester_true_t(tester_base_t):
    USE_CALLDEF_ORGANIZER = True    
    EXTENSION_NAME = 'declarations_order_bug_true'
    
    def __init__( self, *args ):
        tester_base_t.__init__( self, tester_true_t.EXTENSION_NAME, *args )

    def run_tests( self, module):
        tester = module.declarations_order_bug_true.tester_t()
        self.failUnless( tester.do_smth( True ) == "do_smth( bool )" )
        self.failUnless( tester.do_smth( int(0) ) == "do_smth( bool )" )
        self.failUnless( tester.append( chr(10) ) == "append(const char *)" )

class tester_false_t(tester_base_t):
    USE_CALLDEF_ORGANIZER = False
    EXTENSION_NAME = 'declarations_order_bug_false'

    def __init__( self, *args ):
        tester_base_t.__init__( self, tester_false_t.EXTENSION_NAME, *args )

    def run_tests( self, module):
        tester = module.declarations_order_bug_false.tester_t()
        self.failUnless( tester.do_smth( True ) == "do_smth( int )" )
        self.failUnless( tester.do_smth( int(0) ) == "do_smth( int )" )
        self.failUnless( tester.append( chr(10) ) == "append(const char)" )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_true_t))
    suite.addTest( unittest.makeSuite(tester_false_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

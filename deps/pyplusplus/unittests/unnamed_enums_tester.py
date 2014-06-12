# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators 

class unnamed_enums_tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'unnamed_enums'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , unnamed_enums_tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        enum_user_t = mb.class_( 'enum_user_t' )
        if '0.9' in enum_user_t.compiler:
            for do_smth in enum_user_t.mem_funs( lambda x: x.name.startswith( 'do_smth' ) ):
                do_smth.arguments[0].default_value \
                    = 'unnamed_enums::enum_holder_t::' + do_smth.arguments[0].default_value

    def run_tests(self, module):        
        self.failUnless( module.OK == 1 )
        self.failUnless( module.CANCEL == 0 )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(unnamed_enums_tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators
from pyplusplus.module_builder.call_policies import *

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'bug_covariant_returns'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.mem_funs( 'f' ).call_policies = return_value_policy(manage_new_object)
        mb.decl( 'DObjectState' ).opaque = True
        saveState = mb.mem_funs( lambda f: f.name == 'saveState' and 'FObjectState' in f.decl_string )
        saveState.call_policies = return_value_policy(manage_new_object)
        
    def run_tests( self, module):
        pass
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

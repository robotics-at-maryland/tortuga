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
    EXTENSION_NAME = 'ft_inout_bugs'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):        
        #~ mb.global_ns.exclude()
        #~ mb.namespace( 'tests' ).include()
        set_flag = mb.calldefs( 'set_flag' )
        set_flag.add_transformation( ft.inout(1) )
        
        call_set_flag = mb.calldefs( 'call_set_flag' )
        call_set_flag.add_transformation( ft.inout(2) )
        
    def create_py_inventor( self, module ):
        class pyinventor( module.base ):
            def __init__( self ):
                module.base.__init__( self )
                
            def set_flag( self, value, flag ):
                flag = not value
                return flag
        return pyinventor()
        
    def run_tests(self, module):
        x = False
        self.failUnless( True == module.set_flag( True, x ) )
        self.failUnless( False == module.set_flag( False, x ) )
        b = module.base()
        self.failUnless( True == b.set_flag( True, x ) )
        self.failUnless( False == b.set_flag( False, x ) )

        self.failUnless( True == module.call_set_flag( b, True, x ) )
        self.failUnless( False == module.call_set_flag( b, False, x ) )
       
        inventor = module.inventor()        
        self.failUnless( False == inventor.set_flag( True, x ) )
        self.failUnless( True == inventor.set_flag( False, x ) )
        
        self.failUnless( False == module.call_set_flag( inventor, True, x ) )
        self.failUnless( True == module.call_set_flag( inventor, False, x ) )
        
        inventor = self.create_py_inventor( module )      
        self.failUnless( False == inventor.set_flag( True, x ) )
        self.failUnless( True == inventor.set_flag( False, x ) )
        
        self.failUnless( False == module.call_set_flag( inventor, True, x ) )
        self.failUnless( True == module.call_set_flag( inventor, False, x ) )
        
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

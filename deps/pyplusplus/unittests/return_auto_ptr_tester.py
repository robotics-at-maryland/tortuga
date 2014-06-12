# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators
from pyplusplus import messages

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'return_auto_ptr'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
   
    def customize( self, mb ):
        r_input = mb.class_( 'r_input_t' )
        r_input.held_type = 'std::auto_ptr< %s >' % r_input.decl_string
   
    def create_py_input( self, module, rows, cols ):
        class py_input_t( module.generic_input_t ):
            def __init__( self, rows, cols ):
                module.generic_input_t.__init__( self )
                self.rows = rows
                self.cols = cols
            
            def create_r_input( self ):
                return module.r_input_t( self.rows, self.cols )

            def create_r_input_shared( self ):
                return module.r_input_t( self.rows, self.cols )

        return py_input_t(rows, cols)
        
    def run_tests( self, module):
        a = self.create_py_input( module, 3, 7 )
        c = a.create_r_input()
        self.failUnless( c.rows() == 3 and c.cols() == 7 )
        b = module.process_input(a)
        self.failUnless( b.rows() == 3 and b.cols() == 7 )
        c = a.create_r_input_shared()
        self.failUnless( c.rows() == 3 and c.cols() == 7 )
        b = module.process_input_shared(a)
        self.failUnless( b.rows() == 3 and b.cols() == 7 )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

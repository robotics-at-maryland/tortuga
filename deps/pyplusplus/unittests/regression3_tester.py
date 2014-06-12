# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators 

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'regression3'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
        
    def create_python_final(self, class_ ):
        class tester_impl_t( class_ ):
            def __init__(self):
                class_.__init__( self )
                self.value = 21
                
            def get_value(self):
                return self.value
            
            def set_value( self, i ):
                self.value = i
                
        return tester_impl_t()

    def run_tests(self, module):        
        final_inst = module.final()
        final_inst.set_value( -2 )
        self.failUnless( module.get_value( final_inst ) == -2 )
        self.failUnless( final_inst.get_value() == -2 )
        
        inst = self.create_python_final( module.middle )
        self.failUnless( module.get_value( inst ) == 21 )
        self.failUnless( inst.get_value() == 21 )
        inst.set_value( -2 )
        self.failUnless( module.get_value( inst ) == -2 )
        self.failUnless( inst.get_value() == -2 )


                
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
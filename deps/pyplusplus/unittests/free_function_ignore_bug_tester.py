# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'free_function_ignore_bug'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        do_nothing = mb.free_functions( 'do_nothing' )
        do_nothing.exclude()  

        x = mb.free_fun( 'fill_some_dummy_struct_ptr_ptr' )
        self.failUnless( '1051' in ''.join( x.readme() ) )
        x.exclude()
        x = mb.free_fun( 'return_some_dummy_struct_ptr_ptr' )
        self.failUnless( '1050' in ''.join( x.readme() ) )
        x.exclude()

        
        mb.build_code_creator(self.EXTENSION_NAME)
        code = mb.code_creator.create()
        self.failUnless( 'do_nothing' not in code )
        
    def run_tests(self, module):        
        pass
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
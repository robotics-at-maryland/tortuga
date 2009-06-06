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
    EXTENSION_NAME = 'operators_bug'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        mb.classes().redefine_operators = True
        mb.add_declaration_code( 'const operators_bug::vector operators_bug::vector::one(1);' )
        tg = code_creators.target_configuration_t( )
        #tg.boost_python_has_wrapper_held_type = False
        mb.build_code_creator( self.EXTENSION_NAME, target_configuration=tg )
        mb.constructors().allow_implicit_conversion = False
    
    def run_tests(self, module):     
        i = module.integral()
        i.value = 25
        j = i + i
        v = module.vector( 2 ) + module.vector.one
        self.failUnless( v.x == 3 )
        
        call_copy_constructor = module.call_copy_constructor_t( "" )
        call_copy_constructor2 = module.call_copy_constructor_t( call_copy_constructor )
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

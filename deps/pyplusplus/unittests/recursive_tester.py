# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import code_creators

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'recursive'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        matcher = ~declarations.namespace_matcher_t()
        matcher = matcher & declarations.regex_matcher_t( '.*skip.*' )

        decls = mb.decls( matcher )
        decls.ignore = True
        
        mb.build_code_creator( self.EXTENSION_NAME )

        matcher = declarations.match_declaration_t( name='skip_a' )        
        found = code_creators.creator_finder.find_by_declaration( matcher
                                                                  , mb.code_creator.creators )
        self.failUnless( not found, "'skip_a' declaration should not be exported" )
        
        matcher = declarations.match_declaration_t( name='skip_b' )        
        found = code_creators.creator_finder.find_by_declaration( matcher
                                                                  , mb.code_creator.creators )
        self.failUnless( not found, "'skip_b' declaration should not be exported" )                                                          
       
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
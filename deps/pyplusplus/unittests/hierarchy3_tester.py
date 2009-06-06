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
    EXTENSION_NAME = 'hierarchy3'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        mb.build_code_creator( 'hierarchy3' )
        find = code_creators.creator_finder.find_by_declaration_single
        matcher = declarations.match_declaration_t( name='son_t'
                                                    , type=declarations.class_t)
        found = find( matcher, mb.code_creator.body.creators )
        self.failUnless( found )
        self.failUnless( found.wrapper )
        #self.failUnless( 0 == len( found.creators ) )
            
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
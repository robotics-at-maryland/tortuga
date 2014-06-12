# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
import fundamental_tester_base
from pyplusplus import code_creators

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'precompiled_header'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        #add precompiled header
        mb.build_code_creator( self.EXTENSION_NAME )
        stdafx = code_creators.include_t( 'stdafx.h' )            
        mb.code_creator.adopt_creator( stdafx, 0 )
                
        f = file( os.path.join( autoconfig.build_dir, 'stdafx.h' ), 'w+' )
        f.writelines( ['//this should be the first header file' + os.linesep] )
        f.close()

    def run_tests(self, module):        
        lines = file( os.path.join( autoconfig.build_dir, 'precompiled_header.cpp' ) ).readlines()
        lines = filter( lambda l: l.startswith( '#include' ), lines )
        self.failUnless( '#include "stdafx.h"' in lines[0] )
        
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

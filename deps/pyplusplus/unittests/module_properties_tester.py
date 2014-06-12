# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
from pyplusplus import code_creators 
from pyplusplus import module_creator

class tester_t(unittest.TestCase):
    def __init__(self, *args ):
        unittest.TestCase.__init__(self, *args)
   
    #~ def test_precompiled_header(self):
        #~ extmodule = module_creator.create( [], 'X' )
        #~ self.failUnless( not extmodule.precompiled_header )
        #~ extmodule.precompiled_header = 'boost/python.hpp'
        #~ self.failUnless( isinstance( extmodule.precompiled_header
                                     #~ , code_creators.precompiled_header_t ) )
        #~ extmodule.precompiled_header \
            #~ = code_creators.precompiled_header_t( 'boost/date_time.hpp' )
        #~ self.failUnless( isinstance( extmodule.precompiled_header
                                     #~ , code_creators.precompiled_header_t ) )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
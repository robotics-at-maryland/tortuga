# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

from pyplusplus import code_repository

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'convenience'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.add_registration_code( 'bp::def( "ensure_sequence", &pyplusplus::convenience::ensure_sequence );' )
        mb.add_registration_code( 'bp::def( "ensure_int_sequence", &pyplusplus::convenience::ensure_uniform_sequence<int> );' )
        mb.build_code_creator( self.EXTENSION_NAME )

        mb.code_creator.add_include( code_repository.convenience.file_name, system=True )

    def run_tests(self, module):
        self.failIfRaisesAny( module.ensure_sequence, [1,2,3], -1 )
        self.failIfRaisesAny( module.ensure_sequence, [1,2,3], 3 )
        self.failIfNotRaisesAny( module.ensure_sequence, self, 1 )
        self.failIfRaisesAny( module.ensure_int_sequence, [1,2,3], -1 )
        self.failIfRaisesAny( module.ensure_int_sequence, [1,2,3], 3 )
        self.failIfNotRaisesAny( module.ensure_int_sequence, [self], 1 )
        self.failIfNotRaisesAny( module.ensure_int_sequence, [1,2.1,3] , 3 )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

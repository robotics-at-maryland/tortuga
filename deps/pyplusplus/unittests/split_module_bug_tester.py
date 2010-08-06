# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
import fundamental_tester_base

from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'split_module_bug'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , indexing_suite_version=2
            , *args )
        self.files = []

    def customize( self, mb ):
        opaque_cp = call_policies.return_value_policy( call_policies.return_opaque_pointer )
        mb.calldefs( 'get_opaque' ).call_policies = opaque_cp
        mb.calldefs( 'get_opaque2' ).call_policies = opaque_cp

    def generate_source_files( self, mb ):
        files = mb.split_module( autoconfig.build_dir, on_unused_file_found=lambda fpath: fpath )
        self.files = filter( lambda fname: fname.endswith( 'cpp' ), files )
        self.files.append( os.path.join( autoconfig.data_directory
                                         , self.EXTENSION_NAME + '_to_be_exported.cpp' ) )
        print self.files


    def get_source_files( self ):
        return self.files

    def run_tests(self, module):
        module.get_opaque()
        module.get_opaque2()

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

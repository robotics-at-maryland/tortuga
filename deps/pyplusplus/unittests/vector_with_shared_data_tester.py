# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
from pyplusplus import utils
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'vector_with_shared_data'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , indexing_suite_version=2
            , *args )
        self.files = []

    def customize( self, mb ):
        mb.global_ns.exclude()
        mb.namespace( 'samples' ).include()
        do_smth = mb.free_fun( 'do_smth' )
        v = declarations.remove_declarated( do_smth.return_type )
        v.indexing_suite.disable_method( 'sort' )
        v.indexing_suite.disable_method( 'count' )
        v.indexing_suite.disable_method( 'index' )
        v.indexing_suite.disable_method( 'contains' )
        
    def generate_source_files( self, mb ):
        files = mb.split_module( autoconfig.build_dir, on_unused_file_found=lambda fpath: fpath )
        self.files = filter( lambda fname: fname.endswith( 'cpp' ), files )
        self.files.append( os.path.join( autoconfig.data_directory
                                         , self.EXTENSION_NAME + '_to_be_exported.cpp' ) )

    def get_source_files( self ):
        return self.files

    def run_tests(self, module):
        x = module.do_smth()
        self.failUnless( len(x) == 2 )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

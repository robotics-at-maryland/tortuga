# Copyright 2004 Roman Yakovenko.
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
    EXTENSION_NAME = 'split_module'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )
        self.files = []

    def customize( self, mb ):
        mb.global_ns.exclude()
        sm = mb.global_ns.namespace( name='split_module' )
        sm.include()
        sm.class_( 'op_struct' ).exclude()

        mb.free_function( 'check_overload' ).add_declaration_code( '//hello check_overload' )
        mb.free_function( 'get_opaque' ).add_declaration_code( '//hello get_opaque' )
        
        mb.calldefs( 'check_overload' ).use_overload_macro = True
        mb.calldefs( 'get_opaque' ).call_policies \
          = call_policies.return_value_policy( call_policies.return_opaque_pointer )

        mb.class_( 'op_struct' ).exclude()
        item = mb.class_( 'item_t' )
        item.add_declaration_code( '//hello world' )
        nested = item.class_( 'nested_t' )
        nested.add_declaration_code( '//hello nested decl' )
        nested.add_registration_code( '//hello nested reg', False )

    def generate_source_files( self, mb ):
        files = mb.split_module( autoconfig.build_dir, on_unused_file_found=lambda fpath: fpath )
        self.files = filter( lambda fname: fname.endswith( 'cpp' ), files )
        print self.files

    def get_source_files( self ):
        return self.files

    def run_tests(self, module):
        module.get_opaque()
        item = module.item_t()
        item.get_opaque()
        module.check_overload()
        item.check_overload()

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

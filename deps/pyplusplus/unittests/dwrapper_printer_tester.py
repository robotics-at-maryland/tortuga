# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import autoconfig
from pyplusplus import module_builder

class tester_t(unittest.TestCase):
    def _get_files( self ):
        files = [
            'enums_to_be_exported.hpp'
            , 'free_functions_to_be_exported.hpp'
            , 'free_operators_to_be_exported.hpp'
            , 'global_variables_to_be_exported.hpp'
            , 'index_operator_to_be_exported.hpp'
            , 'member_functions_to_be_exported.hpp'
        ]
        return map( lambda f: os.path.join( autoconfig.data_directory, f )
                    , files )
            
    def test(self):
        mb = module_builder.module_builder_t( self._get_files()
                                              , gccxml_path=autoconfig.gccxml.executable
                                              , include_paths=[autoconfig.boost.include]
                                              , undefine_symbols=['__MINGW32__'])
        writer = lambda decl: None
        module_builder.print_declarations( mb.global_ns, writer=writer )
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import module_builder


class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'indexing_suites'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    @staticmethod
    def matcher( item, decl ):
        if not declarations.vector_traits.is_my_case( decl ):
            return False
        element_type = declarations.vector_traits.element_type(decl)
        if item is element_type:
            return True
        return False
    
    def customize(self, generator):
        item = generator.class_( 'item_t' )
        items = generator.class_( lambda decl: self.matcher( item, decl ) )
        items.alias = "items_t"
       
    def run_tests( self, module):
        items = module.items_t()
        item = module.item_t()
        item.value = 1977
        items.append( item )
        self.failUnless( module.get_value( items, 0 ).value == 1977 )
        self.failUnless( len( items ) == 1 )
        names = module.get_names()
        self.failUnless( len( names ) == 3 )
        self.failUnless( names[0] == names[1] == names[2]== "a" )
    
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
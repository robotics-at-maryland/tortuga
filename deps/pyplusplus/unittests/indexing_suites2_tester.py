# Copyright 2004 Roman Yakovenko.
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
    EXTENSION_NAME = 'indexing_suites2'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , indexing_suite_version=2
            , *args)

    def customize(self, generator):
        items = generator.global_ns.typedef( 'items_t' )
        items = declarations.remove_declarated( items.type )
        items.alias = "items_t"
        fvector = generator.global_ns.typedef( 'fvector' )
        fvector = declarations.remove_declarated( fvector.type )
        fvector.indexing_suite.disable_method( 'extend' )
        fvector.indexing_suite.disable_methods_group( 'reorder' )
        #fvector.indexing_suite.call_policies = module_builder.call_policies.default_call_policies()
        items_ptr = generator.global_ns.typedef( 'items_ptr_t' )
        items_ptr = declarations.remove_declarated( items_ptr.type )
        items_ptr.indexing_suite.call_policies = module_builder.call_policies.return_internal_reference()
        
       
    def run_tests( self, module):
        fv = module.fvector()
        self.failUnless( not hasattr( fv, 'extend' ) )
        self.failUnless( not hasattr( fv, 'sort' ) )
        self.failUnless( not hasattr( fv, 'reverse' ) )
        items = module.items_t()
        item = module.item_t()
        item.value = 1977
        items.append( item )
        self.failUnless( module.get_value( items, 0 ).value == 1977 )
        self.failUnless( len( items ) == 1 )
        
        name2value = module.name2value_t()
        name2value[ "x" ] = "y"
        self.failUnless( "x" == module.get_first_name( name2value ) )
        items_ptr = module.items_ptr_t()
        items_ptr.append( item )
        self.failUnless( items_ptr[0].value == 1977 )
        
        set_of_strings = module.create_set_strings()
        set_of_strings.add("s")
        set_of_strings.add("s1")
        set_of_strings.add("s")
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

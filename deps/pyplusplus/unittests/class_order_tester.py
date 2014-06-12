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
    EXTENSION_NAME = 'class_order'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):
        mb.build_code_creator( self.EXTENSION_NAME )
        extmodule = mb.code_creator
        matcher = declarations.match_declaration_t( name='item', type=declarations.class_t )
        item_creator = code_creators.creator_finder.find_by_declaration( matcher, extmodule.body.creators )[0]
        extmodule.body.remove_creator(item_creator)
        #This will add item creator to be last exported class
        extmodule.body.adopt_creator(item_creator)

        mb.calldefs().use_keywords = False

    def run_tests(self, module):      
        item_ = module.item(1,2)
        cont = module.container( item_ );
        self.failUnless( 3 == cont.my_item.m_value )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
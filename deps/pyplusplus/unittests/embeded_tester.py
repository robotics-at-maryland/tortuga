# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators
from pyplusplus.module_builder.call_policies import *

clone_def_code = """

namespace bpl = boost::python;

bpl::object clone_item( const item_t& item ){
    std::auto_ptr< item_t > new_item( new item_t() );
    new_item->i = item.i;
    return bpl::object( new_item );
}

"""

clone_reg_code = """

def("clone_item", &clone_item );

    bp::register_ptr_to_python< std::auto_ptr< item_t > >();

"""


class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'embeded'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        mb.add_declaration_code( clone_def_code )
        mb.add_registration_code( clone_reg_code, True )
        
    def run_tests( self, module):
        item = module.item_t()
        item.i = 78
        
        item2 = module.clone_item(item)
        self.failUnless( item2.i == item.i )        
        self.failUnless( sys.getrefcount( item2 ) == 2 ) #one for boost.python second for current scope
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

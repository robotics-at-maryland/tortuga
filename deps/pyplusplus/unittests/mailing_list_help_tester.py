# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus import code_creators

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'mailing_list_help'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        foo = mb.class_( 'foo' )
        foo.add_wrapper_code( 'static std::string get_buffer(const foo& x) { return std::string(x.buffer); }' )
        foo.add_registration_code( 'def("__str__", &foo_wrapper::get_buffer)' )
        #bar = mb.class_( 'bar' )
        #bar.var( 'f' ).use_make_functions = True
        
    def run_tests( self, module):
        f1 = module.foo()
        print 'f1.buffer: ', f1.buffer
        print 'str(f1):', str(f1)
        b1 = module.bar()
        print 'b1.f.buffer:', b1.f.buffer
        print 'b1.f:', b1.f
        print 'str(b1.f):', str(b1.f)
        
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

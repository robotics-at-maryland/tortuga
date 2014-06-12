# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base
from pyplusplus.module_builder import call_policies

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'properties'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize(self, mb ):
        cls = mb.class_( 'properties_tester_t' )
        count = cls.member_function( 'count' )
        set_count = cls.member_function( 'set_count' )
        count.exclude()
        set_count.exclude()
        cls.add_property( "count", count, set_count )
        cls.add_property( "count_ro", count )

        get_nested = cls.member_function( 'get_nested' )
        get_nested.call_policies = call_policies.return_internal_reference()
        set_nested = cls.member_function( 'set_nested' )
        cls.add_property( "nested_", get_nested, set_nested )

        cls.add_property( "nested_ro", get_nested )

        cls = mb.class_( 'properties_finder_tester_t' )
        cls.add_properties( exclude_accessors=True )
        self.failUnless( 6 == len( cls.properties ) )
        self.failUnless( cls.name in map( lambda pr: pr.name, cls.properties ) )

    def run_tests(self, module):
        pt = module.properties_tester_t()
        self.failUnless( pt.count == 0 )
        pt.count = 21
        self.failUnless( pt.m_count == 21 )


def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import math
import ctypes
import unittest
import fundamental_tester_base
from pygccxml import declarations
from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies


class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'ft_from_address'

    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__(
            self
            , tester_t.EXTENSION_NAME
            , *args )

    def customize( self, mb ):
        mb.global_ns.calldefs().create_with_signature = True
        mb.calldef( 'sum_matrix' ).add_transformation( ft.from_address(0) )
        ptr_ptr = mb.class_( 'ptr_ptr_t' )
        ptr_ptr.var( 'value' ).expose_address = True
        ptr_ptr.mem_fun( 'get_v_address' ).add_transformation( ft.from_address(0 ) )

    def run_tests(self, module):
        rows = 10
        columns = 7
        matrix_type = ctypes.c_uint * columns * rows
        sum = 0
        counter = 0
        matrix = matrix_type()
        for r in range( rows ):
            for c in range( columns ):
                matrix[r][c] = counter
                sum += counter
                counter += 1
        result = module.sum_matrix( ctypes.addressof( matrix ), rows, columns )
        self.failUnless( result == sum )

        ptr = module.ptr_ptr_t()
        double_ptr_type = ctypes.POINTER( ctypes.c_double )
        value = double_ptr_type.from_address( ptr.value )
        self.failUnless( value.contents.value == 5.9 )
        dd = double_ptr_type(ctypes.c_double(0.0) )
        print dir( ctypes.pointer( dd ).contents )
        ptr.get_v_address( ctypes.pointer( dd ).contents.value )
        print ptr.value
        print dd.contents.value

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()

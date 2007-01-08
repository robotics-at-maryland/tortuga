# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
import sys
import unittest

import classes_tester
import abstract_classes_tester
import algorithms_tester
import module_body_tester
import enums_tester
import free_functions_tester
import fundamental_tester_base
import unnamed_enums_tester
import namespaces_tester
import classes_tester
import global_variables_tester
import member_variables_tester
import member_functions_tester
import call_policies_tester
import pointer_to_function_as_argument
import operators_tester
import abstract_tester
import statics_tester
import regression1_tester
import casting_tester
import finalizables_tester
import free_operators_tester
import operators_bug_tester
import smart_pointers_tester
import special_operators_tester
import module_properties_tester
import internal_classes_tester
import temporary_variable_tester
import recursive_tester
import class_order_tester
import class_order2_tester
import class_order3_tester
import class_order4_tester
import noncopyable_tester
import regression2_tester
import regression3_tester
import optional_tester
import index_operator_tester
import dwrapper_printer_tester
import mdecl_wrapper_tester
import user_text_tester
import free_function_ignore_bug_tester
import optional_bug_tester
import pointer_as_arg_tester
import factory_tester
import private_assign_tester
import protected_tester
import indexing_suites_tester
import indexing_suites2_tester
import hierarchy3_tester
import vector3_tester
import default_args_tester
import unnamed_classes_tester
import cppexceptions_tester
import no_init_tester
import overloads_macro_tester
import split_module_tester
import properties_tester
import arrays_bug_tester
import convenience_tester
import inner_class_bug_tester
import declarations_order_bug_tester
import function_transformations_tester
import throw_tester
import duplicate_aliases_tester

def create_suite(times):
    testers = [
        algorithms_tester
        , module_body_tester
        , enums_tester
        , free_functions_tester
        , unnamed_enums_tester
        , namespaces_tester
        , classes_tester
        , global_variables_tester
        , member_variables_tester
        , member_functions_tester
        , call_policies_tester
        , pointer_to_function_as_argument
        , operators_tester
        , abstract_tester
        , statics_tester
        , regression1_tester
        , casting_tester
        , finalizables_tester
        , free_operators_tester
        , operators_bug_tester
        , smart_pointers_tester
        , special_operators_tester
        , module_properties_tester
        , internal_classes_tester
        , temporary_variable_tester
        , recursive_tester
        , class_order_tester
        , noncopyable_tester
        , regression2_tester
        , regression3_tester
        , class_order2_tester
        , class_order3_tester
        , class_order4_tester
        , optional_tester
        , index_operator_tester
        , dwrapper_printer_tester
        , mdecl_wrapper_tester
        , user_text_tester
        , free_function_ignore_bug_tester
        , optional_bug_tester
        , pointer_as_arg_tester
        , factory_tester
        , private_assign_tester
        , protected_tester
        , indexing_suites_tester
        , hierarchy3_tester
        , vector3_tester
        , default_args_tester
        , abstract_classes_tester
        , indexing_suites2_tester
        , unnamed_classes_tester
        , cppexceptions_tester
        , no_init_tester
        , overloads_macro_tester
        , split_module_tester
        , properties_tester
        , arrays_bug_tester
        , convenience_tester
        , inner_class_bug_tester
        , declarations_order_bug_tester
        , function_transformations_tester
        , throw_tester
        , duplicate_aliases_tester
    ]

    main_suite = unittest.TestSuite()
    for i in range( times ):
        for tester in testers:
            main_suite.addTest( tester.create_suite() )

    return main_suite

def run_suite(times=1):
    return unittest.TextTestRunner(verbosity=2).run( create_suite(times) ).wasSuccessful

if __name__ == "__main__":
    times = 1
    if len(sys.argv) > 1:
        try:
            times = int( sys.argv[1] )
        except Exception, error:
            print str( error )
            print 'first argument should be integer, it says how many times to run tests.'

    run_suite(times)

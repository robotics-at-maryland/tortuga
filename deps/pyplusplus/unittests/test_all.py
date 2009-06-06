# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import re
import sys
import time

import autoconfig

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
import non_overridable_tester
import exposed_decls_db_tester
import already_exposed_tester
import bpmodule_tester
import custom_smart_ptr_classes_tester
import custom_string_tester
import final_classes_tester
import templates_tester
import deepcopy_tester
import override_bug_tester
#gui_tester
#gui_wizard_tester
#
import mem_fun_with_exception_tester
import overriden_virtual_functions_bug_tester
import split_module_bug_tester
import tnfox_bugs_tester
import transfer_ownership_old_tester
import transfer_ownership_tester
import unicode_bug
import include_exclude_bug_tester
import vector_with_shared_data_tester
import constructors_bug_tester
import precompiled_header_tester
import balanced_files_tester
import ft_inout_tester
import split_module_indexing_suite_bug_tester
import inner_tmpl_class_tester
import bug_covariant_returns_tester
import embeded_tester
import unions_tester
import cp_return_addressof_tester
import make_constructor_tester
import return_auto_ptr_tester
#import ogre_generate_tester

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
    , non_overridable_tester
    , exposed_decls_db_tester
    , already_exposed_tester
    , bpmodule_tester
    , custom_smart_ptr_classes_tester
    , custom_string_tester
    , final_classes_tester
    , mem_fun_with_exception_tester
    , overriden_virtual_functions_bug_tester
    , split_module_bug_tester
    , tnfox_bugs_tester
    , transfer_ownership_old_tester
    , transfer_ownership_tester
    , unicode_bug
    , include_exclude_bug_tester
    , vector_with_shared_data_tester
    , templates_tester
    , constructors_bug_tester
    , precompiled_header_tester
    , balanced_files_tester
    , ft_inout_tester
    , deepcopy_tester
    , override_bug_tester
    , split_module_indexing_suite_bug_tester
    , inner_tmpl_class_tester
    , bug_covariant_returns_tester
    , embeded_tester
    , unions_tester
    , cp_return_addressof_tester
    , make_constructor_tester
    , return_auto_ptr_tester
#    , ogre_generate_tester too much time
]

class module_runner_t( object ):
    bottom_line_re = re.compile( 'Ran\s(?P<num_of_tests>\d+)\stests?\sin\s(?P<seconds>\d+\.?\d*)s')
    test_name_re = re.compile( '(?P<name>.+ \(.+\))\s\.\.\.' )
    failed_test_re = re.compile( 'FAIL\:\s(?P<name>.+ \(.+\))' )
    error_test_re = re.compile( 'ERROR\:\s(?P<name>.+ \(.+\))' )

    def __init__( self, module ):
        self.module = module
        self.output = None

        self.test_results = {} #test name : result
        self.num_of_tests = 0
        self.total_run_time = 0
        self.exit_status = None

    def __call__( self ):
        print os.linesep, '<*> start %s tester' % os.path.basename( self.module.__file__)

        test_file_name = self.module.__file__
        if test_file_name.endswith( 'pyc' ):
            test_file_name = test_file_name[:-1]
        command_line = ' '.join([ sys.executable, test_file_name ])
        input_, output = os.popen4( command_line )
        input_.close()
        report = []
        while True:
            data = output.readline()
            report.append( data )
            if not data:
                break
            else:
                print data,
        self.output = ''.join( report )
        self.exit_status = output.close()
        self.__update()
        print '<***> finish %s tester' % os.path.basename( self.module.__file__)

    def __create_unique_name( self, name ):
        if '__main__.' in name:
            name = name.replace( '__main__', os.path.basename( self.module.__file__)[:-4] )
        return name

    def __update( self ):
        match_found = self.bottom_line_re.search( self.output )
        if match_found:
            self.num_of_tests += int( match_found.group( 'num_of_tests' ) )
            self.total_run_time += float( match_found.group( 'seconds' ) )

        uname = self.__create_unique_name
        for match_found in self.test_name_re.finditer( self.output ):
            self.test_results[ uname( match_found.group( 'name' ) ) ] = 'ok'

        for match_found in self.failed_test_re.finditer( self.output ):
            self.test_results[ uname( match_found.group( 'name' ) ) ] = 'FAIL'

        for match_found in self.error_test_re.finditer( self.output ):
            self.test_results[ uname( match_found.group( 'name' ) ) ] = 'ERROR'

        assert( self.num_of_tests == len( self.test_results ) )


class process_tester_runner_t( object ):

    def __init__( self, modules ):
        self.__m_runners = [ module_runner_t(m) for m in modules ]
        self.__total_time = 0

    def __dump_statistics( self ):
        num_of_tests = 0
        test_results = {}
        total_tests_only_run_time = 0
        exit_status = 0
        for stat in self.__m_runners:
            num_of_tests += stat.num_of_tests
            total_tests_only_run_time += stat.total_run_time
            test_results.update( stat.test_results )
            exit_status = max( exit_status, stat.exit_status )
        test_failed = len( filter( lambda result: result != 'ok', test_results.values() ) )

        for name, result in test_results.iteritems():
            if result != 'ok':
                print '! ',
            print name, ' - ', result
        print '----------------------------------------------------------------------'
        print 'Final exit status: ', exit_status
        print 'Ran %d test in %fs. Multi-processing overhead: %fs.' \
               % ( num_of_tests, self.__total_time, self.__total_time - total_tests_only_run_time )
        print ' '
        if test_failed:
            print os.linesep.join(['FAILED  (failures=%d)' % test_failed, 'False'])
        else:
            print 'ok'

    def __call__( self ):
        start_time = time.time()
        [ m() for m in self.__m_runners ]
        self.__total_time = time.time() - start_time
        self.__dump_statistics()


if __name__ == "__main__":
    runner = process_tester_runner_t( testers )
    runner()


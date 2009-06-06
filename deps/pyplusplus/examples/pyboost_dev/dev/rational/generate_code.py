#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


import os
import time
import logging
import rational_settings
from pygccxml import parser
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

LICENSE = """// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
"""

#TODO: for some reason unary - was not exported

class code_generator_t(object):    
    def __init__(self):      
        self.__file = os.path.join( rational_settings.working_dir, 'rational_export.hpp' )
        
        self.__mb = module_builder.module_builder_t( 
                        [ parser.create_cached_source_fc( 
                            self.__file
                            , os.path.join( rational_settings.generated_files_dir, 'rational.xml' ) ) ]
                        , gccxml_path=rational_settings.gccxml.executable
                        , include_paths=[rational_settings.boost.include]
                        , define_symbols=rational_settings.defined_symbols
                        , undefine_symbols=rational_settings.undefined_symbols
                        , optimize_queries=False
                        , indexing_suite_version=2 )
        
        for f_decl in self.__mb.free_functions():
            f_decl.alias = f_decl.name
            f_decl.name = f_decl.demangled_name

        self.__mb.run_query_optimizer()

        
    def filter_declarations(self ):
        self.__mb.global_ns.exclude()
        rational = self.__mb.class_('rational<long>')
        rational.include()
        rational.casting_operator( lambda decl: 'long int[2]' in str(decl) ).exclude()
        
        r_assign = rational.calldef( 'assign', recursive=False )
        r_assign.call_policies = call_policies.return_self()

        foperators = self.__mb.free_operators( lambda decl: 'rational<long>' in decl.decl_string )
        foperators.include()
            
        bad_rational = self.__mb.class_('bad_rational' )
        bad_rational.include()
        
        self.__mb.namespace( 'boost' ).free_function( 'lcm<long>', recursive=False ).include()
        self.__mb.namespace( 'boost' ).free_function( 'gcd<long>', recursive=False ).include()
        self.__mb.free_function( 'rational_cast<double, long>' ).include()
        self.__mb.free_function( 'rational_cast<double, long>' ).alias = 'to_double'
        self.__mb.free_function( 'rational_cast<long, long>' ).include()
        self.__mb.free_function( 'rational_cast<long, long>' ).alias = 'to_long'
        self.__mb.free_function( 'abs<long>' ).include()

        self.__mb.global_ns.operators( '++' ).exclude()
        self.__mb.global_ns.operators( '--' ).exclude()
        self.__mb.global_ns.operators( '=' ).exclude()

    def prepare_decls( self ):
        self.__mb.class_('rational<long>').alias = 'rational'

    def customize_extmodule( self ):
        global LICENSE
        extmodule = self.__mb.code_creator
        #beautifying include code generation
        extmodule.license = LICENSE
        extmodule.user_defined_directories.append( rational_settings.boost.include )
        extmodule.user_defined_directories.append( rational_settings.working_dir )
        extmodule.user_defined_directories.append( rational_settings.generated_files_dir )
        extmodule.replace_included_headers( ['boost/rational.hpp'] )

    def write_files( self ):
        self.__mb.write_module( os.path.join( rational_settings.generated_files_dir, 'rational.pypp.cpp' ) )

    def create(self):
        start_time = time.clock()      
        self.filter_declarations()

        self.prepare_decls()
        
        self.__mb.build_code_creator( rational_settings.module_name )
        
        self.customize_extmodule()
        self.write_files( )
        print 'time taken : ', time.clock() - start_time, ' seconds'

def export():
    cg = code_generator_t()
    cg.create()

if __name__ == '__main__':
    export()
    print 'done'
    
    

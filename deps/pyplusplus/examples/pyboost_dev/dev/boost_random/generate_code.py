#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import time
import logging
import random_settings
from pygccxml import parser
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies


LICENSE = """// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
"""

class code_generator_t(object):    
    def __init__(self):
        self.__file = os.path.join( random_settings.working_dir, 'random_export.hpp' )
        self.__mb = module_builder.module_builder_t( 
                        [ parser.create_cached_source_fc( 
                            self.__file
                            , os.path.join( random_settings.generated_files_dir, 'random.xml' ) ) ]
                        , gccxml_path=random_settings.gccxml.executable
                        , include_paths=[random_settings.boost.include]
                        , define_symbols=random_settings.defined_symbols
                        , undefine_symbols=random_settings.undefined_symbols
                        , indexing_suite_version=2)
        self.generators = [   "ecuyer1988"
                              , "hellekalek1995"
                              , "kreutzer1986"
                              , "lagged_fibonacci1279"
                              , "lagged_fibonacci19937"
                              , "lagged_fibonacci2281"
                              , "lagged_fibonacci23209"
                              , "lagged_fibonacci3217"
                              , "lagged_fibonacci4423"
                              , "lagged_fibonacci44497"
                              , "lagged_fibonacci607"
                              , "lagged_fibonacci9689"
                              , "minstd_rand"
                              , "minstd_rand0"
                              , "mt11213b"
                              , "mt19937"
                              , "ranlux3"
                              , "ranlux3_01"
                              , "ranlux4"
                              , "ranlux4_01"
                              , "ranlux64_3_01"
                              , "ranlux64_4_01"
                              , "taus88" ]
    
        self.no_min_max = [ 'py_cauchy_distribution'
                            , 'py_bernoulli_distribution'
                            , 'py_binomial_distribution'
                            , 'py_poisson_distribution'
                            , 'py_normal_distribution'
                            , 'py_gamma_distribution'
                            , 'py_triangle_distribution'
                            , 'py_uniform_on_sphere'
                            , 'py_exponential_distribution'
                            , 'py_geometric_distribution'
                            , 'py_lognormal_distribution'
                            ]
        
    def typedef2class( self, scope, name ):
        typedef = scope.typedef( name )
        return typedef.type.declaration
    
    def filter_declarations(self ):
        self.__mb.global_ns.exclude()
        boost_ns = self.__mb.global_ns.namespace( 'boost', recursive=False )
        for name in self.generators:
            gen_cls = self.typedef2class( boost_ns, name )
            gen_cls.include()
            #TODO: find out why compiler complains
            gen_cls.member_functions( 'seed' ).create_with_signature = True
        
        pyimpl_ns = boost_ns.namespace( 'pyimpl' )
        helpers = pyimpl_ns.classes( lambda decl: decl.name.startswith( 'py_') )
        helpers.include()
        for helper in helpers:
            distrib_cls = self.typedef2class( helper, "distribution" )
            distrib_cls.include()
            var_gen_typedefs = helper.typedefs( lambda decl: decl.name.startswith( 'variate_generator_' ) )
            for var_gen_typedef in var_gen_typedefs:
                var_gen_cls = var_gen_typedef.type.declaration
                var_gen_cls.include()
                var_gen_cls.member_operators( symbol='()' ).create_with_signature = True
                if helper.name in self.no_min_max:
                    var_gen_cls.member_function( 'max' ).exclude()
                    var_gen_cls.member_function( 'min' ).exclude()
                
        ecuyer1988 = self.typedef2class( boost_ns, 'ecuyer1988' )
        ecuyer1988.member_function( 'seed', arg_types=[None, None] ).exclude()
        
    def prepare_declarations( self ):
        boost_ns = self.__mb.namespace( 'boost' )
        for name in self.generators:
            gen_cls = self.typedef2class( boost_ns, name )
            gen_cls.alias = name

        pyimpl_ns = boost_ns.namespace( 'pyimpl' )
        helpers = pyimpl_ns.classes( lambda decl: decl.name.startswith( 'py_') )
        for helper in helpers:
            distrib_cls = self.typedef2class( helper, "distribution" )
            distrib_cls.alias = helper.name[3:] #py_
            var_gen_typedefs = helper.typedefs( lambda decl: decl.name.startswith( 'variate_generator_' ) )
            for var_gen_typedef in var_gen_typedefs:
                var_gen_cls = var_gen_typedef.type.declaration
                var_gen_cls.alias = var_gen_typedef.name + '__' + distrib_cls.alias
                
        self.set_call_policies()
        boost_ns.classes().add_properties( exclude_accessors=True )

    def set_call_policies( self ):
        boost_ns = self.__mb.namespace( 'boost' )
        engine_funcs = boost_ns.member_functions( name="engine"
                                                  , function=lambda decl: not decl.has_const )
        engine_funcs.call_policies = call_policies.return_internal_reference()

        distribution_funcs = boost_ns.member_functions( name="distribution"
                                                        , function=lambda decl: not decl.has_const )
        distribution_funcs.call_policies = call_policies.return_internal_reference()
        
    def customize_extmodule( self ):
        global LICENSE
        extmodule = self.__mb.code_creator
        extmodule.license = LICENSE
        extmodule.user_defined_directories.append( random_settings.boost.include )
        extmodule.user_defined_directories.append( random_settings.working_dir )
        extmodule.user_defined_directories.append( random_settings.generated_files_dir )
        extmodule.replace_included_headers( ['boost/random.hpp', 'boost/nondet_random.hpp', 'random_export.hpp' ] )
        
    def write_files( self ):
        self.__mb.split_module( random_settings.generated_files_dir )

    def create(self):
        start_time = time.clock()      
        self.filter_declarations()
        self.prepare_declarations()
        self.__mb.build_code_creator( random_settings.module_name )
        
        self.customize_extmodule()
        self.write_files( )
        print 'time taken : ', time.clock() - start_time, ' seconds'

def export():
    cg = code_generator_t()
    cg.create()

if __name__ == '__main__':
    export()
    print 'done'
    
    

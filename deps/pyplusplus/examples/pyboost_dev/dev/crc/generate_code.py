#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


import os
import time
import logging
import crc_settings
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import module_builder

LICENSE = """// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
"""

class code_generator_t(object):    
    def __init__(self):
        self.__file = os.path.join( crc_settings.working_dir, 'crc_export.hpp' )
        
        self.__mb = module_builder.module_builder_t( 
                        [ parser.create_cached_source_fc( 
                            self.__file
                            , os.path.join( crc_settings.generated_files_dir, 'crc.xml' ) ) ]
                        , gccxml_path=crc_settings.gccxml.executable
                        , include_paths=[crc_settings.boost.include]
                        , define_symbols=crc_settings.defined_symbols
                        , undefine_symbols=crc_settings.undefined_symbols
                        , indexing_suite_version=2)
        
    def filter_declarations(self ):
        self.__mb.global_ns.exclude()
        boost_ns = self.__mb.global_ns.namespace( 'boost', recursive=False )
        boost_ns.classes( lambda decl: decl.name.startswith( 'crc_basic' ) ).include()
        boost_ns.classes( lambda decl: decl.name.startswith( 'crc_optimal' ) ).include()
        boost_ns.member_functions( 'process_bytes' ).exclude()
        boost_ns.member_functions( 'process_block' ).exclude()
        boost_ns.member_operators( symbol='()' ).exclude()
        
    def prepare_declarations( self ):
        boost_ns = self.__mb.namespace( 'boost' )
        classes = boost_ns.classes( lambda decl: decl.name.startswith( 'crc_basic' ) )
        classes.always_expose_using_scope = True
        for cls in classes:
            name, args = declarations.templates.split(cls.name)
            cls.alias = name + '_' + args[0]
            
        classes = boost_ns.classes( lambda decl: decl.name.startswith( 'crc_optimal' ) )
        classes.always_expose_using_scope = True
        for cls in classes:
            name, args = declarations.templates.split(cls.name)
            InitRem = args[2]
            for f in cls.calldefs():
                for arg in f.arguments:
                    if arg.default_value == 'InitRem':
                        arg.default_value = InitRem
            aliases = set( map( lambda decl: decl.name, cls.aliases ) )
            if 'optimal_crc_type' in aliases:
                aliases.remove( 'optimal_crc_type' )
            if len( aliases ) == 1:
                cls.alias = list(aliases)[0]
            elif cls.name == 'crc_optimal<32, 79764919, 0, 0, false, false>':
                cls.alias = 'fast_crc_type'
            elif cls.name == 'crc_optimal<32, 0, 0, 0, false, false>':
                cls.alias = 'slow_crc_type'
            else:
                print 'no alias for class ', cls.name
                
    def customize_extmodule( self ):
        global LICENSE
        extmodule = self.__mb.code_creator
        #beautifying include code generation
        extmodule.license = LICENSE
        extmodule.user_defined_directories.append( crc_settings.boost.include )
        extmodule.user_defined_directories.append( crc_settings.working_dir )
        extmodule.user_defined_directories.append( crc_settings.generated_files_dir )
        self.__mb.code_creator.replace_included_headers( ['boost/crc.hpp'] )

    def write_files( self ):
        self.__mb.write_module( os.path.join( crc_settings.generated_files_dir, 'crc.pypp.cpp' ) )

    def create(self):
        start_time = time.clock()      
        self.filter_declarations()

        self.prepare_declarations()
        
        self.__mb.build_code_creator( crc_settings.module_name )
        
        self.customize_extmodule()
        self.write_files( )
        print 'time taken : ', time.clock() - start_time, ' seconds'

def export():
    cg = code_generator_t()
    cg.create()

if __name__ == '__main__':
    export()
    print 'done'
    
    
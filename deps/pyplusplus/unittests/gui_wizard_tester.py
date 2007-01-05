# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import autoconfig

import os
from pygccxml import parser
from pyplusplus import module_builder

#Configurating GCC-XML parser
#Basically you should copy here your makefile configuration
parser_configuration = parser.config_t( 
                            #path to GCC-XML binary
                            gccxml_path=r"c:/tools/gccxml/bin/gccxml.exe"
                            , working_directory=r"D:\pygccxml_sources\source\pyplusplus\unittests\data"
                            , include_paths=['D:/pygccxml_sources/source/pyplusplus/gui']
                            , define_symbols=[] )

#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [r"D:\pygccxml_sources\source\pyplusplus\unittests\data\call_policies_to_be_exported.hpp"], parser_configuration )

#Well, don't you want to see what is going on?
mb.print_declarations()

#Creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='pyplusplus' )

#Writing code to file.
mb.write_module( './bindings.cpp' )
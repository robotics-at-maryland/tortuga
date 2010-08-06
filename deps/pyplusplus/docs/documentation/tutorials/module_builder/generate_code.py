#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path.append( '../../../..' )
from environment import gccxml
from pyplusplus import module_builder

mb = module_builder.module_builder_t(
        files=['hello_world.hpp']
        , gccxml_path=gccxml.executable ) #path to gccxml executable

#rename enum Color to color
Color = mb.enum( 'color' )
Color.rename('Color')

#Set call policies to animal::genealogical_tree_ref
animal = mb.class_( 'animal' )
genealogical_tree_ref = animal.member_function( 'genealogical_tree_ref', recursive=False )
genealogical_tree_ref.call_policies = module_builder.call_policies.return_internal_reference()

#next code has same effect
genealogical_tree_ref = mb.member_function( 'genealogical_tree_ref' )
genealogical_tree_ref.call_policies = module_builder.call_policies.return_internal_reference()

#I want to exclude all classes with name starts with impl
impl_classes = mb.classes( lambda decl: decl.name.startswith( 'impl' ) )
impl_classes.exclude()

#I want to exclude all functions that returns pointer to int
ptr_to_int = mb.free_functions( return_type='int *' )
ptr_to_int.exclude()

#I can print declarations to see what is going on
mb.print_declarations()

#I can print single declaration
mb.print_declarations( animal )

#Now it is the time to give a name to our module
mb.build_code_creator( module_name='hw' )

#It is common requirement in software world - each file should have license
mb.code_creator.license = '//Boost Software License( http://boost.org/more/license_info.html )'

#I don't want absolute includes within code
mb.code_creator.user_defined_directories.append( os.path.abspath('.') )

#And finally we can write code to the disk
mb.write_module( os.path.join( os.path.abspath('.'), 'generated.cpp' ) )

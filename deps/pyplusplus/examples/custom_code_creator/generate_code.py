#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This file contains example to custom code creator.
My class point_t has private member variable 'x' and two accessors: get_x and 
set_x. I want to export those accessors as Python property.

In order to do this I created new class property_creator_t. In __init__ it takes
3 arguments:
    reference to get function declaration
    reference to set function declaration
    reference to parent code creator ( we can safely omit this argument )
"""

import os
import sys
sys.path.append( '../..' )
from environment import gccxml
from pygccxml import declarations
from pyplusplus import code_creators
from pyplusplus import module_builder

class property_creator_t( code_creators.code_creator_t ):
    def __init__( self, get_function, set_function ):
        #get/set functions are references to relevant declarations
        code_creators.code_creator_t.__init__( self )
        self.__get_function = get_function
        self.__set_function = set_function
    
    def _create_impl(self):
        template = 'add_property( "%(name)s", &%(getter)s, &%(setter)s )'
        property_name = self.__get_function.name[4:] #get_
        return template % dict( name=property_name
                                , getter=declarations.full_name( self.__get_function )
                                , setter=declarations.full_name( self.__set_function ) )

#This is the most complex code in this example :-)
def replace_functions( class_creator ):
    def is_relevant( code_creator ):
        if not isinstance( code_creator, code_creators.declaration_based_t ):
            #We are looking for code creator, that has to export some declaration
            return False
        if not isinstance( code_creator.declaration, module_builder.member_function_t ):
            #declaration should be member function
            return False
        decl = code_creator.declaration
        #member function name should start from "get" or "set"
        return decl.name.startswith( 'get' ) or decl.name.startswith( 'set' )

    #accessors is a list of all relevant code creators found within the class code creator
    accessors = filter( is_relevant, class_creator.creators )
    #accessors_db is a dictionary, that maps between name and get//set code creators
    accessors_db = {} # name : [get creator, set creator]        
    #Filling accessors_db
    for creator in accessors:
        property_name = creator.declaration.name[3:]
        if not accessors_db.has_key( property_name ):
            accessors_db[ property_name ] = [None, None] 
        accessors_db[ property_name ][ int(creator.declaration.name[0] == 's') ] = creator
    #Now, when we have all pairs of accessors, we can actually start replacing them
    #with instance of property_creator_t
    for get_creator, set_creator in accessors_db.values():
        if not ( get_creator and set_creator ):
            continue 
        #removing get/set code creators
        class_creator.remove_creator( get_creator )
        class_creator.remove_creator( set_creator )
        #instead of them we add an instance of property_creator_t class
        prop_creator = property_creator_t( get_creator.declaration, set_creator.declaration )
        class_creator.adopt_creator( prop_creator )

if __name__ == '__main__':
    module_name = 'properties'
    #1. creating module builder
    mb = module_builder.module_builder_t( files=['properties.hpp']
                                          , gccxml_path=gccxml.executable )
    #2. creating module code creator
    mb.build_code_creator( module_name=module_name )    
    mb.code_creator.user_defined_directories.append( os.path.abspath( '.' ) )
    
    #3. replacing get/set code creators with property code creator
    classes = filter( lambda creator: isinstance( creator, code_creators.class_t )
                      , code_creators.make_flatten( mb.code_creator ) )
    map( replace_functions, classes )
    
    #4. writing module to disk
    mb.write_module( os.path.join( os.path.abspath( '.' ), 'generated', module_name + '.py.cpp') )

    print 'done'
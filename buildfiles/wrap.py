# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/wrap.py

# The following functions were taken and modified from the Python Ogre library:
#  set_implicit_conversion
#  fix_pointer_returns
#  fix_pointer_args (with fix_void_ptr_fun + fix_ptr_fun)
#
# That makes this module under the LGPL as per the license of there project

# Python Imports
import os
import sys
import distutils.sysconfig

# Library Imports
import SCons
from pyplusplus import module_builder
from pygccxml import parser
from pygccxml import declarations

# Build System Imports
import libs

def set_implicit_conversions (classes = None, state = False):
    if classes is None:
        classes = []
    
    for cls in classes:
        cls.constructors().allow_implicit_conversion = state

def add_needed_includes(classes):
    """
    Attempts to add all the missing headers from the given include classes
    """
    if classes is None:
        classes = []
    
    includes = set()

    for cls in classes:
        filename = cls.location.file_name
        if filename.startswith(os.environ['RAM_SVN_DIR']):
            includes.add(filename)

        # Attempt to find all used classes in the header, and include there
        # headers as well
        try:
            for decl in cls.decls(decl_type= declarations.class_t,
                                  recursive = True):
                
                # We only want the file names from our source tree
                filename = decl.location.file_name
                if filename.startswith(os.environ['RAM_SVN_DIR']):
                    includes.add(filename)
        # An exception is returned if we don't find any, so we have to catch
        # and supress
        except RuntimeError:
            pass
        
        # Add needed includes
        print 'Adding includes',includes
        cls.include_files.extend(includes)

def fix_pointer_returns (classes, pointee_types = None, ignore_names = None):
    """
    Change out functions that return a variety of pointer to base types and
    instead have them return the address the pointer is pointing to (the
    pointer value)
    
    This allow us to use CTypes to handle in memory buffers from Python
    
    Also - if documentation has been set then ignore the class/function as it
    means it's been tweaked else where
    """

    if pointee_types is None:
        pointee_types = ['unsigned int', 'int', 'float', 'double', 'char',
                         'unsigned char']
    if ignore_names is None:
        ignore_names = []

    for cls in classes:
        for fun in cls.member_functions( allow_empty = True ):
            if declarations.is_pointer (fun.return_type) and not fun.documentation:
                for i in pointee_types:
                    if fun.return_type.decl_string.startswith ( i ) and not fun.documentation:
                        if not fun.name in ignore_names:
                            print "Excluding (function):", fun, "as it returns (pointer)", i
                        fun.exclude()
        for fun in cls.member_operators( allow_empty = True ):
            if declarations.is_pointer (fun.return_type) and not fun.documentation:
                for i in pointee_types:
                    if fun.return_type.decl_string.startswith ( i ) and not fun.documentation:
                        print "Excluding (operator):", fun
                        fun.exclude()

def fix_pointer_args(classes, pointee_types = None, ignore_names = None):
    """
    We modify functions that take void *'s in their argument list to instead take
    unsigned ints, which allows us to use CTypes buffers
    """
    if pointee_types is None:
        pointee_types = ['unsigned int', 'int', 'float', 'double', 'char',
                         'unsigned char']
    if ignore_names is None:
        ignore_names = []

    for cls in classes:
        for fun in cls.member_functions():
            fix_void_ptr_fun(fun)
            fix_ptr_fun(fun, pointee_types, ignore_names)
            
        for fun in cls.constructors():
            fix_void_ptr_fun(fun)
            fix_ptr_fun(fun, pointee_types, None, Exclude = True)

def fix_void_ptr_fun(fun):
    arg_position = 0
    trans=[]
    desc=""
    for arg in fun.arguments:
        if arg.type.decl_string == 'void const *' or arg.type.decl_string == 'void *':
            trans.append( ft.modify_type(arg_position,_ReturnUnsignedInt ) )
            desc = desc +"Argument: "+arg.name+ "( pos:" + str(arg_position) +") takes a CTypes.addressof(xx). "
        arg_position +=1
    if trans:
        print "Tranformation applied to ", fun, desc
        fun.add_transformation ( * trans , **{"alias":fun.name}  )
        fun.documentation = docit ("Modified Input Argument to work with CTypes",
                                   desc, "...")
        
def fix_ptr_fun(fun, pointee_types = None, ignore_names = None, Exclude = False):
    if pointee_types is None:
        pointee_types = []
    if ignore_names is None:
        ignore_names = []
        
    if fun.documentation or fun.ignore:
        # means it's been tweaked somewhere else
        return
    
    for n in ignore_names:
        if n in fun.name:
            return
    for arg in fun.arguments:
        # and "const" not in arg.type.decl_string:
        if declarations.is_pointer(arg.type): 
            for i in pointee_types:
                if arg.type.decl_string.startswith(i):
                    if Exclude: 
                        print "Excluding:", fun," due to pointer argument", arg.type.decl_string
                        fun.exclude()
                        return
                    else:
                        print "Function has pointer argument: ", fun, arg.type.decl_string
                        fun.documentation=docit("SUSPECT - MAYBE BROKEN due to pointer argument", "....", "...")
                        return
        
def make_already_exposed(global_ns, namespace_name, classes,
                         class_decs = None):
    if type(classes) is not list:
        classes = [classes]

    if class_decs is None:
        class_decs = []

    ns = global_ns
    for name in namespace_name.split('::'):
        print 'Going into namespace',name
        ns = ns.namespace(name)

    for class_name in classes:
        print 'Marking class',class_name
        class_ = ns.class_(class_name)
        class_.include()
        class_.already_exposed = True

    for class_dec in class_decs:
        print 'Marking class dec',class_dec
        class_dec = ns.decl(name = 'Vehicle' ,
                            decl_type = declarations.class_declaration_t)
        class_dec.already_exposed = True

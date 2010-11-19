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
import pyplusplus
from pyplusplus import module_builder
from pygccxml import parser
from pygccxml import declarations

# Setup logger tied into py++
decl_logger = pyplusplus._logging_.loggers.declarations
basedir = os.path.abspath( os.path.join(os.path.dirname(__file__), '..', '..') )

def find_out_container_traits( cls ):
    for ct in declarations.all_container_traits:
        if ct.is_my_case( cls ):
            return ct
    else:
        return None

def mangle_container_names(namespace):
    for cls in namespace.classes():
        # Check to see if we are really a container
        for ct in declarations.all_container_traits:
            if ct.is_my_case( cls ):
                # Mangle name with the string hash function
                mangled_name = 'm' + str(str(cls).__hash__()).replace('-','n')
                cls.rename(mangled_name)
                # Drop out of the inner loop
                break


def exclude_member_functions(cls, members):
    for m in members:
        cls.member_function(m).exclude()

def str_from_ostream(ns):
    """
    Finds all free operators, then exposes only the ones with classes
    currently exposed then Py++ can do the rest.
    """
    for oper in ns.free_operators( '<<' ):
        rtype = declarations.remove_declarated(
            declarations.remove_reference( oper.return_type ) )
        type_or_decl = declarations.remove_declarated(
            declarations.remove_const(
            declarations.remove_reference( oper.arguments[1].type)))
        
        if not isinstance( type_or_decl, declarations.declaration_t ):
            continue
        if type_or_decl.ignore == False:
            decl_logger.info("Exposing operator<<: " + str(oper))
            oper.include()

def set_implicit_conversions (classes = None, state = False):
    """
    Implicit conversions can cause trouble in some cases, so lets you easily
    turn them on and off. Currently they just add a lot of header depenencies
    to the main.cpp file.
    """
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

    #newCls = []
    #for cls in classes:
    #    if isinstance 

    for cls in classes:
        filename = cls.location.file_name
        if filename.startswith(basedir):
            includes.add(filename)

        # Attempt to find all used classes in the header, and include there
        # headers as well
        try:
            for decl in cls.decls(decl_type= declarations.class_t,
                                  recursive = True):
                
                # We only want the file names from our source tree
                filename = decl.location.file_name
                if filename.startswith(basedir):
                    includes.add(filename)
        # An exception is returned if we don't find any, so we have to catch
        # and supress
        except RuntimeError:
            pass
        
        # Add needed includes
        decl_logger.info('Adding includes %s' % includes)
        cls.include_files.extend(includes)

def fix_shared_ptr_vector(cls):
    cls.indexing_suite.disable_method('sort')
    cls.indexing_suite.disable_method('count')
    cls.indexing_suite.disable_method('index')
    cls.indexing_suite.disable_method('contains') 

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
                            decl_logger.info("Excluding (function): %s as it returns (pointer) %s" % (fun, i))
                        fun.exclude()
        for fun in cls.member_operators( allow_empty = True ):
            if declarations.is_pointer (fun.return_type) and not fun.documentation:
                for i in pointee_types:
                    if fun.return_type.decl_string.startswith ( i ) and not fun.documentation:
                        decl_logger.info("Excluding (operator): %s" % fun)
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

def docit ( general, i, o ):
    docs = "\"Modified Function Call\\n" + general +"\\n"
    docs = docs + "Input: " + i + "\\n"
    docs = docs + "Output: " + o + "\\n\\\n\""
    return docs


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
        decl_logger.info("Tranformation applied to %s %s" % (fun, desc))
        fun.add_transformation ( * trans , **{"alias":fun.name}  )
        fun.documentation = docit ("Modified Input Argument to work with CTypes",
                                   desc, "...")
        
def fix_ptr_fun(fun, pointee_types = None, ignore_names = None, Exclude = False):
    """
    Allows to exclude or mark with a warning functions which take pointers as arguments
    It ignores already documented functions, because those have been fixed elsewhere
    """
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
                        decl_logger.info("Excluding: %s due to pointer argument %s" %
                                         (fun, arg.type.decl_string))
                        fun.exclude()
                        return
                    else:
                        decl_logger.info("Function has pointer argument: %s %s" %
                                         (fun, arg.type.decl_string))
                        fun.documentation=docit("SUSPECT - MAYBE BROKEN due to pointer argument", "....", "...")
                        return
        
def make_already_exposed(global_ns, namespace_name, classes,
                         class_decs = None, no_implicit_conversion = False):
    if type(classes) is not list:
        classes = [classes]

    if class_decs is None:
        class_decs = []

    ns = global_ns
    if len(namespace_name):
        for name in namespace_name.split('::'):
            print 'Going into namespace',name
            ns = ns.namespace(name)
            print 'IN'

    for class_name in classes:
        print 'Marking class',class_name
        class_ = ns.class_(class_name)
        class_.include()
        class_.already_exposed = True

        if no_implicit_conversion:
            set_implicit_conversions([class_], False)

    for class_dec in class_decs:
        print 'Marking class dec',class_dec
        class_dec = ns.decl(name = class_dec,
                            decl_type = declarations.class_declaration_t)
        class_dec.already_exposed = True

def add_castTo(cls, from_cls, smart_ptr = False):
    """
    Creates a function which cast from the given class to the current class
    """
    
    if smart_ptr:
        cls_type = declarations.algorithm.full_name(cls).strip("::")
        args = {
            'cls_type' : cls_type,
            'cls_ptr_type' : cls_type + 'Ptr',
            'from_type' : from_cls + 'Ptr'}

        #print args
        cls.add_declaration_code("""
        boost::python::object castTo(%(from_type)s from) {
        return boost::python::object(boost::dynamic_pointer_cast<%(cls_type)s>(from));
        }""" % args)
        cls.add_registration_code("""
        def(\"castTo\", castTo)
        .staticmethod(\"castTo\")""" % args)
        #raise "ERROR SMART PTR NOT SUPPORTED"
    else:
        args = {'cls_type' : declarations.algorithm.full_name(cls).strip("::")+'*',
                'from_type' : from_cls + '*'}
        cls.add_declaration_code("""
        %(cls_type)s castTo(%(from_type)s from) {
        return dynamic_cast<%(cls_type)s>(from);
        }""" % args)
        cls.add_registration_code("""
        def(\"castTo\", (%(cls_type)s (*)(%(from_type)s))(&::castTo),
            boost::python::return_internal_reference<1>())
        .staticmethod(\"castTo\")""" % args)

# Core Specifc Helper Functions
def registerSubsystemConverter(cls):
    cls_type = declarations.algorithm.full_name(cls).strip("::")
    args = {'cls_type' : cls_type,
            'cls_name' : cls_type.replace('::','_').upper()}
    

    cls.add_declaration_code("""
    static ram::core::SpecificSubsystemConverter<%(cls_type)s>
    %(cls_name)s;
    """ % args)

    cls.include_files.append('core/include/SubsystemConverter.h')

def registerConverter(converter_cls, cls, include):
    cls_type = declarations.algorithm.full_name(cls).strip("::")
    args = {'converter_cls' : converter_cls,
            'cls_type' : cls_type,
            'cls_name' : cls_type.replace('::','_').upper()}
    

    cls.add_declaration_code("""
    static %(converter_cls)s<%(cls_type)s>
    %(cls_name)s;
    """ % args)

    cls.include_files.append(include)


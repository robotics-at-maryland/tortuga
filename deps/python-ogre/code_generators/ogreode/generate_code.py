#!/usr/bin/env python

#-------------------------------------------------------------------------------
# TODO:
# 1.    void* as a function argument - they are currently wrapped (and compile/load etc) due to latest CVS of boost.
#       However probably don't actually work
# 2.    Properties.py and calling 'properties.create' - commented out at the moment, not sure if it is really needed?

import os, sys, time

#add environment to the path
sys.path.append( os.path.join( '..', '..' ) )
#add common utils to the pass
sys.path.append( '..' )
sys.path.append( '.' )

import environment
import common_utils
import customization_data
import hand_made_wrappers

from pyplusplus import module_builder
from pyplusplus import module_creator
from pyplusplus.module_builder import call_policies

from pygccxml import parser
from pygccxml import declarations


def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    
    ogreode_ns = global_ns.namespace( 'OgreOde' )
    ogreode_ns.include()

    ogreodeP_ns = global_ns.namespace( 'OgreOde_Prefab' )
    ogreodeP_ns.include()

  
def set_call_policies( mb ):
#
    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the Ogre Default.
    mem_funs = mb.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
    #MSVC 7.1 if function has throw modifier.
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
            mem_fun.call_policies = call_policies.return_value_policy(
                call_policies.reference_existing_object )
                

def add_transformations ( mb ):
    pass
    
def configure_exception(mb):
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
    Exception = mb.namespace( 'OgreOde' ).class_( 'Exception' )
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getFullDescription().c_str()' )

def generate_ogreode():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ogreode.root_dir, "python_ogreode.h" )
                        , environment.ogreode.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogreode.include_dirs
                                          , define_symbols=[]
                                          , indexing_suite_version=2 )

    filter_declarations (mb)

    
    common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases( environment.ogreode.version ) )
    #
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    ogreode_ns = mb.namespace( 'OgreOde' )
    try:
        common_utils.fix_unnamed_classes( ogreode_ns.classes( name='' ), 'OgreOde' )
    except:
        pass # there weren't any unnamed classes

    common_utils.configure_shared_ptr(mb)
#     configure_exception( mb )

    #
    # the manual stuff all done here !!!
    #
    hand_made_wrappers.apply( mb )
    
    #
    # We need to tell boost how to handle calling (and returning from) certain functions
    #
    set_call_policies ( mb.global_ns.namespace ('OgreOde') )
    set_call_policies ( mb.global_ns.namespace ('OgreOde_Prefab') )
    
    # now we fix up the smart pointers ...
#     set_smart_pointers ( mb.global_ns.namespace ('Ogre') )  
    
    # here we fixup functions that expect to modifiy their 'passed' variables    
    add_transformations ( mb )

#     for cls in ogreode_ns.classes():
#     cls.add_properties( recognizer=ogreode_properties.ogreode_property_recognizer_t() )

    ogreode_ns = mb.global_ns.namespace ('OgreOde')
    common_utils.add_properties( ogreode_ns.classes() )
    ogreode_ns = mb.global_ns.namespace ('OgreOde_Prefab')
    common_utils.add_properties( ogreode_ns.classes() )

    
    
    common_utils.add_constants( mb, { 'ogreode_version' :  '"%s"' % environment.ogreode.version
                                      , 'python_version' : '"%s"' % sys.version } )


#     for c in mb.global_ns.classes():
#         print c
#     sys.exit()                                      
    #Creating cogreode creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_ogreode_')
    for inc in environment.ogreode.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )

    mb.code_creator.user_defined_directories.append( environment.ogreode.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.ogreode.version) )

    huge_classes = map( mb.class_, customization_data.huge_classes(environment.ogreode.version) )

    mb.split_module(environment.ogreode.generated_dir, huge_classes)

if __name__ == '__main__':
    start_time = time.clock()
    generate_ogreode()
    print 'Python-ogreode source ogreode was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

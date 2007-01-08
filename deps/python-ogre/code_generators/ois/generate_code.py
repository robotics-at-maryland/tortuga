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
from pyplusplus.module_builder import call_policies

from pygccxml import parser
from pygccxml import declarations


def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    ois_ns = global_ns.namespace( 'OIS' )
    ois_ns.include()
    
    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ois_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()
    
    # exclude this as standard boost libraries don't support std::Multimap's.  Replace by createPythonInputSystem
    ois_ns.class_( "InputManager" ).member_functions("createInputSystem").exclude()
    
def set_call_policies( mb ):
    ois_ns = mb.global_ns.namespace ('OIS')

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the OIS Default.
    mem_funs = ois_ns.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
    #MSVC 7.1 if function has throw modifier.
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
            mem_fun.call_policies = call_policies.return_value_policy(
                call_policies.reference_existing_object )
    #need reference to MouseState as this is where you adjust x/y axis size.
    mouse = mb.class_( "Mouse" )
    mouse.member_function( "getMouseState" ).call_policies =  call_policies.return_value_policy(
                                                                               call_policies.reference_existing_object )
                                                                               
 
def configure_exception(mb):
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
    Exception = mb.namespace( 'OIS' ).class_( 'Exception' )
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.eText' )

def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ois.root_dir, "python_ois.h" )
                        , environment.ois.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ois.include_dirs
                                          , define_symbols=['OIS_NONCLIENT_BUILD']
                                          , indexing_suite_version=2 )

    filter_declarations (mb)

    common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases() )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    configure_exception( mb )

    set_call_policies (mb)
    hand_made_wrappers.apply( mb )

    ois_ns = mb.global_ns.namespace ('OIS')
    common_utils.add_properties( ois_ns.classes() )

    common_utils.add_constants( mb, { 'ois_version' :  '"%s"' % environment.ois.version
                                      , 'python_version' : '"%s"' % sys.version } )


    #Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_ois_')

    for inc in environment.ois.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )
    
    mb.code_creator.user_defined_directories.append( environment.ois.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files() )

    huge_classes = map( mb.class_, customization_data.huge_classes() )

    mb.split_module(environment.ois.generated_dir, huge_classes)

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-OIS source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

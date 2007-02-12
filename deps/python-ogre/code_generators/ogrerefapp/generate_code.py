#!/usr/bin/env python

import os, sys, time, shutil

#add environment to the path
sys.path.append( os.path.join( '..', '..' ) )
#add common utils to the pass
sys.path.append( '..' )
sys.path.append( '.' )

import environment
import common_utils
import customization_data
import hand_made_wrappers

from pygccxml import parser
from pygccxml import declarations
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies
import common_utils.extract_documentation as exdoc
import common_utils.ogre_properties as ogre_properties

def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    ogrerefapp_ns = global_ns.namespace( 'OgreRefApp' )
    ogrerefapp_ns.include()
    
 
    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ogrerefapp_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()
 

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

def configure_exception(mb):
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
    Exception = mb.namespace( 'OgreRefApp' ).class_( 'Exception' )
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getFullDescription().c_str()' )

def find_nonconst ( mb ):
    """ we have problems with sharedpointer arguments that are defined as references
    but are NOT const.  Boost doesn't understand how to match them and you get a C++ Signature match fails.
    In reality the Ogre code probably needs to be patched as all of these should (??) be const.  However we'll fix it 
    with a function transformation wrapper
    """
    funcs = mb.member_functions( )
    for fun in funcs:
        arg_position = 0
        for arg in fun.arguments:
            if 'Ptr' in arg.type.decl_string:
                 if not 'const' in arg.type.decl_string and '&' in arg.type.decl_string:
                    print "Fixing Const", fun.parent.name,"::", fun.name, "::", arg_position
                    fun.add_transformation( ft.modify_type(arg_position,declarations.remove_reference ) )
            arg_position +=1
 
def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ogrerefapp.root_dir, "python_ogre.h" )
                        , environment.ogrerefapp.cache_file )

    defined_symbols = [ 'OGREREFAPP_NONCLIENT_BUILD', 'OGRE_NONCLIENT_BUILD' ]
    defined_symbols.append( 'OGREREFAPP_VERSION_' + environment.ogrerefapp.version )
    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogrerefapp.include_dirs
                                          , define_symbols=defined_symbols
                                          , indexing_suite_version=2 )

    filter_declarations (mb)

#     common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases( environment.ogrerefapp.version ) )
    #
    # fix shared Ptr's that are defined as references but NOT const...
    #
    find_nonconst ( mb.namespace( 'OgreRefApp' ) )
  
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    ogrerefapp_ns = mb.namespace( 'OgreRefApp' )
#     common_utils.fix_unnamed_classes( ogre_ns.classes( name='' ), 'OgreRefApp' )

#     common_utils.configure_shared_ptr(mb)
#     configure_exception( mb )
    
    hand_made_wrappers.apply( mb )

    set_call_policies ( mb.global_ns.namespace ('OgreRefApp') )
    
    for cls in ogrerefapp_ns.classes():
        cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t() )
        ## because we want backwards pyogre compatibility lets add leading lowercase properties
        common_utils.add_LeadingLowerProperties ( cls )

    
    common_utils.add_constants( mb, { 'ogrerefapp_version' :  '"%s"' % environment.ogrerefapp.version
                                      , 'python_version' : '"%s"' % sys.version.replace("\n", "\\\n") } )

    #Creating code creator. After this step you should not modify/customize declarations.
        
    extractor = exdoc.doc_extractor("")

    mb.build_code_creator (module_name='_ogrerefapp_',doc_extractor= extractor)
    for inc in environment.ogrerefapp.include_dirs :
        mb.code_creator.user_defined_directories.append(inc )
    mb.code_creator.user_defined_directories.append( environment.ogrerefapp.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files( environment.ogrerefapp.version ) )

    huge_classes = map( mb.class_, customization_data.huge_classes( environment.ogrerefapp.version ) )

    mb.split_module(environment.ogrerefapp.generated_dir, huge_classes)


if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-OGREREFAPP source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

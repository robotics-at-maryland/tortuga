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
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus import decl_wrappers

from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies

import common_utils.extract_documentation as exdoc
import common_utils.ogre_properties as ogre_properties


def filter_declarations( mb ):

    global_ns = mb.global_ns
    global_ns.exclude()
    
    ogreal_ns = global_ns.namespace( 'OgreAL' )
    ogreal_ns.include()
    ##ogreal_ns.mem_funs( return_type='::OgreAL::Real const *', allow_empty=True).exclude()
    
#     #Virtual functions that return reference could not be overriden from Python
#     query = declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.VIRTUAL ) \
#             & declarations.custom_matcher_t( lambda decl: declarations.is_reference( decl.return_type ) )
#     ogreal_ns.calldefs( query ).virtuality = declarations.VIRTUALITY_TYPES.NOT_VIRTUAL

    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ogreal_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()

         
    # a couple of defined functions without source
    ogreal_ns.class_('Sound').mem_fun('setGainValues').exclude()
    ogreal_ns.class_('Sound').mem_fun('setDistanceValues').exclude()
    
    # need to force these to be included so they get exposed..
    global_ns.namespace( 'Ogre' ).class_('Singleton<OgreAL::Listener>').include()
    global_ns.namespace( 'Ogre' ).class_('Singleton<OgreAL::SoundManager>').include()
    
    # and classes the we need to be exposed but don't want code generated as they already exist in
    # other modules ('Ogre' in this case) that will be "imported" before the OgreAL module
    global_ns.namespace( 'Ogre' ).class_('AxisAlignedBox').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('MovableObject').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('MovableObjectFactory').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Vector3').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Node').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('RenderQueue').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('FrameEvent').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('FrameListener').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('Quaternion').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('SceneManager').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('SceneNode').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('NameValuePairList').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('String').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('ResourceGroupManager').include(already_exposed=True)
#     
#     global_ns.namespace( 'Ogre' ).class_('MapIterator<OgreAL::FormatMap>').include(already_exposed=True)



  
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

                
def set_smart_pointers( mb ):
    for v in mb.variables():
       if not declarations.is_class( v.type ):
           continue
       cls = declarations.class_traits.get_declaration( v.type )
       if cls.name.startswith( 'SharedPtr<' ):
           v.apply_smart_ptr_wa = True    
           print "Applying Smart Pointer: ",  v.name, " of class: ",  cls.name
       elif cls.name.endswith( 'SharedPtr' ):
           v.apply_smart_ptr_wa = True    
           print "Applying Smart Pointer: ",  v.name, " of class: ",  cls.name
                
def configure_exception(mb):
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
    return
    Exception = mb.namespace( 'OgreAL' ).class_( 'Exception' )
    Exception.include()
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getFullDescription().c_str()' )
    

def add_transformations ( mb ):
    ns = mb.global_ns.namespace ('Ogre')
    
    def create_output( size ):
        return [ ft.output( i ) for i in range( size ) ]
    
        
def query_containers_with_ptrs(decl):
    if not isinstance( decl, declarations.class_types ):
       return False
    if not decl.indexing_suite:
       return False
    return declarations.is_pointer( decl.indexing_suite.element_type )


#
# the 'main'function
#            
def generate_code():  
#     messages.disable( 
#           #Warnings 1020 - 1031 are all about why Py++ generates wrapper for class X
#           messages.W1020
#         , messages.W1021
#         , messages.W1022
#         , messages.W1023
#         , messages.W1024
#         , messages.W1025
#         , messages.W1026
#         , messages.W1027
#         , messages.W1028
#         , messages.W1029
#         , messages.W1030
#         , messages.W1031
#         #, messages.W1040 
#         # Inaccessible property warning
#         , messages.W1041 )
    
    #
    # Use GCCXML to create the controlling XML file.
    # If the cache file (../cache/*.xml) doesn't exist it gets created, otherwise it just gets loaded
    # NOTE: If you update the source library code you need to manually delete the cache .XML file   
    #
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ogreal.root_dir, "python_ogreal.h" )
                        , environment.ogreal.cache_file )

    defined_symbols = [ 'OGREAL_NONCLIENT_BUILD', 'OGRE_NONCLIENT_BUILD', 'WIN32', '_LIB', '_MBCS', 'NDEBUG' ]
    defined_symbols.append( 'OGREAL_VERSION_' + environment.ogreal.version )  
    
    #
    # build the core Py++ system from the GCCXML created source
    #    
    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogreal.include_dirs
                                          , define_symbols=defined_symbols
                                          , indexing_suite_version=2
                                           )
    #
    # We filter (both include and exclude) specific classes and functions that we want to wrap
    # 
    filter_declarations (mb)
    
    #
    # fix shared Ptr's that are defined as references but NOT const...
    #
#     find_nonconst ( mb.namespace( 'OgreAL' ) )
      
        
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    ogreal_ns = mb.namespace( 'OgreAL' )
    
    configure_exception( mb )
    
    #
    # the manual stuff all done here !!!
    #
    hand_made_wrappers.apply( mb )
    
    #
    # We need to tell boost how to handle calling (and returning from) certain functions
    #
    set_call_policies ( mb.global_ns.namespace ('OgreAL') )
    set_call_policies ( mb.global_ns.namespace ('Ogre') )  # need this to set singletons to return_existing_object
    
    # now we fix up the smart pointers ...
#     set_smart_pointers ( mb.global_ns.namespace ('OgreAL') )  
    
    # here we fixup functions that expect to modifiy their 'passed' variables    
    add_transformations ( mb )
    

    for cls in ogreal_ns.classes():
        cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t() )

    common_utils.add_constants( mb, { 'ogreal_version' :  '"%s"' % environment.ogreal.version.replace("\n", "\\\n") 
                                      , 'python_version' : '"%s"' % sys.version.replace("\n", "\\\n" ) } )

    ##########################################################################################
    #
    # Creating the code. After this step you should not modify/customize declarations.
    #
    ##########################################################################################
    extractor = exdoc.doc_extractor("")
    mb.build_code_creator (module_name='_ogreal_' , doc_extractor= extractor)
    
    for inc in environment.ogreal.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )
    mb.code_creator.user_defined_directories.append( environment.ogreal.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files( environment.ogreal.version ) )

    huge_classes = map( mb.class_, customization_data.huge_classes( environment.ogreal.version ) )

    mb.split_module(environment.ogreal.generated_dir, huge_classes)

#     if not os.path.exists( os.path.join(environment.ogreal.generated_dir, 'py_shared_ptr.h' ) ):
#         shutil.copy( os.path.join( environment.shared_ptr_dir, 'py_shared_ptr.h' )
#                      , environment.ogreal.generated_dir )

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-OgreAL source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

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

from pygccxml import parser
from pygccxml import declarations
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus import decl_wrappers

from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies
from pyplusplus.module_creator import sort_algorithms

import common_utils.extract_documentation as exdoc
import common_utils.ogre_properties as ogre_properties
import common_utils.process_warnings as process_warnings


def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    
    if not environment.ogre.version.startswith("1.2") and os.name =='nt':
        mb.global_ns.class_( 'vector<Ogre::Vector4, std::allocator<Ogre::Vector4> >' ).exclude( )
    
    ogreode_ns = global_ns.namespace( 'OgreOde' )
    ogreode_ns.include()
    ogreode_ns.class_("Body").variable("MovableType").exclude() ## this "static const Ogre::String" causes msvc7.1 to die!!
    ogreode_ns.class_("Utility").variable("Infinity").exclude() ## this "static const Ogre::String" causes msvc7.1 to die!!

    ogreode_ns.class_("EntityInformer").mem_fun("getIndices").exclude() ## unsigned int const *
    
    # Unfortunately the classes here being used with Maintainedlist don't implement ALL of
    # maintaintlist's functions - hence Py++ has tried to expose them all and it fails at
    # compile time.  Probably need to patch the source to implement null functions.
    ogreode_ns.class_("MaintainedList<OgreOde::Body>").exclude() 
    ogreode_ns.class_("MaintainedList<OgreOde::Geometry>").exclude() 
    ogreode_ns.class_("MaintainedList<OgreOde::Joint>").exclude() 
    ogreode_ns.class_("MaintainedList<OgreOde::JointGroup>").exclude() 
    ogreode_ns.class_("MaintainedList<OgreOde::Space>").exclude() 
#     ogreode_ns.class_("MaintainedListIterator<OgreOde::Body>").exclude() 
#     ogreode_ns.class_("MaintainedListIterator<OgreOde::Geometry>").exclude() 
#     ogreode_ns.class_("MaintainedListIterator<OgreOde::Joint>").exclude() 
#     ogreode_ns.class_("MaintainedListIterator<OgreOde::JointGroup>").exclude() 
#     ogreode_ns.class_("MaintainedListIterator<OgreOde::Space>").exclude() 
    
    # this one has link errors
    ogreode_ns.class_("CircularBuffer<OgreOde::BodyState*>").exclude() 
    ogreode_ns.class_("PlaneBoundedRegionGeometry").member_functions("_planeCallback").exclude() 
    ogreode_ns.class_("TerrainGeometry").member_functions("_heightCallback").exclude() 
    
    ogreodeP_ns = global_ns.namespace( 'OgreOde_Prefab' )
    ogreodeP_ns.include()
    ogreodeP_ns.class_("Vehicle").mem_fun("load").exclude()
     
    ogreodeL_ns = global_ns.namespace( 'OgreOde_Loader' )
    ogreodeL_ns.include()
    ogreodeL_ns.class_('DotLoader').mem_fun('loadFile').exclude() # it returns a pointer to a tinyxml object - could make it opaque?
        ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    ogreode_ns.calldefs( query ).exclude()
    ogreodeP_ns.calldefs( query ).exclude()
    ogreodeL_ns.calldefs( query ).exclude()
    
    ## now expose but don't create class that exist in other modules
    global_ns.namespace( 'Ogre' ).class_('Bone').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Quaternion').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('SimpleRenderable').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('FrameEvent').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Plane').include(already_exposed=True)
#     global_ns.namespace( 'Ogre' ).class_('SceneQuery').struct('WorldFragment').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('MovableObjectFactory').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('MovableObject').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('RaySceneQueryListener').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Root').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Matrix3').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Vector3').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('UserDefinedObject').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('AxisAlignedBox').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Camera').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('FrameListener').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Node').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('SceneNode').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('RenderQueue').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Entity').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('SceneManager').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Matrix4').include(already_exposed=True)
    global_ns.namespace( 'Ogre' ).class_('Plugin').include(already_exposed=True)
    
    #global_ns.namespace( 'Ogre' ).class_('Root').class_('PluginInstanceList').exclude()
    
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
    defined_symbols = [  'OGRE_NONCLIENT_BUILD' ]
    defined_symbols.append( 'OGREODE_VERSION_' + environment.ogreode.version )  

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogreode.include_dirs
                                          , define_symbols= defined_symbols
                                          , indexing_suite_version=2 )

    filter_declarations (mb)

    ## we need to handle "ode" return values etc
    query = lambda decl: isinstance( decl, ( declarations.class_t, declarations.class_declaration_t ) ) \
                         and decl.name.startswith( 'dx' )
    mb.global_ns.decls( query ).opaque = True  
    
    ## here we adjust for functions that return poiners to ODE "ID's", which are really C structs
    ## I may have been over agressive in identifing these functions but hopefully not...
    for func in mb.namespace( 'OgreOde' ).member_functions():  
        if func.return_type.decl_string.endswith('ID'):
            print "Setting ", func.name, "to Opaque"
            func.opaque = True
            func.call_policies = call_policies.return_value_policy(
                call_policies.return_opaque_pointer )
            
                        
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
    set_call_policies ( mb.global_ns.namespace ('OgreOde_Loader') )
    
    # here we fixup functions that expect to modifiy their 'passed' variables    
    add_transformations ( mb )

    #
    # add properties to the 3 namespaces, in a conservative fashion
    #
    namespaces = ['OgreOde', 'OgreOde_Prefab', 'OgreOde_Loader']
    for ns in namespaces:
        for cls in mb.global_ns.namespace(ns).classes():
            cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t() )
    
    common_utils.add_constants( mb, { 'ogreode_version' :  '"%s"' % environment.ogreode.version
                                      , 'python_version' : '"%s"' % sys.version } )
    for ns in namespaces:
        for cls in mb.global_ns.namespace(ns).classes():
            process_warnings.go ( cls )
    #
    # now do the work
    #
    #Creating ogreode creator. After this step you should not modify/customize declarations.
    #
    extractor = exdoc.doc_extractor("")
    mb.build_code_creator (module_name='_ogreode_' , doc_extractor= extractor)
   
    for inc in environment.ogreode.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )

    mb.code_creator.user_defined_directories.append( environment.ogreode.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.ogreode.version) )

    huge_classes = map( mb.class_, customization_data.huge_classes(environment.ogreode.version) )
    print "ABOUT TO SPLIT"
    mb.split_module(environment.ogreode.generated_dir, huge_classes)

if __name__ == '__main__':
    start_time = time.clock()
    generate_ogreode()
    print 'Python-ogreode source ogreode was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

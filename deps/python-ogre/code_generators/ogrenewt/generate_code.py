#!/usr/bin/env python

#
#
# Python-Ogre code generation script for OgreNewt
#
#

import os, sys, time

#add environment to the path
sys.path.append( os.path.join( '..', '..' ) )
#add common utils to the pass
sys.path.append( '..' )
sys.path.append( '.' )

import shutil

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
    
    ogrenewt_ns = global_ns.namespace( 'BasicJoints' )
    ogrenewt_ns.include()
    ogrenewt_ns = global_ns.namespace( 'PrebuiltCustomJoints' )
    ogrenewt_ns.include()
    ogrenewt_ns = global_ns.namespace( 'CollisionPrimitives' )
    ogrenewt_ns.include()
    
    temp_ns = global_ns.namespace( 'Converters' )
    temp_ns.include()
    temp_ns = global_ns.namespace( 'CollisionTools' )
    temp_ns.include()
    temp_ns = global_ns.namespace( 'MomentOfInertia' )
    temp_ns.include()
    
    
    ogrenewt_ns = global_ns.namespace( 'OgreNewt' )
    ogrenewt_ns.include()
    
    ## these need to be excluded due to callback functions - Have been wrapped 
    ogrenewt_ns.class_( "World" ).member_functions("setLeaveWorldCallback").exclude()
    
    ogrenewt_ns.class_( "Body" ).member_functions("addBouyancyForce").exclude()
    
    ogrenewt_ns.class_( "Body" ).member_functions("setAutoactiveCallback").exclude()
    ogrenewt_ns.class_( "Body" ).member_functions("setCustomForceAndTorqueCallback").exclude()
    ogrenewt_ns.class_( "Body" ).member_functions("setCustomTransformCallback").exclude()
    
    ## Replaced these with 'useful' functions in the handwrappers - take and return python objects
    ogrenewt_ns.class_( "Body" ).member_functions("setUserData").exclude()
    ogrenewt_ns.class_( "Joint" ).member_functions("setUserData").exclude()
    ogrenewt_ns.class_( "Body" ).member_functions("getUserData").exclude()
    ogrenewt_ns.class_( "Joint" ).member_functions("getUserData").exclude()
     
       
    ### and we need the free functions 
    for func in ogrenewt_ns.free_functions ():
        ## print "FREE Func:", func.name
        func.include()
            
    ## Exclude protected and private that are not pure virtual
    query = declarations.access_type_matcher_t( 'protected' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ogrenewt_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()

    ## Some varibles that we really do need and aren't exposed by default..    
    cls = ogrenewt_ns.class_("ContactCallback")
    cls.variable('m_body0').include()
    cls.variable('m_body1').include()
    cls.variable('m_contact').include()
    cls.variable('m_material').include()
    
    
    
def set_call_policies_pointee( mb ):
    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the ogrenewt Default.
    ## NOTE AJM 1/1/07 -- this function not used as change to ref_existing_object..
    from pyplusplus import module_creator
    mem_funs = mb.calldefs ()
    mem_funs.create_with_signature = True 
    #MSVC 7.1 if function has throw modifier.
    resolver = module_creator.built_in_resolver_t()
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        decl_call_policies = resolver( mem_fun )
        if decl_call_policies:
            mem_fun.call_policies = decl_call_policies
            continue
        rtype = declarations.remove_alias( mem_fun.return_type )
        if declarations.is_pointer(rtype) or declarations.is_reference(rtype):
#             mem_fun.call_policies \
#                 = call_policies.return_value_policy( call_policies.reference_existing_object )
            mem_fun.call_policies \
               = call_policies.return_value_policy( '::boost::python::return_pointee_value' )
               
    ## now we fix a problem where the getSingleton policy isn't right               
    mb.mem_funs( 'getSingleton' ).call_policies = call_policies.return_value_policy(
                call_policies.reference_existing_object )

                
def set_call_policies( mb ):
#
    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the Ogrenewt Default.
    mem_funs = mb.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
    #MSVC 7.1 if function has throw modifier.
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
            mem_fun.call_policies = call_policies.return_value_policy(
                call_policies.reference_existing_object )
                

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
                
                                               
def add_transformations ( mb ):
    ns = mb.global_ns.namespace ('OgreNewt')
    
    def create_output( size ):
        return [ ft.output( i ) for i in range( size ) ]
   
    ns.mem_fun('::OgreNewt::Body::getPositionOrientation').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::Body::getMassMatrix').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::Body::getInvMass').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::Body::getFreezeThreshold').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::ConvexCollision::calculateInertialMatrix').add_transformation( *create_output(2) )
    
    ns.mem_fun('::OgreNewt::CustomJoint::pinAndDirToLocal') \
        .add_transformation(ft.output('localOrient0'), ft.output('localPos0'), ft.output('localOrient1'), ft.output('localPos1') )
    
    ns.mem_fun('::OgreNewt::Vehicle::Tire::getPositionOrientation').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::ContactCallback::getContactPositionAndNormal').add_transformation( *create_output(2) )
    ns.mem_fun('::OgreNewt::ContactCallback::getContactTangentDirections').add_transformation( *create_output(2) )
   
#     ns.mem_fun('::OgreNewt::CollisionRayCast').add_transformation( ft.output('retColID') )
#     ns.mem_fun('::OgreNewt::CollisionCollideContinue').add_transformation( ft.output('retTimeOfImpact') )

        
def generate_ogrenewt():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ogrenewt.root_dir, "python_ogrenewt.h" )
                        , environment.ogrenewt.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogrenewt.include_dirs
                                          , define_symbols=['ogrenewt_NONCLIENT_BUILD']
                                          , indexing_suite_version=2 )

    filter_declarations (mb)
    #
    # fix shared Ptr's that are defined as references but NOT const...
    #
    find_nonconst ( mb.namespace( 'Ogre' ) )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    #
    # the manual stuff all done here !!!
    #
    hand_made_wrappers.apply( mb )
    
    #
    # We need to tell boost how to handle calling (and returning from) certain functions
    #
    set_call_policies ( mb.global_ns.namespace ('OgreNewt') )
    
    # here we fixup functions that expect to modifiy their 'passed' variables  
    #  
    add_transformations ( mb )
    
    # 
    # now add properties
    #
    for cls in mb.namespace ('OgreNewt').classes():
        cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t()  )
        common_utils.add_LeadingLowerProperties ( cls )
    
    common_utils.add_constants( mb, { 'ogrenewt_version' :  '"%s"' % environment.ogrenewt.version
                                      , 'python_version' : '"%s"' % sys.version } )
    
    # create the doc extractor we'll be using
    extractor = exdoc.doc_extractor("")
    #
    #Creating the actual code. After this step you should not modify/customize declarations.
    #
    mb.build_code_creator (module_name='_ogrenewt_', doc_extractor= extractor)
    for incs in environment.ogrenewt.include_dirs:
        mb.code_creator.user_defined_directories.append( incs )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.ogrenewt.version) )

    huge_classes = map( mb.class_, customization_data.huge_classes(environment.ogrenewt.version) )

    mb.split_module(environment.ogrenewt.generated_dir, huge_classes)

    return_pointee_value_source_path \
        = os.path.join( environment.pyplusplus_install_dir
                        , 'pyplusplus_dev'
                        , 'pyplusplus'
                        , 'code_repository' )
                        ## , 'return_pointee_value.hpp' ) ## Removed AJM 1/1/07

#     return_pointee_value_target_path \
#         = os.path.join( environment.ogrenewt.generated_dir, 'return_pointee_value.hpp' )

#     if not os.path.exists( return_pointee_value_target_path ):
#         shutil.copy( return_pointee_value_source_path, environment.ogrenewt.generated_dir )


if __name__ == '__main__':
    start_time = time.clock()
    generate_ogrenewt()
    print 'Python-ogrenewt source cogrenewt was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

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

import shutil

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
    ode_ns = global_ns  ##  Ode doesn't have it's own namespace..  .namespace( 'ode' )
    
    for cls in ode_ns.classes():
        #print "Checking ", cls.decl_string
        if  cls.decl_string[2]=='d' and cls.decl_string[3].isupper():
            print "Including Class:", cls.name
            cls.include()
    ## and the dxXXclasses        
    for cls in ode_ns.classes():
        #print "Checking ", cls.decl_string
        if  cls.decl_string[2:4]=='dx' and cls.decl_string[4].isupper():
            print "Including dxClass:", cls.name
            cls.include()
     ## and we'll need the free functions as well
    for funcs in ode_ns.free_functions ():
        #print "FREE Func:", funcs.name
        if funcs.name[0]=='d':
            print "Including Function", funcs.name
            funcs.include()
   # these are excluded as they pass real pointers as function varialbles and expectde them to be updated
    # they have been replaced in hand wrappers
    ode_ns.class_( "dBody" ).member_functions( "getPosRelPoint").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getRelPointPos").exclude()
    ode_ns.class_( "dBody" ).member_functions("getPointVel").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getRelPointVel").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getFiniteRotationAxis").exclude()
    ode_ns.class_( "dBody" ).member_functions( "vectorFromWorld").exclude()
    ode_ns.class_( "dBody" ).member_functions( "vectorToWorld").exclude()
    

    # these classes return real *, which are really points to other structures
    # need to be fixed in hand wrappers
    ode_ns.class_( "dBody" ).member_functions( "getPosition").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getRotation").exclude()
    ode_ns.class_( "dBody" ).member_functions("getQuaternion").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getLinearVel").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getAngularVel").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getForce").exclude()
    ode_ns.class_( "dBody" ).member_functions( "getTorque").exclude()
    ode_ns.class_( "dGeom" ).member_functions( "getPosition").exclude()
    ode_ns.class_( "dGeom" ).member_functions( "getRotation").exclude()
    
    
         
#     ptr_to_fundamental_query \
#         = lambda f: declarations.is_pointer( f.return_type ) \
#                     and declarations.is_fundamental( declarations.remove_pointer( f.return_type ) )
                    
#     ode_ns.calldefs( ptr_to_fundamental_query ).exclude()

#     # some internal variables dxXXXX are being exposed via functions 
#     # so we need to looking for functions that return these private xxxID's and remove them 
#     for cls in ode_ns.classes():
#         try:
#             for func in cls.member_functions():
#                 if 'ID' in func.return_type.decl_string[-2:]:
#                     print "Removing :", cls.name,"-",func.name, "-",func.return_type.decl_string 
#                     func.exclude()
#                 ## while we are here lets look for functions returning dReal pointers which are not supported
#                 elif 'dReal' in func.return_type.decl_string:
#                     if '*' in func.return_type.decl_string: ## a pointer to a real :(
#                         print "Removing dREAL:", cls.name,"-",func.name, "-",func.return_type.decl_string 
#                         func.exclude()
#         except:
#             pass
#   
#     # these dx variables are also used in structures so we need to stop these struct members being exposed 
#     for cls in ode_ns.classes():
#         try:
#             for var in cls.variables():
#                 #print "*** ", var,"\n", var.type
#                 if 'ID' in var.type.decl_string[-2:]:
#                     print "Removing VAR:", cls.name,"-",var.name, "-",var.decl_string 
#                     var.exclude()
#                 elif 'as__scope_d' in var.type.decl_string:
#                     print "Removing SCOPE VAR:", cls.name,"-",var.name, "-",var.decl_string 
#                     var.exclude()
#         except:
#             pass
#             
#     cls =  ode_ns.class_('::dJoint')
#     for func in cls.member_functions():
#         print func
        
#     sys.exit()
            
    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ode_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()
    
    #For some reason Py++ does not honor call policies in this case.
    #You will have to expose them by hand
    dContactGeom = ode_ns.class_( 'dContactGeom' )
    g12 = dContactGeom.variables( lambda d: d.name in ('g1', 'g2' ) )
    g12.exclude()
    #g12.getter_call_policies = call_policies.return_value_policy( call_policies.return_opaque_pointer )

 
def set_call_policies( mb ):
    ode_ns = mb.global_ns   ###  Again, no sperate namespace  .namespace ('ode')

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the ode Default.
    mem_funs = ode_ns.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
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


def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ode.root_dir, "python_ode.h" )
                        , environment.ode.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ode.include_dirs
                                          , define_symbols=['ode_NONCLIENT_BUILD', 'ODE_LIB']
#                                          , start_with_declarations=['ode']
                                          , indexing_suite_version=2 )

    filter_declarations (mb)

    query = lambda decl: isinstance( decl, ( declarations.class_t, declarations.class_declaration_t ) ) \
                         and decl.name.startswith( 'dx' )
    mb.global_ns.decls( query ).opaque = True                         
       

    common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases(environment.ode.version) )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    set_call_policies (mb)
    hand_made_wrappers.apply( mb )

    ode_ns = mb.global_ns  ## .namespace ('ode')
    common_utils.add_properties( ode_ns.classes() )

    common_utils.add_constants( mb, { 'ode_version' :  '"%s"' % environment.ode.version
                                      , 'python_version' : '"%s"' % sys.version } )


    #Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_ode_')
    for inc in environment.ode.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )
    mb.code_creator.user_defined_directories.append( environment.ode.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.ode.version) )

    huge_classes = map( mb.class_, customization_data.huge_classes(environment.ode.version) )

    mb.split_module(environment.ode.generated_dir, huge_classes)

    return_pointee_value_source_path \
        = os.path.join( environment.pyplusplus_install_dir
                        , 'pyplusplus_dev'
                        , 'pyplusplus'
                        , 'code_repository'
                        , 'return_pointee_value.hpp' )

    return_pointee_value_target_path \
        = os.path.join( environment.ode.generated_dir, 'return_pointee_value.hpp' )

    if not os.path.exists( return_pointee_value_target_path ):
        shutil.copy( return_pointee_value_source_path, environment.ode.generated_dir )


if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-ode source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

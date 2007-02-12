#!/usr/bin/env python

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

from pyplusplus import module_builder
from pyplusplus import module_creator
from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies

from pygccxml import parser
from pygccxml import declarations

def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    
    FMOD_ns = global_ns.namespace( 'FMOD' )
    FMOD_ns.include()
        
    FMOD_ns = global_ns  ##  need wrappers
    
    for cls in FMOD_ns.classes():       # these are really all the structures etc
        if  cls.decl_string[2:6]=='FMOD' :
            print "Including Class:", cls.name
            cls.include()
    
    ## and we'll need the free functions as well
    ## well in actual fact we can do without th
     
    for funcs in FMOD_ns.free_functions ():
        #print "FREE Func:", funcs.name
        if funcs.name[0:4]=='FMOD':
            print "Including Free Function", funcs.name
            funcs.include() 
# # #      
# # #     for var in FMOD_ns.variables():
# # #         if var.name[0:4] == 'FMOD':
# # #             print "VAR (FMOD ns):", var.name       
# # #             var.include()
# # #             
# # #     for var in global_ns.variables():
# # #         if var.name[0:4] == 'FMOD':
# # #             print "VAR(Global ns):", var.name       
# # #             var.include()
# # #     
# # #     for var in global_ns.typedefs():
# # #         if var.name[0:4] == 'FMOD':
# # #             print "TYPEDEF(Global ns):", var.name       
# # #             var.include()
# # #     
    for var in FMOD_ns.enums():
        if var.name[0:4] == 'FMOD':
            var.include()
            print "ENUM INCLUDED", var.name       
# # #             
# # #     setOpaque = [ "FMOD_SYSTEM", "FMOD_SOUND", "FMOD_CHANNEL", "FMOD_CHANNELGROUP", 
# # #                     "FMOD_REVERB", "FMOD_DSP", "FMOD_POLYGON", "FMOD_GEOMETRY", "FMOD_SYNCPOINT"
# # #                     ]
# # #     for c in setOpaque:
# # #         for t in global_ns.typedefs(c):
# # #             t.opaque=True
                    
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = FMOD_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()
    
    ## now we need to exclude a few things - I thing the return char * * is a problem
    FMOD_ns.class_('FMOD_DSP_STATE').exclude()
    FMOD_ns.class_('FMOD_ADVANCEDSETTINGS').exclude() # calldef('ASIOChannelList').exclude()
    ##mb.free_function('System_Create').exclude() #hand wrapped
    #FMOD_ns.class_('System').mem_fun('update').exclude()
    
#         .add_transformation(ft.outputRef('system'))
       

    # I want the extradatadrive information to be an int address instead of a void pointer
#     c=ns.mem_fun('::FMOD::System::init')  
#     c.arguments[2].default_value = "unsigned int(0)" # unsigned int(%s)" % c.arguments[2].default_value
    for c in FMOD_ns.classes():
        for f in c.member_functions(allow_empty=True):
            for arg in f.arguments:
                if arg.type.decl_string=="void *" and arg.default_value == None:
                    arg.default_value = "unsigned int(0)"
                    
                    
    # exclude hand wrapped functions                    
# # #     FMOD_ns.class_('Sound').mem_fun('release').exclude()
# # #     FMOD_ns.class_('Sound').mem_fun('close').exclude()
# # #     FMOD_ns.class_('System').mem_fun('release').exclude()
# # #     FMOD_ns.class_('System').mem_fun('init').exclude()
# # #     FMOD_ns.class_('System').mem_fun('playDSP').exclude()
# # #     FMOD_ns.free_function('System_Create').exclude()

    excludelist=['addDSP','setChannelGroup','addGroup','addInput','disconnectFrom','setSubSound']
    for c in FMOD_ns.classes():
        for fun in c.member_functions(allow_empty=True):
            if fun.name in excludelist:
                fun.exclude()
#     FMOD_ns.class_('System').mem_fun('playDSP').exclude()        
#     FMOD_ns.class_('System').mem_fun('playSound').exclude()        
#     FMOD_ns.class_('System').mem_fun('recordStart').exclude()        
        
        
        
def set_call_policies( mb ):
    FMOD_ns = mb

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the FMOD Default.
    mem_funs = FMOD_ns.calldefs ()
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
            mem_fun.call_policies = call_policies.return_value_policy( '::boost::python::return_pointee_value' )
                
                
                 
def configure_exception(mb):
    pass
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
#     Exception = mb.namespace( 'FMOD' ).class_( 'Exception' )
#     Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getMessage().c_str()' )

           
def add_transformations ( mb ):
    ns = mb.global_ns.namespace ('FMOD')
    
    def getTransforms ( f, offset ):
        ftlist=[]
        pos = 0
        standardprefix=['boo', 'uns', 'flo', 'int'] # normal types
        for arg in f.arguments:
            if pos >= offset:
                ds = arg.type.decl_string
                if "*" in ds and ds[0:4] in standardprefix: ## a pointer to a 'normal' type
                    ftlist.append( ft.output(pos, AddRef=True) )
                
                elif "* *" in ds and not ds.startswith ('void') and "FMOD::" in ds:   # it's a pointer to a pointer, return by reference
                    ftlist.append( ft.output(pos, AddRef=True, ReturnByRef=True) )
            pos += 1  
        return ftlist 
    
    # fix up transforms in the FMOD classes
    for c in ns.classes():
       for f in c.member_functions(allow_empty=True):
            ftlist = getTransforms( f, 0 )
            if ftlist:
                print "Transforming ", f.name #, " with ", ftlist
                f.add_transformation ( *ftlist ) 
                
# # #     # now handle freefunction transforms                  
# # #     for f in ns.free_functions ():
# # # #         print "FREE checking", f
# # #         ftlist = getTransforms( f, 0 )  
# # #         if ftlist:
# # #             print "Transforming Free", f.name #, " with ", ftlist
# # #             f.add_transformation ( *ftlist ) 
# # #     # now handle freefunction transforms   
# # #                    
# # #     for f in mb.global_ns.free_functions ():
# # #         if f.name.startswith("FMOD"):
# # #             ftlist = getTransforms( f, 0 )
# # #             if ftlist:
# # #                 print "Transforming Global Free", f.name #, " with ", ftlist
# # #                 f.add_transformation ( *ftlist ) 

#     mb.global_ns.free_function('::FMOD_System_Create').add_transformation(ft.output(0, AddRef=True, ReturnByRef=True))
   

def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.fmod.root_dir, "python_FMOD.h" )
                        , environment.fmod.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.fmod.include_dirs
                                          , define_symbols=['FMOD_NONCLIENT_BUILD', '_CONSOLE', 'NDEBUG', 'WIN32']
                                          , indexing_suite_version=2 )
    filter_declarations (mb)
   
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True
    
    configure_exception( mb )
    
    hand_made_wrappers.apply( mb )
    
    set_call_policies (mb.global_ns.namespace ('FMOD'))
    
    add_transformations ( mb )
    

    common_utils.add_constants( mb, { 'FMOD_version' :  '"%s"' % environment.fmod.version
                                       , 'python_version' : '"%s"' % sys.version } )
                                      
    #Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_fmod_')
    for incs in environment.fmod.include_dirs:
        mb.code_creator.user_defined_directories.append( incs )
    mb.code_creator.user_defined_directories.append( environment.fmod.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.fmod.version) )
    huge_classes = map( mb.class_, customization_data.huge_classes(environment.fmod.version) )
    mb.split_module(environment.fmod.generated_dir, huge_classes)
    
    return_pointee_value_source_path = os.path.join( environment.pyplusplus_install_dir
                    , 'pyplusplus_dev'
                    , 'pyplusplus'
                    , 'code_repository'
                    , 'return_pointee_value.hpp' )

    return_pointee_value_target_path \
        = os.path.join( environment.fmod.generated_dir, 'return_pointee_value.hpp' )

    if not os.path.exists( return_pointee_value_target_path ):
        shutil.copy( return_pointee_value_source_path, environment.fmod.generated_dir )

    
    
    

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-FMOD source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

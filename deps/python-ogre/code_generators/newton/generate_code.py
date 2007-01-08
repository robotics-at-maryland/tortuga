#!/usr/bin/env python

#
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
    newton_ns = global_ns    ## no namespace
    ## so lets get all the 'classes' we can that start with Newton
    for cls in newton_ns.classes():
        print "Checking ", cls.decl_string
        if  cls.decl_string[:8]=='::Newton': 
            print "Including:", cls.name
            cls.include()
    ## and we'll need the free functions as well
    for funcs in newton_ns.free_functions ():
        #print "FREE Func:", funcs.name
        if funcs.name[:6]== "Newton":
            print "Including Function", funcs.name
            funcs.include()
     
    
def set_call_policies( mb ):
    newton_ns = mb.global_ns

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the newton Default.
    mem_funs = newton_ns.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
    #MSVC 7.1 if function has throw modifier.
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
            mem_fun.call_policies = call_policies.return_value_policy(
                call_policies.reference_existing_object )
 
def configure_exception(mb):
    pass
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
#     Exception = mb.namespace( 'newton' ).class_( 'Exception' )
#     Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getMessage().c_str()' )

## this is to fix specific challenges where a class (CaratIndex for example) is defined in multiple namespaces
##   

def change_cls_alias( ns ):
   for cls in ns.classes():
       if 1 < len( ns.classes( cls.name ) ):
           alias = cls.decl_string[ len('::newton::'): ]
           print "Adjust:",cls.decl_string
           cls.alias = alias.replace( '::', '' )
           cls.wrapper_alias = cls.alias + 'Wrapper' # or 'Wrapper' ??
           ##cls.exclude()


def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.newton.root_dir, "python_newton.h" )
                        , environment.newton.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.newton.include_dirs
                                          , define_symbols=['newton_NONCLIENT_BUILD','_NEWTON_USE_LIB', '_CONSOLE', 'NDEBUG']
#                                          , start_with_declarations=['newton']
                                          , indexing_suite_version=2 )
    filter_declarations (mb)
   
#    change_cls_alias( mb.global_ns.namespace ('newton') )

    common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases(environment.newton.version) )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

#    configure_exception( mb )

    hand_made_wrappers.apply( mb )

    set_call_policies (mb)
    common_utils.add_properties(  mb.global_ns.classes() )

    common_utils.add_constants( mb, { 'newton_version' :  '"%s"' % environment.newton.version
                                       , 'python_version' : '"%s"' % sys.version } )
                                      
    #Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_newton_')
    for incs in environment.newton.include_dirs:
        mb.code_creator.user_defined_directories.append( incs )
    mb.code_creator.user_defined_directories.append( environment.newton.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.newton.version) )
    huge_classes = map( mb.class_, customization_data.huge_classes(environment.newton.version) )
    mb.split_module(environment.newton.generated_dir, huge_classes)

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-newton source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

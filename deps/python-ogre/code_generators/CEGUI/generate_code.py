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
import common_utils.extract_documentation as exdoc
import common_utils.ogre_properties as ogre_properties


def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    CEGUI_ns = global_ns.namespace( 'CEGUI' )
    CEGUI_ns.include()
    
    ## Dumb fix to remove all Properties classes -  unfortunately the properties in these classes are indicated as public
    ## however within their 'parent' class they are defined as private..
    ## see MultiLineEditboxProperties
    ##
    for cls in mb.global_ns.namespace ('CEGUI').classes():
        if "Properties" in cls.decl_string:
            print "Excluding:", cls.name
            cls.exclude()
           
    ## EventNamespace causes failure when loading the python module
    ## possibly because of the ugly Properties fix above :) 
    for cls in mb.global_ns.namespace ('CEGUI').classes():
        try:
            cls.variable('EventNamespace').exclude()
        except:
            pass
    ## turns out that in SOME classes this also fails registration (Combodroplist for example)
    for cls in mb.global_ns.namespace ('CEGUI').classes():
        try:
            cls.variable('WidgetTypeName').exclude()
        except:
            pass
            
    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = CEGUI_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()
    
    ## lets work around a bug in GCCXMl - http://language-binding.net/pygccxml/design.html#patchers
    draws = mb.member_functions( 'draw' )   # find all the draw functions
    for draw in draws:
        for arg in draw.arguments:  
            if arg.default_value == '0ffffffff':
                arg.default_value = '0xffffffff'

         
    ## this one fails at link time
    mb.class_( 'ScriptFunctor' ).exclude()   
    mb.class_( 'CEGUIRQListener' ).exclude()     
    ## one version of createFont fails so lets find it and exclude it
#     fm = mb.class_( 'FontManager' )
#     ## can't use the simple, if it had 2 args exclude it as there is a version with 2 args that we want
#     ## i.e.      fm.member_function( 'createFont', arg_types=[None,None ] ).exclude() 
#     ## so instead need a function that looks for arg 2 to be XMLAttributes
#     is_my_case = lambda decl: len( decl.arguments ) == 2 and 'XMLAttributes' in decl.arguments[1].type.decl_string 
#     fm.member_function( name='createFont', function=is_my_case ).exclude()
#   
    ## couple of functions that fail when compiling
    
    if environment.ogre.version == "CVS":    
        CEGUI_ns.class_( "RawDataContainer" ).member_functions( 'getDataPtr' ).exclude()
        CEGUI_ns.class_( "ItemListBase" ).member_functions( 'getSortCallback' ).exclude()
    else:   
        CEGUI_ns.class_("DataContainer<unsigned char>").exclude()
        CEGUI_ns.class_("Menubar").exclude()
        CEGUI_ns.class_("MenuBase").exclude()
        CEGUI_ns.class_("PopupMenu").exclude()
        CEGUI_ns.class_("Referenced").exclude()
        CEGUI_ns.class_("System").member_functions("getXMLParser").exclude()

    ## now have functions in String that return uint arrays a need to be wrapped
    sc = CEGUI_ns.class_( "String" )
    sc.member_functions('data').exclude()
    sc.member_functions('ptr').exclude()
    ## and only remove the at functions that are not returning constants
    ## the const version returns by value which is good, the non const returns a reference which doesn't compile
    sc.member_function( 'at', lambda decl: decl.has_const == False ).exclude()
    
    ## CEGUI::WindowManager::loadWindowLayout can take a function pointer as an agrument whcih isn't supported yet
    ## so lets remove the versions that expose the pointer 
    lo = CEGUI_ns.class_( 'WindowManager' ).member_function( 'loadWindowLayout' )
    lo.arguments[3].type = lo.arguments[4].type     #AJM Not sure how args work so setting the func pointer to a void pointer
    
    ## OgreCEGUIRenderer.h has an assumed namespace in one of the default agrs that we need to fix
    orRe = CEGUI_ns.constructor('OgreCEGUIRenderer', arg_types=[None, None, None, None] )
    pos = orRe.arguments[1].default_value.index("RENDER_QUEUE_OVERLAY")
    tempstring = orRe.arguments[1].default_value[:pos]+"::Ogre::"+orRe.arguments[1].default_value[pos:]
    orRe.arguments[1].default_value = tempstring
    
    ## a string one that stops pydoc working against CEGUI
    CEGUI_ns.class_('ListHeader').variable('SegmentNameSuffix').exclude()
#     #Exclude non default constructors of iterator classes. 
#     for cls in CEGUI_ns.classes():
#        if not declarations.templates.is_instantiation( cls.name ):
#            continue
#        name = declarations.templates.name( cls.name )
#        if not name.endswith( 'Iterator' ):
#            continue
#        #default constructor does not have arguments
#        constructors = cls.constructors( lambda decl: bool( decl.arguments )
#                                                       , allow_empty=True
#                                                       , recursive=False )
#        constructors.exclude()

    ## I'm going to exclude all iterators as there is a problem with CEGUIIteratorBase.h
    for cls in CEGUI_ns.classes():
        print "checking", cls.name
        if 'iterator' in cls.name.lower() :
            cls.exclude()
            print "Excluding Iterator", cls.name
            
    
def set_call_policies( mb ):
    CEGUI_ns = mb.global_ns.namespace ('CEGUI')

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the CEGUI Default.
    mem_funs = CEGUI_ns.calldefs ()
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
    Exception = mb.namespace( 'CEGUI' ).class_( 'Exception' )
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getMessage().c_str()' )

## this is to fix specific challenges where a class (CaratIndex for example) is defined in multiple namespaces
##   
def change_cls_alias( ns ):
   for cls in ns.classes():
       if 1 < len( ns.classes( cls.name ) ):
           alias = cls.decl_string[ len('::CEGUI::'): ]
           print "Adjust:",cls.decl_string
           cls.alias = alias.replace( '::', '' )
           cls.wrapper_alias = cls.alias + 'Wrapper' # or 'Wrapper' ??
           ##cls.exclude()


def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.CEGUI.root_dir, "python_CEGUI.h" )
                        , environment.CEGUI.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.CEGUI.include_dirs
                                          , define_symbols=['CEGUI_NONCLIENT_BUILD']
                                          , indexing_suite_version=2 )
    filter_declarations (mb)
   
    change_cls_alias( mb.global_ns.namespace ('CEGUI') )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    configure_exception( mb )

    hand_made_wrappers.apply( mb )

    set_call_policies (mb)
    for cls in mb.global_ns.namespace ('CEGUI').classes():
        cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t()  )
        common_utils.add_LeadingLowerProperties ( cls )

    common_utils.add_constants( mb, { 'CEGUI_version' :  '"%s"' % environment.CEGUI.version
                                       , 'python_version' : '"%s"' % sys.version } )
                                      
    #Creating code creator. After this step you should not modify/customize declarations.
    extractor = exdoc.doc_extractor("")
    
    mb.build_code_creator (module_name='_cegui_', doc_extractor= extractor)
    for incs in environment.CEGUI.include_dirs:
        mb.code_creator.user_defined_directories.append( incs )
    mb.code_creator.user_defined_directories.append( environment.CEGUI.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.CEGUI.version) )
    huge_classes = map( mb.class_, customization_data.huge_classes(environment.CEGUI.version) )
    mb.split_module(environment.CEGUI.generated_dir, huge_classes)

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-CEGUI source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

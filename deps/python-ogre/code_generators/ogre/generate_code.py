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

import ogre_properties

def filter_declarations( mb ):

    global_ns = mb.global_ns
    global_ns.exclude()
    
    ogre_ns = global_ns.namespace( 'Ogre' )
    ogre_ns.include()
    
    startswith = [
        # Don't include, we'll never need.
        'D3D', 'GL', 'WIN32', '_'  
        , 'MemoryManager'   ## it's a specialised C++ mem manger not needed in Python
        ## , 'Any' ## it tries to hold any Ogre type - we get it for free in Python, however other funcs need it
        , 'RadixSort' ## these show up as ugly aliases but are never exposed - and are really protected
    ]

    if not environment.ogre.version.startswith("1.2"):
        mb.global_ns.class_( 'vector<Ogre::Vector4, std::allocator<Ogre::Vector4> >' ).exclude( )
    
    

    ## Now get rid of a wide range of classes as defined earlier in startswith...
    for prefix in startswith:
        ### NOTE the PREFIX is used here !!!!
        classes = ogre_ns.classes( common_utils.decl_starts_with(prefix), allow_empty=True)
        classes.exclude()

    #AJM Set of functions in Particle system that don't get wrapped properly.. Rechecked 30Nov06 AJM
    PartSys = ogre_ns.class_( "ParticleSystem" )
    PartSys.class_( "CmdIterationInterval" ).exclude()
    PartSys.class_( "CmdLocalSpace" ).exclude()
    PartSys.class_( "CmdNonvisibleTimeout" ).exclude()
    PartSys.class_( "CmdSorted" ).exclude()
    
    ## Functions defined in .h files but not implemented in source files
    ogre_ns.class_("TargetManager").member_functions("getPositionTargetAt").exclude() 
    ogre_ns.class_('Root').mem_fun('termHandler').exclude()
    ogre_ns.class_( "StaticGeometry" ).class_("Region").member_functions('getLights').exclude() 
    
    ## It's a structure that doesn't get included by default...
    ogre_ns.class_("VertexBoneAssignment_s").include()

    #
    # problem areas as defermined by Py++'s "unexposed" warning message
    # compile problems - There are a number fo Factories that don't expose well
    # lets hope that they aren't really needed from Python....
    ogre_ns.class_('ArchiveFactory').exclude() 
    ogre_ns.class_('ArchiveManager').mem_fun('addArchiveFactory').exclude() # this needs ArchiveFactory
    ogre_ns.class_('ParticleSystemRendererFactory').exclude()
    ogre_ns.class_('FactoryObj<Ogre::Archive>').exclude()
    ogre_ns.class_('FactoryObj<Ogre::ParticleSystemRenderer>').exclude()
    ## these need ParticleSystemRendererFactory
    ogre_ns.class_('MapIterator<std::map<std::string, Ogre::ParticleSystemRendererFactory*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::ParticleSystemRendererFactory*> > > >').mem_fun('getNext').exclude()
    ogre_ns.class_('MapIterator<std::map<std::string, Ogre::ParticleSystemRendererFactory*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::ParticleSystemRendererFactory*> > > >').mem_fun('peekNextValue').exclude()
    ogre_ns.class_('BillboardParticleRendererFactory').exclude()
    ogre_ns.class_('ParticleSystemManager').mem_fun('addRendererFactory').exclude()
    
  
    #ogre_ns.class_('Mesh').typedef('SubMeshNameMap').include()
    #ogre_ns.class_('Mesh').mem_fun('getSubMeshNameMap').include()

    
    ## Ogre::GpuProgramParameters::AutoConstantEntry is flagged as private but other functions rely on it
    ## however we can't expose it as Py++ doesn't like the Union that's defined within the class def..
    ogre_ns.class_('Renderable').mem_fun('_updateCustomGpuParameter').exclude()
    ogre_ns.class_('ConstVectorIterator<std::vector<Ogre::GpuProgramParameters::AutoConstantEntry, std::allocator<Ogre::GpuProgramParameters::AutoConstantEntry> > >').exclude() # ::getNext(), ::peekNext()
    ogre_ns.class_('GpuProgramParameters').mem_fun('getAutoConstantEntry').exclude()
    ogre_ns.class_('SubEntity').mem_fun('_updateCustomGpuParameter').exclude()
    
    # there are a set of consiterators that I'm not exposing as they need better understanding and testing
    # these functions rely on them so they are being excluded as well
    ogre_ns.class_('SceneNode').member_functions('getAttachedObjectIterator').exclude()
    ogre_ns.class_('Mesh').mem_fun('getBoneAssignmentIterator').exclude()
    ogre_ns.class_('SubMesh').mem_fun('getBoneAssignmentIterator').exclude()
    ogre_ns.class_('GpuProgramParameters').mem_fun('getAutoConstantIterator').exclude()
     
    # A couple of Std's that need exposing
    std_ns = global_ns.namespace("std")
    std_ns.class_("pair<unsigned, unsigned>").include()
    std_ns.class_("pair<bool, float>").include()
    
    # HashMap is defined in OgrePrerequisits.h and is different based on platform
    # ::stdext::hash_map
#     if sys.platform=='win32':
#         for s in global_ns.namespace("stdext").classes():
# #            print "stdex", s, s.alias
# #         global_ns.namespace("stdext").class_('hash_map<Ogre::String, Ogre::ushort>').include()
#     else:
#         global_ns.namespace("__gnu_cxx").class_('hash_map<Ogre::String, Ogre::ushort>').include()

    # exclude functions and operators that return Real * as we don't handle them well :(
    # 
    # TODO  could change them to return_pointee            
    #
    ogre_ns.mem_funs( return_type='::Ogre::Real const *', allow_empty=True).exclude()
    ogre_ns.mem_funs( return_type='::Ogre::Real *',allow_empty=True).exclude()
    ogre_ns.mem_funs( return_type='float *',allow_empty=True).exclude()
    ogre_ns.member_operators( return_type='::Ogre::Real const * const', allow_empty=True ).exclude()  ## YUCK Matrix4!
    ogre_ns.member_operators( return_type='::Ogre::Real const *', allow_empty=True ).exclude()
    ogre_ns.member_operators( return_type='::Ogre::Real *', allow_empty=True).exclude()
    ogre_ns.member_operators( return_type='float *', allow_empty=True).exclude()
    
    ogre_ns.mem_funs( return_type='::Ogre::uchar *', allow_empty=True).exclude() #light::getdata xxx.opaque = True ??
    ogre_ns.mem_funs( return_type='::Ogre::uchar const *', allow_empty=True).exclude() #light::getdata xxx.opaque = True ??
   
    #all constructors in this class are private, also some of them are public.
    Skeleton = ogre_ns.class_( 'Skeleton' ).constructors().exclude()

    ogre_ns.free_functions ('any_cast').exclude () #not relevant for Python

    #AttribParserList is a map from string to function pointer, this class could not be exposed
    AttribParserList = ogre_ns.typedef( name="AttribParserList" )
    declarations.class_traits.get_declaration( AttribParserList ).exclude()

    #VertexCacheProfiler constructor uses enum that will be defined later.
    #I will replace second default value to be int instead of enum
    #arg_types=[None,None] - 2 arguments, with whatever type
    VertexCacheProfiler = ogre_ns.constructor( 'VertexCacheProfiler', arg_types=[None,None] )
    VertexCacheProfiler.arguments[1].default_value = "int(%s)" % VertexCacheProfiler.arguments[1].default_value

    ## now specifically remove functions that we have wrapped in hand_made_wrappers.py
    ogre_ns.class_( "RenderTarget" ).member_functions( 'getCustomAttribute' ).exclude()
    ogre_ns.class_( "Mesh" ).member_functions( 'suggestTangentVectorBuildParams' ).exclude()

    ## Expose functions that were not exposed but that other functions rely on    
    ogre_ns.class_("OverlayManager").member_functions('addOverlayElementFactory').include()

    ## AJM Error at compile time - errors when compiling or linking
    ogre_ns.class_( "MemoryDataStream" ).member_functions( 'getCurrentPtr' ).exclude()
    ogre_ns.class_( "MemoryDataStream" ).member_functions( 'getPtr' ).exclude()
    ogre_ns.calldefs ('peekNextPtr').exclude ()
    ogre_ns.calldefs ('peekNextValuePtr').exclude ()    #in many of the Iterator classes
    ogre_ns.calldefs ('getChildIterator').exclude ()
    
    ogre_ns.class_( "ErrorDialog" ).exclude()   # doesn't exist for link time
    ogre_ns.class_( 'CompositorInstance').class_('RenderSystemOperation').exclude() # doesn't exist for link time
    ogre_ns.class_( 'CompositorChain').mem_fun('_queuedOperation').exclude() #needs RenderSystemOperation
    
##  Note - you need to do all the 'excludes' AFTER you've included all the classes you want..
##  Otherwise things get screwed up...

## Remove private classes , and those that are internal to Ogre...
    private_decls = common_utils.private_decls_t(environment.ogre.include_dirs)
    for cls in ogre_ns.classes():
        if private_decls.is_private( cls ):
            cls.exclude()
            print '{*} class "%s" is marked as private' % cls.decl_string


    for func in ogre_ns.calldefs():
        if private_decls.is_private( func ):
            if func.virtuality == declarations.VIRTUALITY_TYPES.PURE_VIRTUAL:
                continue
            func.exclude()
            print '{*} function "%s" is marked as internal' % declarations.full_name( func )

    ## Exclude protected and private that are not pure virtual
    query = ~declarations.access_type_matcher_t( 'public' ) \
            & ~declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.PURE_VIRTUAL )
    non_public_non_pure_virtual = ogre_ns.calldefs( query )
    non_public_non_pure_virtual.exclude()

    #Virtual functions that return reference could not be overriden from Python
    query = declarations.virtuality_type_matcher_t( declarations.VIRTUALITY_TYPES.VIRTUAL ) \
            & declarations.custom_matcher_t( lambda decl: declarations.is_reference( decl.return_type ) )
    ogre_ns.calldefs( query ).virtuality = declarations.VIRTUALITY_TYPES.NOT_VIRTUAL

    #RenderOperation class is marked as private, but I think this is a mistake
    ogre_ns.class_('RenderOperation').include()
    
    if sys.platform == 'win32':
        # need to force these
        hwnd = global_ns.class_("HWND__")
        hwnd.opaque = True
        _iobuf = global_ns.class_("_iobuf")# need the file handle in Ogre::FileHandleDataStream::FileHandleDataStream
        _iobuf.opaque = True

    #Exclude non default constructors of iterator classes. 
    for cls in ogre_ns.classes():
       if not declarations.templates.is_instantiation( cls.name ):
           continue
       name = declarations.templates.name( cls.name )
       if not name.endswith( 'Iterator' ):
           continue
       #default constructor does not have arguments
       constructors = cls.constructors( lambda decl: bool( decl.arguments )
                                                      , allow_empty=True
                                                      , recursive=False )
       constructors.exclude()


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
                    #print "Fixing Const", fun.parent.name,"::", fun.name, "::", arg_position
                    fun.add_transformation( ft.modify_type(arg_position,declarations.remove_reference ) )
            arg_position +=1
                    
##
## Note that I've left this function in here afor reference purposes - it's not used as its an ugly
## workaround/fix to something that isn't broken or should be fixed in a python wrapper
##                 
def fixup_specials ( mb ):
    """ fixup to create override functions that accept tuples instead of Vector3's or Colourvalues
    'override_type' should be Vector3 or ColourValue
    """
    FUN_CODE  = """
/// Python-Ogre override to enable property creation passing a python tuple
void
%(class_name)s_%(function_name)s( ::Ogre::%(class_name)s & me, ::boost::python::tuple tuplein) {
    me.%(function_name)s ( %(override_type)s( boost::python::extract<Ogre::Real> (tuplein[0]),
                            boost::python::extract<Ogre::Real> (tuplein[1]),
                            boost::python::extract<Ogre::Real> (tuplein[2]) )
                            );
}
"""   

    EXPOSER_CODE = """
          def( "%(function_name)s" , &%(class_name)s_%(function_name)s );    /// Python-Ogre Setter Extension (tuple as input)
"""
    ## the list of ogre values we want to accept as 3 tuples
    overrides= ['::Ogre::Vector3 const &', '::Ogre::ColourValue const &']
    exclude_classes = ['AnimableValue']
    
    for override in overrides:
        override_clean  = (override.split()[0])[2:] ## need a simple version of Ogre::Vector3 or Ogre::ColourValue etc..
        ## get the list of matching functions
        funcs = mb.member_functions( return_type='void' , arg_types=[override])
        for fun in funcs:
            if fun.name.startswith ('set') and fun.parent.name not in exclude_classes : ## we only override set's
                tc = mb.class_( fun.parent.name )
                tc.add_declaration_code(
                    FUN_CODE % { 'class_name': fun.parent.name
                                               , 'function_name': fun.name
                                               , 'override_type' : override_clean } )
                tc.add_registration_code( 
                    EXPOSER_CODE % { 'class_name': fun.parent.name
                                               , 'function_name': fun.name } )
                propname = fun.name[3:] # remove the 'set'
                ### AJm WARNING - this bit is to make things easier to create python proerties and isn't gauranteed to be right
                print "Setter_Override Ogre.%(class_name)s.%(propname)s= property( Ogre.%(class_name)s.get%(propname)s,Ogre.%(class_name)s.set%(propname)s ) " % {'class_name' : fun.parent.name,
                                'propname' : propname }
                lprop = propname[0].lower() + propname[1:]                                
                print "Setter_Override Ogre.%(class_name)s.%(lprop)s= property( Ogre.%(class_name)s.get%(propname)s, Ogre.%(class_name)s.set%(propname)s ) " % {'lprop' : lprop,
                                'class_name' : fun.parent.name,
                                'propname' : propname }
  
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
    Exception = mb.namespace( 'Ogre' ).class_( 'Exception' )
    Exception.include()
    Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getFullDescription().c_str()' )
    
def get_pyplusplus_alias( typedef ):
    dpath = declarations.declaration_path( typedef )
#     print "checking", len(dpath), typedef.name
    if len( dpath ) != 4:
       return None
    #dpath[0] is global namespace
    if dpath[1] != 'pyplusplus':
       return None
    if dpath[2] != 'aliases':
       return None
    return typedef.name


def add_transformations ( mb ):
    ns = mb.global_ns.namespace ('Ogre')
    
    def create_output( size ):
        return [ ft.output( i ) for i in range( size ) ]
   
    rt_cls = ns.class_('RenderTarget')
    rt_cls.mem_fun('getMetrics').add_transformation( *create_output(3) )
    rt_cls.mem_fun( 'getStatistics', arg_types=['float &']*4 ).add_transformation( *create_output(4) )
    
    ns.mem_fun('::Ogre::RenderQueueListener::renderQueueEnded') \
        .add_transformation(ft.output('repeatThisInvocation'))
    ns.mem_fun('::Ogre::RenderQueueListener::renderQueueStarted') \
        .add_transformation(ft.output('skipThisInvocation'))
    ns.mem_fun('::Ogre::RenderWindow::getMetrics').add_transformation( *create_output(5) )
    ns.mem_fun('::Ogre::Viewport::getActualDimensions').add_transformation( *create_output(4) )
    ns.mem_fun('::Ogre::CompositorChain::RQListener::renderQueueStarted') \
        .add_transformation(ft.output("skipThisQueue"))
    ns.mem_fun('::Ogre::CompositorChain::RQListener::renderQueueEnded') \
        .add_transformation(ft.output("repeatThisQueue"))
    ns.mem_fun('::Ogre::PanelOverlayElement::getUV') \
        .add_transformation(ft.output('u1'), ft.output('v1'), ft.output('u2'), ft.output('v2') )
    ### ns.class_('Matrix3').mem_fun('Matrix3').add_transformation(ft.inputarray(9))........        
    ns.mem_fun('::Ogre::Font::getGlyphTexCoords') \
        .add_transformation(ft.output(1), ft.output(2),ft.output(3), ft.output(4))

        
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
    messages.disable( 
          #Warnings 1020 - 1031 are all about why Py++ generates wrapper for class X
          messages.W1020
        , messages.W1021
        , messages.W1022
        , messages.W1023
        , messages.W1024
        , messages.W1025
        , messages.W1026
        , messages.W1027
        , messages.W1028
        , messages.W1029
        , messages.W1030
        , messages.W1031
        #, messages.W1040 
        # Inaccessible property warning
        , messages.W1041 )
    
    #
    # Use GCCXML to create the controlling XML file.
    # If the cache file (../cache/*.xml) doesn't exist it gets created, otherwise it just gets loaded
    # NOTE: If you update the source library code you need to manually delete the cache .XML file   
    #
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.ogre.root_dir, "python_ogre.h" )
                        , environment.ogre.cache_file )

    defined_symbols = [ 'OGRE_NONCLIENT_BUILD' ]
    if not environment.ogre.version.startswith("1.2"):
        defined_symbols.append( 'OGRE_VERSION_CVS' )  
    
    #
    # build the core Py++ system from the GCCXML created source
    #    
    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.ogre.include_dirs
                                          , define_symbols=defined_symbols
                                          , indexing_suite_version=2
                                           )
#     #
#     # Fix aliases so we don't get ugly classes
#     #    
#     for cls in mb.classes():
#         for alias in cls.aliases:
#             ppya=get_pyplusplus_alias( alias)
#             if ppya:
#                 cls.alias = ppya
#                 print "Fixing alias: ",cls," == ",ppya
#                 break                                       
    #
    # We filter (both include and exclude) specific classes and functions that we want to wrap
    # 
    filter_declarations (mb)
 
    #
    # fix shared Ptr's that are defined as references but NOT const...
    #
    find_nonconst ( mb.namespace( 'Ogre' ) )
      
    #
    # lets fix containers that hold pointers - the return value needs adjusting...   
    #
#     tt = mb.namespace( 'std' ).decls( query_containers_with_ptrs )
#     for t in tt:
#         t.indexing_suite.call_policies  = module_builder.call_policies.return_internal_reference()
        
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    ogre_ns = mb.namespace( 'Ogre' )
    
    #Py++ can not expose static pointer member variables
    ogre_ns.vars( 'ms_Singleton' ).disable_warnings( messages.W1035 )
    
    common_utils.fix_unnamed_classes( ogre_ns.classes( name='' ), 'Ogre' )

    common_utils.configure_shared_ptr(mb)
    configure_exception( mb )
    
    #
    # the manual stuff all done here !!!
    #
    hand_made_wrappers.apply( mb )
    
    #
    # We need to tell boost how to handle calling (and returning from) certain functions
    #
    set_call_policies ( mb.global_ns.namespace ('Ogre') )
    
    # now we fix up the smart pointers ...
    set_smart_pointers ( mb.global_ns.namespace ('Ogre') )  
    
    # here we fixup functions that expect to modifiy their 'passed' variables    
    add_transformations ( mb )
    
    cls = mb.class_( "IndexData" )
    v = cls.variable( "indexBuffer" )
    v.apply_smart_ptr_wa = True
    cls = mb.class_( "SubMesh" )
    v = cls.variable( "vertexData" )
    v.apply_smart_ptr_wa = True
    v = cls.variable( "indexData" )
    v.apply_smart_ptr_wa = True

#     for cls in ogre_ns.classes():
    for cls in mb.global_ns.classes():
        print "Adding Prop to:", cls
        cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t() )
        ## because we want backwards pyogre compatibility lets add leading lowercase properties
        common_utils.add_LeadingLowerProperties ( cls )

    common_utils.add_constants( mb, { 'ogre_version' :  '"%s"' % environment.ogre.version
                                      , 'python_version' : '"%s"' % sys.version } )

    ##########################################################################################
    #
    # Creating the code. After this step you should not modify/customize declarations.
    #
    ##########################################################################################
    extractor = exdoc.doc_extractor("")
    
    mb.build_code_creator (module_name='_ogre_' , doc_extractor= extractor)
    
    for inc in environment.ogre.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )
    mb.code_creator.user_defined_directories.append( environment.ogre.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files( environment.ogre.version ) )

    huge_classes = map( mb.class_, customization_data.huge_classes( environment.ogre.version ) )

    mb.split_module(environment.ogre.generated_dir, huge_classes)

    if not os.path.exists( os.path.join(environment.ogre.generated_dir, 'py_shared_ptr.h' ) ):
        shutil.copy( os.path.join( environment.shared_ptr_dir, 'py_shared_ptr.h' )
                     , environment.ogre.generated_dir )

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-OGRE source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )

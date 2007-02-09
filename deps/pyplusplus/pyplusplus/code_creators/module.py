# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import types
import custom
import license
import include
import namespace    
import compound
import algorithm
import module_body
import include_directories

class module_t(compound.compound_t):
    """This class represents the source code for the entire extension module.

    The root of the code creator tree is always a module_t object.
    """
    def __init__(self):
        """Constructor.
        """
        compound.compound_t.__init__(self)
        self.__system_headers = []
    
    def add_system_header( self, header ):
        normalize = include_directories.include_directories_t.normalize
        self.__system_headers.append( normalize( header ) )

    def is_system_header( self, header ):
        normalize = include_directories.include_directories_t.normalize
        return normalize( header ) in self.__system_headers
    
    def _get_include_dirs(self):
        include_dirs = algorithm.creator_finder.find_by_class_instance( 
            what=include_directories.include_directories_t
            , where=self.creators
            , recursive=False)        
        if 0 == len( include_dirs ):
            include_dirs = include_directories.include_directories_t()
            if self.license:
                self.adopt_creator( include_dirs, 1 )
            else:
                self.adopt_creator( include_dirs, 0 )
            return include_dirs
        elif 1 == len( include_dirs ):
            return include_dirs[0]
        else: 
            assert not "only single instance of include_directories_t should exist"
            
    def _get_std_directories(self):
        include_dirs = self._get_include_dirs()
        return include_dirs.std
    std_directories = property( _get_std_directories )

    def _get_user_defined_directories(self):
        include_dirs = self._get_include_dirs()
        return include_dirs.user_defined
    user_defined_directories = property( _get_user_defined_directories )

    def _get_body(self):
        found = algorithm.creator_finder.find_by_class_instance( what=module_body.module_body_t
                                                                 , where=self.creators
                                                                 , recursive=False )
        if not found:
            return None
        else:
            return found[0]
    body = property( _get_body,
                     doc="""A module_body_t object or None.
                     @type: L{module_body_t}
                     """
                     )

    def _get_license( self ):
        if isinstance( self.creators[0], license.license_t ):
            return self.creators[0]
        return None
    
    def _set_license( self, license_text ):
        if not isinstance( license_text, license.license_t ):
            license_inst = license.license_t( license_text )
        if isinstance( self.creators[0], license.license_t ):
            self.remove_creator( self.creators[0] )
        self.adopt_creator( license_inst, 0 )
    license = property( _get_license, _set_license,
                        doc="""License text.

                        The license text will always be the first children node.
                        @type: str or L{license_t}""")

    def last_include_index(self):
        """Return the children index of the last L{include_t} object.

        An exception is raised when there is no include_t object among
        the children creators.

        @returns: Children index
        @rtype: int
        """
        for i in range( len(self.creators) - 1, -1, -1 ):
            if isinstance( self.creators[i], include.include_t ):
                return i
        else:
            return 0
        
    def first_include_index(self):
        """Return the children index of the first L{include_t} object.

        An exception is raised when there is no include_t object among
        the children creators.

        @returns: Children index
        @rtype: int
        """
        for i in range( len(self.creators) ):
            if isinstance( self.creators[i], include.include_t ):
                return i
        else:
            raise RuntimeError( "include_t creator has not been found." )
    
    def replace_included_headers( self, headers, leave_system_headers=True ):
        to_be_removed = []
        for creator in self.creators:
            if isinstance( creator, include.include_t ):
                to_be_removed.append( creator )
            elif isinstance( creator, module_body.module_body_t ):
                break
        
        for creator in to_be_removed:
            if creator.header in self.__system_headers: 
                if not leave_system_headers:
                    self.remove_creator( creator )
            elif creator.is_user_defined:
                pass
            else:
                self.remove_creator( creator )
        map( lambda header: self.adopt_include( include.include_t( header=header ) )
             , headers )

    
    def adopt_include(self, include_creator):
        """Insert an L{include_t} object.

        The include creator is inserted right after the last include file.

        @param include_creator: Include creator object
        @type include_creator: L{include_t}
        """
        self.adopt_creator( include_creator, self.last_include_index() + 1 )

    def do_include_dirs_optimization(self):
        include_dirs = self._get_include_dirs()
        includes = filter( lambda creator: isinstance( creator, include.include_t )
                           , self.creators )
        for include_creator in includes:
            include_creator.include_dirs_optimization = include_dirs
    
    def _create_impl(self):
        self.do_include_dirs_optimization()
        index = 0
        includes = []
        for index in range( len( self.creators ) ):
            if not isinstance( self.creators[index], include.include_t ):
                break
            else:
                includes.append( self.creators[index].create() )
        code = compound.compound_t.create_internal_code( self.creators[index:] )
        code = self.unindent(code)        
        return os.linesep.join( includes ) + 2 * os.linesep + code + os.linesep
    
    def add_include( self, header ):
        self.adopt_include( include.include_t( header=header, user_defined=True ) )
    
    def add_namespace_usage( self, namespace_name ):
        self.adopt_creator( namespace.namespace_using_t( namespace_name )
                            , self.last_include_index() + 1 )

    def add_namespace_alias( self, alias, full_namespace_name ):
        self.adopt_creator( namespace.namespace_alias_t( 
                                alias=alias
                                , full_namespace_name=full_namespace_name )
                            , self.last_include_index() + 1 )

    def adopt_declaration_creator( self, creator ):
        self.adopt_creator( creator, self.creators.index( self.body ) )

    def add_declaration_code( self, code, position ):
        creator = custom.custom_text_t( code )
        last_include = self.last_include_index()
        pos = max( last_include + 1, position )
        pos = min( pos, self.creators.index( self.body ) )
        self.adopt_creator( creator, pos )
        
        
        
        
        
        
        
        
        
        

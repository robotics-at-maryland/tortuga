from pygccxml import parser
from pygccxml import declarations
from pyplusplus import module_builder

from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies
from pygccxml import declarations
from pyplusplus import decl_wrappers

#
# class to add properties to ogre.  It follows a 'simple' set of rules
# 1. Find all functions that have a standard set of prefixes as defined in Py++
#      Currently this list is 'get', 'is'
# 2. remove the prefix
# 3. Do a case insensitive search on the non-prefixed name against all other functions 
#         ie. if original function is getPosition
#         We check 'position' against all other (lowercased) functions
#         so if 'position', or 'Position', or 'posItion' already exists we DO NOT add the property
#       This is intentionally agressive in NOT adding properties as we don't want any confusion :)
# 4. If there is NOT a conflict we can add a property



class ogre_property_recognizer_t( decl_wrappers.name_based_recognizer_t ):
    def __init__( self ):
        decl_wrappers.name_based_recognizer_t.__init__( self )
    
    def create_read_only_property( self, fget ):
        found = self.find_out_ro_prefixes( fget.name )
        if found in ( None, '' ):
            return None

        pure_name = fget.name[len( found ):]
        pure_name = pure_name.lower()

        setters = self.class_accessors( fget.parent )[1] 
        setters.extend( self.inherited_accessors( fget.parent )[1] )
        
        setters = filter( lambda decl: decl.name.lower() == pure_name and self.is_setter( decl )
                          , setters )
        if setters:
            #we found member function that could serve as setter. Py++ did not find it
            #so there is a difference in cases.
            return None
        return decl_wrappers.property_t( self.find_out_property_name( fget, found ), fget )

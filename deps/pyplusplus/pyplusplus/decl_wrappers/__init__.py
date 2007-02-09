# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""Declaration decorators.

This sub-package contains the Py++ specific declaration objects
that are the nodes of the declaration tree. In addition to the
interface of the declarations in the pygccxml package the objects in
this package also provide a I{decoration} interface. This interface
allows customizing the bindings and influences the code creators that
have to be generated in subsequent steps.

Each node is derived from its corresponding node in the pygccxml
package and from a decorator base class.

You may encounter the following objects in a declaration tree:

  - L{namespace_t}
  - L{typedef_t}
  - L{variable_t}
  - L{enumeration_t}
  - L{casting_operator_t}
  - L{free_function_t}
  - L{free_operator_t}
  - L{class_declaration_t}
  - L{class_t}
  - L{constructor_t}
  - L{destructor_t}
  - L{member_function_t}
  - L{member_operator_t}

"""

import algorithm

from decl_wrapper import decl_wrapper_t

from calldef_wrapper import calldef_t
from calldef_wrapper import member_function_t
from calldef_wrapper import constructor_t
from calldef_wrapper import destructor_t
from calldef_wrapper import member_operator_t
from calldef_wrapper import casting_operator_t
from calldef_wrapper import free_function_t
from calldef_wrapper import free_operator_t

from class_wrapper import class_declaration_t
from class_wrapper import class_t

from enumeration_wrapper import enumeration_t

from namespace_wrapper import namespace_t

from typedef_wrapper import typedef_t

from variable_wrapper import variable_t

from scopedef_wrapper import scopedef_t

from pygccxml import declarations

from call_policies import call_policy_t
from call_policies import default_call_policies_t
from call_policies import default_call_policies
from call_policies import compound_policy_t
from call_policies import return_argument_t
from call_policies import return_arg
from call_policies import return_self
from call_policies import return_internal_reference_t
from call_policies import return_internal_reference
from call_policies import with_custodian_and_ward_t
from call_policies import with_custodian_and_ward
from call_policies import with_custodian_and_ward_postcall_t
from call_policies import with_custodian_and_ward_postcall
from call_policies import return_value_policy_t
from call_policies import copy_const_reference
from call_policies import copy_non_const_reference
from call_policies import manage_new_object
from call_policies import reference_existing_object
from call_policies import return_by_value
from call_policies import return_opaque_pointer
from call_policies import return_value_policy
from call_policies import return_pointee_value
from call_policies import is_return_opaque_pointer_policy
from call_policies import is_return_pointee_value_policy
from call_policies import custom_call_policies_t
from call_policies import custom_call_policies
from call_policies import convert_array_to_tuple_t
from call_policies import convert_array_to_tuple
from call_policies import memory_managers

from decl_wrapper_printer import decl_wrapper_printer_t
from decl_wrapper_printer import print_declarations

from user_text import user_text_t
from user_text import class_user_text_t

from indexing_suite1 import indexing_suite1_t
from indexing_suite2 import indexing_suite2_t

from doc_extractor import doc_extractor_i

from properties import property_t
from properties import property_recognizer_i
from properties import name_based_recognizer_t

import python_traits

class dwfactory_t( declarations.decl_factory_t ):
    """
    declarations factory class
    """
    def __init__(self):
        declarations.decl_factory_t.__init__(self)

    def create_member_function( self, *arguments, **keywords ):
        return member_function_t(*arguments, **keywords)

    def create_constructor( self, *arguments, **keywords ):
        return constructor_t(*arguments, **keywords)

    def create_destructor( self, *arguments, **keywords ):
        return destructor_t(*arguments, **keywords)

    def create_member_operator( self, *arguments, **keywords ):
        return member_operator_t(*arguments, **keywords)

    def create_casting_operator( self, *arguments, **keywords ):
        return casting_operator_t(*arguments, **keywords)

    def create_free_function( self, *arguments, **keywords ):
        return free_function_t(*arguments, **keywords)

    def create_free_operator( self, *arguments, **keywords ):
        return free_operator_t(*arguments, **keywords)

    def create_class_declaration(self, *arguments, **keywords ):
        return class_declaration_t(*arguments, **keywords)

    def create_class( self, *arguments, **keywords ):
        return class_t(*arguments, **keywords)

    def create_enumeration( self, *arguments, **keywords ):
        return enumeration_t(*arguments, **keywords)

    def create_namespace( self, *arguments, **keywords ):
        return namespace_t(*arguments, **keywords)

    def create_typedef( self, *arguments, **keywords ):
        return typedef_t(*arguments, **keywords)

    def create_variable( self, *arguments, **keywords ):
        return variable_t(*arguments, **keywords)

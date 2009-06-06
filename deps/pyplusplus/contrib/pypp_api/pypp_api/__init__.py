# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""This is an experimental module to high-level API testing.

This module provides a simplified high-level API for using
pygccxml and pyplusplus.  It is currently under heavy construction.

Quickstart Usage
================

 1. Create an instance of the L{ModuleBuilder} class that serves as main
    entry point for controlling pygccxml and pyplusplus. Parameters for
    configuring pygccxml/pyplusplus can be passed in the constructor::

      from pypp_api import *
   
      mb = ModuleBuilder(...)

 2. Parse the header files using L{parse()<ModuleBuilder.parse>} and
    customize your bindings::

      toplevel_namespace = mb.parse()

      MyClass = toplevel_namespace.Class("MyClass")
      MyClass.expose()

      MyClass.Method("foo").setPolicy(return_internal_reference())
      ...

 3. Write the C++ source files using
    L{writeModule()<ModuleBuilder.writeModule>}::

      mb.writeModule()


Detailed Usage
==============

Add detailed usage here.

@todo: Add quickstart and detailed documentation for API.
@author: Allen Bierbaum
@author: Matthias Baas
@license: Boost Software License, Version 1.0 (see http://www.boost.org/LICENSE_1_0.txt)
@copyright: 2006 Allen Bierbaum, Matthias Baas
"""


# Bring in call policies to use
from pyplusplus.decl_wrappers.call_policies import *
#from pyplusplus.decl_wrappers import return_self
#from pyplusplus.decl_wrappers import return_internal_reference
#from pyplusplus.decl_wrappers import with_custodian_and_ward
#from pyplusplus.decl_wrappers import copy_const_reference
#from pyplusplus.decl_wrappers import copy_non_const_reference
#from pyplusplus.decl_wrappers import manage_new_object
#from pyplusplus.decl_wrappers import reference_existing_object
#from pyplusplus.decl_wrappers import return_by_value
#from pyplusplus.decl_wrappers import return_opaque_pointer
#from pyplusplus.decl_wrappers import return_value_policy

from pygccxml.declarations import ACCESS_TYPES
PUBLIC = ACCESS_TYPES.PUBLIC
PROTECTED = ACCESS_TYPES.PROTECTED
PRIVATE = ACCESS_TYPES.PRIVATE

from decltypes import *
#from argpolicy import *
from pyplusplus.function_transformers.transformers import output_t as Output
from pyplusplus.function_transformers.transformers import input_t as Input
from pyplusplus.function_transformers.transformers import inout_t as InOut
from pyplusplus.function_transformers.transformers import input_array_t as InputArray
from pyplusplus.function_transformers.transformers import output_array_t as OutputArray

from modulebuilder import ModuleBuilder

import extendcreators

# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Authors: 
#   Allen Bierbaum
#

# Dictionary of this module. Useful for adding symbols
mod_dict = globals()

# Bring in the module builder and alias it
import pyplusplus.module_builder
ModuleBuilder = pyplusplus.module_builder.module_builder_t
set_logger_level = pyplusplus.module_builder.set_logger_level

# Bring in all call policy symbols
from pyplusplus.module_builder.call_policies import *

from pyplusplus.decl_wrappers import print_declarations


# Type traits
# - just import them all.  This isn't pretty, but it will work for now
from pygccxml.declarations.type_traits import *

# cpptypes
# - import them all and leave them named X_t because they are "types" and
#   this seems like a good way to keep that in mind.
#   This may end up being a bad idea.  I don't know yet, so for now we will 
#   try it and see what happens.
from pygccxml.declarations.cpptypes import *
from pygccxml.declarations.calldef import *

# Matchers
# - Bring in all matchers but rename then without the '_t' at the end
import pygccxml.declarations.matchers
#for n in ["matcher_base_t","or_matcher_t","and_matcher_t","not_matcher_t",
#          "declaration_matcher_t","calldef_matcher_t","namespace_matcher_t",
#          "variable_matcher_t","regex_matcher_t","access_type_matcher_t",
#          "operator_matcher_t","custom_matcher_t","virtuality_type_matcher_t"]:
#   mod_dict[n[:-2]] = pygccxml.declarations.matchers.__dict__[n]

from pygccxml.declarations import (or_matcher, and_matcher, not_matcher, declaration_matcher,
                                   calldef_matcher, namespace_matcher, variable_matcher,
                                   regex_matcher, access_type_matcher, operator_matcher,
                                   custom_matcher, virtuality_type_matcher)

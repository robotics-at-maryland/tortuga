# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Library Imports
from pygccxml import declarations
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

# Project Imports
import buildfiles.wrap as wrap
from buildfiles.wrap import make_already_exposed

def generate(module_builder, local_ns, global_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    global_ns.exclude()
    classes = []

    # Wrap Events
    EventType = local_ns.class_('EventType')
    EventType.include()
    classes.append(EventType)

    # Append the approaite include files
#    for cls in classes:
#        cls.include()
    wrap.add_needed_includes(classes)

    include_files = set([cls.location.file_name for cls in classes])
    for cls in classes:
        include_files.update(cls.include_files)
    return list(include_files)

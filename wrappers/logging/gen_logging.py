# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# STD Imports
import pprint

# Library Imports
from pyplusplus import module_builder
mod_builder = module_builder
from pyplusplus.module_builder import call_policies
from pygccxml import declarations

# Project Imports
import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):
    """
    local_ns: is the namespace that coresponds to the given namespace
    global_ns: is the module builder for the entire library
    """
    global_ns.exclude()
    classes = []

    # Mark class from other modules as already exposed
    module_builder.class_('::ram::core::Subsystem').already_exposed = True

    # EventPlayer
    EventPlayer = local_ns.class_('EventPlayer')
    EventPlayer.include()
    classes.append(EventPlayer)

    events = wrap.expose_events(local_ns)

    if events:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])
        classes += events

    # Do class wide items
    wrap.add_needed_includes(classes)

    # Add registrations functions for hand wrapped classes
    module_builder.add_registration_code("registerLoggingPtrs();")

    include_files = set([cls.location.file_name for cls in classes])
    for cls in classes:
        include_files.update(cls.include_files)
    return ['wrappers/logging/include/RegisterFunctions.h'] + list(include_files)

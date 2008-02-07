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
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    global_ns.exclude()
    classes = []

    # Lets Py++ know to make VisionSystem a subclass of Subsystem
    wrap.make_already_exposed(global_ns, 'ram::core', ['Subsystem'])

    # Vision System
    VisionSystem = local_ns.class_('VisionSystem')
    VisionSystem.include()
    classes.append(VisionSystem)   
#    IController.include_files.append(os.environ['RAM_SVN_DIR'] +
#                                     '/packages/control/include/IController.h')

    # Wrap Events
    eventsFound = False
    for cls in local_ns.classes(function= lambda x: x.name.endswith('Event'),
                                allow_empty = True):
        cls.include()
        classes.append(cls)
    ImageEvent = local_ns.class_('ImageEvent')
    ImageEvent.include_files.append('vision/include/Image.h')
    wrap.set_implicit_conversions([ImageEvent], False)

    if eventsFound:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])

    # Append the approaite include files
    wrap.add_needed_includes(classes)

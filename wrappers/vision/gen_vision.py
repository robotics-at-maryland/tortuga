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
    wrap.make_already_exposed(global_ns, 'ram::core', ['Subsystem', 'Event'])

    # Vision System
    VisionSystem = local_ns.class_('VisionSystem')
    VisionSystem.include()
    VisionSystem.include_files.append('vision/include/Camera.h')
    classes.append(VisionSystem)   

    # Wrap Events
    EventType = local_ns.class_('EventType')
    EventType.include()
    classes.append(EventType)

    Suit = local_ns.class_('Suit')
    Suit.include()
    classes.append(Suit)
    
    eventsFound = False
    for cls in local_ns.classes(function= lambda x: x.name.endswith('Event'),
                                allow_empty = True):
        cls.include()
        classes.append(cls)

    ImageEvent = local_ns.class_('ImageEvent')
    ImageEvent.include_files.append('vision/include/Image.h')
    wrap.set_implicit_conversions([ImageEvent], False)

    BinEvent = local_ns.class_('BinEvent')
    wrap.set_implicit_conversions([BinEvent], False)

    if eventsFound:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])

    # Append the approaite include files
#    for cls in classes:
#        cls.include()
    wrap.add_needed_includes(classes)

    module_builder.add_registration_code("registerImageClass();")
    module_builder.add_registration_code("registerCameraClass();")
    return ['wrappers/vision/include/RegisterFunctions.h']

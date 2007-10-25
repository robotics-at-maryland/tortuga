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
import buildfiles.wrap
from buildfiles.wrap import make_already_exposed

def generate_control(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    make_already_exposed(global_ns, 'ram::pattern', 'Subject')
    make_already_exposed(global_ns, 'ram::core', ['IUpdatable'])
#    make_already_exposed(global_ns, 'ram::math', ['Vector3', 'Quaternion'])
    make_already_exposed(global_ns, 'ram::core', ['ConfigNode', 'IUpdatable',
                                                  'Updatable'])

    
    # Include controller classes
    IController = local_ns.class_('IController')
    IController.include()

    BWPDController = local_ns.class_('BWPDController')
    BWPDController.include()
    BWPDController.include_files.append( "vehicle/include/Vehicle.h" )


def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'control' : generate_control})


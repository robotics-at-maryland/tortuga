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
import wrap
from wrap import make_already_exposed

def generate_vision(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Base classes
    Camera = local_ns.class_('Camera')
    Camera.include()
    Camera.member_function('getImage').exclude()
    Camera.member_function('waitForImage').exclude()
    Camera.member_function('capturedImage').exclude()
    
    OpenCVCamera = local_ns.class_('OpenCVCamera')
    OpenCVCamera.include()
#    OpenCVCamera.member_function('getImage').exclude()
#    OpenCVCamera.member_function('waitForImage').exclude()


    # Pull in decector classes
    local_ns.class_('GateDetector').include()
    local_ns.class_('BinDetector').include()
    local_ns.class_('RedLightDetector').include()
    local_ns.class_('OrangePipeDetector').include()



def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vision' : generate_vision})

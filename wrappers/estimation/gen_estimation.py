# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# STD Imports
import os

# Library Imports
from pyplusplus import module_builder
mod_builder = module_builder
from pyplusplus.module_builder import call_policies

# Project Imports
import buildfiles.wrap as wrap

def export_typedef(ns, typedef):
    cls = ns.typedef(typedef).type.declaration
    cls.rename(typedef)
    cls.include()
    return cls

def generate(module_builder, local_ns, global_ns):
    """
    local_ns: is the namespace that coresponds to the given namespace
    global_ns: is the module builder for the entire library
    """
    classes = []

    local_ns.exclude()

    # Mark class from other modules as already exposed
    module_builder.class_('::ram::core::Subsystem').already_exposed = True
    module_builder.class_('::ram::math::Vector3').already_exposed = True
    module_builder.class_('::ram::math::Vector2').already_exposed = True
    Quaternion = module_builder.class_('::ram::math::Quaternion')
    Quaternion.already_exposed = True
    Quaternion.constructors().allow_implicit_conversion = False

    # Include obstacle class
    Obstacle = local_ns.class_('Obstacle')
    Obstacle.include()

    Obstacle.include_files.append(os.environ['RAM_SVN_DIR'] +
                                  '/packages/estimation/include/Obstacle.h')

    # Include state estimator class
    IStateEstimator = local_ns.class_('IStateEstimator')
    IStateEstimator.include()

    IStateEstimator.include_files.append(os.environ['RAM_SVN_DIR'] +
                                         '/packages/estimation/include/IStateEstimator.h')
    classes.append(IStateEstimator)

    wrap.registerSubsystemConverter(IStateEstimator)

    module_builder.add_registration_code("registerIStateEstimatorPtrs();")

    include_files = set([cls.location.file_name for cls in classes])
    for cls in classes:
        include_files.update(cls.include_files)
    return ['wrappers/estimation/include/RegisterFunctions.h'] + list(include_files)

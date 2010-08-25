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

    local_ns.exclude()
    classes = []

    # Mark class from other modules as already exposed
    module_builder.class_('::ram::core::Subsystem').already_esposed = True

    # Include state estimator class
    IStateEstimator = local_ns.class_('IStateEstimator')
    IStateEstimator.include()

    IStateEstimator.include_files.append(os.environ['RAM_SVN_DIR'] +
                                         '/packages/estimation/include/IStateEstimator.h')

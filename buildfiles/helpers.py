# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/helpers.py

# STD Imports
import os
import glob as _glob

# Build System imports
import libs

def glob(env, path, pattern):
    """
    Returns the list of paths relative to the current SConscript directory
    in the given path, with the given pattern.
    """
    base_dir = os.path.dirname(env.GetBuildPath('SConscript'))
    directory = os.path.join(base_dir, path)

    results = []
    for p in _glob.glob(directory + '/' + pattern):
        results.append(p.replace(base_dir + '/', ''))
        
    return results

def add_int_deps(env, int_deps):
    """
    Add internal dependencies 
    """
    if type(int_deps) is not list:
        int_deps = [int_deps]
    for dep in int_deps:
        libs.add_internal(env, dep)

def SharedLibrary(env, name, *args, **kwargs):
    """
    Simple helper function to all easier building of shared libraries that
    integrate with the rest of the build system.
    """

    # Setup environment to build library based on info in libs.py
    my_lib = libs._get_internal_lib(name)
    my_lib.setup_environment(env, building_self = True)

    target_name = 'ram_' + name
    lib = env.SharedLibrary(target = target_name, *args, **kwargs)
    env.Install(dir = env['LIB_DIR'], source = lib)

def Program(env, *args, **kwargs):
    """
    Repleces env.Program allow automatic inclusinon of settings from dependency
    libraries, and makes program depend on them being installed.
    """

    # Make sure settings for dependent libaries is included
    int_deps = kwargs.get('int_deps', [])
    add_int_deps(env, int_deps)
    
    prog = env.Program(*args, **kwargs)

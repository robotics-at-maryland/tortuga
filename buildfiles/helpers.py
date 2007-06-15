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
    
def SharedLibrary(env, name, *args, **kwargs):
    """
    Simple helper function to all easier building of shared libraries that
    integrate with the rest of the build system.
    """
    target_name = 'ram_' + name
    lib = env.SharedLibrary(target = target_name, *args, **kwargs)
    env.Install(dir = env['LIB_DIR'], source = lib)

def Program(env, *args, **kwargs):
    """
    Repleces env.Program allow automatic inclusinon of settings from dependency
    libraries, and makes program depend on them being installed.
    """
    int_deps = kwargs.get('int_deps', [])
    # Make sure settings for dependent libaries is included
    if type(int_deps) is not list:
        int_deps = [int_deps]
    for dep in int_deps:
        libs.add_internal(env, dep)
    
    prog = env.Program(*args, **kwargs)

    for dep in int_deps:
        env.Depends(dep, env['LIB_DIR'] + '/libram_' + dep + '.so')

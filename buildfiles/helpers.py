# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/helpers.py

# STD Imports
import os
import sys
import glob as _glob
import subprocess

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

def _ensure_list(possible_list):
    if type(possible_list) is not list:
        possible_list = [possible_list]
    return possible_list

def add_int_deps(env, int_deps):
    """
    Add internal dependencies 
    """
    for dep in _ensure_list(int_deps):
        libs.add_internal(env, dep)

def add_ext_deps(env, ext_deps):
    """
    Add internal dependencies 
    """
    for dep in _ensure_list(ext_deps):
        libs.add_external(env, dep)

# Keeps track of created libraries to provide better warnings
CREATED_LIBRARIES = set()

def SharedLibrary(env, name, *args, **kwargs):
    """
    Simple helper function to all easier building of shared libraries that
    integrate with the rest of the build system.
    """

    global CREATED_LIBRARIES
    if name in CREATED_LIBRARIES:
        print 'Already created library: "%s"' % name
        print 'Please fix: %s', env.GetBuildPath('SConscript')
        sys.exit(1)
    else:
        CREATED_LIBRARIES.add(name)

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
    add_int_deps(env, kwargs.get('int_deps', []))
    add_ext_deps(env, kwargs.get('ext_deps', []))
    
    return env.Program(*args, **kwargs)

def Tests(env, _target, _source, **kwargs):
    # Add 'UnitTest++' to the list of ext_deps
    ext_deps = _ensure_list(kwargs.get('ext_deps', []))
    ext_deps.append('UnitTest++')
    kwargs['ext_deps'] = ext_deps

    # Allow the user to over ride the test name and/or sources
    if _target is None:
        _target = ['test/Tests']
    if _source is None:
        _source = glob(env, 'test/src', '*.cxx')

    prog = Program(env, target = _target, source = _source, **kwargs)

    def run_test(env, target, source):
        if not subprocess.call(str(source[0].abspath)):
            open(str(target[0]), 'w').write("PASSED\n")

    env.Command(_target[0] + '.successful', prog, run_test)

def add_helpers_to_env(env):
    env['BUILDERS']['RAMSharedLibrary'] = SharedLibrary
    env['BUILDERS']['RAMProgram'] = Program
    env['BUILDERS']['Tests'] = Tests

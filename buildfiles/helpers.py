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
import SCons

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
        results.append(p.replace(base_dir + os.path.sep, ''))

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

def SharedLibrary(env, name, _source, **kwargs):
    """
    Simple helper function to all easier building of shared libraries that
    integrate with the rest of the build system.
    """
    
    if type(name) is list:
        if len(name) != 1:
            print 'Library name cannot be a list'
            print 'Please fix: %s', env.GetBuildPath('SConscript')
            sys.exit(1)
        name = name[0]
    
    # Keep track off all created libraries so we don't make the same one twice
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

    # Needed for DLL symbol export on windows
    env.AppendUnique(CPPDEFINES = ['RAM_PKG_' + name.upper()])
    
    target_name = 'ram_' + name
    lib = env.SharedLibrary(target = target_name, source = _source, **kwargs)
    env.Install(dir = env['LIB_DIR'], source = lib)
    
    return lib

def Program(env, *args, **kwargs):
    """
    Repleces env.Program allow automatic inclusinon of settings from dependency
    libraries, and makes program depend on them being installed.
    """

    # Make sure settings for dependent libaries is included
    add_int_deps(env, kwargs.get('int_deps', []))
    add_ext_deps(env, kwargs.get('ext_deps', []))
    
    return env.Program(*args, **kwargs)

def run_tests(env, output, inputs, message = None, deps = None):    
    def run_test_imp(env, target, source):
        """
        Runs both C++ tests (executables) and Python unittest.TestCase's located
        in the given modules

        @rtype : int
        @return: A 0 on success and 1 on failure
        """
        
        # Split tests by type 
        pytests = []
        cpptests = []
        for f in source:
            fpath = f.abspath
            if fpath.endswith('.py'):
                pytests.append(fpath)
            elif os.name == 'posix' and 0 == fpath.count('.'):
                cpptests.append(fpath)
            elif os.name != 'posix' and fpath.endswith('.exe'):
                cpptests.append(fpath)
            else:
                pass
                #raise Exception('Coult not determine type of test file: ' + fpath)

        # Run the C++ Testsls
        for cpptest in cpptests:
            result = subprocess.call(str(cpptest))
            if result:
                return 1 # Failure

        # Run Python Tests
        if len(pytests) > 0:
            testerpath = os.path.join(os.environ['RAM_SVN_DIR'], 'scripts',
                                      'pytester.py')
            tests = '"' + '" "'.join(pytests) + '"'
            cmd_str = '%s "%s" %s' % (sys.executable, testerpath, tests)
            result = subprocess.call(cmd_str, shell = True)
            if result:
                return 1 # Failure

        # Record the test success to the file
        open(str(target[0]), 'w').write("PASSED\n")

        # Success
        return 0

    msg = 'Runnting Tests'
    if not message is None:
        msg = message
    if not deps is None:
        inputs.extend(deps)
    return env.Command(output, inputs,
                       SCons.Action.Action(run_test_imp, msg))

def Tests(env, _target, _source, run = True, **kwargs):
    # Add 'UnitTest++' to the list of ext_deps
    ext_deps = _ensure_list(kwargs.get('ext_deps', []))
    ext_deps.append('UnitTest++')
    kwargs['ext_deps'] = ext_deps

    # Allow the user to over ride the test name and/or sources
    if _target is None:
        _target = ['test/Tests']
    if _source is None:
        _source = glob(env, 'test/src', '*.cxx')

    # Create the Test Program
    prog = Program(env, target = _target, source = _source, **kwargs)

    # Gather Up the C++ and Python based tests
    tests = [prog]
    root = os.path.dirname(env.GetBuildPath('SConscript'))
    pytests = glob(env, 'test/src', '*.py')
    for pytest in pytests:
        # Need the full path here for python's 'imp' module later
        tests.append(os.path.join(root, pytest))
    
    if run:
        cmd = run_tests(env, _target[0] + '.successful', tests,
                        'Running Tests in: ' + \
                        os.path.dirname(env.GetBuildPath('SConscript')))

        return (prog, cmd)
    
    else:
        def test_runner(env, output, message = None, deps = None):
            return run_tests(env, output, tests, message, deps)
        
        return (prog, test_runner)
    

def add_helpers_to_env(env):
    env['BUILDERS']['RAMSharedLibrary'] = SharedLibrary
    env['BUILDERS']['RAMProgram'] = Program
    env['BUILDERS']['Tests'] = Tests
    from SCons.Script.SConscript import SConsEnvironment # just do this once
    SConsEnvironment.Glob = glob

def setup_printing(env):
    def print_cmd_line(s, target, src, env):
        if env['verbose']:
            sys.stdout.write("%s\n"%s);
        else:
            sys.stdout.write("Making: %s...\n" %(' and '.join([str(x) for x in
                                                             target])))
            # Save real cmd to log file
            open(env['CMD_LOGFILE'], 'a').write("%s\n"%s)

          
    env['PRINT_CMD_LINE_FUNC'] = print_cmd_line
    env['CMD_LOGFILE'] = 'build.log'  


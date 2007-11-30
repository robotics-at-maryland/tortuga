# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tools/pypp.py

"""
This module contains a custom Py++ tool for SCons
"""

# STD Imports
import os
import sys
import os.path
import types
import shutil
import subprocess

# Library Imports
import SCons.Builder
import SCons.Tool
import SCons.Defaults

def get_from_env(env, name):
    if not env.has_key(name):
        raise Exception("Must give the Py++ helper a '%s'" % name)
    return env[name]
        
def build_module(env, target, source): #, actual_target = None):
    if type(source) is types.ListType:
        if len(source) > 1:
            raise Exception("Can't have more than one generated source list")
        source = source[0]

    target_base = get_from_env(env, 'target_base')
    tester = get_from_env(env, 'tester')
    extra_sources = get_from_env(env, 'extra_sources')
    dep_wrappers = get_from_env(env, 'dep_wrappers')


    # All paths in the file are relative to the directory of the source list 
    srclist_dir = os.path.join(os.path.split(source.abspath)[0], 'generated')

    # Parse the generated source list file
    srclist = open(source.abspath, 'r')
    src_dir = os.path.split(source.path)[0]

    # For some reason SCons doesn't want me to have source files in the build
    # directory, this hask the node to just tell everyone that is where it lives
    def hack_srcnode(node):
        node.actual_srcnode = node.srcnode

        def hacked_srcnode():
            return node
        node.srcnode = hacked_srcnode
        return node

    filelines = srclist.readlines()
    sources = [env.File(srclist_dir + '/' + f.strip())
               for f in filelines]

    # Copy in the extra sources into the generated directory
    for f in extra_sources:
        # Create the new name for the file
        dir, fname = os.path.split(f)
        newName = os.path.join(srclist_dir, fname)

        shutil.copy(f, newName)

        # Add it to our list of sources
        sources.append(env.File(newName))


    sources = [hack_srcnode(s) for s in sources]

    target_dir = os.path.join('build_ext','ext')
    target_name = target_dir + '/' + target_base
    
    suffix = '.pyd'
    if os.name == 'posix':
        suffix = '.so'
    else:
        env.AppendUnique(CCFLAGS = ['/wd4244'])
    extension_mod = env.SharedLibrary(target_name, sources, SHLIBPREFIX='',
                                      SHLIBSUFFIX = suffix)

                                      
    if os.name != 'posix':
        env.AddPostAction(extension_mod, 
        'mt.exe -nologo -manifest /outputresource:"$TARGET;#2" /manifest "${TARGET}.manifest"')
    # Run the tests
    if not tester is None:
        output = os.path.join(target_dir, target_base + 'Tests.success')
        test = tester(env, output, deps = [target_name + suffix])

        # Make the tests dependent on the right modules, otherwise the modules
        # will try and modules that have not been build yet
        for dep in dep_wrappers:
            env.Depends(test, os.environ['RAM_SVN_DIR'] + '/build_ext/ext/' +
                        dep + suffix)

    # Setup init file
    if not os.path.exists(target_dir):
        env.Execute(SCons.Defaults.Mkdir(target_dir))
    
    init_file = os.path.join(target_dir, '__init__.py')
    if not os.path.exists(init_file):
        env.Execute(SCons.Defaults.Touch(init_file))

    # Set depedencies and install
    env.Depends(extension_mod, '#' + target_base + '_wrapper')
    env.Alias('TopLevelAlias', extension_mod)

def run_pypp(env, target, source, module, tester = None, extra_sources = None,
             dep_wrappers = None):
    """
    The root Py++ builder
    
    @param target: the output library name
    
    @param source: the xml files to process
    
    @param env: the construction environment
    
    @param module: the python module to call to generate the cpp code
    
    @param tester: Generates the scons Command to call the test programs
    
    @param extra_sources: Extra sources that you wish to have compiled into the
                          wrapping module

    @param dep_wrappers: The wrappers modules which have to be compiled before
                         this one.
    """

    if extra_sources is None:
        extra_sources = []

    if dep_wrappers is None:
        dep_wrappers = []

    # Add CPPPATH to XMLCPPPATH (so GCC-XML can find our headers)
    #print 'Path',env['CPPPATH']
    env.AppendUnique(XMLCPPPATH = [env['CPPPATH']])
    #print 'XMLPath',env['XMLCPPPATH']
    #print 'TEST',env['_CPPINCFLAGS']

    # Build XML files
    xmlfiles = []
    for f in source:
        # Remove directory and extension from the filename
        target_file = os.path.splitext(os.path.split(f)[1])[0]

        # Build the file and add the output to our results
        xmlfiles.extend(env.XMLHeader(target = target_file, source = f))

    # Runs the code generation module using Py++
    sources = xmlfiles
    sources.append(env.File(os.path.join(
        os.path.dirname(env.GetBuildPath('SConscript')), module)))

    target_str = str(target[0]).replace("::","_")

    # Generate the command line call to our py++ script
    slist = '"' + '" "'.join([s.abspath for s in sources]) + '"'
    prog_path = os.path.join(os.environ['RAM_SVN_DIR'], 'scripts', 'pypp.py')
    outfile_path = env.File(target_str + '_gen-sources.txt').abspath
    cmd_str = '%s "%s" -t "%s" -m "%s" %s' % (sys.executable, prog_path,
                                        outfile_path, target_str, slist)
    commands = [cmd_str]

    # The command to create the output file directory
    dir_path = 'generated' + '-'.join(str(target[0]).split('::')[1:])
    dir_path = env.Dir(dir_path).abspath
    if not os.path.exists(dir_path):
        commands.insert(0, SCons.Defaults.Mkdir(dir_path))
    
    
    srclist = env.Command(target_str + '_gen-sources.txt',
                          sources, commands)

    # The special builder for our module
    # Needed to handled nested module case
    target_str = target_str.replace("::","_") 
    dummy = env.PyppHelper('#' + target_str + '_wrapper', srclist,
                           target_base = target_str,
                           tester = tester,
                           extra_sources = extra_sources,
                           dep_wrappers = dep_wrappers)
    env.AlwaysBuild(dummy)
    env.Alias('TopLevelAlias', dummy)
    
def check_modules(env):
    try:
        import pygccxml
        import pyplusplus
    except ImportError, e:
        mod = e.message.split(' ')[-1]
        print 'Could not import %s, please make sure its installed' % mod
        print 'and on your PYTHONPATH'
        env.Exit(1)



PyppBuilderHelper = SCons.Builder.Builder(action = build_module);
        
def generate(env):
    check_modules(env)
        
    env.Append(BUILDERS = {'Pypp' : run_pypp,
                           'PyppHelper' : PyppBuilderHelper})

def exists(env):
    here = True
    try:
        import pygccxml
        import pyplusplus
    except ImportError:
        here = False

    return here

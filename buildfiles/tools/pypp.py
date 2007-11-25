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
import imp
import os
import os.path
import types
import logging
import shutil

# Library Imports
import SCons.Builder
import SCons.Tool
import SCons.Defaults

def generate_code_base(env, target, source, module):
    """
    The builder which actually calls Py++.
    
    @param target: A file which lists the generated sources
    @param source: A list of input xml files and *one* python file.
                   The python file will be imported as module, and given the
                   list of xmlfiles, and an output dir.
    """

    # Seperate out the modules and xmlfiles
    mod_path = None
    xmlfiles = []
    for s in source:
        if s.abspath.endswith('.py'):
            if not mod_path is None:
                print "Error only one python module can be used to generate"
                print "code"
                env.Exit(1)
            mod_path = s.abspath
        else:
            xmlfiles.append(s.abspath)
    
    # Import module
    (search_path, name) = os.path.split(mod_path)
    # Stip '.py' from the end
    name = name[:-3]
    (f, pathname, decs) = imp.find_module(name, [search_path])
    mod = imp.load_module(name, f, pathname, decs)

    # Tramsform files into so that pygccxml now they are already xml
    import pygccxml
    import pyplusplus
    from pyplusplus import module_builder

    # Turn off logging (for normal builds)
#    pygccxml.utils.loggers.gccxml.setLevel(logging.ERROR)
    pyplusplus._logging_.loggers.module_builder.setLevel(logging.ERROR)
    pyplusplus._logging_.loggers.declarations.setLevel(logging.ERROR)
    
    xmlfiles = [pygccxml.parser.create_gccxml_fc(f) for f in xmlfiles]
    mb = module_builder.module_builder_t(files = xmlfiles,
                                         indexing_suite_version = 2)

    # Exclude everything by default, then include just the classes we want
    mb.global_ns.exclude()
    local_ns = mb.global_ns
    for ns in module.replace("::","_").split("_"):
        local_ns = local_ns.namespace(ns)
    
    # Call entry point
    include_files = mod.generate(local_ns, mb.global_ns)
    if include_files is None:
        include_files = []

    # Now lets build the code
    mb.build_code_creator( module_name= module.replace("::","_"))
    for include in include_files:
        mb.code_creator.add_include(include)

    # And finally we can write code to the disk
    (output_dir, name) = os.path.split(target[0].abspath)
    output_dir = os.path.join(output_dir, 'generated')
    files_created = mb.split_module(output_dir,
                                    on_unused_file_found = lambda x: x)

    # Create list of files
    cpp_files = [os.path.split(f)[1] for f in files_created
                 if f.endswith('.cpp')]
    output_file = open(target[0].abspath, 'w')
    for cpp_file in cpp_files:
        output_file.write(cpp_file + '\n')
    output_file.close()

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

    sources = [env.File(srclist_dir + '/' + f.strip())
               for f in srclist.readlines()]

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
    
    suffix = '.dll'
    if os.name == 'posix':
        suffix = '.so'

    extension_mod = env.SharedLibrary(target_name, sources, SHLIBPREFIX='',
                                      SHLIBSUFFIX = suffix)

    # Run the tests
    if not tester is None:
        output = os.path.join(target_dir, target_base + 'Tests.success')
        test = tester(env, output, deps = [target_name + '.so'])

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
    env.AppendUnique(XMLCPPPATH = [env['CPPPATH']])

    # Build XML files
    xmlfiles = []
    for f in source:
        # Remove directory and extension from the filename
        target_file = os.path.splitext(os.path.split(f)[1])[0]

        # Build the file and add the output to our results
        xmlfiles.extend(env.XMLHeader(target = target_file, source = f))

    # Runs the code generation module using Py++
    sources = xmlfiles
    sources.append(env.File(module))

    target_str = str(target[0])

    def generate_code(env, target, source):
        generate_code_base(env, target, source, target_str)
    commands = [generate_code]
    if not os.path.exists(env.Dir('generated').abspath):
        commands.insert(0, SCons.Defaults.Mkdir(env.Dir('generated').abspath))
    
    
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

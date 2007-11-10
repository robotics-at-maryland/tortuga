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
import os.path
import types
import logging

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
    local_ns = mb.global_ns.namespace(module)
    
    # Call entry point
    mod.generate(local_ns, mb.global_ns)

    # Now lets build the code
    mb.build_code_creator( module_name= module)

    # And finally we can write code to the disk
    (output_dir, name) = os.path.split(target[0].abspath)
    output_dir = os.path.join(output_dir, 'generated')
    files_created = mb.split_module(output_dir)

    # Create list of files
    cpp_files = [os.path.split(f)[1] for f in files_created
                 if f.endswith('.cpp')]
    output_file = open(target[0].abspath, 'w')
    for cpp_file in cpp_files:
        output_file.write(cpp_file + '\n')
    output_file.close()
        
def build_module(env, target, source): #, actual_target = None):
    if type(source) is types.ListType:
        if len(source) > 1:
            raise Exception("Can't have more than one generated source list")
        source = source[0]

    if not env.has_key('target_base'):
        raise Exception("Must give the Py++ helpder 'build_module' a target'")
    target_base = env['target_base']

    if not env.has_key('tester'):
        raise Exception("Must give the Py++ helpder 'tester' a target'")
    tester = env['tester']

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

    # Add extra depends
#    for s in sources:
#        env.Depends(source, s)

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
        tester(env, output, deps = [target_name + '.so'])

    # Setup init file
    if not os.path.exists(target_dir):
        env.Execute(SCons.Defaults.Mkdir(target_dir))
    
    init_file = os.path.join(target_dir, '__init__.py')
    if not os.path.exists(init_file):
        env.Execute(SCons.Defaults.Touch(init_file))

    # Set depedencies and install
    env.Depends(extension_mod, '#' + target_base + '_wrapper')
    env.Alias('TopLevelAlias', extension_mod)

def run_pypp(env, target, source, module, tester = None):
    """
    The root Py++ builder
    
    @param target: the output library name
    @param source: the xml files to process
    @param env: the construction environment
    @param module: the python module to call to generate the cpp code
    """

    # Add CPPPATH to XMLCPPPATH (so GCC-XML can find our headers)
    env.AppendUnique(XMLCPPPATH = [env['CPPPATH']])

    # Build XML files
    xmlfiles = []
    for f in source:
        #print 'START',f
        # Remove directory and extension from the filename
        target_file = os.path.splitext(os.path.split(f)[1])[0]
        #print 'TARGET',target_file
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
    
    
    srclist = env.Command('generated-sources.txt', sources, commands)

    # The special builder for our module
    dummy = env.PyppHelper('#' + target_str + '_wrapper', srclist,
                           target_base = target_str,
                           tester = tester)
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

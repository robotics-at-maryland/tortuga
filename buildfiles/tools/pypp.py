# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tools/pypp.py

"""
This module contains a custom Py++ tool for SCons
"""

import os.path
import types

import SCons.Builder
import SCons.Tool
import SCons.Defaults

def generate_code(env, target, source):
    """
    The builder which actually calls Py++.
    
    @param target: A file which lists the generated sources
    @param source: A list of input xml files and *one* python file.
                   The python file will be imported as module, and given the
                   list of xmlfiles, and an output dir.
    """

    # Seperate out the modules and xmlfiles
    mod = None
    xmlfiles = []
    for s in source:
        if s.abspath.endswith('.py'):
            if not mod is None:
                print "Error only one python module can be used to generate"
                print "code"
                env.Exit(1)
            mod = s.abspath
        else:
            xmlfiles.append(s.abspath)

    print 'Module', mod
    print 'XMLFiles', xmlfiles
    print 'Target', target[0].abspath

    output_dir = os.path.split(target[0].abspath)[0]
    output_dir = os.path.join(output_dir, 'generated')

    f = open(target[0].abspath, 'w')
    f.write("Bob.cpp\nJohn.cpp")
    f.close()

    print 'Output',os.path.join(output_dir,"Bob.cpp")
    f = open(os.path.join(output_dir,"Bob.cpp"), "w")
    f.write("int test(int a, int b) { return a + b; }\n")
    f.close();

    f = open(os.path.join(output_dir, "John.cpp"), "w")
    f.write("int test2(int a, int b) { return a - b; }\n")
    f.close();
    
    # Import module
    #mod = __import__(source.abspath)

    # Create our module builder
    # not done yet...
    #if isinstance(files[0], SCons.Node.FS.File):
    #    files = [f.abspath for f in files]
    #xml_files = [parser.create_gccxml_fc(f) for f in files]
    #mb = module_builder.module_builder_t(files = xml_files,
    #                                     indexing_suite_version = 2)
    # Call entry point
    #mod.generate(target.abspath, mb)

def build_module(env, target, source): #, actual_target = None):
    if type(source) is types.ListType:
        if len(source) > 1:
            raise Exception("Can't have more than one generated source list")
        source = source[0]

    if not env.has_key('target_base'):
        raise Exception("Must give the Py++ helpder 'build_module' a target'")
    target_base = env['target_base']

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
    for s in sources:
        env.Depends(source, s)

    sources = [hack_srcnode(s) for s in sources]


    src_dir = os.path.split(source.abspath)[0]
    print 'BLD DIR',src_dir

    envc = env.Copy()

    # Another hack needed, SCons seems to have lost track of the build dir so
    # we hack it in here
    envc['SHLINKCOM'] = env['SHLINKCOM'].replace('$TARGET', src_dir +'/$TARGET')
    extension_mod = envc.SharedLibrary(target_base + '_ext', sources)

    # Set depedencies here
    envc.Depends(extension_mod, '#' + target_base + '_dummy')
    envc.Alias('TopLevelAlias', extension_mod)

def run_pypp(env, target, source, module):
    """
    The root Py++ builder
    
    @param target: the output library name
    @param source: the xml files to process
    @param env: the construction environment
    @param module: the python module to call to generate the cpp code
    """

#    if not env.has_key('module'):
#        raise Exception("Must give the Pypp build a python 'module' which generates code")
#    module = env['module']
    # Add CPPPATH to XMLCPPPATH (so GCC-XML can find our headers)
    env.AppendUnique(XMLCPPPATH = [env['CPPPATH']])

    #print 'XML BUILD'
    #print env.Dir('.').abspath
    # Build XML files
    xmlfiles = []
    for f in source:
        #print 'START',f
        # Remove directory and extension from the filename
        target_file = os.path.splitext(os.path.split(f)[1])[0]
        #print 'TARGET',target_file
        # Build the file and add the output to our results
        xmlfiles.extend(env.XMLHeader(target = target_file, source = f))


    #print 'xmlfiles', [f.abspath for f in xmlfiles]
#    print 'Path', env.Dir('.')
#    module = env.File(module)
#    print 'MOD', module.abspath
    # Runs the code generation module using Py++
    sources = xmlfiles
    sources.append(env.File(module))
    #print 'sources', sources
    #srclist_file = os.path.join('generated', 'sources.txt')
#    print 'DIR',env.Dir('generated')
    commands = [generate_code]
    print 'Checking for:',env.Dir('generated').abspath
    if not os.path.exists(env.Dir('generated').abspath):
        print 'Not found'
        commands.insert(0, SCons.Defaults.Mkdir(env.Dir('generated').abspath))
    
    
    srclist = env.Command('generated-sources.txt', sources, commands)

    target_str = str(target[0])
    # The special builder for our module
    dummy = env.PyppHelper('#' + target_str + '_dummy', srclist,
                           target_base = target_str)
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

#PyppBuilder = SCons.Builder.Builder(action = run_pypp);
PyppBuilderHelper = SCons.Builder.Builder(action = build_module);

def test_build2(env, target, source):
    print 'TEST_DIR2',env.Dir('.').abspath

def test_build(env, target, source, module):
    print 'TEST_DIR',env.Dir('.').abspath
#    env.Command(SCons.Defaults.Touch('bob')
    SCons.Defaults.Touch(env.File('jhon').abspath)
    env.PyppHelper('bob','jhon')
        
def generate(env):
    check_modules(env)
        
    # Added the builder to the given environment
#    env.Append(BUILDERS = {'Pypp' : test_build,
#                           'PyppHelper' : SCons.Builder.Builder(action = test_build2)})
    env.Append(BUILDERS = {'Pypp' : run_pypp,#PyppBuilder,
                           'PyppHelper' : PyppBuilderHelper})


def exists(env):
    here = True
    try:
        import pygccxml
        import pyplusplus
    except ImportError:
        here = False

    return here

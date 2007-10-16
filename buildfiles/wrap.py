# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/wrap.py

# Python Imports
import os
import sys
import distutils.sysconfig

# Library Imports
import SCons
from pyplusplus import module_builder
from pygccxml import parser
from pygccxml import declarations as decls_package

# Build System Imports
import libs

def make_already_exposed(global_ns, namespace_name, classes,
                         class_decs = None):
    if type(classes) is not list:
        classes = [classes]

    if class_decs is None:
        class_decs = []

    ns = global_ns
    for name in namespace_name.split('::'):
        print 'Going into namespace',name
        ns = ns.namespace(name)

    for class_name in classes:
        print 'Marking class',class_name
        class_ = ns.class_(class_name)
        class_.include()
        class_.already_exposed = True

    for class_dec in class_decs:
        print 'Marking class dec',class_dec
        class_dec = ns.decl(name = 'Vehicle' ,
                            decl_type = declarations.class_declaration_t)
        class_dec.already_exposed = True

def add_needed_includes(decl):
    includes = set()

    try:
        for cls in decl.decls(decl_type=decls_package.class_t, recurisve = True):
            filename = cls.location.file_name
            print 'INCLUDE',filename
            if filename.startswith(os.environ['RAM_SVN_DIR']):
                includes.add(filename)

        # Add needed includes
        print 'Adding includes',includes
        cls.include_files.extend(includes)
        
    except RuntimeError:
        pass

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes, module_map, insert_map = {}):
    
    # If we calling from SCons we get file node objects, so we have to translate
    # them to strings
    if isinstance(files[0], SCons.Node.FS.File):
        files = [f.abspath for f in files]
    xml_files = [parser.create_gccxml_fc(f) for f in files]

    mb = module_builder.module_builder_t(files = xml_files,
                                     #    cache = 'test.cache',
                                         indexing_suite_version = 2)
    mb.logger.info('Wrapping header in "%s" namespace' % module_name)

    # Exclude everything by default, then include just the namespace we want
    mb.global_ns.exclude()

    # Call the sub function for our partical namespace
    local_ns = mb.global_ns
    print 'Finding local namespace for:', module_name
    for name in module_name.split('::'):
        local_ns = local_ns.namespace(name)

    # Call proper functions
    module_map[module_name](module_name, mb.global_ns, local_ns)

    # Now lets build the code
    # The leading '_' is a slight hack, but its needed now
    mb.build_code_creator( module_name= module_name.replace('::','_'))

    #I don't want absolute includes within code
    mb.code_creator.user_defined_directories.append( os.path.abspath('.') )

    # This has to be done because py++ is not smart about includes and doesn't
    # autotmatically include the needed header file for each class!
    for include in include_files:
        mb.code_creator.add_include(include)
    for include in extra_includes:
        mb.code_creator.add_include(include)

    # The insert code step
    if insert_map.has_key(module_name):
        insert_map[module_name](mb)

    #And finally we can write code to the disk
    return mb.split_module(output_dir)

def get_modules(mod_name, base_files):
    """
    From a given group of base files finds the actual modules you have to
    create, this assumes namespaces = directories.

    Returns a tuple: (short_name, full_name, base_files)

    The base_files are just filtered versions fo the base_files
    """
    mod_roots = set()

    # Get all the unique directories the headers are in
    for f in base_files:
        root = os.path.split(f)[0]
 #       print 'file: %s root: %s' %(f, root)
        # Ignore the root directory
        if len(root) != 0:
            mod_roots.add(root)

#    print 'Roots',mod_roots
    results = []
    for root in mod_roots:
        # Gather are files that are in this directory
        files = [f for f in base_files if f.startswith(root)]

        # Replace the '/' in the path to create a full name
        full_name = mod_name + '::' + root.replace(os.sep, '::')
        split_name = full_name.split('::')
        short_name = split_name[len(split_name) - 1]
        results.append((short_name, full_name, files))

    # Add the root module with only the files in that module
    root_files = [f for f in base_files if len(f.split(os.sep)) == 1]

    if len(root_files) > 0:
        results.append((mod_name, mod_name, root_files))

    return results

class Generator(object):
    def __init__(self, gen_mod, mod_name, output_dir, headers,
                 extra_includes = []):
        self.gen_mod = gen_mod
        self.mod_name = mod_name
        self.includes = headers
        self.output_dir = output_dir
        self.extra_includes = extra_includes

    def __call__(self, target = None, source = None, env = None):
        self.gen_mod.generate_code(self.mod_name, source,
                                   os.path.abspath(self.output_dir),
                                   self.includes, self.extra_includes)

def setup_environ(env):
    """
    Setup our custom extension buidling environment (will be replaced
     with simpler code later)
    """
    envw = env.Clone(tools = ['gccxml'])
    envw.AppendUnique(XMLCPPPATH = [env['CPPPATH']])

    libs.add_external(envw, 'Boost.Python')
    
    return envw

def import_wrapping_mod(gen_mod, src_dir):
    # Make a copy of the path
    sys_path = [p for p in sys.path]

    # Add the source directory to the front of the path
    if sys.path.count(src_dir) == 0:
        sys.path.insert(0, src_dir)

    # Import out moduel
    mod = __import__(gen_mod)

    # Restore the path
    sys.path = sys_path

    return mod

# Prevent warning about mutliple installs
EXT_DIR_INSTALLED = False

def wrap_headers(env, mod_name, headers,
                 namespace_prefix = '',
                 output_root = 'generated',
                 extra_includes = [],
                 install_dir = 'lib/ext',
                 gen_mod = 'generate_code',
                 header_dir_name = 'include',
                 header_suffix = '.h'):
    if type(headers) is not list:
        headers = [headers]

    src_dir = os.path.dirname(env.GetBuildPath('SConscript'))
    build_dir = env.Dir('.').abspath

    # Pull in GCC-XML, and the Boost.Python + Python library
    envw = setup_environ(env)

    base_files = []
    xml_files = []
    # Build XML reprsentation of headers
    for header in headers:
        target = header_dir_name + header.split(header_dir_name)[1]
        
        # Remove .h/.hpp
        target = os.path.splitext(target)[0]
        # Remove leading 'include' (header_dir_name) if there is any
        if target.startswith(header_dir_name):
            target = target[len(header_dir_name) + len(os.sep):]

        base_files.append(target)
        t = os.path.join(output_root, target)
        s = os.path.join(os.environ['RAM_SVN_DIR'],header)
        print 'XML: %s Header: %s' % (t,s)
        xml_files.append(envw.XMLHeader(target = t,
                                        source = s))

    
    # Import out module which generates the code
#    generate_module = import_wrapping_mod(gen_mod, src_dir)

#    mod_infos = get_modules(mod_name, base_files)

    #from pprint import pprint
    #pprint(mod_infos)

    # Strip leading output dir
#    for short_name, full_dot_name, bfiles in mod_infos:
#        name = '_'.join(full_dot_name.split('::'))
#        output_path = os.path.join(str(os.sep).join(full_dot_name.split('::')[1:]))
#        output_dir = os.path.join(output_root, output_path)
#        
        # Make an educated guess at the output files
#        output_files = []
#        include_files = []
#        xml_files = []
#      for f in bfiles:
            # Py++ generates a .pypp.cpp/hpp for every class, we assume one
            # class per header
#            output_files.append(os.path.join(output_root, f + '.pypp.cpp'))
#            output_files.append(os.path.join(output_root, f + '.pypp.hpp'))

            # Rebuild original include directories for that directory
#            include_files.append(os.path.join(header_dir_name,
#                                              f + header_suffix))
#            xml_files.append(os.path.join(output_root, f + '.xml'))
                                 
#        output_files.append(os.path.join(output_dir, name + '.main.cpp'))

        #print 'XML     :',xml_files
        #print 'Output  :',output_files
        #print 'Includes:',include_files
        
#        dot_name = full_dot_name
#        if len(namespace_prefix) > 0:
#            dot_name = namespace_prefix + '::' + dot_name
                                                        
        #print 'Root',output_root
        #print 'Output dir', output_dir
#        gen = Generator(generate_module, dot_name,
#                        os.path.join(build_dir, output_dir), headers,
#                        extra_includes)
        # Generate a custom action which calls the right generate_code
        #print 'Output dir',output_dir
#        envw.Command(output_files, xml_files, SCons.Action.Action(gen))
        
        # Setup extra depends with the generate_code module
#        envw.Depends(output_files, gen_mod + '.py')
        
        # Create the shared library (This doesn't handle extra classes)
#        src_set = set([f for f in output_files if f.endswith('.cpp')])
#        if os.path.exists(output_dir):
#            [src_set.add(os.path.join(output_dir, f)) for f in
#             os.listdir(output_dir) if f.endswith('.cpp')]
#        sources = [i for i in src_set]

#        envw.AppendUnique(CPPPATH = [os.environ['RAM_SVN_DIR']])

        # Remove -Wall if present
#        flags = envw['CCFLAGS']
#        if flags.count('-Wall') > 0:
#            flags.remove('-Wall')
#            envw.Replace(CCFLAGS = flags)
            
#        lib = envw.SharedLibrary(target = os.path.join(output_dir, name),
#                                 source = sources, SHLIBPREFIX = '')

        # Create init files for importing
#        install_dir = os.path.join(env['BUILD_DIR'], install_dir)
#        init_file = os.path.join(install_dir, '__init__.py')
        
#        global EXT_DIR_INSTALLED
#        if EXT_DIR_INSTALLED:
#            envw.Command(install_dir, '', SCons.Defaults.Mkdir(install_dir))
#            envw.Command(init_file, '', SCons.Defaults.Touch(init_file))
#            EXT_DIR_INSTALLED = True

        # Create the extra depends on the output files and generate code files
        # Setup the install command
#        lib_name = os.path.basename(str(lib[0]))
#        envw.InstallAs(target = os.path.join(install_dir, lib_name),
#                       source = lib)

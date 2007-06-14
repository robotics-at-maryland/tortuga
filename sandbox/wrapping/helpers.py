import os
import sys
import distutils.sysconfig

import SCons
from pyplusplus import module_builder
from pygccxml import parser


def generate_code(module_name, files, output_dir, include_files,
                  module_map):
    
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
    def __init__(self, gen_mod, mod_name, output_dir, headers):
        self.gen_mod = gen_mod
        self.mod_name = mod_name
        self.includes = headers
        self.output_dir = output_dir

    def __call__(self, target = None, source = None, env = None):
        self.gen_mod.generate_code(self.mod_name, source,
                                   os.path.abspath(self.output_dir),
                                   self.includes)

def setup_environ(env):
    """
    Setup our custom extension buidling environment (will be replaced
     with simpler code later)
    """
    envw = env.Clone()

    # Setup paths for Boost.Python and and Python
    root_dir = os.environ['RAM_ROOT_DIR']
    envw.Append(CPPPATH = [os.path.join(root_dir,'include','boost-1_35'),
                          distutils.sysconfig.get_python_inc(),
                          '.'])
    envw.Append(LIBPATH = [os.path.join(root_dir,'lib'), '.'])
    
    # Add out extra libraries (and some python ldflags)
    envw.Append(LIBS = ['boost_python-gcc'])
    envw.MergeFlags(['!python-config --ldflags'])

    # Now some flags for python
    envw.Append(CFLAGS = ['-fno-strict-aliasing'])
    envw.Append(CPPDEFINES = ['NDEBUG'])

    return envw

def wrap_headers(env, mod_name, headers,
                 namespace_prefix = '',
                 output_root = 'wrappers',
                 install_dir = 'lib/ext',
                 gen_mod = 'generate_code',
                 header_dir_name = 'include',
                 header_suffix = '.h'):

    #namespace_prefix = namespace_prefix + '::'

    base_files = []
    # Build XML reprsentation of headers
    for header in headers:
        # Remove .h/.hpp
        target = os.path.splitext(header)[0]
        # Remove leading 'include' (header_dir_name) if there is any
        if header.startswith(header_dir_name):
            target = target[len(header_dir_name) + len(os.sep):]

        base_files.append(target)
        env.XMLHeader(target = os.path.join(output_root, target),
                      source = header)

    # TODO: switch me to be build system based
    envw = setup_environ(env)

    # Import out module which generates the code
    generate_module = __import__(gen_mod)

#    print 'Base Files:',base_files

    mod_infos = get_modules(mod_name, base_files)

#    print 'Mod infos:',mod_infos
    # Strip leading output dir
    for short_name, full_dot_name, bfiles in mod_infos:
        name = '_'.join(full_dot_name.split('::'))
        output_path = os.path.join(str(os.sep).join(full_dot_name.split('::')[1:]))
        output_dir = os.path.join(output_root, output_path)
        
        #print 'Name',name
        # Make an educated guess at the output files
        output_files = []
        include_files = []
        xml_files = []
        for f in bfiles:
            # Py++ generates a .pypp.cpp/hpp for every class, we assume one
            # class per header
            output_files.append(os.path.join(output_root, f + '.pypp.cpp'))
            output_files.append(os.path.join(output_root, f + '.pypp.hpp'))

            # Rebuild original include directories for that directory
            include_files.append(os.path.join(header_dir_name,
                                              f + header_suffix))
            xml_files.append(os.path.join(output_root, f + '.xml'))
                                 
        output_files.append(os.path.join(output_dir, name + '.main.cpp'))

        #print 'XML',xml_files
        #print 'Output',output_files
        #print 'Includes',include_files
        
        dot_name = full_dot_name
        if len(namespace_prefix) > 0:
            dot_name = namespace_prefix + '::' + dot_name
      
                                  
                                                  
        #print 'Root',output_root
        gen = Generator(generate_module,
                        dot_name,
                        output_dir,
                        headers)
        # Generate a custom action which calls the right generate_code
        #print 'Output dir',output_dir
        envw.Command(output_files, xml_files, SCons.Action.Action(gen))
        
        # Setup extra depends with the generate_code module
        envw.Depends(output_files, gen_mod + '.py')
        
        # Create the shared library (This doesn't handle extra classes)
        src_set = set([f for f in output_files if f.endswith('.cpp')])
        if os.path.exists(output_dir):
            [src_set.add(os.path.join(output_dir, f)) for f in
             os.listdir(output_dir) if f.endswith('.cpp')]
        sources = [i for i in src_set]

        lib = envw.SharedLibrary(target = os.path.join(output_dir, name),
                                 source = sources, SHLIBPREFIX = '')

        # Create init files for importing
        init_file = os.path.join(install_dir, '__init__.py')
        envw.Command(install_dir, '', SCons.Defaults.Mkdir(install_dir))
        envw.Command(init_file, '', SCons.Defaults.Touch(init_file))

        # Now the one for our module
        install_path = os.path.join(install_dir,
                                    str(os.sep).join(full_dot_name.split('::')))

        init_file = os.path.join(install_path, '__init__.py')
        envw.Command(install_path, '', SCons.Defaults.Mkdir(install_path))
        envw.Command(init_file, '', SCons.Defaults.Touch(init_file))
        # Create the extra depends on the output files and generate code files
        # Setup the install command
        lib_name = os.path.basename(str(lib[0]))
        envw.InstallAs(target = os.path.join(install_path, lib_name),
                       source = lib)
                                             

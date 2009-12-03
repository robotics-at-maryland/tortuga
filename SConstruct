# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  SConstruct

EnsureSConsVersion(0, 96, 93)

# STD Imports
import os
import os.path
import sys
import platform
import subprocess

# Build system imports
import buildfiles.helpers as helpers
import buildfiles.platfrm as platfrm
import buildfiles.features as features
import buildfiles.variants as variants

# Ensure we are using the proper version of python
import ram_version_check


# --------------------------------------------------------------------------- #
#                              O P T I O N S                                  #
# --------------------------------------------------------------------------- #

# Options either come from command line of config file
opts = Options('options.py')

# Platform independent options
opts.AddOptions(
     BoolOption('check',
                'Runs checks on dependent libraries to ensure a proper installation',
                True),
     BoolOption('verbose',
                "Shows full command line build, normally recored to 'build.log'",
                False),
     ListOption('with_features', 'This list of features to build', 'all',
                features.available()),
     ListOption('without_features',
                'This list of features to exclude from the  build', 'none',
                features.available()),
     EnumOption('variant', 'The aviable variants to build', 'release',
                allowed_values = variants.available()),
     PathOption('build_dir', 'Path to place build products in', 'build',
                PathOption.PathIsDirCreate),
     BoolOption('bfin', "Build for the Blackfin", False)
     )

# Platform specific options
if os.name == 'posix':
    opts.AddOptions(
        ('CC', 'The C compiler to use','gcc'),
        ('CXX', 'The C++ compiler to use', 'g++'))


# --------------------------------------------------------------------------- #
#                           E N V I R O N M E N T                             #
# --------------------------------------------------------------------------- #

# Setup the build environment
tpath =  os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles', 'tools')

env = Environment(ENV = os.environ,
                  options = opts,
                  tools = ['default', 'gccxml', 'pypp'],
                  toolpath = [tpath])

if env['bfin']:
    env.Replace(CC =
                '/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-gcc')
    env.Replace(CXX = 
                '/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-g++')
    env.Replace(AS = 
                '/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-as')

Help(opts.GenerateHelpText(env))

# Add out helper functions to the environment
# Ex: env.Tests(), env.RAMProgram(), env.RAMLibrary()
helpers.add_helpers_to_env(env)
helpers.setup_printing(env)


# --------------------------------------------------------------------------- #
#                                P A T H S                                    #
# --------------------------------------------------------------------------- #

env.Append(BUILD_DIR = os.path.join(env.Dir('.').abspath, env['build_dir']))
env.Append(PACKAGE_DIR = [os.path.join(env.Dir('.').abspath, 'packages')])
env.Append(LIB_DIR = os.path.join(env['BUILD_DIR'], 'lib'))
env.Append(BIN_DIR = os.path.join(env['BUILD_DIR'], 'bin'))

# Sets platform specific settings, includes LIBPATH and CPPPATH
# See: buildfiles/platform.py for more information
platfrm.setup_environment(env)


# --------------------------------------------------------------------------- #
#                                F L A G S                                    #
# --------------------------------------------------------------------------- #

# Sets CCFLAGS and LDFLAGS based on desired build variant
# (ie 'release', 'debug')
# See: buildfiles/variants.py for more information
variants.setup_environment(env)

# Add to base compiler and linker paths
env.AppendUnique(CPPPATH = [env['PACKAGE_DIR']])
env.AppendUnique(LIBPATH = [env['LIB_DIR'],
                            os.path.join(os.environ['RAM_ROOT_DIR'],'lib')])

# Add flags
# TODO: Factor me out into a variant module 
# (ie, 'debug', 'release', 'release-dbg', 'profile')
if os.name == 'posix':
    env.AppendUnique(CCFLAGS = [#['-g',    # Debugging symbols
                                '-Wall',   # All Warnings
                                '-Werror', # Warnings as Errors
                                '-fmessage-length=0'] # For gathering stats
                     )
    if 'Darwin' != platform.system():
        env.AppendUnique(LINKFLAGS = ['-Wl,-E'])
else:
    env.AppendUnique(CCFLAGS = ['/W3',   # Level 3 out of 4 warnings
                                '/WX',   # Warnings as Errors
                                '/MD',   # Multithreaded runtime
                                '/EHa',  # Structured Exception Handling
                                '/GR'])  # C++ RTTI
    
    # These Warnings are disabled from the command line because they cause 
    # problems with STD headers and are just too pedantic (!)
    # 4820 = Had to add padding to structure/class
    # 4625 = Copy constructor not accessible in base class
    # 4626 = Assignement Operator is not accisble in base class
    # 4710 = inline was requested but not preformed
    # 4512 = assignment operator could not be generated
    # 4127 = conditional expression is constant
    # 4514 = unreferenced inline function has been removed
    # 4100 = unreferenced formal parameter
    # 4255 = no function prototype given: converting '()' to '(void)'
    # 4686 = possible change in behaviours with FOREACH
    # 4251 = needs to have dll interface
    # 4275 = needs to have dll interface
    # 4244 = conversion from one type to another
    # 4121 = alignment issue
    # 4347 = odd template vs. normal function issue
    # 4350 = behaviour change
    # 4263 = hide virtual function in base class
    # 4264 = same as the above
    # 4191 = odd calling convention issue
    # 4996 = fopen issue
    # 4273 = inconsistant dll linkage on windows
    env.AppendUnique(CCFLAGS = ['/wd4820', '/wd4625', '/wd4626', '/wd4710',
                                '/wd4512', '/wd4127', '/wd4640', '/wd4061', 
                                '/wd4514', '/wd4100', '/wd4255', '/wd4686',
                                '/wd4251', '/wd4275', '/wd4244', '/wd4121',
                                '/wd4347', '/wd4350', '/wd4928', '/wd4263',
                                '/wd4264', '/wd4266', '/wd4191', '/wd4996',
                                '/wd4273'])

    
# --------------------------------------------------------------------------- #
#                      C U S T O M   T A R G E T S                            #
# --------------------------------------------------------------------------- #

# Dist clean
def dist_func(target = None, source = None, env = None):
    import shutil
    
    print 'Removing', env['BUILD_DIR']
    shutil.rmtree(env['BUILD_DIR'], True)
    build_ext = os.path.join(env.Dir('.').abspath, 'build_ext')
    print 'Removing', build_ext
    shutil.rmtree(build_ext, True)

# Creat are "phony" distclean target
dist_clean = env.Alias('dist-clean', 'SConstruct', env.Action(dist_func))
env.AlwaysBuild(dist_clean)

# Docs
if 1 == sys.argv.count('docs'):
    doxygen_path = env.WhereIs('doxygen')
    epydoc_path = env.WhereIs('epydoc')
    
    if doxygen_path is None:
        print 'Could not find doxygen, please make sure it is on your PATH'
        env['epydoc_path'] = epydoc_path
    elif epydoc_path is None:
        print 'Could not find epydoc, please make sure it is on your PATH'
        env['doxygen_path'] = doxygen_path
    elif (doxygen_path is None) and (epydoc_path is None):
        print 'Could not find any documentation tools!'
        Exit(1)
    else:
        env['doxygen_path'] = doxygen_path
        env['epydoc_path'] = epydoc_path

        
def docs_func(target = None, source = None, env = None):
    # Config paths
    dox_cfg_path = os.path.join(os.environ['RAM_SVN_DIR'], 'scripts',
                                'doxygen.cfg')
    epy_cfg_path = os.path.join(os.environ['RAM_SVN_DIR'], 'scripts',
                                'epydoc.cfg')

    # Make sure directories exist
    output_dir = os.path.join('docs','api')

    # Run documentation generation
    if env['doxygen_path'] is not None:
        cpp_dir = os.path.join(output_dir,'cpp')
        if not os.path.exists(cpp_dir):
            os.makedirs(cpp_dir)
        
        subprocess.call("%s %s" % (env['doxygen_path'], dox_cfg_path),
                        shell = True)

    if env['epydoc_path'] is not None:
        epy_dir = os.path.join(output_dir,'python')
        if not os.path.exists(epy_dir):
            os.makedirs(epy_dir)

        # Small hack to make sure ogre can be imported by epydoc
        import ram.sim
        
        # Another small hack to run the command program without our environment
        # so we can setup the python path properly
        origArgv = sys.argv
        sys.argv = ['', '--config', epy_cfg_path]
        
        # Call into epydoc API
        import epydoc
        import epydoc.cli
        epydoc.cli.cli()

        # Restore args
        sys.argv = origArgv

# Creat are "phony" distclean target
make_docs = env.Alias('docs', 'SConstruct', env.Action(docs_func))
env.AlwaysBuild(make_docs)

# --------------------------------------------------------------------------- #
#                              B U I L D                                      #
# --------------------------------------------------------------------------- #

# Determines which directories to process and build
# See: buildfiles/features.py for how 
features.setup_environment(env)

def has_help():
    """
    Helper function to determine whether or not the user has asked for help

    This returns true if they asked for build system specific help, or SCons
    help.
    """
    if sys.argv.count('--help') or sys.argv.count('-h'):
        return True
    return False

# Don't process any directories if we just want the help options
if (not has_help()) and (0 == sys.argv.count('dist-clean')):
    for directory in features.dirs_to_build(env):
        build_dir= os.path.join(env['build_dir'], directory)
        env['ABS_BUILD_DIR'] = os.path.abspath(env['build_dir'])
    
        Export('env')

        print 'Dir:',build_dir
        # Build seperate directories (this calls our file in the sub directory)
        env.SConscript(os.path.join(directory, 'SConscript'), 
                       build_dir = build_dir,
                       duplicate = 0)

    # On Windows generate a solution which runs everything for us
    if env['RAM_OS'] == 'windows':
        env.MSVSSolution(target = 'All' + env['MSVSSOLUTIONSUFFIX'],
                         projects = helpers.MSVS_PROJECTS,
                         variant = [env['VARIANT'].name])

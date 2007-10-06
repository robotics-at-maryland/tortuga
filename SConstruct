# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  SConstruct

import os
import sys

EnsureSConsVersion(0, 96, 93)

# --------------------------------------------------------------------------- #
#                              S E T U P                                      #
# --------------------------------------------------------------------------- #

# Add the buildfiles dir to the path
sys.path.insert(1, os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles'))
import helpers
import platfrm

# Options either come from command line of config file
opts = Options('options.py')
opts.AddOptions(
     BoolOption('check', 'Runs checks on dependent libraries to ensure a proper installation',
      True),
     BoolOption('verbose', "Shows full command line build, normally recored to 'build.log'",
                False)
     )
     
if os.name == 'posix':
    opts.AddOptions(
        ('CC', 'The C compiler to use','gcc'),
        ('CXX', 'The C++ compiler to use', 'g++'))

# Setup the build environment
tpath =  os.path.join(os.environ['RAM_SVN_DIR'],'buildfiles', 'tools')
env = Environment(ENV=os.environ, options=opts, toolpath = [tpath])
Help(opts.GenerateHelpText(env))

# Add platform Specifc setup
platfrm.setup_environment(env)

# Set directories
env.Append(BUILD_DIR = os.path.join(env.Dir('.').abspath, 'build'))
env.Append(PACKAGE_DIR = [os.path.join(env.Dir('.').abspath, 'packages')])
env.Append(LIB_DIR = os.path.join(env['BUILD_DIR'], 'lib'))
env.Append(BIN_DIR = os.path.join(env['BUILD_DIR'], 'bin'))

# Add to base compiler and linker paths
env.AppendUnique(CPPPATH = [env['PACKAGE_DIR']])
if os.name == 'nt':
    env.AppendUnique(CPPPATH = [os.path.join(os.environ['RAM_ROOT_DIR'],'include')])
env.AppendUnique(LIBPATH = [env['LIB_DIR'],
                            os.path.join(os.environ['RAM_ROOT_DIR'],'lib')])

helpers.setup_printing(env)

# Add flags
# TODO: Factor me out into a variant module 
# (ie, 'debug', 'release', 'release-dbg', 'profile')
if os.name == 'posix':
    env.AppendUnique(CCFLAGS = ['-g',       # Debugging symbols
                                '-Wall',    # All Warnings
                                '-Werror']) # Warnings as Errors
else:
    env.AppendUnique(CCFLAGS = ['/Wall', # All Warnings
                                '/WX',   # Warnings as Errors
                                '/MD',   # Multithreaded runtime
                                '/EHa',  # Structured Exception Handling
                                '/GR'])  # C++ RTTI
    
    # These Warnings are disabled from the command line because the cause 
    # problems with STD headers and are just too pedantic (!)
    # 4820 = Had to add pading to structure/class
    # 4625 = Copy constructor not accesible in base class
    # 4626 = Assignement Operator is not accisble in base class
    # 4710 = inline was requested but not preformed
    # 4512 = assignment operator could not be generated
    # 4127 = conditional expression is constant
    env.AppendUnique(CCFLAGS = ['/wd4820', '/wd4625', '/wd4626', '/wd4710',
                                '/wd4512', '/wd4127', '/wd4640', '/wd4061'])
    
# Add out helper functions to the environment
helpers.add_helpers_to_env(env)

# --------------------------------------------------------------------------- #
#                              B U I L D                                      #
# --------------------------------------------------------------------------- #

# Our build subdirectory
buildDir = 'build'

from buildfiles import dirs_to_build
for directory in dirs_to_build.get_dirs():
    Export('env')

    print 'Dir:',os.path.join(buildDir,directory)
    # Build seperate directories (this calls our file in the sub directory)
    env.SConscript(os.path.join(directory, 'SConscript'), 
                   build_dir = os.path.join(buildDir, directory), 
                   duplicate = 0)



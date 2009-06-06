# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/platform.py

"""
Sets the platform specific SCons settings for the build system
"""

import sys
import os
import platform

def setup_environment(env):
    """
    Entry point for the module
    """

    check_supported_os(env)
    add_platform_defines(env)
    add_scons_variables(env)
    add_platform_paths(env)
    
def check_supported_os(env):
    """
    Ensures that we are on a supported platform
    """
    # Check OS Type
    os_type = os.name
    if not (os_type in ['posix', 'nt']):
        print 'ERROR: OS Type "%s" not supported' % os_type
        sys.exit(1)

    # Check OS
    os_kind = platform.system()
    if not (os_kind in ['Linux', 'Darwin', 'Windows', 'Microsoft']):
        print 'ERROR: OS "%s" not supported' % os_kind
        sys.exit(1)

def add_platform_defines(env):
    """
    Adds the definies so we know what platform and OS we are on.
    """
    # Defines Basic OS Type (ie. Posix or NT)
    env.Append(CPPDEFINES = ['RAM_%s' % os.name.upper()])
    
    # Define Specific OS
    system_map = {
        'Linux' : 'Linux',
        'Darwin' : 'Darwin',
        'Windows' : 'Windows',
        'Microsoft' : 'Windows'
        }
    
    env.Append(CPPDEFINES = ['RAM_%s' % system_map[platform.system()].upper()])

def add_scons_variables(env):
    """
    Sets RAM_PLATFORM and RAM_OS scons environment variable
    """
    system_map = {
        'Linux' : 'Linux',
        'Darwin' : 'Darwin',
        'Windows' : 'Windows',
        'Microsoft' : 'Windows'
        }
    
    env['RAM_PLATFORM'] = os.name.lower()
    env['RAM_OS'] = system_map[platform.system()].lower()

def add_platform_paths(env):
    """
    Add LIBPATH and CPPPATH that are dependent on platform
    """

    plat = env['RAM_PLATFORM']

    if 'nt' == os.name:
        sdkDir = r'C:\Program Files\Microsoft Platform SDK for Windows ' + \
                 r'Server 2003 R2'
        
        env.AppendUnique(CPPPATH =
                         [os.path.join(os.environ['RAM_ROOT_DIR'],'include'),
                          sdkDir + r'\Include'])
        
        env.AppendUnique(LIBPATH = [sdkDir + r'\Lib'])
        env.AppendUnique(LIBPATH = [env['LIB_DIR'],
                                    os.path.join(os.environ['RAM_ROOT_DIR'],'lib')])
    

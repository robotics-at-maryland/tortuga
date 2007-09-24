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
    if not (os_kind in ['Linux', 'Darwin', 'Windows']):
        print 'ERROR: OS "%s" not supported' % os_kind
        sys.exit(1)

def add_platform_defines(env):
    """
    Adds the definies so we know what platform and OS we are on.
    """
    # Defines Basic OS Type (ie. Posix or NT)
    env.Append(CPPDEFINES = ['RAM_%s' % os.name.upper()])
    
    # Define Specific OS
    env.Append(CPPDEFINES = ['RAM_%s' % platform.system().upper()])

def add_scons_variables(env):
    env['RAM_PLATFORM'] = os.name.lower()
    env['RAM_OS'] = platform.system().lower()

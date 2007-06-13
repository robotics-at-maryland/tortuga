# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tools/gccxml.py

"""
This module contains a custom GCC-XML tool for SCons
"""

import SCons.Builder
import SCons.Tool

GCCXMLBuilder = Scons.Builder.Builder(action = "$GCCXML $_XML_CPPINCFLAGS $_XML_CPPDEFFLAGS $SOURCE -fxml=$TARGET",
                              suffix='xml',
                              src_suffic = ['h', 'hpp'],
                              source_scanner = SCons.Tool.CScanner)

def generate(env):
    env['GCCXML'] = env.WhereIs('gccxml')

    # These variables hold the expanded form of the include and defines lists
    env['_XML_CPPINCFLAGS'] = '$( ${_concat(INCPREFIX, XMLCPPPATH, INCSUFFIX, __env__, RDirs)} $)'
    env['_XML_CPPDFFFLAGS'] = '${_defines(CPPDEFPREFIX, XML_CPPDEFINES, CPPDEFSUFFIX, __env__)}'

    # Added the builder to the given environment
    env.Append(BUILDERS = {'XMLHeader' : GCCXMLBuilder })

def exists(env):
    return env.Detect('gccxml')

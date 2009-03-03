# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tools/gccxml.py

"""
This module contains a custom GCC-XML tool for SCons
"""

import os
import platform
import SCons.Builder
import SCons.Tool



GCCXMLBuilder = SCons.Builder.Builder(
    action = "$GCCXML $GCCXML_EXTRA_FLAGS $_XML_CPPINCFLAGS $_XML_CPPDEFFLAGS $SOURCE -fxml=$TARGET",
    suffix='xml',
    src_suffic = ['h', 'hpp'],
    source_scanner = SCons.Tool.CScanner)

def generate(env):
    gccxml_path = env.WhereIs('gccxml')
    if gccxml_path is None:
        print 'Could not find gccxml, please make sure it is on your PATH'
        env.Exit(1)
        
    env['GCCXML'] = gccxml_path

    gccxml_dir = os.path.dirname(gccxml_path)
    extra = ''
    if platform.system() == 'Linux':
        extra += '--gccxml-compiler g++-4.2'

    if os.name != 'posix':
        extra = '--gccxml-config "' + os.path.abspath(os.path.join(gccxml_dir, 'gccxml_config')) +'"'
        extra += ' --gccxml-cxxflags " /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MT" '
    env['GCCXML_EXTRA_FLAGS'] = extra
    #env['GCCXML_EXTRA_FLAGS'] = ''
    
    # These variables hold the expanded form of the include and defines lists
    env['_XML_CPPINCFLAGS'] = '$( ${_concat(INCPREFIX, XMLCPPPATH, INCSUFFIX, __env__, RDirs)} $)'
    env['_XML_CPPDEFFLAGS'] = '${_defines(CPPDEFPREFIX, CPPDEFINES, CPPDEFSUFFIX, __env__)}'

    if os.name != 'posix':
        env['GCCXML_INCPREFIX'] = '-I'
        env['GCCXML_CPPDEFPREFIX'] = '-D'
        env['_XML_CPPINCFLAGS'] = '$( ${_concat(GCCXML_INCPREFIX, CPPPATH, INCSUFFIX, __env__, RDirs, TARGET, SOURCE) } $)'
        env['_XML_CPPDEFFLAGS'] = '${_defines(GCCXML_CPPDEFPREFIX, CPPDEFINES, CPPDEFSUFFIX, __env__)}'
    
    # Added the builder to the given environment
    env.Append(BUILDERS = {'XMLHeader' : GCCXMLBuilder })

def exists(env):
    return env.Detect('gccxml')

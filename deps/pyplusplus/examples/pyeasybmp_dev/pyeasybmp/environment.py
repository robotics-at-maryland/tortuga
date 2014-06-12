#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys

class settings:  
    module_name = 'pyeasybmp'
    boost_path = ''
    boost_libs_path = ''
    gccxml_path = ''    
    pygccxml_path = ''
    pyplusplus_path = ''
    easybmp_path = ''
    python_libs_path = ''
    python_include_path = ''
    working_dir = ''
    generated_files_dir = ''
    unittests_dir = ''
    
    @staticmethod
    def setup_environment():
        sys.path.append( settings.pygccxml_path )
        sys.path.append( settings.pyplusplus_path )
   
if sys.platform == 'linux2':
    settings.boost_path = '/home/roman/boost_cvs'
    settings.boost_libs_path = '/home/roman/boost_cvs/bin'
    settings.gccxml_path = '/home/roman/gccxml/bin/gccxml'    
    settings.pygccxml_path = '/home/roman/pygccxml_sources/source'
    settings.pyplusplus_path = '/home/roman/pygccxml_sources/source'
    settings.easybmp_path = '/home/roman/pygccxml_sources/source/pyplusplus/examples/py_easybmp/EasyBMP'
    settings.python_include_path = '/usr/include/python2.3'
    settings.working_dir = '/home/roman/pygccxml_sources/source/pyplusplus/examples/py_easybmp'
    settings.generated_files_dir = '/home/roman/pygccxml_sources/source/pyplusplus/examples/py_easybmp/generated'
    settings.unittests_dir = '/home/roman/pygccxml_sources/source/pyplusplus/examples/py_easybmp/unittests'
elif sys.platform == 'win32':
    settings.boost_path = 'd:/boost_cvs'
    settings.boost_libs_path = 'd:/boost_cvs/bin'
    settings.gccxml_path = 'c:/tools/gcc_xml/bin'    
    settings.pygccxml_path = 'd:/pygccxml_sources/source'
    settings.pyplusplus_path = 'd:/pygccxml_sources/source'
    settings.easybmp_path = 'd:/pygccxml_sources/source/pyplusplus/examples/py_easybmp/EasyBMP'
    settings.python_libs_path = 'c:/python/libs'
    settings.python_include_path = 'c:/python/include'
    settings.working_dir = 'd:/pygccxml_sources/source/pyplusplus/examples/py_easybmp'
    settings.generated_files_dir = 'd:/pygccxml_sources/source/pyplusplus/examples/py_easybmp/generated'
    settings.unittests_dir = 'd:/pygccxml_sources/source/pyplusplus/examples/py_easybmp/unittests'
else:
    raise RuntimeError( 'There is no configuration for "%s" platform.' % sys.platform )

settings.setup_environment()
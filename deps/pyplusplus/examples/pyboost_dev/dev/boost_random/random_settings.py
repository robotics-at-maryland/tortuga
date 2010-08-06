#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys

_script_dir = os.path.abspath( os.getcwd() )
environment_path = os.path.realpath( os.path.join( _script_dir, '..', '..', '..', '..' ) )
sys.path.append( environment_path )

from environment import boost, scons, gccxml, python

module_name = '_random_'
working_dir = _script_dir
generated_files_dir = os.path.join( _script_dir, 'generated' )
generated_files_dir = os.path.join( _script_dir, '..', '..', 'pyboost', 'boost_random', 'generated' )

defined_symbols = [ 'BOOST_NO_INCLASS_MEMBER_INITIALIZATION', 'BOOST_NO_INT64_T' ]
undefined_symbols = [ '__MINGW32__' ]
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

module_name = '_date_time_'
working_dir = _script_dir
generated_files_dir = os.path.join( _script_dir, 'generated' )
generated_files_dir = os.path.join( _script_dir, '..', '..', 'pyboost', 'date_time', 'generated' )

date_time_pypp_include = os.path.join( _script_dir, 'include' )

undefined_symbols = [ '__MINGW32__' ]
defined_symbols = ['BOOST_DATE_TIME_NO_MEMBER_INIT']
if sys.platform == 'win32':
    defined_symbols.extend( [ 'BOOST_DATE_TIME_DYN_LINK' ] )
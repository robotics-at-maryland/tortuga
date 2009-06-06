#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import shutil

def create( source_dir, target_dir ):
    sys.path.append( source_dir )
    environment = __import__( 'environment' )
    files_dir = os.path.join( source_dir, 'unittests' )
    files = ['grayscale.py', 'source.bmp', 'target.bmp', 'LICENSE_1_0.txt' ]
    if 'win32' == sys.platform:
        files.append( 'pyeasybmp.pyd' )
    else:
        files.append( 'pyeasybmp.so' )


    files = map( lambda fname: os.path.join( files_dir, fname ), files )
    if 'win32' == sys.platform:
        files.append( os.path.join( environment.settings.boost_libs_path, 'boost_python.dll' ) )
    else:
        files.append( os.path.join( environment.settings.boost_libs_path, 'libboost_python.so' ) )
        files.append( os.path.join( environment.settings.boost_libs_path, 'libboost_python.so.1.33.1' ) )

    for f in files:
        shutil.copy( f, target_dir )
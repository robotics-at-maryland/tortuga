# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This package contains few classes, that writes L{code_creators.module_t} to files.

Right now 3 strategies were implemented:

    1. All code is written in one file

    2. Classic strategy of deviding classes to files: one class in one header + source files.

        2.1 Huge classes are splitten to few source files.

"""

import types
from writer import writer_t
from single_file import single_file_t
from multiple_files import multiple_files_t
from class_multiple_files import class_multiple_files_t

def has_pypp_extenstion( fname ):
    """returns True if file has Py++ specific extension, otherwise False"""
    for ext in ( multiple_files_t.HEADER_EXT, multiple_files_t.SOURCE_EXT ):
        if fname.endswith( ext ):
            return True
    return False

def write_file( data, file_path ):
    """writes data to file"""
    if isinstance( data, types.StringTypes ):
        writer_t.write_file( data, file_path )
    else:
        sf = single_file_t( data, file_path )
        sf.write()

def write_multiple_files( extmodule, dir_path ):
    """writes extmodule to multiple files"""
    mfs = multiple_files_t( extmodule, dir_path )
    mfs.write()
    return mfs.written_files

def write_class_multiple_files( extmodule, dir_path, huge_classes ):
    """writes extmodule to multiple files and splits huge classes to few source files"""
    mfs = class_multiple_files_t( extmodule, dir_path, huge_classes )
    mfs.write()
    return mfs.written_files

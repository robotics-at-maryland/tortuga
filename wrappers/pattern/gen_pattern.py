# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/gen_pattern.py

import os

def include(decl):
    decl.include()
    decl.include_files.append(decl.location.file_name)
    return decl

def generate(global_ns, local_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    include_base = os.environ['RAM_SVN_DIR'] + '/packages/pattern/include'
    
    Observer = include(local_ns.class_('Observer'))
    Subject = include(local_ns.class_('Subject'))
    CachedObserver = include(local_ns.class_('CachedObserver'))

    Observer.include_files.append(Subject.location.file_name)
    CachedObserver.include_files.append(Subject.location.file_name)
    Subject.include_files.append(Observer.location.file_name)

# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  scripts/pypp.py

# STD Imports
import optparse
import sys
import imp
import os
import logging

# pyplusplus pickles some objects that require a higher recursion limit
# than normal. This is only sometimes needed, but always included to be safe.
sys.setrecursionlimit(1500)

# Library Imports
import pygccxml
import pyplusplus
from pyplusplus import module_builder

def main(argv = None):
    if argv is None:
        argv = sys.argv
           
    parser = optparse.OptionParser()
    parser.add_option('-m', '--module',
                      help = 'Name of python module to produce')
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    parser.add_option('-t', '--target',
                      help = 'Name of generate file record file')

    options, args = parser.parse_args(args = argv[1:])

    mod_path = None
    xmlfiles = []
    for s in args:
        if s.endswith('.py'):
            if not mod_path is None:
                print "Error only one python module can be used to generate"
                print "code"
                return 1
            mod_path = s
        else:
            xmlfiles.append(s)
    assert mod_path is not None, 'Must provide a module path'

    # Define variables for easy access
    target = options.target
    module = options.module

    # Import module
    (search_path, name) = os.path.split(mod_path)
    # Stip '.py' from the end
    name = name[:-3]
    (f, pathname, decs) = imp.find_module(name, [search_path])
    mod = imp.load_module(name, f, pathname, decs)

    # Turn off logging (for normal builds)
    #pygccxml.utils.loggers.gccxml.setLevel(logging.ERROR)
    if not options.verbose:
        pyplusplus._logging_.loggers.module_builder.setLevel(logging.ERROR)
        pyplusplus._logging_.loggers.declarations.setLevel(logging.ERROR)

    # Create the module builder
    cache_file = pygccxml.parser.file_cache_t(target)
    xmlfiles = [pygccxml.parser.create_gccxml_fc(f) for f in xmlfiles]
    mb = module_builder.module_builder_t(files = xmlfiles,
                                         indexing_suite_version = 2,
                                         cache = cache_file)

    # Exclude everything by default, then include just the classes we want
    mb.global_ns.exclude()
    local_ns = mb.global_ns
    for ns in module.replace("::","_").strip("_").split("_"):
        local_ns = local_ns.namespace(ns)
    
    # Call entry point
    include_files = mod.generate(mb, local_ns, mb.global_ns)
    if include_files is None:
        include_files = []

    # Now lets build the code
    mb.build_code_creator( module_name= module.replace("::","_"))
    for include in include_files:
        mb.code_creator.add_include(include)

    # And finally we can write code to the disk
    (output_dir, name) = os.path.split(target)
    dir_path = 'generated' + '-'.join(module.strip("_").split('::')[1:])
    output_dir = os.path.join(output_dir, dir_path)
    files_created = mb.write_module(
        os.path.join(output_dir, module + '.main.cpp'))

    # Touch the output file
    # This is because pyplusplus won't update the file if nothing has changed,
    # but the build system always expects this file to be changed.
    os.utime(os.path.join(output_dir, module + '.main.cpp'), None)

if __name__ == '__main__':
    sys.exit(main())

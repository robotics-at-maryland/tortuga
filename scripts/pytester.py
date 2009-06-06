# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  scripts/pytester.py

# STD Imports
import sys
import imp
import os
import unittest
from distutils import sysconfig

# Ensure we are using the proper version of python
import ram_version_check


def main(argv = None):
    if argv is None:
        argv = sys.argv    

    # Handle the python tests
    testLoader = unittest.TestLoader()
    suite = unittest.TestSuite()
    pytests = argv[1:]

    for mod_path in pytests:
        # Import Test Module
        (search_path, name) = os.path.split(mod_path)
        # Stip '.py' from the end
        name = name[:-3]
        (f, pathname, decs) = imp.find_module(name, [search_path])
        mod = imp.load_module(name, f, pathname, decs)
        
        # Load all the tests from the file into a single suite
        suite.addTest(testLoader.loadTestsFromModule(mod))

    if len(pytests) > 0:
        # Run the tests
        result = unittest.TextTestRunner().run(suite)

        if not result.wasSuccessful():
            return 1 # Failure

    return 0

if __name__ == '__main__':
    sys.exit(main())

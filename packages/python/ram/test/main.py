# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/test.py

"""
Finds and runs all tests
"""

# STD Imports
import os
import sys
import imp
import unittest

# Project Imports
import ram.test

def importFromPath(path):
    testDir = os.path.dirname(ram.test.__file__)
    
    # Import Test Module
    (search_path, name) = os.path.split(path)
    # Stip '.py' from the end
    name = name[:-3]
    (f, pathname, decs) = imp.find_module(name, [search_path])
    
    try:
        return imp.load_module(name, f, pathname, decs)
    except ImportError,e:
        print 'Could import %s: '% path.replace(testDir, ''),
        print '\t',e
    
    return None

def main(argv = None):
    if argv is None:
        argv = sys.argv
        
    testLoader = unittest.TestLoader()
    suite = unittest.TestSuite()   
    testDir = os.path.dirname(ram.test.__file__)
    
    # Recursively walk the path 
    for root, dirs, files in os.walk(testDir):
        # Remove *.pyc files and add the directory
        pathes = [os.path.join(root,f) for f in files if not f.endswith('.pyc')]
        # Remove __init__ files
        pathes = [p for p in pathes if 0 == p.count('__init__')]
                                          
        for path in pathes:
            mod = importFromPath(path)

            if mod is not None:
                print 'Gathering From: ',path.replace(testDir,'')
                suite.addTest(testLoader.loadTestsFromModule(mod))
        
        # don't visit SVN directories    
        if '.svn' in dirs:
            dirs.remove('.svn')  
        
    # Run the tests
    print '\nRunning Tests:'
    result = unittest.TextTestRunner().run(suite)

    if not result.wasSuccessful():
        return 1 # Failure
    
    return 0
        
if __name__ == '__main__':
    sys.exit(main())
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

if not os.environ.has_key('RAM_SVN_DIR'):
   raise Exception('R@M Environment Not Setup. Run scripts/setenv')

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
        print "Couldn't import %s: "% path.replace(testDir, ''),
        print '\t',e
    finally:
        f.close()
    
    return None

def main(argv = None):
    if argv is None:
        argv = sys.argv
        
    testLoader = unittest.TestLoader()
    testDir = os.path.dirname(ram.test.__file__)
    suite = unittest.TestSuite()
    baseName = 'ram.test'
    exclude = []

    # Recursively walk the path 
    for root, dirs, files in os.walk(testDir):
        # Only take *.py files in add the directory
        pathes = [os.path.join(root,f) for f in files 
                  if f.endswith('.py') and not f.startswith('.')]

        # Remove __init__ files
        pathes = [p for p in pathes if 0 == p.count('__init__')]
        pathes = filter(lambda x: 'trajectories.py' in x, pathes)

        for path in pathes:
            name = path.replace(testDir,'')
            print 'Gathering From: ',path.replace(testDir,'')

            # Turn path into dotted name
            try:
                dottedName = baseName + name.replace('/', '.')[:-3]
                if dottedName not in exclude:
                   test = testLoader.loadTestsFromName(dottedName)
                   suite.addTest(test)
            except AttributeError:
                print "Could not find module: %s" % (dottedName)
        
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

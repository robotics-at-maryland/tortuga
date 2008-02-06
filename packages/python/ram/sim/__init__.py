# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/sim/__init__.py

import os as __os
import sys as __sys

# Attempt to import Ogre, if it fails try with PYTHON_OGRE_HOME on sys.path
try:
    import ogre.renderer.OGRE as ogre
except ImportError:
    
    # Check for PYTHON_OGRE_HOME in the environment
    if __os.environ.has_key('PYTHON_OGRE_HOME'):
        pythonOgreHome = __os.environ['PYTHON_OGRE_HOME']
        
        # Only insert path if its not already on the path
        __path = __os.path.join(pythonOgreHome, 'packages_2.5')
        if __sys.path.count(__path) == 0:
            __sys.path.insert(0, __path)
# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/python/control.py

# STD Imports
import os
import sys
import StringIO
from distutils import sysconfig

# Ensure we are using the proper version of python
if os.environ['RAM_PYTHON_EXE'] !=  os.path.realpath(sys.executable):
    print 'Error running with python exe:', os.path.realpath(sys.executable)
    print 'Bootstrap run with:', os.environ['RAM_PYTHON_EXE']
    sys.exit(1)

if os.environ['RAM_PYTHON_PREFIX'] != sysconfig.PREFIX:
    print 'Error running with python prefix:',sysconfig.PREFIX
    print 'Bootstrap run with:', os.environ['RAM_PYTHON_PREFIX']
    sys.exit(1)

if os.environ['RAM_PYTHON_VERSION'] !=  sys.version.replace('\n',''):
    print 'Error running with python version:', sys.version.replace('\n','')
    print 'Bootstrap run with:', os.environ['RAM_PYTHON_VERSION']
    sys.exit(1)
    

# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # Core more first because of Boost.Python wrapping dependencies
    import ext.core

    from ext._vision import *

finally:
    sys.stderr = stderr


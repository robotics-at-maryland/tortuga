# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram_version_check.py

"""
Checks python versions.
"""

import os
import sys
from distutils import sysconfig


if os.environ['RAM_PYTHON_PREFIX'] != sysconfig.PREFIX:
    print 'Error running with python prefix:',sysconfig.PREFIX
    print 'Bootstrap run with:', os.environ['RAM_PYTHON_PREFIX']
    raise Exception("Python version mismatch")

if os.environ['RAM_PYTHON_VERSION'] !=  str(sys.subversion):
    print 'Error running with python version:',str(sys.subversion)
    print 'Bootstrap run with:', s.environ['RAM_PYTHON_VERSION']
    raise Exception("Python version mismatch")

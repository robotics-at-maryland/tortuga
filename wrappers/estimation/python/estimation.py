# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/core/python/core.py

# STD Imports
import os
import sys
import StringIO
import inspect
from distutils import sysconfig

# Ensure we are using the proper version of python
import ram_version_check


# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # Project Imports
    import ext._estimation as _estimation
    from ext._estimation import *

finally:
    sys.stderr = stderr

class IStateEstimator(_estimation.IStateEstimator):
    pass

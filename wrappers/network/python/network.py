# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  wrappers/network/python/network.py

# STD Imports
import os
import sys
import StringIO
from distutils import sysconfig

# Ensure we are using the proper version of python
import ram_version_check
    

# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # Core more first because of Boost.Python wrapping dependencies
    import ext.core

    from ext._network import *

finally:
    sys.stderr = stderr


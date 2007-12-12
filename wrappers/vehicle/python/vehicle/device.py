# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/vehicle/python/vehicle/device.py

# STD Imports
import sys
import StringIO

# Capture stderr, to suppress unwanted warningsb
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # These have to be first because of Boost.Python wrapping dependencies
    import ext.core
    import ext.vehicle

    from ext._vehicle_device import *

finally:
    sys.stderr = stderr

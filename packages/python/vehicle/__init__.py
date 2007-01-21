# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/__init__.py

"""
Initailize module properly on import
"""

# Pull in our classes (so we don't have odd import statements)
from vehicle import *
from device import *

# Import submodules to register any objects that are needed
import sim as sim

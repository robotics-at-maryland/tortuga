# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/sim/__init__.py

"""
This module contains all the tests for sim module.  All suites in the module
will be found in 'test.sim.suites'.
"""

# Python Imports
import unittest

# Here we import all submodules and gather there TestSuite into a list and
# make a combine TestSuite out of them.
def get_suite():
    """
    This gathers all TestSuites from sub modules into one large TestSuite.
    """
    suites = []
    
    # Import sub modules
    import tests.sim.robot as robot
    
    # Gather there suite
    suites.append(robot.get_suite())
    
    return unittest.TestSuite(suites)
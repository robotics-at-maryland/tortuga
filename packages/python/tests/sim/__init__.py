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

# Project Impors
import test

# Here we import all submodules and gather there TestSuite into a list and
# make a combine TestSuite out of them.
#def get_suite():
#    """
#    This gathers all TestSuites from sub modules into one large TestSuite.
#    """
#    return test.gather_suites_from_package(__name__)
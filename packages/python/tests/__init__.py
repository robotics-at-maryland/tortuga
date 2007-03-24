# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/__init__.py

"""
This module contains all the tests for project module.  All suites in the can
be retrieved as a single TestSuite using the tests.get_combine_suite() 
function.
"""

# Python Imports
import unittest

# Project Imports
import test

#def get_suite():
#    """
#    This gathers all TestSuites from sub modules into one large test suite and
#    then runs them.
#    """
#    return test.gather_suites_from_package(__name__)
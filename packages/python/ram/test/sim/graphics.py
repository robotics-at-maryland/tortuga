# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/sim/graphics.py

"""
This module contains all the tests for sim.robot module.  The TestSuite for the
module is retreived with tests.sim.robot.get_suite().
"""

# Ptyhon Imports
import unittest

# Library Imports
import ogre.renderer.OGRE as Ogre

# Project Imporst
import core

# Module to test
import ram.sim.graphics as graphics

class TestVisual(unittest.TestCase):
    def setUp(self):
        pass
    
    def tearDown(self):
        pass  

if __name__ == '__main__':
    unittest.main()

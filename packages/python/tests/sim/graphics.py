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
import Ogre

# Project Imporst
import core

# Module to test
import sim.graphics as graphics

class TestVisual(unittest.TestCase):
    def setUp(self):
        pass
    
    def tearDown(self):
        pass
    
    def test_kml_load(self):
        test_rml = {
            'Graphical' : {
                'mesh': 'box.mesh',
                'material': 'metal',
                'scale': [1,2,3]
                }
            }
        
        params = graphics.Visual.kml_load(test_rml)
        
        self.assertEquals('box.mesh', params['mesh'])
        self.assertEquals('metal', params['material'])
        self.assertEquals(Ogre.Vector3(1,2,3), params['scale'])
    

def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestVisual))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())
# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/sim/physics.py

"""
This module contains all the tests for sim.physics module.  The TestSuite for 
the module is retreived with tests.sim.physics.get_suite().
"""

# Ptyhon Imports
import unittest

# Library Imports
import Ogre

# Project Imporst
import core

# Module to test
import sim.physics as physics

class TestBody(unittest.TestCase):
    def setUp(self):
        pass
    
    def tearDown(self):
        pass
    
    def test_kml_load(self):
        test_rml = {
            'Physical' : {
               'mass' : 24.6, 
               'Shape' : {
                  'type' : 'box',
                  'size': [0.3, 0.5, 0.7],
                  }
               },
            }
        
        params = physics.Body.kml_load(test_rml)
        
        self.assertEquals(24.6, params['mass'])
        self.assertEquals('box', params['shape_type'])
        self.assertEquals({'size' : [0.3, 0.5, 0.7]}, params['shape_props'])

def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestBody))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())
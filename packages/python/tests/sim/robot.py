# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tests/sim/robot.py

"""
This module contains all the tests for sim.robot module.  The TestSuite for the
module is retreived with tests.sim.robot.get_suite().
"""

# Ptyhon Imports
import unittest

# Library Imports
#import ogre.renderer.OGRE as Ogre

# Project Imporst
import core

# Module to test
#import sim.robot as robot

class TestKMLRobotLoader(unittest.TestCase):
    def setUp(self):
        pass
    
    def tearDown(self):
        pass
    
    def test_can_load(self):
        loader = robot.KMLRobotLoader
        self.assertTrue(loader.can_load('robot.rml'))
        self.assertFalse(loader.can_load('.rml'))
        self.assertFalse(loader.can_load('rml'))
        self.assertFalse(loader.can_load('robot.yml'))
    
    def test_create_object(self):
        test_case = self
        class MockRobot(core.Component):
            core.implements(robot.IRobot)
            
            def __init__(self, name, position, orientation, mass, shape_type,
                         shape_props, mesh, material, scale):
                # Universal
                test_case.assertEquals('Mock', name)
                test_case.assertEquals(Ogre.Vector3(1, 2, 3), position)
                test_case.assertEquals(Ogre.Quaternion(Ogre.Degree(d = 45), [0, 1, 2]),
                                       orientation)
                
                # Graphical
                test_case.assertEquals('box.mesh', mesh)
                test_case.assertEquals('metal', material)
                test_case.assertEquals(Ogre.Vector3(1,2,3), scale)
                        
        test_rml = {
            'type' : 'tests.sim.robot.MockRobot',
            'name' : 'Mock',
            'position' : [1, 2, 3],
            'orientation' : [0, 1, 2, 45],
            'Physical' : {
               'mass' : 24.6, 
               'Shape' : {
                  'type' : 'box',
                  'size': [0.3, 0.5, 0.7],
                  }
               },
            'Graphical' : {
                'mesh': 'box.mesh',
                'material': 'metal',
                'scale': [1,2,3]
                }
            }
        
        loader = robot.KMLRobotLoader()
        new_robot = loader._create_object(test_rml, 'Robot')
    

def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    #suites.append(test_loader.loadTestsFromTestCase(TestKMLRobotLoader))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())
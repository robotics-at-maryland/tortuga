# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/sim/physics.py

"""
This module contains all the tests for sim.physics module.  The TestSuite for 
the module is retreived with tests.sim.physics.get_suite().
"""

# Ptyhon Imports
import unittest

# Project Imporst
import ram.core as core
from ram.test import Mock

# Module to test
import ram.sim.physics as physics

# Library Imports
import ogre.renderer.OGRE as Ogre

class TestBody(unittest.TestCase):
    # TODO: Some basic math to determine better values for position and 
    #       velocities after these time steps.
    def setUp(self):
        _world = physics.World()
        self.scene = Mock(world = _world)
        self.body = physics.Body(None, 'Test', self.scene, 'box', {'size':[1,1,1]},1)
        
    def tearDown(self):
        del self.body
        self.scene.world.shutdown()
        del self.scene
    
    def test_position(self):
        self.assertEqual(Ogre.Vector3(0,0,0), self.body.position)
           
    def test_orientation(self):
        self.assertEqual(Ogre.Quaternion(1,0,0,0), self.body.orientation)
        
    def test_force(self):
        self.body.force += [10,0,0]

        for i in xrange(100):
            self.scene.world.update(0.05)

        self.assertTrue(self.body.position.x > 0)
        
    def test_gravity(self):
        for i in xrange(100):
            self.scene.world.update(0.05)

        self.assertTrue(self.body.position.z < 0)
        
    def test_velocity(self):
        for i in xrange(100):
            self.scene.world.update(0.05)

        # Make sure the object has gained falling velocity
        self.assertTrue(self.body.velocity.z < 0)
        
    def test_local_force(self):
        # Generate a torque around the z axis
        self.body.add_local_force([10,0,0], [0,1,0])
        
        for i in xrange(100):
            self.scene.world.update(0.05)

        self.assert_(self.body.omega.z < 0 )
        
    def test_torque(self):
        # Apply a torque around the z axis
        self.body.torque = (0,0,10)
        
        for i in xrange(100):
            self.scene.world.update(0.05)

        self.assertNotEqual(self.body.omega.z, 0)
        
#    def test_kml_load(self):
#        test_rml = {
#            'Physical' : {
#               'mass' : 24.6, 
#               'Shape' : {
#                  'type' : 'box',
#                  'size': [0.3, 0.5, 0.7],
#                  }
#               },
#            }
#        
#        params = physics.Body.kml_load(test_rml)
#        
#        self.assertEquals(24.6, params['mass'])
#        self.assertEquals('box', params['shape_type'])
#        self.assertEquals({'size' : [0.3, 0.5, 0.7]}, params['shape_props'])

if __name__ == '__main__':
    unittest.main()

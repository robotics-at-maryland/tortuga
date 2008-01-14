# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/control/test/src/TestBWPDController.py

# STD Imports
import unittest

# Project Imports
import ext.control as control
import ext.core as core
import ext.vehicle as vehicle
import ext.math as math

class MockVehicle(vehicle.IVehicle):
    def __init__(self):
        vehicle.IVehicle.__init__(self, 'Test')
        self.depth = 0
        self.orien = math.Quaternion.IDENTITY
        self.linAccel = math.Vector3.ZERO
        self.angRate = math.Vector3.ZERO
        self.force = math.Vector3.ZERO
        self.torque = math.Vector3.ZERO
        
    def getLinearAcceleration(self):
        return self.linAccel
    
    def getOrientation(self):
        return self.orien
    
    def getAngularRate(self):
        return self.angRate
    
    def getDepth(self):
        return self.depth
    
    def applyForcesAndTorques(self, force, torque):
        self.force = force
        self.torque = torque

class TestBWPDController(unittest.TestCase):
    def setUp(self):
        self.vehicle = MockVehicle()
        
        cfg = {
            'name' : 'TestController',
            'type' : 'BWPDController',
            'angularPGain' : 10,
            'angularDGain' : 1,
            'desiredQuaternion' : [0, 0, 0, 1] }
        cfg = core.ConfigNode.fromString(str(cfg))
        deps = core.SubsystemList()
        deps.append(self.vehicle)
        
        self.controller = core.SubsystemMaker.newObject(cfg, deps)
        self.actualDepth = 0

    def depthHelper(self, event):
        self.actualDepth = event.number
        
    def testAtDepth(self):
        # Subscribe to the event
        self.controller.subscribe(control.IController.AT_DEPTH,
                                  self.depthHelper)

        # Default Depth Threshold is 0.5
        self.vehicle.depth = 4;
        self.controller.update(1);
        self.controller.setDepth(3.7);
        self.assertEqual(True, self.controller.atDepth())

        # Ensure it does go off
        self.controller.setDepth(4.3);
        self.controller.update(1);
        self.assertEqual(4, self.actualDepth)


if __name__ == '__main__':
    unittest.main()

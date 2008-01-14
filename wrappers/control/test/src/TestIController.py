# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vehicle/test/src/TestIVehicle.py

# STD Imports
import os
import unittest

# Project Imports
import ext.core as core
import ext.control

class TestIController(unittest.TestCase):
    def test(self):
        cfg = {
            "type" : "BWPDController",
            "angularPGain" : 10,
            "angularDGain" : 1,
            "speedPGain" : 3,
            "depthPGain" : 20,
            "desiredSpeed" : 0,
            "desiredDepth" : 0.25,
            "desiredQuaternion" : [0, 0, 0, 1]
        }
        cfg = core.ConfigNode.fromString(str(cfg))
        obj = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        # Make sure the casting works and is needed
        #self.assert_(not hasattr(obj,'setSpeed'))
        #controller = ext.control.IController.castTo(obj)
        #self.assert_(hasattr(controller,'setSpeed'))
        

if __name__ == '__main__':
    unittest.main()

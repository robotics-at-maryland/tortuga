# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: wrapper/slam/test/src/TestISlam.py

# STD Imports
import os
import unittest

# Project Imports
import ext.core as core
import ext.slam

class TestISlam(unittest.TestCase):
    def test(self):
        #cfg = {
        #    "type" : "BWPDController",
        #    "angularPGain" : 10,
        #    "angularDGain" : 1,
        #    "speedPGain" : 3,
        #    "depthPGain" : 20,
        #    "desiredSpeed" : 0,
        #    "desiredDepth" : 0.25,
        #    "desiredQuaternion" : [0, 0, 0, 1]
        #}
        #cfg = core.ConfigNode.fromString(str(cfg))
        #obj = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        # Make sure we have the right methods on the controller object
        #controller = obj
        #self.assert_(hasattr(controller,'setSpeed'))
        #self.assert_(hasattr(controller,'yawVehicle'))
        #self.assert_(hasattr(controller,'setDepth'))
        # Placeholder for now
        pass
        

if __name__ == '__main__':
    unittest.main()

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
            "type" : "CombineController",
            "TranslationalController" : {"type" : "OpenLoopTranslationalController"},
            "DepthController" : {"type" : "PIDDepthController"},
            "RotationalController" : {"type" : "NonlinearPDRotationalController"},
            "holdCurrentDepth" : 0,
            "holdCurrentHeading" : 0
        }
        cfg = core.ConfigNode.fromString(str(cfg))
        obj = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        # Make sure we have the right methods on the controller object
        controller = obj
        self.assert_(hasattr(controller,'setSpeed'))
        self.assert_(hasattr(controller,'yawVehicle'))
        self.assert_(hasattr(controller,'setDepth'))
        

if __name__ == '__main__':
    unittest.main()

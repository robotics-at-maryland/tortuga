# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vision/test/src/TestImport.py

# STD Imports
import unittest

# Project Imports
import ext.core as core
import ext.vision as vision

class TestVisionSystem(unittest.TestCase):
    def testCreate(self):
        cfg = { 'name' : 'Test', 'type' : 'TestSubsystem' }
        cfg = core.ConfigNode.fromString(str(cfg))
        deps = core.SubsystemList()

        #visionSystem = vision.VisionSystem(cfg, deps)
        #del visionSystem

if __name__ == '__main__':
    unittest.main()
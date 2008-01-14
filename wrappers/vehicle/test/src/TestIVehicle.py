# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vehicle/test/src/TestIVehicle.py

# STD Imports
import unittest

# Project Imports
import ext.core as core
import ext.vehicle

class TestIVehicle(unittest.TestCase):
    def test(self):
        cfg = {
            'depthCalibSlope': 33.01,
            'depthCalibIntercept': 94,
            'name' : 'TestVehicle',
            'type' : 'Vehicle'
            }
        cfg = core.ConfigNode.fromString(str(cfg))
        veh = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        # Make sure the type was properly converted
        self.assert_(hasattr(veh,'getDepth'))
        

if __name__ == '__main__':
    unittest.main()

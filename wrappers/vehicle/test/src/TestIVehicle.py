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

class Vehicle(ext.vehicle.IVehicle):
    def __init__(self, config, deps):
        ext.vehicle.IVehicle.__init__(self, config["name"])
        self.depth = 5

    def getDepth(self):
        return self.depth

core.SubsystemMaker.registerSubsystem('MockVehicle', Vehicle)


class TestIVehicle(unittest.TestCase):
    def testSubsystemMaker(self):
        # Test the existing C++ Vehicle
        if 'Vehicle' in core.SubsystemMaker.getRegisteredKeys():
            cfg = {
                'depthCalibSlope': 33.01,
                'depthCalibIntercept': 94,
                'name' : 'TestVehicle',
                'type' : 'Vehicle',
                'sensor_board_file' : '/dev/DOESNOTEXIST'
                }
            cfg = core.ConfigNode.fromString(str(cfg))
            veh = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

            # Make sure the type was properly converted
            self.assert_(hasattr(veh,'getDepth'))
            self.assert_(hasattr(veh,'safeThrusters'))
            self.assertEquals(None, veh.safeThrusters())
            

        # Test a mock Python one
        cfg = {
            'name' : 'TestVehicle',
            'type' : 'MockVehicle'
            }
        cfg = core.ConfigNode.fromString(str(cfg))
        veh = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        self.assertEquals(5, veh.depth)
        self.assertEquals(5, veh.getDepth())
        self.assert_(isinstance(veh, ext.vehicle.IVehicle))

if __name__ == '__main__':
    unittest.main()

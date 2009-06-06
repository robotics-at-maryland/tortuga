# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/vehicle/test/src/TestIDeviceMaker.py

# STD Imports
import unittest

# Project Imports
import ext.vehicle.device as device
import ext.core as core

class MockDevice(device.Device):
    def __init__(self, config, eventHub, vehicle):
        device.Device.__init__(self, config["name"], eventHub)

device.IDeviceMaker.registerDevice('MockDevice', MockDevice)

class TestIDeviceMaker(unittest.TestCase):
    def testNewObject(self):
        cfg = { 'name' : 'TestDevice', 'type' : 'MockDevice' }
        cfg = core.ConfigNode.fromString(str(cfg))
        newDevice = device.IDeviceMaker.newObject(cfg, None, None)

        self.assertEquals('TestDevice', newDevice.getName())
        self.assert_(isinstance(newDevice, MockDevice))

if __name__ == '__main__':
    unittest.main()

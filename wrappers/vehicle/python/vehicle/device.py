# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/vehicle/python/vehicle/device.py

# STD Imports
import sys
import StringIO

# Capture stderr, to suppress unwanted warningsb
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # These have to be first because of Boost.Python wrapping dependencies
    import ext.core
    import ext.vehicle

    import ext._vehicle_device as _device
    from ext._vehicle_device import *

finally:
    sys.stderr = stderr


class IDeviceMaker(_device.IDeviceMaker):
    """
    A helper class to registery subsystem makers.  An example of the usage
    of this class:

    class TestDevice(vehicle.device.Device):        
        def __init__(self, config):
            vehicle.device.Device.__init__(self, config['name'].asString())
            ... code ...
    core.IDeviceMaker.registerDevice('TestDevice', TestDevice)
    """
    def __init__(self, name, createClass):
        _device.IDeviceMaker.__init__(self, name)
        self.args = None
        self._createClass = createClass

    def makeObject(self, config, eventHub, vehicle):
        return self._createClass(eval(config.toString()), eventHub, vehicle)

def registerDevice(name, cls):
    cls.maker = IDeviceMaker(name, cls)

IDeviceMaker.registerDevice = staticmethod(registerDevice)

class Device(_device.IDevice):
    def __init__(self, name, eventHub = None):
        _device.IDevice.__init__(self, eventHub)
        self._name = name

    def getName(self):
        return self._name

    def background(self, interval = -1):
        pass

    def backgrounded(self):
        return False
    
    def unbackground(self, join = False):
        pass

    def update(self, timestep):
        pass

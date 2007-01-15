# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/device.py

"""
    Defines the base interface for the Device objects and the factory to 
create them.
"""

from core import property

# Device Creation Classes
class DeviceFactory(object):
    """
    Break out into a common base class, or meta class?
    """
    createFunc = {};

    @staticmethod    
    def create(type, args, kwargs = {}):
        # apply calls the given function with args, as the list of arugments
        # and the dict as the set of keyword arguments
        return apply(DeviceFactory.createFunc[type], args, kwargs)
    
    @staticmethod
    def register(type, function):
        DeviceFactory.createFunc[type] = function
    
class IDevice(object):
    """
    Represents a physical object on the vehicle like thrusters, sensors,
    actuators, etc.
    """
    
    def update(self, timestep):
        """
        Update the device, the timestep value may not be needed
        """
        pass

    def start_update(self):
        """
        Start automatic update (this would utilize threads and locking to keep
        the vehicle up to date)
        """
        pass
    
class IThruster(IDevice):
    """
    A non rotating thruster that can have its powerlevel set from -100 to 100
    """
    pass
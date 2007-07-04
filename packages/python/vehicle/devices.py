# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/devices.py

# Project Imports
import ext.vehicle_device as _device # Import C++ Devices
from core import Interface, classImplements

# Bring C++ Devices into scope
from ext.vehicle_device import Thruster
from ext.vehicle_device import IMU
    
class IDevice(Interface):
    """
    The base interface that all devices must meet
    """
    
    def getName():
        """
        Gets the name of the device
        """
        pass
    
    
# Put C++ classes in the interface system
classImplements(Thruster, IDevice)
classImplements(IMU, IDevice)

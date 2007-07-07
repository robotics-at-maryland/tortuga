# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/devices.py

# Python Imports
import warnings

# Project Imports
from core import Interface, classImplements

# Bring C++ Devices into scope
warnings.simplefilter('ignore', RuntimeWarning)
import ext.pattern
import ext.core
import ext.vehicle_device as _device # Import C++ Devices
from ext.vehicle_device import Thruster
from ext.vehicle_device import IMU
from ext.vehicle_device import PSU
warnings.simplefilter('default', RuntimeWarning)
    
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
classImplements(PSU, IDevice)
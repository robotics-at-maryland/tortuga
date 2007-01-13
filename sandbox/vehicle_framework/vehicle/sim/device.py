# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/device.py

"""
    This defines all the devices that will fufill the interfaces defined in
the vehicle.device module.
"""

from vehicle import IThruster

class Thruster(IThruster):
    def __init__(self, config, vehicle):
        pass
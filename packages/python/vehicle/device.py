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

from core import Interface, Attribute

    
class IDevice(Interface):
    """
    Represents a physical object on the vehicle like thrusters, sensors,
    actuators, etc.
    """
    
    def update(timestep):
        """
        Update the device, the timestep value may not be needed
        """
        pass

    def start_update():
        """
        Start automatic update (this would utilize threads and locking to keep
        the vehicle up to date)
        """
        pass
    
class IThruster(IDevice):
    """
    A non rotating thruster that can have its powerlevel set from -100 to 100
    """
    
    min_force = Attribute(\
        """
        The minimum force the thruster can apply (ie max reverse force)
        """)

    max_force  = Attribute(\
        """
        The maximum force the thruster can apply (ie max forward force)
        """)

      
        

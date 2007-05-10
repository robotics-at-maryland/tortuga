# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/vehicle.py

"""
    Defines the base interface the Vehicle objects and the factory to create
them.
"""

import core

# Definition of the vehicle interface
class IVehicle(core.Interface):
    """
    Needed properties: Heading, Depth, possibly some accelerations,
    incorperate other vehicle models and plans here.
    
    Notice the lack of methods like "addXXX" and "setXXX" the internals of the
    vehicle will be very protected.  Generally a device will be turned "on",
    ie told to update, and it will set the proper data in the vehicle.
    
    The attitude and position attributes hold the current attitude and position
    of the vehicle.  These are currently Ogre Vectors, and Quaternions
    """

    # Possible add this to and "IUpdatedable" interface
    def update(self, timestep):
        """
        Update the vehicle for a certain timestep (uses non threaded 
        syncrounous operation)
        """
        pass

    def start_continuous_update(self):
        """
        Start automatic update (this would utilize threads and locking to keep
        everything up to date)
        """
        pass

    def stop_continuous_update(self):
        """
        Stops what ever current background update system there is.
        """
        pass


    def get_device(self, name):
        """
        Gets the current device associated with the given name

        @type  name: string
        @param name: The name of the device.

        @rtype : IDevice implementer, or None
        @return: The requested device, None happens on failure.
        """
        pass


    def acceleration():
        pass

    def angular_acceleration():
        pass

    def angular_velocity():
        pass

    def depth():
        pass
    
    def orientation():
        pass

    # TODO: Add stuff to deal with power, temps, and water leakage
    

    

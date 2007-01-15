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
from vehicle import DeviceFactory

class Thruster(IThruster):
    """
    Implements the simulated thruster for the vehicle
    """
    thrusters = []
    
    def __init__(self, config, vehicle):
        # Attach the thruster mesh to the scene node
        entity = vehicle.graphics_sys.scene_manage.createEntity(config['name'],
                                                                config['mesh'])
        vehicle.hull_node.attachObject(entity)
        
        # Read in our configuration data
        self.position = config['position']
        self.direction = config['direction']    
        self.strength = config['strength']
        
        # Append to current list of thrusters
        Thrusters.thrusters.append(self)
    
    @staticmethod
    def clear_thrusters():
        """
        Releases the current set of thrusters
        """
        Thrusters.thrusters = []
        
    @staticmethod
    def apply_thruster_force(body):
        """
        Applies the force of the current thrusters to the given body.  This 
        uses the global list of thrusters.
        """
        for thruster in Thruster.thrusters:
            if 0 != thruster.power :
                force = thruster.direction * thruster.power * strength
                body.addLocalForce(force, thruster.position)
        
# Register Thruster
DeviceFactory.register('SimThruster', Thruster)
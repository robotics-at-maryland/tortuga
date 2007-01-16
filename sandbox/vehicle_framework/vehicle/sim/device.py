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
from vehicle.sim import SimulationError
from vehicle.sim.core import Vector, Quat

class Thruster(IThruster):
    """
    Implements the simulated thruster for the vehicle
    """
    thrusters = []
    
    def __init__(self, name, config, vehicle):
        try:
            scene_manager = vehicle.graphics_sys.scene_manager
            gfx_cfg = config['Graphical']
            
            entity = scene_manager.createEntity(name, gfx_cfg['mesh'])
            entity.setMaterialName(gfx_cfg['material'])
            
            # Read in our properties from configuration data
            self.position = Vector(config['position'])
            self.direction = Vector(config['direction'])
            self.strength = config['strength']
            
            # Attach the thruster mesh to the scene node   

            # This position scaling is needed to undo Ogre scaling of parent
            # child positions
            pscale = vehicle.hull_node.getScale()
            position = self.position * Vector(((1 / pscale.x),(1 / pscale.y),
                                               (1 / pscale.z)))
            orientation =  Quat(gfx_cfg['orientation'], axis_angle = True)
            node = vehicle.hull_node.createChildSceneNode(name, position, 
                                                          orientation)
            node.attachObject(entity)
            node.setInheritScale(False)
            node.setScale(Vector(gfx_cfg['scale']))
            
        except KeyError, e:
            raise SimulationError("The Thruster device must have property:"
                                  " %s" % e)
        
        # Append to current list of thrusters
        Thruster.thrusters.append(self)
        
        self.power = 0
    
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
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

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Library Imports
import ogre.renderer.OGRE as Ogre

# Project Imports
from vehicle.device import IThruster, DeviceFactory
from sim.simulation import SimulationError, Simulation
from sim.util import Vector, Quat

class Thruster(IThruster):
    """
    Implements the simulated thruster for the vehicle
    """
    thrusters = []
    
    def __init__(self, name, config, vehicle):
        try:
            scene_manager = Simulation.get().graphics_sys.scene_manager
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
            
            self.node = node
            self.scene_mgr = scene_manager
            self.name = name
            self.manual = None
            
        except KeyError, e:
            raise SimulationError("The Thruster device must have property:"
                                  " %s" % e)
        
        # Append to current list of thrusters
        Thruster.thrusters.append(self)
        
        self._power = 0
    
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
            if 0 != thruster._power :
                force = thruster.direction * thruster._power * thruster.strength
                body.addLocalForce(force, thruster.position)
                
                # This should be cleaned up and moved somewhere else
                if thruster.manual is not None:
                    thruster.node.detachObject(thruster.manual)
                    thruster.scene_mgr.destroyManualObject(thruster.manual)
                    
                manual = thruster.scene_mgr.createManualObject(thruster.name + "manual");
                manual.begin("BaseRedNoLighting", Ogre.RenderOperation.OT_LINE_STRIP);

                base_pt = thruster.node.position
                thrust_pt = base_pt + ((thruster.node.getOrientation() * \
                                        thruster.direction) * (thruster._power * 10))
                manual.position(base_pt)
                manual.position(thrust_pt)
                
                manual.end()
                
                thruster.manual = manual
                thruster.node.attachObject(thruster.manual)
                        
# Register Thruster
DeviceFactory.register('SimThruster', Thruster)
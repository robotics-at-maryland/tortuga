# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/src/sim/vision.py

# STD Imports
import math

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core
from sim.subsystems import Simulation
from sim.vehicle import SimVehicle

import core
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual
from ram.sim.serialization import IKMLStorable, two_step_init


class IBuoy(IObject):
    """ An object which you can see in the simulation"""
    pass

# TODO: Fill out the methods for the class

class Buoy(Visual):
    core.implements(IVisual, IBuoy)
    
    _plane_count = 0
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

#    def init(self, parent, name, scene, mesh, material,
#             position = Ogre.Vector3.ZERO, 
#             orientation = Ogre.Quaternion.IDENTITY,
#             scale = Ogre.Vector3(1,1,1)):
#        Visual.init(self, parent, name, scene, mesh, material, position, 
#                    orientation, scale)

    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (scene, parent, kml_node)
        """
        scene, parent, node = data_object
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"

class SimVision(ext.core.Subsystem):
    LIGHT_FOUND = ext.core.declareEventType('LIGHT_FOUND')
    LIGHT_LOST = ext.core.declareEventType('LIGHT_LOST')
    
    def __init__(self, config, deps):
        ext.core.Subsystem.__init__(self, config.get('name', 'SimVision'), deps)
        
        # Grab the scene we are operating in
        sim = ext.core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)
        self.vehicle = ext.core.Subsystem.getSubsystemOfType(SimVehicle, deps, 
                                                             nonNone = True)
        self._horizontalFOV = config.get('horizontalFOV', 107)
        self._verticalFOV = config.get('verticalFOV', 78)
        
        # Find all the Buoy's
        self._foundLight = False
        self._bouys = []
        for obj in sim.scene._objects:
            if IBuoy in core.providedBy(obj):
                self._bouys.append(obj)
        
    def redLightDetectorOn(self):
        pass
    
    def redLightDetectorOff(self):
        pass
    

    def update(self, timeSinceLastUpdate):
        """
        Checks against the obstacles, and generates the proper events
        """
        self._checkRedLight()
    
    def _checkRedLight(self):
        """
        Check for the red light
        """
        
        # Find the closest bouy
        # Note: This is inefficient but the buoy count is to remain low, so
        # we are fine.
        closest = None
        for b in self._bouys:
            toBuoy = b.position - self.vehicle.robot.position
            
            if closest is None:
               closest = (b, toBuoy)
            else:
                buoy, realtivePos = closest
                if toBuoy.squaredLength() < realtivePos.squaredLength():
                    closest = (b, toBuoy)
                    
        # Determine orientation to the bouy
        lightVisible = False
        bouy, relativePos = closest
        forwardVector = self.vehicle.robot.orientation * ogre.Vector3.UNIT_X
        
        quat = forwardVector.getRotationTo(relativePos)
        yaw = -quat.getRoll(True).valueDegrees()
        pitch = quat.getYaw(True).valueDegrees()
        
        # Check to see if its the field of view
        if (math.fabs(yaw) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            lightVisible = True
        
        if lightVisible:
            event = ext.core.Event()
            event.x = yaw / (self._horizontalFOV/2)
            # Negative because of the corindate system
            event.y = -pitch / (self._verticalFOV/2)
            self.publish(SimVision.LIGHT_FOUND, event)
            
        else:
            if self._foundLight:
                self.publish(SimVision.LIGHT_LOST, ext.core.Event())

        self._foundLight = lightVisible
        

ext.core.SubsystemMaker.registerSubsystem('SimVision', SimVision)
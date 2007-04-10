# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   /sim/robot.py

# Library Imports
import ogre.renderer.OGRE as Ogre

# Project Impports
import core
from sim.util import Vector, Quat, SimulationError
from sim.serialization import IKMLStorable, KMLLoader, two_step_init
from sim.physics import IBody, Body
from sim.graphics import IVisual, Visual


class IPart(IBody, IVisual):
    """
    A part of a robot
    """
    
    robot = core.Attribute("""The robot this part belongs to""")
    
class IRobot(core.Interface):
    pass
    
# TODO: Improve the documentation of this class and join it with a common 
#       Interface base clas.
class IRobotLoader(core.Interface):
    """
    Loads a robot.
    """
    pass

# TODO: Break some of this functionality out into a place it can be used by the 
# scene loader
class KMLRobotLoader(core.Component, KMLLoader):
    """
    Loads a robot from our structured KML Loader
    """
    
    core.implements(IRobotLoader)
    
    @staticmethod
    def can_load(file_name):
        if file_name.endswith('.rml') and len(file_name) > 4:
            return True
        return False
    

    
class Part(Body, Visual):
    # Inherits IBody, IVisual, IKMLStorable
    core.implements(IRobot)
            
    @two_step_init
    def __init__(self):
        self._parts = []
        Body.__init__(self)
        Visual.__init__(self)
            
            
    def init(self, parent, name, scene, shape_type, shape_props, nass, mesh, 
             material, position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY,
             scale = Ogre.Vector3(1,1,1)):
        
        Body.init(self, None, name, scene, mass, shape_type, 
                      shape_props, position, orietnation)
        Visual.init(self, None, name, scene, mesh, material, position, 
                    orietnation, scale)
        
    def _create(self, parent, name, scene, shape_type, shape_props, nass, mesh, 
                material, position, orientation, scale):
        pass
    
    def load(self, data_object):
        Body.load(self, data_object)
        Visual.load(self, data_object)
        
    def save(self, data_object):
        raise "Not yet implemented"
         
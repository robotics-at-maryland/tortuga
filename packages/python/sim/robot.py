# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   /sim/robot.py

# Library Imports
import Ogre

# Project Impports
import core
from sim.util import Vector, Quat, SimulationError
from sim.serialization import IKMLLoadable, KMLLoader
from sim.physics import IBody, Body
from sim.graphics import IVisual, Visual


class IPart(IBody, IVisual):
    """
    A part of a robot
    """
    
    robot = core.Attribute("""The robot this part belongs to""")
    
class IRobot(IBody, IVisual):
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
    

    
class Robot(Body, Visual):
    core.implements(IRobot)
            
    def __init__(self, name, position, orientation, mass, shape_type,
                 shape_props, mesh, material, scale):
        
        # Create 
        Body.__init__(self, None, position, orietnation, mass, shape_type, 
                      shape_props)
        Visual.__init__(self, None, position, orietnation, mesh, material, 
                        scale)
            
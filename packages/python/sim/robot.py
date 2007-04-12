# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   /sim/robot.py

# Library Imports
import ogre.renderer.OGRE as Ogre
import yaml

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
    def can_load(robot_data):
        pass
    
    def load(robot, scene, robot_data):
        pass

# TODO: Break some of this functionality out into a place it can be used by the 
# scene loader
class KMLRobotLoader(core.Component, KMLLoader):
    """
    Loads a robot from our structured KML Loader
    """
    
    core.implements(IRobotLoader)
    
    @staticmethod
    def can_load(robot_data):
        try:
            if robot_data.endswith('.rml') and len(robot_data) > 4:
                return True
            return False
        except AttributeError:
            return False
        
    def load(self, robot, scene, robot_data):
        config = yaml.load(file(robot_data))
        rbt_node = config['Robot']
        
        robot.name = rbt_node['name']
        main_part = Part()
        main_part.load((scene, None, rbt_node))
        robot._main_part = main_part

class Robot(core.Component):
    
    robot_loaders = core.ExtensionPoint(IRobotLoader)
    
    def __init__(self, scene, robot_data):
        """
        @type name: string
        @param name: The name of robot
        
        @type  robot_data: Anything
        @param robot_data: The object that will be passed to the robot loader
        """
        self.name = None
        self._main_part = None
        
        # Search for a scene loader compatible with file format then load it
        loaders = [loader for loader in self.robot_loaders \
                   if loader.can_load(robot_data) ]
        
        if len(loaders) == 0:
            raise SceneError('No Loader found for "%s".' % robot_data)
        
        loader = loaders[0]()
        loader.load(self, scene, robot_data)


    def save(self, location):
        raise SceneError("Save not yet implemented")
        # TODO: Finish me
    
    def reload(self):
        raise SceneError("Reload not yet implemented")
        # TODO: Finish me
    
class Part(Body, Visual):
    # Inherits IBody, IVisual, IKMLStorable
    #core.implements(IRobot)
            
    @two_step_init
    def __init__(self):
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
        # Link body and node toghether
        self._body.attachToNode(self._node)
        self.set_buoyancy((0,1,0))
        
    def save(self, data_object):
        raise "Not yet implemented"
         
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
class KMLRobotLoader(core.Component):
    """
    Loads a robot from our structured KML Loader
    """
    
    core.implements(IRobotLoader)
    
    @staticmethod
    def can_load(file_name):
        if file_name.endswith('.rml') and len(file_name) > 4:
            return True
        return False
    
    def load(self, file_name, world, scene_mgr):
        """
        <Interface Without I>:
            type: <Class that implements interface>
        """
        
        return _create_object() 
        
    def _create_object(self, node, node_name, parent = None):
        
        interface = None
        if node_name == 'Robot':
            interface = IRobot
        
        _class = core.Component.get_class(interface, node['type'])
        kwargs = {}
        
        # Build parameters to pass to __init__ based on the interfaces 
        # implemented by the class
        
        # General parameters
        
        
        kwargs['name'] = node['name']
        kwargs['position'] = node['position']
        kwargs['orientation'] = Quat(node['orientation'], axis_angle = True)
        
        # Interface specific Paramters
        if IBody.implementedBy(_class):    
            physical_node = node['Physical']    
            # Find shape type and its properties
            shape_type = physical_node['Shape']['type'].lower() 
            shape_props = {}
            for param, value in physical_node['Shape'].iteritems():
                if param != 'type':
                    shape_props[param] = value 
                    
            kwargs['shape_type'] = shape_type
            kwargs['shape_props'] = shape_props
                    
            # Grab and validate Mass
            mass = physical_node['mass']
            if (type(mass) is not int) and (type(mass) is not float):
                raise SimulationError('Mass must be an interger of float')
            
            kwargs['mass'] = mass
            
        
        if IVisual.implementedBy(_class):
            gfx_node = node['Graphical'] 
            kwargs['mesh'] = gfx_node['mesh']
            kwargs['material'] = gfx_node['material']
            kwargs['scale'] = Ogre.Vector3(gfx_node['scale'])
            
        return _class(**kwargs)
    
class Robot(Body, Visual):
    core.implements(robot.IRobot)
            
    def __init__(self, name, position, orientation, mass, shape_type,
                 shape_props, mesh, material, scale):
        
        # Create 
        Body.__init__(self, None, position, orietnation, mass, shape_type, 
                      shape_props)
        Visual.__init__(self, None, position, orietnation, mesh, material, 
                        scale)
            
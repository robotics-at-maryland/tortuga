# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  sim/physics.py

# Library Imports
import OgreNewt
import Ogre

# Project Imports
import core
import sim.util
from sim.serialization import IKMLLoadable

class PhysicsError(sim.util.SimulationError):
    """
    The error exception class for all PhysicsErrors
    """

class IBody(sim.util.IObject):
    """
    A physical body in the simulation
    """
    
    force = core.Attribute(
        """
        type: Ogre.Vector
        param: The current force on the object.
        """)
    
    gravity = core.Attribute(
       """
       type: number
       param: The gravity on the object in meters per second
       """) 
    
    # TODO: Document local, global, and buoyancy forces   
    
class Body(object):
    core.implements(IBody, IKMLLoadable)
    
    def __init__(self, world, shape, shape_props, 
                 position = Ogre.Vector3.ZERO, 
                 orientation = Ogre.Quaternion.IDENTITY):
        """
        @type  world: sim.physics.World
        @param world: The physical simulation to create the body in
        
        @type  shape: str
        @param shape: Denotes, the type of shape, like 'box' or 'sphere'
        
        @type  shape_props: Dict
        @param shape_props: Maps paramater name to value for the shapes 
                            constructor.
        """
        
        self._force = Ogre.Vector3(0,0,0)
        self._gravity = Ogre.Vectory3(0,0,0)
        self._local_force = []
        self._global_force = []
        self._buoyancy_plane = None
        
        # Create Collision Object
        # TODO: Improve collision support
        col = None
        if shape_type == 'box':
            size = Ogre.Vector3(shape_props['size'])
            col = OgreNewt.Box(world, size)
        
        world.create_body(self, col, position, orientation)
    
    @staticmethod
    def _make_force_pos_pair(force, pos):
        return (Ogre.Vector3(force), Ogre.Vector3(pos))
    
    # IBody Methods
    class force(core.cls_property):
        def fget(self):
            return self._force
        def fset(self, force):
            self._force = Ogre.Vector3(force)
            
    class gravity(core.cls_property):
        def fget(self):
            return self._gravity
        def fset(self, gravity):
            self._gravity = Ogre.Vector3(gravity)
    
    def add_local_force(self, force, pos):
        self._local_force.append(Body._make_force_pos_pair(force, pos))
        
    def set_local_force(self, force, pos):
        self._local_force = [Body._make_force_pos_pair(force, pos)]
    
    def get_local_forces(self):
        return self._local_force
        
    def add_global_force(self):
        self._global_force = append(Body._make_force_pos_pair(force, pos))

    def set_global_force(self, force, pos):
        self._global_force = [Body._make_force_pos_pair(force, pos)]
        
    def get_global_forces(self):
        return self._global_force
        
    def set_buoyancy(self, plane):
        """
        type  plane: Ogre.Vector3
        param plane: The normal of the buoyancy plane. If none, the object will
                     not be buoyanct
        """
        if plane is not None:
            self._buoyancy_plane = Ogre.Plane(normal, (0,0,0))
        else:
            self._buoyancy_plane = None
            
    def get_buoyancy_plane(self):
        return self._buoyancy_plane
    
    # IKMLLoadable Methods
    @staticmethod
    def kml_load(node):
        kwargs = {}
        
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
        
        return kwargs
    
class Shape(object):
    pass

    
class World(OgreNewt.World):
    """
    This is a wrapper for the OgreNewt.World object, it provides a more managed
    existence.
    
    type world_count: number
    cvar world_count: The number of worlds created
    
    type world_map: Dict
    cvar world_map: Maps world to int use for lookup of world from body in the
                    force callbacks.
                    
    type bodies: Dict
    cvar bodies: Maps Newton body to supplied Object
    """
    
    would_count = 0
    world_map = {}
    
    def __init__(self):
        OgreNewt.World.__init__(self)
        
        # Setup int -> world mapping
        World.would_count += 1
        World.world_map[World.would_count] = self
        
        self.bodies = {}
        
    def create_body(self, body, shape, position = Ogre.Vector3.ZERO, 
                    orientation = Ogre.Quaternion.IDENTITY):
        """
        Creates a new body and then keeps and internal reference to the 
        
        @type  shape: OgreNewt.Shape
        @param shape: The shape to assign to the new body
        
        @type  body: Implementer of IBody
        @param body: The body you wish to have mapped to this newton body 
        
        @rtype:  OgreNewt.Body
        @return: The created OgreNewt Body
        """
        
        if not IBody.implementedBy(body):
            raise PhysicsError("body must implement IBody interface")
        
        new_body = OgreNewt.Body(self, shape, World.world_count)
        new_body.setPositionOrientation(position, orientation)
        new_body.setCustomForceAndTorqueCallback(World.force_torque_callback)
        self.bodies[body] = new_body
        
        return body
    
    @staticmethod
    def force_torque_callback(newton_body):
        """
        This is called by newton for every body, we use this functions to 
        relieve the use of having to deal with this issue.
        
        @type  newton_body: OgreNewt.Body
        @param newton_body: The body to add force and torque to
        """
        
        body = World.world_map[newton_body.type][newton_body]
        
        # Apply forces
        newton_body.addForce(body.force)
        for force, pos in body.get_local_forces():
            newton_body.addLocalForce(force, pos)
        for force, pos in body.get_global_forces():
            newton_body.addGlobalForce(force, pos)
            
        # Apply gravity
        mass, inertia = newton_body.getMassMatrix()
        nowton_body.addForce(body.force * mass)
        
        # Set bouyancy callback
        nowton_body.addBouyancyForce(1000, 0.03, 0.03, body.gravity, 
                                     World.buoyancy_callback, "")
        
    @staticmethod
    def buoyancy_callback(colID, newton_body, orient, pos, plane):
        body = World.world_map[newton_body.type][newton_body]
        bplane = body.get_buoyancy_plane()

        if bplane is not None:
            # we need to copy the normals and 'd' to the plane we were passed...
            plane.normal = bplane.normal
            plane.d = bplane.d
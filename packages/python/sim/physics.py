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
from sim.serialization import IKMLStorable, two_step_init, parse_position_orientation

DEFAULT_GRAVITY = Ogre.Vector3(0,-9.8,0)

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
        This force is applied to the center gravity of the object in the global 
        cordinate frame.
        
        @type: Ogre.Vector
        @param: The current force on the object, applied in the global 
                cordinate frame.
        """)
    
    gravity = core.Attribute(
        """
        Applied in global coordinates

        @type: number
        @param: The acceleration on the object due to gravity in meters per second
        """)
    
    position = core.Attribute(
        """
        @type: Ogre.Vector
        @param: The current position of the body in the global coordinate frame
        """) 
    
    orientation = core.Attribute(
        """
        @type: Ogre.Quaternion
        @param: The current orientation of the body in the global coordinate 
                frame
        """)
    
    velocity = core.Attribute(
        """
        @type: Ogre.Vector
        @param: The current velocity vector of the body
        """) 
    
    omege = core.Attribute(
        """
        @type: Ogre.Vector
        @param: The current angular velocity vector of the body
        """) 
    
    def add_local_force(force, pos):
        """
        The local force is applied at an offset to the Body, in its own local
        cordinate frame.
        
        @type  pos: Ogre.Vector3
        @param pos: Offset from the object
        
        @type  force: Ogre.Vector3
        @param force: Adds to the local force, applied in the bodies coordinate 
                      frame.
        """
        pass
        
    def set_local_force(force, pos):
        """
        @type  pos: Ogre.Vector3
        @param pos: Offset from the object
        
        @type  force: Ogre.Vector3
        @param force: The new local force, applied in the bodies coordinate 
                      frame.
        """
        pass

    def get_local_forces():
        """
        @rtype: list of tuples
        @return: A list of tuples that are force offset pairs
        """
        pass

    def add_global_force(force, pos):
        """
        The global force is applied at an offset to the Body, in the global 
        cordinate frame.
        
        @type  pos: Ogre.Vector3
        @param pos: Offset from the object
        
        @type  force: Ogre.Vector3
        @param force: Adds to the local force, applied in the global cordinate 
                      frame.
        """
        pass
        

    def set_global_force(sforce, pos):
        """
        @type  pos: Ogre.Vector3
        @param pos: Offset from the object
        
        @type  force: Ogre.Vector3
        @param force: Adds to the local force, applied in the global cordinate 
                      frame.
        """
        pass
        
    def get_global_forces():
        """
        @rtype: list of tuples
        @return: A list of tuples that are force offset pairs
        """
        pass
    
class Body(sim.util.Object):
    core.implements(IBody, IKMLStorable)
    
    @two_step_init
    def __init__(self):
        """
        If you call this, make sure to call create, before using the object
        """
        self._force = Ogre.Vector3(0,0,0)
        self._gravity = DEFAULT_GRAVITY
        self._local_force = []
        self._global_force = []
        self._buoyancy_plane = None
        self._body = None
    
        sim.util.Object.__init__(self)
    
    def init(self, parent, name, scene, shape_type, shape_props, mass,
             position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY):
        """
        @type  world: sim.physics.World
        @param world: The physical simulation to create the body in
        
        @type  shape_type: str
        @param shape_type: Denotes, the type of shape, like 'box' or 'sphere'
        
        @type  shape_props: Dict
        @param shape_props: Maps paramater name to value for the shapes 
                            constructor.
        """  
        sim.util.Object.init(self, parent, name)
        Body._create(self, scene, shape_type, shape_props, mass, position, 
                     orientation)
        
 
    def _create(self, scene, shape_type, shape_props, mass, position, 
                orientation):
        # Create Collision Object
        # TODO: Improve collision support
        col = None
        if shape_type == 'box':
            size = Ogre.Vector3(shape_props['size'])
            col = OgreNewt.Box(scene.world, size)
        
        self._body = scene.world.create_body(self, col, mass, position, 
                                             orientation)
        
    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (scene, parent, kml_node)
        """
        scene, parent, node = data_object
        
        # Load Object based values
        Object.load(self, (parent, node))
        
        physical_node = node['Physical']    
        # Find shape type and its properties
        shape_type = physical_node['Shape']['type'].lower() 
        shape_props = {}
        for param, value in physical_node['Shape'].iteritems():
            if param != 'type':
                shape_props[param] = value 
                    
        # Grab and validate Mass
        mass = physical_node['mass']
        if (type(mass) is not int) and (type(mass) is not float):
            raise SimulationError('Mass must be an interger of float')
        
        position, orientation = parse_position_orientation(node)
        
        Body._create(self, shape_type, shape_props, mass, position, orientation)
    
    def save(self, data_object):
        raise "Not yet implemented"
    
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
            
    class position(core.cls_property):
        def fget(self):
            return self._body.getPositionOrientation()[0]
            
    class orientation(core.cls_property):
        def fget(self):
            return self._body.getPositionOrientation()[1]   
        
    class velocity(core.cls_property):
        def fget(self):
            return self._body.getVelocity()
            
    class omega(core.cls_property):
        def fget(self):
            return self._body.getOmega()           
    
    def add_local_force(self, force, pos):
        self._local_force.append(Body._make_force_pos_pair(force, pos))
        
    def set_local_force(self, force, pos):
        self._local_force = [Body._make_force_pos_pair(force, pos)]
    
    def get_local_forces(self):
        return self._local_force
        
    def add_global_force(self, force, pos):
        self._global_force.append(Body._make_force_pos_pair(force, pos))

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
            
        
        
    
class Shape(object):
    pass

    
class World(OgreNewt.World):
    """
    This is a wrapper for the OgreNewt.World object, it provides a more managed
    existence.
    
    @warning: You must release all references to Body (or derived) objects 
              before the World object's last reference disappears.  Otherwise
              the bodies will have an invalid world and will cause a crash on 
              exit.
    
    @type world_count: number
    @cvar world_count: The number of worlds created
    
    @type world_map: Dict
    @cvar world_map: Maps world to int use for lookup of world from body in the
                    force callbacks.
                    
    @type bodies: Dict
    @cvar bodies: Maps Newton body to supplied Object
    """
    
    world_count = 0
    world_map = {}
    
    def __init__(self):
        OgreNewt.World.__init__(self)
        
        # Setup int -> world mapping
        self._world_num = World.world_count
        World.world_map[self._world_num ] = self
        World.world_count += 1
        
        self._bodies = {}
        
        # This is part of the temporary body hack
        self._body_count = 0
        
    def __del__(self):
        """
        Here to ensure that all body reference counts go to zero so that there 
        won't be any OgreNewt bodies without a valid world.
        """
        del self._bodies
        
    @staticmethod
    def remove_world(world):
        """
        Remove world from the global map of worlds
        """
        World.world_map.pop(world._world_num)
        
    def create_body(self, body, shape, mass, position = Ogre.Vector3.ZERO, 
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
        
        if not IBody.providedBy(body):
            raise PhysicsError("body must implement IBody interface")
        
        # For some reason we can't rely on the newton_body passed to the force 
        # callback to be the same one we create here.  We can't use the userdata
        # field, so we our just left with type.  Type is a single integer but
        # we need to be able to determine a the world and actual body from it.
        # So we pack the world number into the upper 8 bits, and the body number
        # into the lower 24.
        identity_number = self._body_count | (self._world_num << 24)
        
        new_body = OgreNewt.Body(self, shape, identity_number)
        new_body.setPositionOrientation(position, orientation)
        new_body.setCustomForceAndTorqueCallback(World._force_torque_callback, 
                                                 "")
        inertia = shape.calculateInertialMatrix()[0]
        new_body.setMassMatrix(mass, inertia)
        
        # Make the body number to our actual body object
        self._bodies[self._body_count] = body
        self._body_count += 1
        
        return new_body
    
    @staticmethod
    def _get_body_from_newt_body(newton_body):
        identity_number = newton_body.type
        
        # Here we need to upack out two numbers from the one number we have to 
        # work with.  world_num is in the upper 8 bits and body_num to lower 24
        world_num = int((0xFF000000 & identity_number) >> 24)
        body_num = int(0x00FFFFFF & identity_number)

        world = World.world_map[world_num]
        return world._bodies[body_num]
    
    @staticmethod
    def _force_torque_callback(newton_body):
        """
        This is called by newton for every body, we use this functions to 
        relieve the use of having to deal with this issue.
        
        @type  newton_body: OgreNewt.Body
        @param newton_body: The body to add force and torque to
        """
        body = World._get_body_from_newt_body(newton_body)
        
        # Apply forces
        newton_body.addForce(body.force)
        for force, pos in body.get_local_forces():
            newton_body.addLocalForce(force, pos)
        for force, pos in body.get_global_forces():
            newton_body.addGlobalForce(force, pos)
            
        # Apply gravity
        mass, inertia = newton_body.getMassMatrix()
        newton_body.addForce(body.gravity * mass)
        
        # Set bouyancy callback (only if object is bouyant
        if body.get_buoyancy_plane() is not None:
            nowton_body.addBouyancyForce(1000, 0.03, 0.03, body.gravity, 
                                         World._buoyancy_callback, "")
        
    @staticmethod
    def _buoyancy_callback(colID, newton_body, orient, pos, plane):
        body = World._get_body_from_newt_body(newton_body)
        bplane = body.get_buoyancy_plane()

        # we need to copy the normals and 'd' to the plane we were passed...
        plane.normal = bplane.normal
        plane.d = bplane.d
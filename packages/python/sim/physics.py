# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  sim/physics.py

# Library Imports
import ogre.physics.OgreNewt as OgreNewt
import ogre.renderer.OGRE as Ogre

# Project Imports
import core
import sim.util
import sim.defaults as defaults
from sim.object import Object, IObject
from sim.serialization import IKMLStorable, two_step_init, parse_position_orientation


class PhysicsError(sim.util.SimulationError):
    """
    The error exception class for all PhysicsErrors
    """

class IBody(IObject):
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
    
class ITrigger(IBody):
    """
    This is a massless body which detects when other objects go in and out of
    its space and sends event approriately
    """
    
class Body(Object):
    core.implements(IBody, IKMLStorable)

    _tree_mesh_hack = 0
    
    @two_step_init
    def __init__(self):
        """
        If you call this, make sure to call create, before using the object
        """
        self._force = Ogre.Vector3(0,0,0)
        self._gravity = defaults.gravity
        self._local_force = []
        self._global_force = []
        self._buoyancy_plane = None
        self._body = None
        self._old_velocity = Ogre.Vector3.ZERO
    
        Object.__init__(self)
    
    def init(self, parent, name, scene, shape_type, shape_props, mass,
             center_of_mass = Ogre.Vector3.ZERO, inertia = None,
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
        Object.init(self, parent, name)
        Body._create(self, scene, shape_type, shape_props, mass, center_of_mass,
                     inertia, position, orientation)
        
 
    def _create(self, scene, shape_type, shape_props, mass, center_of_mass, 
                inertia, position, orientation):
        # Create Collision Object
        # TODO: Improve collision support
        col = None
        if shape_type == 'box':
            size = Ogre.Vector3(shape_props['size'])
            col = OgreNewt.Box(scene.world, size)
        elif shape_type == 'mesh':
            # Fix this by later (we shouldn't need a node!!!)
            name = 'TREE_HACK' + str(Body._tree_mesh_hack)
            Body._tree_mesh_hack += 1
            mesh_name = shape_props['mesh_name']
            
            tree_ent = scene.scene_mgr.createEntity(name, mesh_name)
            temp_node = scene.scene_mgr.getRootSceneNode().createChildSceneNode()
            temp_node.attachObject(tree_ent)
            temp_node.setVisible(False)
            
            col = OgreNewt.TreeCollision(scene.world, temp_node, True)
            # When this works remove and destory our node temporary node!!
        else:
            raise PhysicsError, '"%s" is not a valid shape type' % shape_type
        
        self._body = scene.world.create_body(self, col, mass, center_of_mass,
                                             inertia, position, orientation)
        
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
        
        # Inertia and COM
        center_of_mass = physical_node.get('center_of_mass',Ogre.Vector3.ZERO)
        inertia = physical_node.get('inertia', None)
        
        Body._create(self, scene, shape_type, shape_props, mass, center_of_mass,
                     inertia, position, orientation)
    
    def save(self, data_object):
        raise "Not yet implemented"
    
    @staticmethod
    def _make_force_pos_pair(force, pos):
        return (Ogre.Vector3(force), Ogre.Vector3(pos))
    
    # IBody Methods
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
    
    class acceleration(core.cls_property):
        def fget(self):
            return self._old_velocity - self.velocity
    
    # Force Applying Methods
    class force(core.cls_property):
        def fget(self):
            return self._force
        def fset(self, force):
            self._force = Ogre.Vector3(force)
    
    def add_local_force(self, force, pos):
        self._local_force.append(Body._make_force_pos_pair(force, pos))
        
    def set_local_force(self, force, pos):
        if (force is None) and (pos is None):
            self._local_force = []
        else:
            self._local_force = [Body._make_force_pos_pair(force, pos)]
    
    def get_local_forces(self):
        return self._local_force
        
    def add_global_force(self, force, pos):
        self._global_force.append(Body._make_force_pos_pair(force, pos))

    def set_global_force(self, force, pos):
        if (force is None) and (pos is None):
            self._global_force = []
        else:
            self._global_force = [Body._make_force_pos_pair(force, pos)]
        
    def get_global_forces(self):
        return self._global_force
        
    def set_buoyancy(self, normal):
        """
        type  normal: Ogre.Vector3
        param normal: The normal of the buoyancy plane. If none, the object will
                     not be buoyanct
        """
        if normal is not None:
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
    
    def __init__(self):
        OgreNewt.World.__init__(self)
        self._bodies = []
    
    def __del__(self):
        """
        Here to ensure that all body reference counts go to zero so that there 
        won't be any OgreNewt bodies without a valid world.
        """
        del self._bodies
        
    def create_body(self, body, shape, mass, center_of_mass = Ogre.Vector3.ZERO, 
                    inertia = None, position = Ogre.Vector3.ZERO, 
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
        
        newt_body = OgreNewt.Body(self, shape)
        newt_body.setUserData(body)
        newt_body.setPositionOrientation(position, orientation)
        newt_body.setCustomForceAndTorqueCallback(World._force_torque_callback, 
                                                 "")
        if type(shape) is not OgreNewt.TreeCollision:
            if inertia is None:
                inertia = shape.calculateInertialMatrix()[0]
            newt_body.setMassMatrix(mass, inertia)
            newt_body.autoFreeze = False
            newt_body.centerOfMass = center_of_mass
        
        # Make the body number to our actual body object
        self._bodies.append(body)
        
        return newt_body
    
    @staticmethod
    def _force_torque_callback(newton_body):
        """
        This is called by newton for every body, we use this functions to 
        relieve the use of having to deal with this issue.
        
        @type  newton_body: OgreNewt.Body
        @param newton_body: The body to add force and torque to
        """
        body = newton_body.getUserData()
        body._old_velocity = body.velocity
        
        # Apply forces
        force = body.force
        if force != (0,0,0):
            newton_body.addForce(force)
        for force, pos in body.get_local_forces():
            newton_body.addLocalForce(force, pos)
        for force, pos in body.get_global_forces():
            newton_body.addGlobalForce(force, pos)
        
        # Todo apply torques
        
        # Damping hack
        newton_body.omega = newton_body.omega * 0.8
        if newton_body.omega.length() < 0.001:
            newton_body.omega = 0
        
        # Zero force on body
        body.force = (0,0,0)
        body.set_local_force(None, None)
        body.set_global_force(None, None)
        
        # Apply gravity
        mass, inertia = newton_body.getMassMatrix()
        newton_body.addForce(body.gravity * mass)
        
        # Set bouyancy callback only if object is bouyant
        if body.get_buoyancy_plane() is not None:
            newton_body.addBouyancyForce(1000, 0.03 , 0.03, body.gravity,
                                         World._buoyancy_callback, "")
        return True
        
    @staticmethod
    def _buoyancy_callback(colID, newton_body, orient, pos, plane):
        body = newton_body.getUserData()
        bplane = body.get_buoyancy_plane()

        # we need to copy the normals and 'd' to the plane we were passed...
        plane.normal = bplane.normal
        plane.d = bplane.d
        
        return True
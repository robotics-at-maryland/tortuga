# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/sim/physics.py

# Library Imports
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt


# Project Imports
import ram.sim as sim
import ram.core as core
import ram.event as event
import ram.sim.util as util
import ram.sim.defaults as defaults
from ram.sim.object import Object, IObject
from ram.sim.serialization import IKMLStorable, two_step_init, parse_position_orientation

class PhysicsError(util.SimulationError):
    """
    The error exception class for all PhysicsErrors
    """
    pass

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
    
    omega = core.Attribute(
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
        

    def set_global_force(force, pos):
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
    pass

class IWorldUpdateListener(core.Interface):
    """
    Called before and after each world update.
    """
    
#    def pre_update():
#        """
#        Call before world update
#        """
#        pass
#    
#    def post_update():
#        """
#        """
#        pass
    pass
    
class Body(Object):
    core.implements(IBody, IKMLStorable)

    _tree_mesh_hack = 0
    
    @two_step_init
    def __init__(self):
        """
        If you call this, make sure to call create, before using the object
        """
        self._force = Ogre.Vector3(0,0,0)
        self._omega = Ogre.Vector3(0,0,0)
        self._torque = Ogre.Vector3(0,0,0)
        self._gravity = defaults.gravity
        self._local_force = []
        self._global_force = []
        self._buoyancy_plane = None
        self._body = None
        self._material = None
        self._old_velocity = Ogre.Vector3.ZERO
        self._previous_omega = Ogre.Vector3.ZERO
    
        Object.__init__(self)
    
    def init(self, parent, name, scene, shape_type, shape_props, mass,
             center_of_mass = Ogre.Vector3.ZERO, inertia = None,
             position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY):
        """
        @type  scene: ram.sim.scene.Scene
        @param scene: The scene to create the body in
        
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
            size = sim.OgreVector3(shape_props['size'])
            col = OgreNewt.Box(scene.world, size)
        elif shape_type == 'cylinder':
            col = OgreNewt.Cylinder(scene.world, shape_props['radius'], 
                                    shape_props['height'])
        elif shape_type == 'mesh':
            # Fix this by later (we shouldn't need a node!!!)
            name = 'TREE_HACK' + str(Body._tree_mesh_hack)
            Body._tree_mesh_hack += 1
            mesh_name = shape_props['mesh_name']
            scale = sim.OgreVector3(shape_props.get('mesh_scale',
                                                    Ogre.Vector3(1,1,1)))
            
            tree_ent = scene.scene_mgr.createEntity(name, mesh_name)

            # Need extra node, for scaling error
            temp_node = \
                scene.scene_mgr.getRootSceneNode().createChildSceneNode()
            temp_node.attachObject(tree_ent)
            temp_node.setVisible(False)
            temp_node.setScale(scale)
            
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

        physical_node = node.get('Physical', None)
        if physical_node is None:
            msg = 'Object "%s" needs a "Phsyical" section' % node['name']
            raise PhysicsError(msg)

        # Find shape type and its properties
        shape_node = physical_node.get('Shape', None)
        if shape_node is None:
            msg = 'Object "%s" needs a "Physical.Shape" section' % node['name']
            raise PhysicsError(msg)
        
        shape_type = shape_node['type'].lower() 
        shape_props = {}
        for param, value in shape_node.iteritems():
            if param != 'type':
                shape_props[param] = value

        scale = shape_node.get('mesh_scale', None)
        if scale is not None:
            shape_props['mesh_scale'] = scale
        else:
            gfx_node = node.get('Graphical', {})
            shape_props['mesh_scale'] = gfx_node.get('scale', [1,1,1])
                    
        # Grab and validate Mass
        mass = physical_node.get('mass', 0)
        if (type(mass) is not int) and (type(mass) is not float):
            raise SimulationError('Mass must be an interger of float')
        
        position, orientation = parse_position_orientation(node)
        
        # Inertia and COM
        center_of_mass = physical_node.get('center_of_mass',Ogre.Vector3.ZERO)
        inertia = physical_node.get('inertia', None)
        
 
        Body._create(self, scene, shape_type, shape_props, mass, center_of_mass,
                     inertia, position, orientation)
        
        # Small hack, this should be integrated integrated better
        # I should probably reduce the number of constructor parameters as well
        material = physical_node.get('material', None)
        if material is not None:
            self._material = material
            material_id = scene.world.get_material_id(material)
            self._body.setMaterialGroupID(material_id)
        
    
    def save(self, data_object):
        raise "Not yet implemented"
    
    @staticmethod
    def _make_force_pos_pair(force, pos):
        return (sim.OgreVector3(force), sim.OgreVector3(pos))
    
    # IBody Methods
    class material(core.cls_property):
        def fget(self):
            return self._material
    
    class gravity(core.cls_property):
        def fget(self):
            return self._gravity
        def fset(self, gravity):
            self._gravity = sim.OgreVector3(gravity)
            
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
    
    class angular_accel(core.cls_property):
        def fget(self):
            return self._previous_omega - self.omega
    
    # Force Applying Methods
    class force(core.cls_property):
        def fget(self):
            return self._force
        def fset(self, force):
            self._force = sim.OgreVector3(force)
    
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
    
    # Apply Torque
    class torque(core.cls_property):
        def fget(self):
            return self._torque
        def fset(self, torque):
            self._torque = sim.OgreVector3(torque)
    
        
    def set_buoyancy(self, normal):
        """
        @type  normal: Ogre.Vector3
        @param normal: The normal of the buoyancy plane. If none, the object
                       will not be buoyanct
        """
        if normal is not None:
            self._buoyancy_plane = Ogre.Plane(normal, (0,0,0))
        else:
            self._buoyancy_plane = None
            
    def get_buoyancy_plane(self):    
        return self._buoyancy_plane

class Trigger(Body, OgreNewt.ContactCallback):
    core.implements(ITrigger, IWorldUpdateListener)
    
    _trigger_count = 0
    
    @two_step_init
    def __init__(self):
        self._contact_events = {}
        self._contactors = {}
        
        Body.__init__(self)
        OgreNewt.ContactCallback.__init__(self)
            
    def init(self, parent, name, scene, contact_properties,
             shape_type, shape_props, 
             position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY):
        """
        @param contact_properties: tuple (string, string, string)
        @type contact_properties: The first item is the material to contact 
            with the second is the enter event type, and the third item is the
            leave event type.
        """
        
        Body.init(self, None, name, scene, 0, shape_type, shape_props, 
                  position, orietnation)
        
        Trigger._create(self, scene, contact_properties)
    
    # IWorldUpdateListener methods
    def pre_update(self):   
        pass
    
    def post_update(self):
        new_contactors = {}
        for contact_info, contacting in self._contactors.iteritems():
            if contacting:
                new_contactors[contact_info] = False
            else:
                contactor, material = contact_info
                leave_event = self._contact_events[material][1]
                if leave_event is not None:
                    event.post(leave_event, contactor)
              
        self._contactors.clear()
        self._contactors.update(new_contactors)

    def _create(self, scene, contact_properties):
        """
        """
        # Nobody has given us a material yet, autogenerate one
        if self._material is None:
            self._material = 'Trigger' + str(Trigger._trigger_count)
            Trigger._trigger_count += 1
            material_id = scene.world.get_material_id(self._material)
            self._body.setMaterialGroupID(material_id)
        
        # Setup our selves as the callback for all the material pairs
        # we are supposed to trigger events for
        for material, enter_event, leave_event in contact_properties:
            
            # Make ourselves the contact callback for this material pair
            pair = scene.world.get_material_pair(self.material, material)
            pair.setContactCallback(self)
            
            # Store what to do when bodies enter and leave the area with this
            # material
            self._contact_events[material] = (enter_event, leave_event)
            
        # Register our self to be notified before and after phyiscs updates
        scene.world.add_update_listener(self)
        
        
        
    def load(self, data_object):
        Body.load(self, data_object)

        scene, parent, node = data_object

        # Extract contact information
        physical_node = node['Physical']
        contact_node = physical_node.get('contact_information', None)
        if contact_node is None:
            raise PhysicsError, "Trigger must a have 'contact_information' node"
        
        contact_properties = []
        for contact_material, contact_events in contact_node.iteritems():
            enter_event = contact_events[0]
            leave_event = None
            
            #print type(contact_events)    
            if type(contact_events) is not list:
                raise PhysicsError, "Contact events must be a list"
            if len(contact_events) > 1:
                leave_event = contact_events[1]
                
            contact_properties.append((contact_material, enter_event, 
                                       leave_event))
        
        self._create(scene, contact_properties)
                
        
    def save(self, data_object):
        raise "Not yet implemented"

    # OgreNewt.ContactCallback Method
    def userProcess(self):
        """
        Extract which body is which, send off 
        """
        # Determin which body is the trigger, and which is the contacting object
        me = self.m_body0.getUserData();
        contactor = self.m_body1.getUserData();
        
        # Our guess was wrong
        if ITrigger.providedBy(contactor):
            me, contactor = contactor, me # swap values
        
        # Store the contact information for latter event firing
        contact_material = contactor.material
        contact_info = (contactor, contact_material)
        
        # If this is the first time we are contacting post the event
        if not self._contactors.has_key(contact_info):
            enter_event = self._contact_events[contact_material][0]
            if enter_event is not None:
                event.post(enter_event, contactor) 
        self._contactors[contact_info] = True
             
        # Ignore all contacts
        return 0
    
    
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
                    
    @type bodies: Dict
    @cvar bodies: Maps Newton body to supplied Object
    """
    
    def __init__(self):
        OgreNewt.World.__init__(self)
        self._bodies = []
        # Maps material name to newton ID
        self._materials = {}
        self._material_pairs = {}
        self._listeners = set()
    
    def shutdown(self):
        """
        Here to ensure that all body reference counts go to zero so that there 
        won't be any OgreNewt bodies without a valid world.
        """
        del self._material_pairs
        del self._materials
        del self._listeners
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
    
    def get_material_id(self, material_str):
        """
        Return the newton material ID that is mapped to this string
        """
        try:
            return self._materials[material_str]
        except KeyError:
            id = OgreNewt.MaterialID(self)
            self._materials[material_str] = id
            return id
    
    def get_material_pair(self, material_a, material_b):
        """
        Returns the Newton material pair representing the two strings given
        """
        try:
            return self._material_pairs[(material_a, material_b)]
        except KeyError:
            a_id = self.get_material_id(material_a)
            b_id = self.get_material_id(material_b)
            pair = OgreNewt.MaterialPair(self, a_id, b_id)
            
            self._material_pairs[(material_a, material_b)] = pair
            self._material_pairs[(material_b, material_a)] = pair
            return pair
    
    def update(self, time_step):
        for listener in self._listeners:
            listener.pre_update()

        OgreNewt.World.update(self, time_step)
        for listener in self._listeners:
            listener.post_update()
    
    def add_update_listener(self, listener):
        """
        Called before and after each physics update to allow items to do book
        keeping.
        """
        if IWorldUpdateListener.providedBy(listener):
        #core.verifyClass(IWorldUpdateListener, listener)
            self._listeners.add(listener)
        
    def remove_update_listener(self, listener):
        #core.verifyClass(IWorldUpdateListener, listener)
        if IWorldUpdateListener.providedBy(listener):
            self._listeners.remove(listener)
    
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
        body._previous_omega = body.omega
        
        # Apply forces
        force = body.force
        if force != (0,0,0):
            newton_body.addForce(force)
        for force, pos in body.get_local_forces():
            newton_body.addLocalForce(force, pos)
        for force, pos in body.get_global_forces():
            newton_body.addGlobalForce(force, pos)
        
        # Apply torques
        torque = body.torque
        if torque != (0,0,0):
            newton_body.addTorque(torque)
            
        # Damping hack
        #newton_body.omega = newton_body.omega * 0.8
        #if newton_body.omega.length() < 0.001:
        #    newton_body.omega = (0,0,0)
        
        # Zero force on body
        body.force = (0,0,0)
        body.set_local_force(None, None)
        body.set_global_force(None, None)
        
        # Apply gravity
        mass, inertia = newton_body.getMassMatrix()
        newton_body.addForce(body.gravity * mass)
        
        # Set bouyancy callback only if object is bouyant
        if body.get_buoyancy_plane() is not None:
            newton_body.addBouyancyForce(1000, 0.10 , 0.10, body.gravity,
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

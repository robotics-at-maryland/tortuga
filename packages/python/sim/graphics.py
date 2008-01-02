# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  sim/graphics.py

"""
Wraps up the initialization and management of CEGUI and GUI activites
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Stdlib Imports
import os
import sys
import time
import logging

# Library Imports
import ogre.renderer.OGRE as Ogre
#import ogre.io.OIS as OIS

# Project Imports
import logloader    
import event
from core import fixed_update, Component, implements, Interface, Attribute, cls_property
from ram.sim.util import SimulationError
#from sim.input import KeyStateObserver
from sim.serialization import IKMLStorable, two_step_init, parse_position_orientation
from sim.object import IObject, Object
from sim.input import ButtonStateObserver, toggle

class GraphicsError(SimulationError):
    """ Error from the graphics system """
    pass

class IVisual(IObject):
    """ An object which you can see in the simulation"""
    pass

# TODO: Fill out the methods for the class

class Visual(Object):
    implements(IVisual, IKMLStorable)
    
    _plane_count = 0
    
    @two_step_init
    def __init__(self):
        self._node = None
        Object.__init__(self)

    def init(self, parent, name, scene, mesh, material,
             position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY,
             scale = Ogre.Vector3(1,1,1)):
        Object.init(parent, name)
        Visual._create(self, scene, mesh, material, position, orientation, 
                       scale)

    def _create(self, scene, mesh, material, position, orientation, scale):
        # Create the graphical representation of the object
        entity = scene.scene_mgr.createEntity(self.name, mesh)
        entity.setMaterialName(material)
        
        # Attach graphical entity to a new node in the scene graph
        self._node = scene.scene_mgr.getRootSceneNode().createChildSceneNode()
        self._node.attachObject(entity)

        # Apply scalling and normalized normals if object is actually scalled
        if scale != Ogre.Vector3(1,1,1):
            self._node.setScale(scale)
            entity.setNormaliseNormals(True)       
            
        self._node.position = position
        self._node.orientation = orientation
            
    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (scene, parent, kml_node)
        """
        scene, parent, node = data_object
        
        # Load Object based values
        Object.load(self, (parent, node))
        
        gfx_node = node['Graphical'] 
        mesh = gfx_node['mesh']
        material = gfx_node['material']
        scale = Ogre.Vector3(gfx_node.get('scale', Ogre.Vector3(1,1,1)))
        
        # Handle special mesh generation
        if mesh.startswith('PLANE'):
            if ':' in mesh:
                mesh = mesh.split(':')[1]
            else:
                mesh = 'Plane' + str(Visual._plane_count)
                
            norm = gfx_node['normal']
            width = gfx_node['width']
            height = gfx_node['height']
            plane = Ogre.Plane(norm, 0 );
            group_name = gfx_node.get('group', Ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)
            
            Ogre.MeshManager.getSingletonPtr().createPlane(mesh, \
                group_name, plane, width, height);
            Visual._plane_count += 1
        
        # Orientation defaults: IDENTITY, Position: (0,0,0)
        position, orientation = parse_position_orientation(node)
        Visual._create(self, scene, mesh, material, position, orientation, 
                       scale)
        
        # Check visibility
        if not gfx_node.get('visible', True):
            self._node.setVisible(False)
        
    def save(self, data_object):
        raise "Not yet implemented"


class ICamera(Interface):
    """
    Represents a camera in the simulation
    """
    
    camera = Attribute("""
        @type: Ogre.Camera
        @param: The wrapped Ogre Camera, set only.
        """)
    
    node = Attribute("""
        @type: Ogre.SceneNode
        @param: The scene node controller the camera, get only.
        """)
    
#    controller = core.Attribute("""
#        @type: implements ICameraController
#        @param: the current controller of the camera, set and get.
#        """)
    

class Camera(Component):
    implements(ICamera)
    
    def __init__(self, name, scene, position, offset, near_clip = 0.5):
        self._camera = scene.scene_mgr.createCamera(name)
        self._camera.position = offset
        self._camera.lookAt((0, 0, 0))
        self._camera.nearClipDistance = near_clip
                
        # Allows easier movement of camera
        self._node = scene.scene_mgr.getRootSceneNode().createChildSceneNode()
        self._node.position = (0,0,0)
        self._node.attachObject(self._camera)
        self._camera.lookAt(0, 0, 0)
    
        # position camera
        self._node.position = position
    
    #ICamera methods
    class camera(cls_property):
        def fget(self):
            return self._camera
        
    class node(cls_property):
        def fget(self):
            return self._node
        
#    class controller(core.cls_property):
#        def fget(self):
#            return self._controller
#        def fset(self, controller):
#            self._controller.release()
            

class CameraController(object):
    """
    Here we have our camera attached to a node, looking at then node.
    
    """
    
    event.add_event_types(['CAM_FORWARD', 'CAM_LEFT', 'CAM_BACK', 'CAM_RIGHT',
                           'CAM_UP', 'CAM_DOWN',
                           'CAM_TOGGLE_FOLLOW'])
    
    def __init__(self, camera):
        self._camera = camera.camera
        self._camera_node = camera.node
        self.original_parent = None
        
        self.handler_map = {
            'CAM_TOGGLE_FOLLOW' : self.test_follow }
#            'CAM_INDEPENDENT' : self._make_independent}
        
        event.register_handlers(self.handler_map)
        
        # This sets up automatic setting of the key down properties
        watched_buttons = {'_forward' : ['CAM_FORWARD'],
                           '_backward' : ['CAM_BACK'],
                           '_left' : ['CAM_LEFT'],
                           '_right' : ['CAM_RIGHT'],
                           '_down' : ['CAM_DOWN'],
                           '_up' : ['CAM_UP']}

        self.key_observer = ButtonStateObserver(self, watched_buttons)
    
    def __del__(self):
        # Make sure to remove event handlers so they are called after the 
        # object is gone
        event.remove_handlers(self.handler_map)
        pass
    
    def update(self, time_since_last_frame):
        quat = self._camera_node.getOrientation()
        
        # A really bad way to generate the rotation vectors I want
        trans = quat * Ogre.Vector3(0,0,-5) * time_since_last_frame
        trans.y = 0
        
        strafe = quat * Ogre.Vector3(5,0,0) * time_since_last_frame
        strafe.y = 0
        
        height = quat * Ogre.Vector3(0,5,0) * time_since_last_frame
        height.z = 0
        
        if self.original_parent is None:
            if self._forward:
                self._camera_node.translate(trans, Ogre.Node.TS_WORLD)
            if self._backward:
                self._camera_node.translate(trans * -1.0, Ogre.Node.TS_WORLD)
                
            if self._up:
                self._camera_node.translate(height, Ogre.Node.TS_WORLD)
            if self._down:
                self._camera_node.translate(height * -1.0, Ogre.Node.TS_WORLD)
     
            if self._left:
                self._camera_node.translate(strafe * -1.0, Ogre.Node.TS_WORLD)
            if self._right:
                self._camera_node.translate(strafe, Ogre.Node.TS_WORLD)
                
    @toggle('CAM_TOGGLE_FOLLOW')
    def test_follow(self, state):
        print 'CAM_TOGGLE',state
    
    def _mouse_moved(self, arg):
        """
        If the shift key is down, swing the camera
        """
        
        if self.shift_key:
            ms = arg.get_state()
            # Rotate around our object
            self._camera_node.pitch(Ogre.Radian(ms.Y.rel * -0.5))
            self._camera_node.yaw(Ogre.Radian(ms.X.rel * -0.5), 
                                 Ogre.Node.TS_WORLD)
            
            # Zoom in or out of our objective
            if ms.Z.rel < 0 or ms.Z.rel > 0:
                pos = self._camera.position
                self._camera.setPosition(pos + (pos * ms.Z.rel * 0.002))
                
    def _follow_node(self, node):
        if self.original_parent is not None:
            raise GraphicsError('Camera is already free')
        
        # Remove node from its current parent
        self.original_parent = self._camera_node.parent
        self.original_parent.removeChild(self._camera_node)
        
        # Reparent node and 
        node.addChild(self._camera_node)
        self._camera_node.setPosition(Ogre.Vector3(0,0,0))
        #self.camera_node = node
        #self.camera_node.attachObject(self.camera)
        self._camera.lookAt(node._getDerivedPosition())
        
    def _make_independent(self):
        if self.original_parent is None:
            raise GraphicsError('Camera is already free floating')
        
        self._camera_node.parent.removeChild(self._camera_node)
        self.original_parent.addChild(self._camera_node)
        self._camera.lookAt(self._camera_node._getDerivedPosition())
        self.original_parent = None

class Py2OgreLog(Ogre.Log):
    """
    This pipes Ogre's logging into the python logging system and supresses 
    normal console and file output.
    """
    def __init__(self, config, logger):
        # Call to the C++ base class, 
        # First Bool - console output, second - no file output
        Ogre.Log.__init__(self, os.path.join('..', "logs", "ogre.txt"), False, 
                          False)
        self.config = config
        self.logger = logger
        
        self.log_level_map = {Ogre.LML_CRITICAL : self.logger.critical,
                              Ogre.LML_NORMAL : self.logger.info,
                              Ogre.LML_TRIVIAL : self.logger.debug}
        
    def logMessage(self, message, level, debug):
        self.log_level_map[level](message)
        
               
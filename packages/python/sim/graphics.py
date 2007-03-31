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
import Ogre
import OIS

# Project Imports
import logloader    
import event
from core import fixed_update, Component, implements
import sim.simulation as simulation
from sim.input import KeyStateObserver
from sim.serialization import IKMLStorable, two_step_init, parse_position_orientation
import sim.util

class GraphicsError(simulation.SimulationError):
    """ Error from the graphics system """
    pass

class IVisual(sim.util.IObject):
    """ An object which you can see in the simulation"""
    pass

# TODO: Fill out the methods for the class

class Visual(sim.util.Object):
    implements(IVisual, IKMLStorable)
    
    @two_step_init
    def __init__(self):
        self._node = None
        sim.util.Object.__init__()

    def init(self, parent, name, scene, mesh, material,
            position = Ogre.Vector3.ZERO, 
            orientation = Ogre.Quaternion.IDENTITY,
            scale = Ogre.Vector3(1,1,1)):
        sim.util.Object.init(parent, name)
        Visual._create(self, scene, mesh, material, position, orientation, 
                       scale)

    def _create(self, scene, mesh, material, position, orientation, scale):
        
        # Create the graphical representation of the object
        entity = scene.scene_mgr.createEntity(name, mesh)
        entity.setMaterialName(material)
        
        # Attach graphical entity to a new node in the scene graph
        self._node = scene.scene_mgr.getRootSceneNode().createChildSceneNode()
        self._node.attachObject(entity)

        # Apply scalling and normalized normals if object is actually scalled
        if scale != Ogre.Vector3(1,1,1):
            self._node.setScale(scale)
            self._node.setNormaliseNormals(True)       
            
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
        scale = Ogre.Vector3(gfx_node['scale'])
        
        position, orientation = parse_position_orientation(node)
        Visual._create(self, scene, mesh, material, position, orientation, scale)
        
    def save(self, data_object):
        raise "Not yet implemented"

class GraphicsSystem(object):
    """
    This class handles all initialization of graphics with the Ogre rendering
    engine.  It is driven completely by config file.  Current a bug in 
    Python-Ogre means with have the rendersystem config dialog.
    """    
    def __init__(self, config, create_window = True):      
        self.root = None
        self.render_window = None
        self.scene_manager = None
        self.logManager = None
        self.ogre_log = None
        self.own_window = create_window
        
        # Setups Ogre, step by step
        self._setup_logging(config.get('Logging', {'name' : 'Graphics',
                                                   'level': 'INFO'}))
        
        self.logger.info('* * * Beginning initialization')
        self._setup(config)
        self.camera_controller = CameraController(self.camera, self.camera_node)
        self.logger.info('* * * Initialized')
        
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        # Make sure the C++ based objects are deleted in the proper order
        
        #del self.camera_controller
        #del self.scene_manager
        del self.root
        #del self.render_window  
        del self.logManager
        self.logger.info('* * * Shutdown complete, closing log')
        del self.ogre_log
        
    def update(self, time_since_last_update):
        """
        Renders one frame
        """
        Ogre.WindowEventUtilities.messagePump()
        self.root.renderOneFrame()
        self.camera_controller.update(time_since_last_update)
        return True
    
    def create_window(self, name, width, height, params):
        """
        This creates a new render window.  If the post render window setup has
        not already been done, it will be done with the creation of the first
        window.
        """
        
        render_system = self.root.getRenderSystem()
        new_window = render_system.createRenderWindow(name, width, height, 
                                                      false, params)
        
        if render_window is None:
            self._post_window_setup()
            render_window = new_window
            
        return new_window
    
    def _setup(self, config):
        self.root = Ogre.Root("");
        
        self._load_plugins(config['Plugins'])
        self._addResourceLocations(config['Resources']);        
        self._init_ogre_core(config['RenderSystem'])
    
        if self.own_window:
            self._post_window_setup()
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
        # Broken in Python-Ogre 0.70
        #logManager = Ogre.LogManager.getSingletonPtr()
        self.logManager = Ogre.LogManager()
 
        # Create That, doesn't output anything, just calls its listeners
        self.ogre_log = Py2OgreLog(config, self.logger) 
        self.logManager.setDefaultLog(self.ogre_log)
        
    def _load_plugins(self, config):
        """
        This loads the plugins Ogre needs to run based on the config file.
        Here is an example:
        Plugins:
            # Where to search for the plugins
            search_path: [ C:\Libraries\Python-Ogre-0.7\plugins,
                           C:\Developement\Python-Ogre\Python-Ogre-0.7\plugins]
               
            # The plugins to load
            plugins: [ RenderSystem_GL, 
                       Plugin_ParticleFX, 
                       Plugin_OctreeSceneManager ]   
        """
        import platform
        
        # Filter out non-existant paths from search, this keeps windows paths
        # out of unix and unix paths out of windows
        
        search_path = [p for p in config['search_path'] if os.path.exists(p)]
        self.logger.info('Loadings Ogre Plugins on path:')
        for path in search_path: 
            self.logger.info('\t%s' % path )
        
        extension = '.so'
        if 'Windows' == platform.system():
            extension = '.dll'
            
        for plugin in config['plugins']:
            plugin_name = plugin + extension
            self.logger.info('\tSearching for: %s' % plugin_name)
            found = False
            
            for dir in search_path:
                plugin_path = os.path.join(dir, plugin_name)
                
                if os.path.exists(plugin_path):
                    self.root.loadPlugin(plugin_path)
                    found = True
                
            if not found:
                raise GraphicsError('Could not load plugin: %s' % plugin_name)
    
    def  _addResourceLocations(self, config):
        """
        This replaces Ogre's resources.cfg, Each node under resources
        is a group and each node under that is an archive type, with a 
        list of directories to load for that type. Example:
        
        Ogre:
            Resources:
                General:
                    Zip: ['media/packs/OgreCore.zip',
                              'media/packs/cubemapsJS.zip']
                    FileSystem: ['media/models',
                                 'media/primitives',
                                 'media/materials/textures',
                                 'media/materials/scripts']
        """
        rsrcMrg = Ogre.ResourceGroupManager.getSingleton()
        for group in config:
            group_cfg = config[group];
            for type in group_cfg:
                for location in group_cfg[type]:
                    location = os.path.abspath(location)
                    rsrcMrg.addResourceLocation(location, type, group)
                    
    def _init_ogre_core(self, config):
        """
        This secion here takes over for Ogre's ogre.cfg.  The type 
        indicates which Ogre RenderSystem to create and which section to
        load the options from.  Example:
        
        Ogre:
            RenderSystem:
                type: GLRenderSystem
                
                GLRenderSystem:
                    - [Colour Depth, '32']
                    - [Display Frequency, 'N/A']
                    - [FSAA, '0']
                    - [Full Screen, 'No']
                    - [RTT Preferred Mode, 'FBO']
                    - [VSync, 'No']

        """
        
        # Allows looser specification of RenderSystem names
        typemap = {'GLRenderSystem' : 'GLRenderSystem',
                   'OpenGL' : 'GLRenderSystem',
                   'DirectX' : 'D3D9RenderSystem',
                   'DirectX9' : 'D3D9RenderSystem',
                   'Direct 3D' : 'D3D9RenderSystem'}
                   
        try:
            type = typemap[config['type'].replace(' ','')]
            # Todo: replace this with proper render system iteration
            #RenderSystemCls = getattr(ogre, type)
        except AttributeError, KeyError:
            raise GraphicsError('Improper Ogre RenderSystem')
        
        #renderSystem = RenderSystemCls()
        # Load our options from the custom config system          
        #for name, value in config[str(RenderSystemCls)]:
        #    renderSystem.setConfigOption(name, value])

        # If we have gotten this far, the we can let Ogre create a default
        # window from the settings loaded above
        #self.root.setRenderSystem(renderSystem)
        
        # Small hack until we can iterater render systems
        ogrecfg = file('ogre.cfg','w')
        if type == 'GLRenderSystem':
            ogrecfg.write("Render System=OpenGL Rendering Subsystem\r\n\r\n"
                          "[OpenGL Rendering Subsystem]\r\n")
        else:
            ogrecfg.write("Render System=Direct3D9 Rendering Subsystem\r\n\r\n"
                          "[Direct3D9 Rendering Subsystem]\r\n")
        for name, value in config[type]:
            ogrecfg.write('%s=%s\r\n'% (name,value)) 
        ogrecfg.close()
        
        restored = self.root.restoreConfig()
        os.remove(ogrecfg.name)
        
        if not restored:
            if not self.root.showConfigDialog():
                raise GraphicsError('Could not initialize render system')
            
        if self.own_window:
            self.render_window = self.root.initialise(True, "RAM AUV SIM")
        else:
            self.root.initialise(False)
        
    def _post_window_setup(self):
        self.scene_manager = self.root.createSceneManager(Ogre.ST_GENERIC,
                                                        'SimSceneMgr')
        
        if self.own_window:
            # Possibly considate/config drive these
            self._createCamera()
            self._createViewports()

        # Set default mipmap level (NB some APIs ignore this)
        Ogre.TextureManager.getSingleton().setDefaultNumMipmaps(5)
        # Initialise resources
        Ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        
    def _createCamera(self):
        """
        Creates the camera.
        """        
        
        self.camera = self.scene_manager.createCamera('PlayerCam')
        self.camera.setPosition(Ogre.Vector3( 0.0, 5.0, 5.0))
        self.camera.lookAt(Ogre.Vector3(0, 0, 0))
        self.camera.nearClipDistance = 0.5
                
        # Allows easier movement of camera
        self.camera_node = self.scene_manager.getRootSceneNode().createChildSceneNode()
        self.camera_node.setPosition(0,0,0)
        self.camera_node.attachObject(self.camera)

    def _createViewports(self):
        """
        Creates the Viewport.
        """
        self.viewport = self.render_window.addViewport(self.camera)
        self.viewport.BackgroundColour = Ogre.ColourValue(0,0,0)

event.add_event_types(['CAM_FORWARD', 'CAM_LEFT', 'CAM_BACK', 'CAM_RIGHT',
                       'CAM_FOLLOW',       # A hack, follow the given node
                       'CAM_INDEPENDENT']) # Return back to free floating

class CameraController(object):
    """
    Here we have our camera attached to a node, looking at then node.
    
    """
    def __init__(self, camera, camera_node):
        self._update_interval = 1.0 / 60;
        
        self.camera = camera
        self.camera_node = camera_node
        self.original_parent = None
        
        self.handler_map = {
            'KEY_PRESSED': self._key_pressed,
            'KEY_RELEASED': self._key_released,
            'MOUSE_MOVED': self._mouse_moved,
            'CAM_FOLLOW' : self._follow_node,
            'CAM_INDEPENDENT' : self._make_independent}
        
        event.register_handlers(self.handler_map)
        
        # This sets up automatic setting of the key down properties
        watched_keys = [('shift_key', [OIS.KC_LSHIFT, OIS.KC_RSHIFT]), 
                        ('forward_key', [OIS.KC_W, OIS.KC_UP]), 
                        ('left_key', [OIS.KC_A, OIS.KC_LEFT]), 
                        ('back_key', [OIS.KC_DOWN, OIS.KC_S]), 
                        ('right_key', [OIS.KC_D, OIS.KC_RIGHT]),
                        ('up_key', [OIS.KC_Q]),
                        ('down_key', [OIS.KC_E])]
        self.key_observer = KeyStateObserver(self, watched_keys)
    
    def __del__(self):
        # Make sure to remove event handlers so they are called after the 
        # object is gone
        event.remove_handlers(self.handler_map)
    
    @fixed_update('_update_interval')
    def _update(self, time_since_last_frame):
        quat = self.camera_node.getOrientation()
        
        # A really bad way to generate the rotation vectors I want
        vec = Ogre.Vector3(0,0,-0.2)
        trans = quat * vec
        trans.y = 0
        
        vec = Ogre.Vector3(0.2,0,0)
        strafe = quat * vec
        strafe.y = 0
        
        vec = Ogre.Vector3(0,0.2,0)
        height = quat * vec
        height.z = 0
        
        if self.original_parent is None:
            if self.up_key:
                self.camera_node.translate(height, Ogre.Node.TS_WORLD)
            if self.down_key:
                self.camera_node.translate(height * -1.0, Ogre.Node.TS_WORLD)
            if self.forward_key:
                self.camera_node.translate(trans, Ogre.Node.TS_WORLD)
            if self.back_key:
                self.camera_node.translate(trans * -1.0, Ogre.Node.TS_WORLD)
            if self.left_key:
                self.camera_node.translate(strafe * -1.0, Ogre.Node.TS_WORLD)
            if self.right_key:
                self.camera_node.translate(strafe, Ogre.Node.TS_WORLD)
    
    def _mouse_moved(self, arg):
        """
        If the shift key is down, swing the camera
        """
        
        if self.shift_key:
            ms = arg.get_state()
            # Rotate around our object
            self.camera_node.pitch(Ogre.Radian(ms.Y.rel * -0.5))
            self.camera_node.yaw(Ogre.Radian(ms.X.rel * -0.5), 
                                 Ogre.Node.TS_WORLD)
            
            # Zoom in or out of our objective
            if ms.Z.rel < 0 or ms.Z.rel > 0:
                pos = self.camera.position
                self.camera.setPosition(pos + (pos * ms.Z.rel * 0.002))

    def _key_pressed(self, key_event):
        # Update the state of *_key properties  
        self.key_observer.key_pressed(key_event)

    def _key_released( self, key_event ):
        # Update the state of *_key properties
        self.key_observer.key_released(key_event)
        
    def _follow_node(self, node):
        if self.original_parent is not None:
            raise GraphicsError('Camera is already free')
        
        # Remove node from its current parent
        self.original_parent = self.camera_node.parent
        self.original_parent.removeChild(self.camera_node)
        
        # Reparent node and 
        node.addChild(self.camera_node)
        self.camera_node.setPosition(Ogre.Vector3(0,0,0))
        #self.camera_node = node
        #self.camera_node.attachObject(self.camera)
        self.camera.lookAt(node._getDerivedPosition())
        
    def _make_independent(self):
        if self.original_parent is None:
            raise GraphicsError('Camera is already free floating')
        
        self.camera_node.parent.removeChild(self.camera_node)
        self.original_parent.addChild(self.camera_node)
        self.camera.lookAt(self.camera_node._getDerivedPosition())
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
        
               
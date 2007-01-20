# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/graphics.py

"""
Wraps up the initialization and management of CEGUI and GUI activites
"""

import os
import sys
import time
import logging

import Ogre
import OIS

import logloader
import event
from vehicle.sim.core import SimulationError
from vehicle.sim.input import KeyStateObserver

class GraphicsError(SimulationError):
    """ Error from the graphics system """
    pass

class GraphicsSystem(object):
    """
    This class handles all initialization of graphics with the Ogre rendering
    engine.  It is driven completely by config file.  Current a bug in 
    Python-Ogre means with have the rendersystem config dialog.
    """    
    def __init__(self, config):      
        self.camera = None
        self.root = None
        self.render_window = None
        self.scene_manager = None
        self.logManager = None
        self.ogre_log = None
        
        # Setups Ogre, step by step
        self._setup(config)
        
        self.camera_controller = CameraController(self.camera, self.camera_node)
        
    def __del__(self):
        # Make sure the C++ based objects are deleted in the proper order
        del self.camera;
        del self.scene_manager
        del self.root
        del self.render_window  
        del self.logManager
        del self.ogre_log
        
    def update(self, time_since_last_update):
        """
        Renders one frame
        """
        self.root.renderOneFrame()
        self.camera_controller.update()
        return True
    
    def _setup(self, config):
        self._setup_logging(config["Logging"])  # Broken in Python-Ogre 0.70
        self.root = Ogre.Root("");
        
        self._load_plugins(config['Plugins'])
        self._addResourceLocations(config['Resources']);        
        self._initOgreCore(config['RenderSystem'])
        self.scene_manager = self.root.createSceneManager(Ogre.ST_GENERIC,
                                                        'SimSceneMgr')
        
        # Possibly considate/config drive these
        self._createCamera();
        self._createViewports();

        # Set default mipmap level (NB some APIs ignore this)
        Ogre.TextureManager.getSingleton().setDefaultNumMipmaps(5)
        # Initialise resources
        Ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
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
                    
    def _initOgreCore(self, config):
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
            
        self.render_window = self.root.initialise(True, "MRBC AUV SIM")
        
    def _createCamera(self):
        """
        Creates the camera.
        """        
        self.camera = self.scene_manager.createCamera('PlayerCam')
        self.camera.setPosition(Ogre.Vector3( 0.0, 0.0, 0.0))
        self.camera.lookAt(Ogre.Vector3(0, 0, -300))
        self.camera.nearClipDistance = 0.5
                
        # Allows easier movement of camera
        self.camera_node = self.scene_manager.getRootSceneNode().createChildSceneNode()
        self.camera_node.attachObject(self.camera)

    def _createViewports(self):
        """
        Creates the Viewport.
        """
        self.viewport = self.render_window.addViewport(self.camera)
        self.viewport.BackgroundColour = Ogre.ColourValue(0,0,0)

event.add_event_type(['CAM_FORWARD', 'CAM_LEFT', 'CAM_BACK', 'CAM_RIGHT'])

class CameraController(object):
    def __init__(self, camera, camera_node):
        self.camera = camera
        self.camera_node = camera_node
        
        self.handler_map = {
            'KEY_PRESSED': self._key_pressed,
            'KEY_RELEASED': self._key_released,
            'MOUSE_MOVED': self._mouse_moved}
        
        event.register_handlers(self.handler_map)
        
        # This sets up automatic setting of the key down properties
        watched_keys = [('shift_key', [OIS.KC_LSHIFT, OIS.KC_RSHIFT]), 
                        ('up_key', [OIS.KC_W, OIS.KC_UP]), 
                        ('left_key', [OIS.KC_A, OIS.KC_LEFT]), 
                        ('down_key', [OIS.KC_DOWN, OIS.KC_S]), 
                        ('right_key', [OIS.KC_D, OIS.KC_RIGHT])]
        self.key_observer = KeyStateObserver(self, watched_keys)
    
    def __del__(self):
        # Make sure to remove event handlers so they are called after the 
        # object is gone
        event.remove_handlers(self.handler_map)
    
    def update(self):
        quat = self.camera_node.getOrientation()
        vec = Ogre.Vector3(0.0,0.0,-0.2)
        trans = quat * vec
        vec = Ogre.Vector3(0.2,0.0,0.0)
        strafe = quat * vec
        
        if self.up_key:
            self.camera_node.translate(trans)
        if self.down_key:
            self.camera_node.translate(trans * -1.0)
        if self.left_key:
            self.camera_node.translate(strafe * -1.0)
        if self.right_key:
            self.camera_node.translate(strafe)
    
    def _mouse_moved(self, arg):
        """
        If the shift key is down, swing the camera
        """
        if self.shift_key:
            ms = arg.get_state()
            self.camera_node.pitch(Ogre.Radian(ms.Y.rel * -0.5))
            self.camera_node.yaw(Ogre.Radian(ms.X.rel * -0.5), 
                                 Ogre.Node.TS_WORLD)

    def _key_pressed(self, key_event):
        # Update the state of *_key properties  
        self.key_observer.key_pressed(key_event)

    def _key_released( self, key_event ):
        # Update the state of *_key properties
        self.key_observer.key_released(key_event)

class Py2OgreLog(Ogre.Log):
    """
    This pipes Ogre's logging into the python logging system and supresses 
    normal console and file output.
    """
    def __init__(self, config, logger):
        # Call to the C++ base class
        Ogre.Log.__init__(self, os.path.join('..', "logs", "ogre.txt"), False, 
                          False)
        self.config = config
        self.logger = logger
        
        self.log_level_map = {Ogre.LML_CRITICAL : self.logger.critical,
                              Ogre.LML_NORMAL : self.logger.info,
                              Ogre.LML_TRIVIAL : self.logger.debug}
        
    def logMessage(self, message, level, debug):
        self.log_level_map[level](message)
        
               
# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/sim/simulation.py

"""
This module provides the core functionallity of the simulation
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Stdlib Imports
import os
import sys
import imp
import warnings

# Library imports
warnings.simplefilter('ignore', RuntimeWarning)
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt
warnings.simplefilter('default', RuntimeWarning)

# Project Imports
import logloader
import event
import ram.sim.scene as scene
from ram.sim.input import InputSystem
from ram.sim.util import SimulationError
import ram.sim.defaults as defaults

from core import Singleton, log, log_init, implements, Component, environmentSub
from ram.module import Module, IModule

# Events
event.add_event_types(['SIM_UPDATE',
                       'OGRE_RENDERER_CREATED']) 

class GraphicsError(SimulationError):
    """ Error from the graphics system """
    pass

class Simulation(Singleton, Module):
    """
    The root object of the simulation, this is a singleton object just use
    'Simulation.get()' to access it.
    """
    implements(IModule)
    
    #@log_init(defaults.simulation_log_config)
    def init(self, config = {}):
        self._ogre_root = None
        self._scenes = {}
        self._config = config
        
        self._graphics_init(config.get('Graphics', {}))
        self.input_system = InputSystem(config.get('Input', {}))

        config['name'] = 'Simulation'
        Module.__init__(self, config)
    
#    def __del__(self):
#        # Ensure proper of C++ desctuctors
#        # TODO: Ensure I have to do this, I might be able to ignore this
#        del self._scenes
#        del self._ogre_root
   
    #@log('* * * Beginning shutdown', '* * * Shutdown complete') 
    def shutdown(self):
        print 'Beginning shutdown'
        Module.pause(self)
        
        # Close all scenes
        print 'Destoying Scenes'
        for scene in self._scenes.itervalues():
            scene.destroy()
        
        # Release references then shutdown
        print 'Releasing scene references'
        del self._scenes
        print 'Shutting down Ogre'
        self._ogre_root.shutdown()
        del self._ogre_root
        
    def update(self, time_since_last_update):
        """
        Updates all simulation components with the given timesteps
        
        @type time_since_last_update: a decimal number
        @param time_since_last_update: name says it all
        """
        if self._running:
            Ogre.WindowEventUtilities.messagePump()
   
            for scene in self._scenes.itervalues():
                scene.update(time_since_last_update)
   
            event.send('SIM_UPDATE', time_since_last_update)
        return self._running
    
    def iterscenes(self):
        """
        Iterate over the scenes: (name, scene)
        """
        for name, scene in self._scenes.iteritems():
            yield (name, scene)
    
    def get_scene(self, name):
        """
        @type name: string
        @param name: The name of the scene to retrive
        
        @rtype: None or sim.Scene
        @return: None if scene doesn't exist
        """
        return self._scenes.get(name, None)
    
    def create_all_scenes(self):
        """
        This load all scenes present in the config file.
        """
        scene_path = self._config.get('scene_path', defaults.scene_search_path)

        # Interpolate scene path
        scene_path = [os.path.normpath(environmentSub(p)) for p in scene_path]
        print 'TEST -1', scene_path
        for name, scene_file in self._config.get('Scenes', {}).iteritems():
            print 'TEST -2', scene_path
            self.create_scene(name, scene_file, scene_path)
    
    def create_scene(self, name, scene_file, scene_path):
        """
        @type name: string
        @param name: The name of the scene to create
        
        @type scene_file: string
        @param scene_file: the name of the scene file
        
        @type scene_path: list of strings
        @param scene_path: The path to search for the scene_file on
        """
        
        # Filter out non-existant paths from search, this keeps windows paths
        # out of unix and unix paths out of windows
        search_path = [p for p in scene_path if os.path.exists(p)]
        print 'TEST',search_path,len(search_path)
        if len(search_path) == 0:
            raise SimulationError('No valid directory found on scene path')
        
        #self.logger.info('Loading %s on path:', scene_file)
        #for path in search_path: 
        #    self.logger.info('\t%s' % path )
        
        found = False
        for dir in search_path:
            scene_path = os.path.abspath(os.path.join(dir, scene_file))
         
            if os.path.exists(scene_path):
                self._scenes[name] = scene.Scene(name, scene_path)
                found = True
        
        if not found:
            raise SimulationError('Could not find scene file: %s' % scene_file)
    
    
    # ----------------------------------------------------------------------- #
    #            G R A P H I C S   I N I T I A L I Z A T I O N                #
    # ----------------------------------------------------------------------- #
    
    def _graphics_init(self, config):
        # Create Ogre.Root singleton with no default plugins
        self._ogre_root = Ogre.Root("");
        
        # Start up Ogre piecewise, passing a default empty config if needed
        self._load_ogre_plugins(config.get('Plugins', {}))
        self._create_render_system(config.get('Rendering',{}))
        self._ogre_root.initialise(False)
        
    def _load_ogre_plugins(self, config):
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
        search_path = config.get('search_path', defaults.ogre_plugin_search_path)
        # Run Environment varialbe replacement
        search_path = [os.path.normpath(environmentSub(p)) for p in search_path]
        
        search_path = [p for p in search_path if os.path.exists(p)]
        if len(search_path) == 0:
            raise GraphicsError('All plugin directories do not exist')
        
        #self.logger.info('Loadings Ogre Plugins on path:')
        #for path in search_path: 
        #    self.logger.info('\t%s' % path )
        
        
        extension = '.so'
        if 'nt' == os.name:
            extension = '.dll'
            
        for plugin in config.get('plugins', defaults.ogre_plugins):
            plugin_name = plugin + extension
            #self.logger.info('\tSearching for: %s' % plugin_name)
            found = False
            
            for dir in search_path:
                plugin_path = os.path.join(dir, plugin_name)
                
                # Only the plugin once
                if not found and os.path.exists(plugin_path):
                    self._ogre_root.loadPlugin(plugin_path)
                    found = True
                
            if not found:
                raise GraphicsError('Could not load plugin: %s' % plugin_name)
            
    def _create_render_system(self, config):
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
        typemap = {'GL' : 'OpenGL Rendering Subsystem',
                   'GLRenderSystem' : 'OpenGL Rendering Subsystem',
                   'OpenGL' : 'OpenGL Rendering Subsystem',
                   'DirectX' : 'D3D9RenderSystem',
                   'DirectX9' : 'D3D9RenderSystem',
                   'Direct 3D' : 'D3D9RenderSystem'}
        
        # Attempt to find the selected renderer based on given input
        render_system = None
        try:
            type = config.get('type', defaults.render_system)
            type_name = typemap[type]
            for renderer in self._ogre_root.getAvailableRenderers():
                if type_name == renderer.getName():
                    render_system = renderer
                
            if render_system is None:
                raise GraphicsError("Could not load " + type + " make sure "
                                    "the proper plugin is loaded")
        except AttributeError, KeyError:
            raise GraphicsError('"%s" is not a valid render system' % type)
        
        self._ogre_root.setRenderSystem(render_system)
        
        
        # Load our options from the custom config system
        render_system_opts = config.get(type, defaults.render_system_options)          
        for name, value in render_system_opts:
            render_system.setConfigOption(name, value)

        # Give Ogre our new render system 
        self._ogre_root.setRenderSystem(render_system)
        
        event.send('OGRE_RENDERER_CREATED')


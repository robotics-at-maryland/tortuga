# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   /sim/core.py

"""
This module provides the core functionallity of the simulation
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Stdlib Imports
import os
import sys
import imp

# Project Imports
import logloader
import event

from core import Singleton, property

# Events
event.add_event_types('SIM_SHUTDOWN') # Called to shutdown the simulation

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

class Simulation(Singleton):
    """
    The root object of the simulation, this is a singleton object just use
    'Simulation.get()' to access it.
    """
    
    class graphics_sys(property):
        """
        The object that handles the graphical side of the simulation
        """
        def fget(self):
            return self._graphics_sys
    
    class physics_sys(property):
        """
        The object that handles the physics side of the simulation
        """
        def fget(self):
            return self._physics_sys
        
    class input_sys(property):
        """
        The object that handles all user input into the simulation
        """
        def fget(self):
            return self._input_sys
    
    def init(self, config):
        self._components = None
        self._scene = None
        self._gui_sys = None
        self._input_sys = None
        self._physics_sys = None
        self._graphics_sys = None
        
        self._setup_logging(config.get('Logging', {'name' : 'Simulation',
                                                   'level': 'INFO'}))
        
        self.logger.info('* * * Beginning initialization')
        
        self._config = config
        self._create_components()
        self.load_scene()
        event.register_handlers('SIM_SHUTDOWN', self._shutdown)
        self._run = True
        
        self.logger.info('* * * Initialized')
    
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        
        del self._components
        del self._scene
        del self._gui_sys
        del self._input_sys
        del self._physics_sys
        del self._graphics_sys
    
        self.logger.info('* * * Shutdown complete')
    
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
        
    def _create_components(self):
        import sim.graphics
        import sim.input
        import sim.gui
        import sim.physics
        
        graphics_cfg = self._config.get('Graphics',{})
        self._graphics_sys = \
            sim.graphics.GraphicsSystem(graphics_cfg,
                                        not graphics_cfg.get('embedded', True))
        self._physics_sys = sim.physics.PhysicsSystem(self._config.get('Physics',{}))
        self._input_sys = sim.input.InputSystem(self._config.get('Input',{}))
        self._gui_sys = sim.gui.GUISystem(self._config.get('GUI',{}))
        
        self._components = [self._input_sys, self._physics_sys, self._graphics_sys]
        
    def _shutdown(self):
        self._run = False
        
    def update(self, time_since_last_update):
        """
        Updates all simulation components with the given timesteps
        
        @type time_since_last_update: a decimal number
        @param time_since_last_update: name says it all
        """
            
        if self._run:
            # Update all components, drop out if one returns false
            for component in self._components:
                component.update(time_since_last_update)
   
        return self._run
    
    def load_scene(self, scene_name = None):
        """
        Loads a scene based on the configuration data given to simulation on
        start. This expects a configuration of the following format.
        
        Scenes:  
            current: name_of_module
            path: path_on_which_module_exits
        
        @type scene_name = string
        @param scene_name = The name of the scene to load, must be on the scene
            path of the configuration given to module.

        """
        
        # Read config value, falling back to default if needed
        scene_cfg = self._config.get('Scene', {})
        mod_name = scene_cfg.get('current', 'basic')
        scene_path = scene_cfg.get('path', ['../data/scenes'])
        
        search_path = [os.path.abspath(p) for p in scene_path]
        sys_path = sys.path
        
        try:
            # Load the modules
            modfile, path, desc = imp.find_module(mod_name, search_path)
            
            # Prepend current directory to the module loading path the module can
            # import modules in that directory
            sys.path.insert(0, os.path.split(path)[0])
            
            scene = None
            try:
                scene = imp.load_module(mod_name, modfile, path, desc)
            finally:
                # Always restore path
                sys.path = sys_path
                # Remove file if needed
                if modfile:
                    modfile.close()
                    
            new_scene = scene.Scene()
            new_scene.create_scene(self._graphics_sys, self._physics_sys)
            return new_scene
                    
        except ImportError, e:
            raise SimulationError('Could not load scene "%s"\n On path: %s\n Error: %s' % (mod_name, search_path, str(e)))
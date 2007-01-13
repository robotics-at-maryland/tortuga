# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   vehicle/sim/core.py

"""
Provides core fuctionality for the simulation
"""

import os
import sys
import imp

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

def load_scene(config, graphics_sys, physics_sys):
    """
    Loads a scene based on the configuration data. This expects a configuration
    of the following format (with the Scenes node being passed in):
    Scenes:  
        current: name_of_module
        path: path_on_which_module_exits
    """
    mod_name = config['current']
    search_path = [os.path.abspath(p) for p in config['path']]
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
        new_scene.create_scene(graphics_sys, physics_sys)
        return new_scene
                
    except ImportError, e:
        raise SimulationError('Could not load scene "%s"\n On path: %s\n Error: %s' % (mod_name, search_path, str(e)))
        
    
        
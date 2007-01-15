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

import Ogre

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

def Vector(values, length = 0):
    """ 
    Converts Lists and Tuples to Ogre.Vector2/3/4, this is just a place holder
    till Python-Ogre does the automatically
    """
    if not length:
        length = len(values)
    if 2 == length:
        return Ogre.Vector2(values[0], values[1])
    elif 3 == length:
        return Ogre.Vector3(values[0], values[1], values[2])
    elif 4 == length:
        return Ogre.Vector4(values[0], values[1], values[2], values[3])
    raise SimulationError("Cannon convert %s to a vector" % str(values))

def Quat(values, axis_angle = False):
    """
    Converts to list  of values to a Quaternion, either with axis angles, when 
    the flag is true, the last value is treated as the angle.  Otherwise its
    (w,x,y,z)
    """
    if axis_angle:
        return Ogre.Quaternion( values[3], Vector(values, 3))
    else:
        return Ogre.Quaternion(values[0], values[1], values[2], values[3])

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
        
    
        
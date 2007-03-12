# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sim/util.py

"""
Provides core fuctionality for the simulation
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Standard Library Imports
import os
import sys
import imp

# Libraries Imports
import Ogre

import core

# TODO: Move these base classes somewhere a little more obvious
class IObject(core.Interface):
    """
    A basic object in the simulation it has and can have children.
    """
    
    parent = core.Attribute("""The parent of current object, none if root""")
    name = core.Attribute("""The name of the object""")
    
    def get_child(name):
        """
        @type  name: string
        @param name: The name fo the child you wish to retrieve
        """
        pass
    
    def add_child(name, child):
        """
        @type  name: string
        @param name: The name fo the child you wish to add
        
        @type  child: Implementer of IObject
        @param child: The child to add
        """
        pass
    
    def remove_child(self, child):
        """
        @type  child: IOjbect or string
        @param child: The actual object you wish to remove, or its name
        """
        
        if IObject.providedBy(child):
            pass

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

def Vector(*args, **kwargs):
    """ 
    Converts Lists and Tuples to Ogre.Vector2/3/4, this is just a place holder
    till Python-Ogre does the automatically
    """
    values = args # Assumes each argument is an element
    length = len(values)
    if kwargs.has_key('length'):
        length = kwargs['length']
        values = args[0] 
    elif length == 1:
        values = args[0]
        length = len(values)
        
    if 2 == length:
        return Ogre.Vector2(values[0], values[1])
    elif 3 == length:
        return Ogre.Vector3(values[0], values[1], values[2])
    elif 4 == length:
        return Ogre.Vector4(values[0], values[1], values[2], values[3])
    raise "Cannot convert %s to a vector" % str(values)

def Quat(*args, **kwargs):
    """
    Converts to list  of values to a Quaternion, either with axis angles, when 
    the flag is true, the last value is treated as the angle.  Otherwise its
    (w,x,y,z)
    """
    values = args # Assumes each argument is an element
    if len(values) == 1:
        values = args[0]
    
    if kwargs.has_key('axis_angle') and kwargs['axis_angle'] == True:
        return Ogre.Quaternion( Ogre.Degree(d = values[3]), Vector(values, length = 3))
    else:
        return Ogre.Quaternion(values[0], values[1], values[2], values[3])
        
    
def gravityAndBouyancyCallback(me):
    mass, inertia = me.getMassMatrix()

    gravity = Ogre.Vector3(0, -9.8, 0) * mass
    me.addForce(gravity)

    # also don't forget buoyancy force.
    # just pass the acceleration due to gravity, not the force (accel * mass)! 
    me.addBouyancyForce(1000, 0.03, 0.03, Ogre.Vector3(0.0,-9.8,0.0), 
                        buoyancyCallback, "")
    
def buoyancyCallback(colID, me, orient, pos, plane):
    """
    Here we need to create an Ogre::Plane object representing the surface of 
    the liquid.  In our case, we're just assuming a completely flat plane of 
    liquid, however you could use this function to retrieve the plane
    equation for an animated sea, etc.
    """
    plane1 = Ogre.Plane( Ogre.Vector3(0,1,0), Ogre.Vector3(0,0,0) )
    
    # we need to copy the normals and 'd' to the plane we were passed...
    plane.normal = plane1.normal
    plane.d = plane1.d
    
    # pos = Ogre.Vector3(0,0,0)
   
    return True  

class ModuleLoader(object):
    """
    The base class for all module based loaders.
    """
    
    @staticmethod
    def can_load(file_name):
        """
        Any name ending in ".py" will be accepted.
        """
        if file_name.endswith('.py'):
            return True
        return False
        
    def load(self, file_name):
        """
        Uses the python imp module to load the module given the path to it.
        
        @type  file_name: string
        @param file_name: The full path to the module
        
        @rtype:  module
        @return: the module requsted
        """
        
        # Sanity check to make sure we can load the scene
        if not self.__class__.can_load(file_name):
            raise SimulationError("%s cannon load: %s" % (self.__name__, file_name))
        
        directory, mod_name = os.path.split(file_name)
        search_path = [directory]
        
        # Strip off extension
        mod_name = mod_name[0:-3]
        
        try:
            # Load the modules
            modfile, path, desc = imp.find_module(mod_name, search_path)
            
            # Prepend current directory to the module loading path the module can
            # import modules in that directory
            sys.path.insert(0, os.path.split(path)[0])
            
            mod = None
            try:
                mod = imp.load_module(mod_name, modfile, path, desc)
            finally:
                # Always restore path
                sys.path = sys.path[1:len(sys.path)]
                # Remove file if needed
                if modfile:
                    modfile.close()
                    
            return mod
                    
        except ImportError, e:
            raise SimulationError('Could not load scene "%s"\n On path: %s\n Error: %s' % (mod_name, search_path, str(e)))
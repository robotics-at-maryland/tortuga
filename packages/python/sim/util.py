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
import serialization

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
    
    def add_child(child):
        """        
        @type  child: Implementer of IObject
        @param child: The child to add
        """
        pass
    
    def remove_child(child):
        """
        @type  child: IOjbect or string
        @param child: The actual object you wish to remove, or its name
        """
        pass
        
        #if IObject.providedBy(child):
        #    pass

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

class Object(core.Component):
    core.implements(IObject, serialization.IKMLStorable)
    
    @staticmethod
    def assert_object_implementer(obj):
        if not IObject.providedBy(obj):
            raise SimulationError('Object must implement IObject interface')
    
    @serialization.two_step_init
    def __init__(self):
        self._children = {}
        self.parent = None
        self.name = None
    
    def init(self, parent, name):
        Object._create(self, parent, name)
        
    def _create(self, parent, name):
        self._children = {}
        self.parent = parent
        self.name = name
        
        if self.parent is not None:
            self.assert_object_implementer(parent)
            self.parent.add_child(self)
            
    # IStorable Methods
    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (parent, kml_node)
        """
        Object._create(self, parent, node['name'])
        
    def save(self, data_object):
        raise "Not yet implemented"
        
    def get_child(self, name):
        if not self._children.has_key(name):
            raise SimulationError('Object does not have child %s' % name)
        
        return self._children[name]
        
    def add_child(self, child):
        self.assert_object_implementer(child)
        
        self._children[child.name] = child
    
    def remove_child(self, child):
        actual_child = child
        # If its not an implementer of IObject
        if not IObject.providedBy(child):
            actual_child = self.get_child(child)
            
        del self._children[actual_child.name]

# Monkey patch Ogre to properly print vectors and quats
def _pvec(vec):
    return '(%f,%f,%f)' % (vec.x,vec.y,vec.z)

def _pquat(quat):
    return '(%f,%f,%f,%f)' % (quat.w,quat.x,quat.y,quat.z)

Ogre.Vector3.__str__ = _pvec
Ogre.Vector3.__repr__ = _pvec

Ogre.Quaternion.__str__ = _pquat
Ogre.Quaternion.__repr__ = _pquat


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
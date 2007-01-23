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

# Libraries Imports
import Ogre

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
        
    
        
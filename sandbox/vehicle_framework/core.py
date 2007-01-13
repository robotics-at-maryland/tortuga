# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  core.py

"""
Offers core functionallity for the vehicle framework
"""

class property_meta(type):
    """
    This allow easy creation of properties that can be get, set, or deleted.
    See the python reciepe (in the dicussion) here: 
        http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/410698
    """
    def __new__(meta, class_name, bases, new_attrs):
        if bases == (object,):
            # The property class itself
            return type.__new__(meta, class_name, bases, new_attrs)
        fget = new_attrs.get('fget')
        fset = new_attrs.get('fset')
        fdel = new_attrs.get('fdel')
        fdoc = new_attrs.get('__doc__')
        return real_property(fget, fset, fdel, fdoc)

class property(object):
    """
    Uses this as such:
        class Angle(object):
        def __init__(self,rad):
            self._rad = rad
    
        class rad(property):
            '''The angle in radians'''
            def fget(self):
                return self._rad
            def fset(self,angle):
                if isinstance(angle,Angle): angle = angle.rad
                self._rad = angle
    """
    
    __metaclass__ = property_meta

    def __new__(cls, fget=None, fset=None, fdel=None, fdoc=None):
        if fdoc is None and fget is not None:
            fdoc = fget.__doc__
        return real_property(fget, fset, fdel, fdoc)

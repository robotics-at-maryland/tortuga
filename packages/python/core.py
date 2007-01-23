# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  core.py

"""
Offers core functionallity
"""
real_property = property

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

class Singleton(object):
    """
    Subclass this object to create a singleton.  Never hold onto an reference
    of a singleton object.  This will prevent the delete method from freeing
    the last instance of the object.
    """
    def __new__(cls, *args, **kwds):
        # Attempt to retreive singleton object
        it = cls.__dict__.get("__singleton__")
        if it is not None:
            return it
        
        # Generate class wide methods
        @staticmethod
        def delete():
            """ Call this to remove the reference to singleton instance """
            if cls.__singleton__ is None:
                raise 'Error, no singleton instance to release'
            else:
                cls.__singleton__ = None
                
        @staticmethod
        def get():
            """ Grab the singleton instance """
            if cls.__singleton__ is None:
                raise 'Error, no singleton instance to retrieve'
            else:
                return cls.__singleton__
            
        cls.delete = delete
        cls.get = get
        
        cls.__singleton__ = singleton = object.__new__(cls)
        singleton.init(*args, **kwds)
        return singleton
    
    def init(self, *args, **kwds):
        """
        Overide the 'init' method instead of the normal '__init__' for the
        constructor
        """
        pass
    

class FixedUpdater(object):
    """
    A basic scheduling class, that lets a class update on an aproximate fixed
    interval and continusously at the same time.
    """
    
    def __init__(self, update_interval, threshold = 1.0):
        """
        @type update_interal: number
        @param update_iterval: the time you would like between updates
        
        @type threshold: number
        @param threshold: the maximum time between updates where you try to 
            catch up by calling lots of updates in  row, instead of skipping 
            them.
        """
        self.update_interval = update_interval
        self.threshold = threshold
        self.elapsed = 0
        
    def update(self, time_since_last_update):
        """
        @type time_since_last_update: number
        @param time_since_last_update: just read the name
        """
        self._always_updated(time_since_last_update)
        
        self.elapsed += time_since_last_update
        if ((self.elapsed > self.update_interval) and (self.elapsed < (1.0)) ):
            while (self.elapsed > self.update_interval):
                self._update(time_since_last_update)
                self.elapsed -= self.update_interval
        else:
            if (self.elapsed < self.update_interval):
                # not enough time has passed this loop, so ignore for now.
                pass
            else:
                self._update(time_since_last_update)
                # reset the elapsed time so we don't become "eternally behind"
                self.elapsed = 0.0
                
    def _update(self, time_since_last_update):
        """
        Called only at the fixed interval you provide
        """
        pass
        
    def _always_updated(self, time_since_last_update):
        """
        Called everytime update is called
        """
        pass
        
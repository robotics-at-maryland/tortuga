# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  core.py

"""
Offers core functionallity
"""

# TODO: Split this off into a util and core module

real_property = property

import types
import logging

# Place all of the zope interfaces package under our current one
from zope.interface import *
from zope.interface import classImplements as _classImplements

BEGIN_INIT_MESSAGE = '* * * Beginning initialization'
END_INIT_MESSAGE = '* * * Initialized'

def log_init(default_config, level = logging.INFO):
    """
    This is a decorator, which should only be used on init methods.  These 
    methods must have a config object as there second parameter.
    
    It first creates the logger for the class based on the given config,
    then wraps the init function in logging messages.  
    
    @type  default_config: dict
    @param default_config: The config to pass to the logloader see the module
                           for more information.
    """
    def decorator(func):
        def wrapper(self, config, *args, **kwargs):
            import logloader
            # Setup the logger and log the start of our function
            cfg = config.get('Logging', default_config)
            self.logger = logloader.setup_logger(cfg, cfg)
            self.logger.info(BEGIN_INIT_MESSAGE)

            # Now Call our actual function and log its completion
            ret = func(self, *args, **kwargs)
            self.logger.info(END_INIT_MESSAGE)
            
            # Save function infromation (allows nesting of dectorators)
            wrapper.__name__ = func.__name__
            wrapper.__dict__ = func.__dict__
            wrapper.__doc__ = func.__doc__
            return ret
        return wrapper
    return decorator

def log(begin, end = None, level = logging.INFO):
    def decorator(func):
        def wrapper(self, *args, **kwargs):
            self.log(level, begin)
            ret = func(self, *args, **kwargs)
            if end is not None:
                self.log(level, end)
            
            # Save function infromation (allows nesting of dectorators)
            wrapper.__name__ = func.__name__
            wrapper.__dict__ = func.__dict__
            wrapper.__doc__ = func.__doc__
            return ret
        return wrapper
    return decorator

# --------------------------------------------------------------------------- #
#                      P R O P E R T Y   P R O T E C T O R                    #
# --------------------------------------------------------------------------- #

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

class cls_property(object):
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

# --------------------------------------------------------------------------- #
#                    S I N G L E T O N   P A T T E R N                        #
# --------------------------------------------------------------------------- #

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
        # TODO: Fix this so I can pass no arguments
        singleton.init(*args, **kwds)
        return singleton
    
    def init(self, *args, **kwds):
        """
        Overide the 'init' method instead of the normal '__init__' for the
        constructor
        """
        pass
    
# --------------------------------------------------------------------------- #
#                     C O M P O N E N T   S Y S T E M                         #
# --------------------------------------------------------------------------- #
    
class ExtensionPoint(property):
    """Marker class for extension points in components."""

    def __init__(self, interface):
        """Create the extension point.
        
        @type interface: Subclass of Interface
        @param interface: the `Interface` subclass that defines the protocol
            for the extension point
        """
        property.__init__(self, self.extensions)
        self.interface = interface
        self.__doc__ = 'List of components that implement `%s`' % \
                       self.interface.__name__

    def extensions(self, component):
        """Return a list of components that declare to implement the extension
        point interface."""
        extension_map = ComponentMeta._registry.get(self.interface, {})
        return [interface for interface in extension_map.itervalues()]

    def __repr__(self):
        """Return a textual representation of the extension point."""
        return '<ExtensionPoint %s>' % self.interface.__name__
    
class ComponentMeta(type):
    """
    The metaclass (the class of class) for the compent class.  This takes care 
    of registering each new component as supplying its interface in the 
    registry.

    This ends up with a registry of the following form:
        { IInterfaceOne : 
            {'mypkg.submod.MyClass' : MyClass, 
             'module1.OtherCls' : OtherCls },
          IAnotherInterface : 
              {'myotherpkg.subpkg.submod.AClass' : AClass} }
    """
    _registry = {}
    def __new__(cls, name, bases, d):
        """
        Called when you do a class declaration, ie "class(object): ..." and 
        this is that classes metaclass.  This is called after the class
        declartion has been executed.
        """
        new_class = None
    
        # We not dealing with the Component base class so register the classes
        # implemented interfaces
        if name != 'Component':

            # Check for the property that holds what interfaces a class implements
            if not d.has_key('__implements_advice_data__'):
                raise "Component must implement an interface"
            new_class = type.__new__(cls, name, bases, d)

            # Loop over those interfaces to add them to our registry
            for interface in d['__implements_advice_data__'][0]:
                # Generated the full package name of the class ie:
                # (mypkg.submod.Myclass) and map it to the new class in the 
                # registry.

                # Check to make sure the class fufills the interface 
                for required_attr in interface.names():
                    if not d.has_key(required_attr):
                        raise "%s must implement '%s' method of %s interface" % \
                            (name, required_attr, interface.getName())
                    

                full_name = d['__module__'] + '.' + name
                ComponentMeta._registry.setdefault(interface, {})[full_name] = new_class

        else:
            # Provide the component class access to the component registry
            d['_registry'] = ComponentMeta._registry
            new_class = type.__new__(cls, name, bases, d)

        return new_class

class Component(object):
    """
    The base class for all components, each subclass must implement and 
    interface.  If it doesn't, it shouldn't be a component.
    """
    __metaclass__ = ComponentMeta

    def create(interface, cls, *args, **kwargs):
        """
        Finds and creates 
        """
        return Component._registry[interface][cls](*args, **kwargs)



def fixed_update(update_interval_attr, _elapsed_attr = None,
                  update_time_pos = 0):
    """
    A decorator function that wraps a normal update function in a way which 
    such that is get its called at a given rate.
    
    @type update_interval_attr: string
    @param update_interval_attr: The name of the class attribute that holds
                                 the desired time between updates.
                                 
    @type elapsed_attr: string
    @param elapsed_attr: The name of the object attribute that will hold the 
                         elapsed time between updates. If None 
                         func.__name__ + '_elapsed' is used.
                         
    @type update_time_pos: number
    @param update_time_pos: The position of the variable in the args list that
                            holds the time since the last update.
    """
    def decorator(func):
        # Determine the name of attributes
        if _elapsed_attr is None:
            elapsed_attr = '%s_%s' %(func.__name__, 'elapsed')
            setattr(func, elapsed_attr, 0)
        
        def wrapper(self, *args, **kwargs):
            # Grab attributes off object
            update_interval = getattr(self, update_interval_attr)
            elapsed = None
            try:
                elapsed = getattr(self, elapsed_attr)
            except AttributeError:
                setattr(self, elapsed_attr, 0)
                elapsed = getattr(self, elapsed_attr)
                
            # Add the time since the last update to the elapsed time
            elapsed += args[update_time_pos]
            if ((elapsed > update_interval) and (elapsed < (1.0)) ):
                while (self.elapsed > self.update_interval):
                    func(*args, **kwargs)
                    elapsed -= update_interval
            else:
                if (elapsed < update_interval):
                    # not enough time has passed this loop, so ignore for now.
                    pass
                else:
                    func(*args, **kwargs)
                    # reset the elapsed time so we don't become "eternally behind"
                    self.elapsed = 0.0
            
            # Save function infromation (allows nesting of dectorators)
            wrapper.__name__ = func.__name__
            wrapper.__dict__ = func.__dict__
            wrapper.__doc__ = func.__doc__
        return wrapper
    return decorator
        
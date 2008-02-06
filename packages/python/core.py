# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  core.py

"""
Offers core functionallity
"""

import os

# TODO: Split this off into a util and core module

real_property = property

import types
import logging

# Place all of the zope interfaces package under our current one
from zope.interface import *
from zope.interface.verify import *
from zope.interface import classImplements as _classImplements
import zope.interface.declarations as declarations
import zope.interface.advice as advice

BEGIN_INIT_MESSAGE = '* * * Beginning initialization'
END_INIT_MESSAGE = '* * * Initialized'

# --------------------------------------------------------------------------- #
#                             L O G G I N G                                   #
# --------------------------------------------------------------------------- #

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
            ret = func(self, config, *args, **kwargs)
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

        @staticmethod
        def singletonCreated():
            """Returns true is the singleton has been created, false if not"""
            if cls.__singleton__ is None:
                return False
            else:
                return True

        cls.delete = delete
        cls.get = get
        cls.singletonCreated = singletonCreated

        cls.__singleton__ = singleton = object.__new__(cls)
        # TODO: Fix this so I can pass no arguments
        singleton.init(*args, **kwds)
        return singleton

    def __init__(self, *args, **kwargs):
        pass

    def init(self, *args, **kwds):
        """
        Overide the 'init' method instead of the normal '__init__' for the
        constructor
        """
        pass

# --------------------------------------------------------------------------- #
#                     C O M P O N E N T   S Y S T E M                         #
# --------------------------------------------------------------------------- #

class ComponentError(Exception):
    """Base class for simulation errors"""
    pass

class ExtensionPoint(property):
    """Marker class for extension points in components."""

    def __init__(self, iface):
        """Create the extension point.

        @type interface: Subclass of Interface
        @param interface: the `Interface` subclass that defines the protocol
            for the extension point
        """
        property.__init__(self, self.extensions)
        self.iface = iface
        self.__doc__ = 'List of components that implement `%s`' % \
                       self.iface.__name__

    def extensions(self, component):
        """Return a list of components that declare to implement the extension
        point interface."""
        extension_map = Component._registry.get(self.iface, {})
        return [iface for iface in extension_map.itervalues()]

    def __repr__(self):
        """Return a textual representation of the extension point."""
        return '<ExtensionPoint %s>' % self.iface.__name__

class Component(object):
    """
    The base class for all components, each subclass must implement and
    interface.  If it doesn't, it shouldn't be a component.

    The registry is of the form
        { IInterfaceOne :
            {'mypkg.submod.My
        # Determine the name of attributes
        if _elapsed_attr is None:
            elapsed_attr = '%s_%s' %(func.__name__, 'elapsed')
            setattr(func, elapsed_attr, 0)
        Class' : MyClass,
             'module1.OtherCls' : OtherCls },
          IAnotherInterface :
              {'myotherpkg.subpkg.submod.AClass' : AClass} }
    """


    _registry = {}

    @staticmethod
    def get_class(iface, class_name):
        """
        @type  iface: str or Interface
        @param iface: The actual interface class or its complete name in string
                      form (ie mod.submod.IFace).

        @type  class_name: str
        @param class_name: The complete name of the class ie: mod.submod.MyClass
        """

        classes = Component._registry[iface]
        return classes[class_name]

    @staticmethod
    def create(interface, class_name, *args, **kwargs):
        """
        Finds and creates objects based on the interface and class names
        """
        _class = Component.get_class(interface, class_name)
        return _class(*args, **kwargs)

def _register_class(_class):
    """
    Registers the given class in the component registry

    This ends up with a registry of the following form:
        { IInterfaceOne :
            {'mypkg.submod.MyClass' : MyClass,
             'module1.OtherCls' : OtherCls },
          IAnotherInterface :
              {'myotherpkg.subpkg.submod.AClass' : AClass}
          'mod.IInterfaceOne' :
            {'mypkg.submod.MyClass' : MyClass,
             'module1.OtherCls' : OtherCls },
          'othermod.IAnotherInterface' :
              {'myotherpkg.subpkg.submod.AClass' : AClass} }

    """

    implemented_interfaces = [i for i in implementedBy(_class)]
    if len(implemented_interfaces) == 0:
        raise "Component must implement an interface"

    # Loop over those interfaces to add them to our registry
    for iface in implemented_interfaces:
        # Generated the full package name of the class ie:
        # (mypkg.submod.Myclass) and map it to the new class in the
        # registry.
        full_name = _class.__module__ + '.' + _class.__name__
        iface_name = iface.__module__ + '.' + iface.__name__

        Component._registry.setdefault(iface, {})[full_name] = _class
        Component._registry.setdefault(iface_name, {})[full_name] = _class

    return _class

def _verify_class(_class):
    """
    This ensures that the class properly implements all its interfaces
    """

    for iface in implementedBy(_class):
        try:
            verifyClass(iface, _class)
        # Catch an rethrow the error to increase readability
        except BrokenImplementation, e:
            raise ComponentError(str(e).replace('An object', str(_class)))
    return _class

def implements(*ifaces):
    """
    This is a slight hack, it overrides the default zope.interface implements
    function because it to uses the addClassAdvisor functionality to delay its
    work till after the class is created.  So we must as well, and by placing
    our hook in here we avoid the client having to manually add it.
    """
    declarations._implements("implements", ifaces, _classImplements)

    # These are called after the class that
    advice.addClassAdvisor(_register_class)
    advice.addClassAdvisor(_verify_class)

def classImplements(cls, *implements):
    """
    Declare additional interfaces implemented for instances of a class
    
    The arguments after the class are one or more interfaces or
    interface specifications (``IDeclaration`` objects).
    
    The interfaces given (including the interfaces in the specifications)
    are added to any interfaces previously declared.
    """
    
    # Call the normal zope.interface function
    _classImplements(cls, *implements)
    
    # Now register in our system
    _verify_class(cls)
    _register_class(cls)
        

# --------------------------------------------------------------------------- #
#                                M I S C                                      #
# --------------------------------------------------------------------------- #

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
                while (elapsed > update_interval):
                    func(self, *args, **kwargs)
                    elapsed -= update_interval
            else:
                if (elapsed < update_interval):
                    # not enough time has passed this loop, so ignore for now.
                    pass
                else:
                    func(self, *args, **kwargs)
                    # reset the elapsed time so we don't become "eternally behind"
                    elapsed = 0.0

            setattr(self, elapsed_attr, elapsed)
            # Save function infromation (allows nesting of dectorators)
            wrapper.__name__ = func.__name__
            wrapper.__dict__ = func.__dict__
            wrapper.__doc__ = func.__doc__
        return wrapper
    return decorator


# Taken from python recipe at:
#     http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/413486
def Enum(*names):
   ##assert names, "Empty enums are not supported" # <- Don't like empty enums? Uncomment!

    class EnumClass(object):
        __slots__ = names

        def __iter__(self):
            return iter(constants)

        def __len__(self):
            return len(constants)

        def __getitem__(self, i):
            return constants[i]

        def __repr__(self):
            return 'Enum' + str(names)

        def __str__(self):
            return 'enum ' + str(constants)

    class EnumValue(object):
        __slots__ = ('__value')
        def __init__(self, value):
            self.__value = value
        Value = property(lambda self: self.__value)
        EnumType = property(lambda self: EnumType)
        def __hash__(self):
            return hash(self.__value)
        def __cmp__(self, other):
            # C fans might want to remove the following assertion
            # to make all enums comparable by ordinal value {;))
            assert self.EnumType is other.EnumType, "Only values from the same enum are comparable"
            return cmp(self.__value, other.__value)

        def __invert__(self):
            return constants[maximum - self.__value]

        def __nonzero__(self):
            return bool(self.__value)

        def __repr__(self):
            return str(names[self.__value])

    maximum = len(names) - 1
    constants = [None] * len(names)
    for i, each in enumerate(names):
        val = EnumValue(i)
        setattr(EnumClass, each, val)
        constants[i] = val
    constants = tuple(constants)
    EnumType = EnumClass()
    return EnumType

def environmentSub(str):
    """
    Does string replacement using pythons '%(<var_name>)s syntax with 
    environment variables.  Where <var_name> can be any envionment variable.
    """
    return str % os.environ
# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sim/serialization.py

"""
This module handles the saving and loading of objects
"""

# Projects Import
import core
import decorator

class IStorable(core.Interface):
    def __init__(*args, **kwargs):
        """
        This function most work with, no args, or the same set of arguments 
        that init() takes.  This means you can either initilize and object
        like:
            obj = GameObject()
            obj.init(10, 20, 'My Info')
        or:
            obj = GameObject(10, 20, 'My Info')
            
        and you will get exactly the same object.
        """
        pass
    
    def save(data_store):
        """
        Save the object to location.
        
        @type  data_store: Any
        @param data_store: The object to which the data will be saved.
        """
        pass
    
    def load(data_store):
        """
        Load the object from given location.
        
        @type  data_store: Any
        @param data_store: The object which supplies the data to load the object
                           from.
        """
        pass

class IKMLStorable(IStorable):
    """
    This is just a tag class to indicate you can store a class to KML.
    """
    pass
    
class ILoader(core.Interface):    
    def can_load(file_name):
        """
        This is a static method of the class it looks at the name to determine
        if the file is of a type it can load from.
        
        @type  file_name: string
        @param file_name: The absolute path to the file
        
        @rtype : bool
        @return: True if loader can load from the file, false if not
        """
        pass
    
    def load(file_name, scene):
        """
        Loads the scene from the file into the given scene object.
        
        @type scene_file: string
        @param scene_file: The absolute path to the scene file
        
        @type scene: Scene
        @param scene: The scene into which the objects should be loaded
        """
        pass

def two_step_init(func):
    """
    This is meant to be used on the classes init method, it always call the no
    argument __init__, method.  But if there are any arguments it also calls 
    the 'init' method.  This allow an object to either be used with two stage
    initialization or single.
    
    >>> class Obj(object):
    ...     @two_step_init
    ...     def __init__(self):
    ...         print '__init__'
    ...     def init(self, param):
    ...         print 'init'
    ...         self.val = param
    ...     
    >>> a = Obj()
    __init__
    >>> a.init(1)
    init
    >>> b = Obj(1)
    __init__
    init
    >>> a.val == b.val
    True
    """
    def new_init(func, self, *args, **kwargs):
        # Call Class.__init__(self)
        func(self)
        # If called with any arguments, call the create method
        if ((len(args) + len(kwargs)) > 0):
            self.init(*args, **kwargs)
    return decorator.decorate(func, new_init, 
                              decorator.make_weak_signature(func))

def parse_position_orientation(node):
    position = node['position']
    orientation = Quat(node['orientation'], axis_angle = True)
    
    return (position, orientation)

class KMLLoader(object):
    """
    The base class for all KML based loaders.
    """
        
    def _create_object(self, node, iface, parent = None):
        
        _class = core.Component.get_class(iface, node['type'])
        core.verifyClass(IKMLStorable, _class)
 
        # Build parameters to pass to __init__ 
        kwargs = {}
        
        # General parameters
        kwargs['name'] = node['name']
        kwargs['position'] = node['position']
        kwargs['orientation'] = Quat(node['orientation'], axis_angle = True)
        
        # Get all the class specific arguments
        kwargs.update(_class.kml_load(node))
        
        # Create the class    
        return _class(**kwargs)    
    
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
    
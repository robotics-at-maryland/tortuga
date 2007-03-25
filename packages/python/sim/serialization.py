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

class IKMLLoadable(core.Interface):
    """
    Any class that implements this interface can be loaded from a KML file
    """
    
    #@staticmethod
    def kml_load(node):
        """
        @type  node: Dict like object
        @param node: This will contain all the information needeed to generate
                     the parameters.
                     
        @rtype:  Dict
        @return: Maps __init__ parameters to there needed values for this 
                 object.
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


class KMLLoader(object):
    """
    The base class for all KML based loaders.
    """
        
    def _create_object(self, node, iface, parent = None):
        
        _class = core.Component.get_class(iface, node['type'])
        core.verifyClass(IKMLLoadable, _class)
 
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
    
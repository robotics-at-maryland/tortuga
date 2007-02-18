# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   /sim/scene.py


# Python imports
import imp
import os.path

# Library Ipmorts
import Ogre
import OgreNewt

# Project Imports
import core
import sim.simulation.SimulationError
from core import fixed_updated

class SceneError(sim.simulation.SimulationError):
	pass


class ISceneLoader(core.Interface):
    """
    Provides an abstract interface to loading a scene.
    """
    
    def can_load(name):
        """
        This is a static method of the class it looks at the name to determine
        if the scene is of a type it can load.
        
        @type name: string
        @param name: The absolute path to the scene file
        
        @rtype: bool
        @return: True if loader can load the scene, false if not
        """
        pass
    
    def load(name, scene):
        """
        Loads the scene from the file into the given scene object.
        
        @type name: string
        @param name: The absolute path to the scene file
        
        @type scene: Scene
        @param scene: The scene into which the objects should be placed
        """
        pass

class Scene(object):
    """
    This provides access to all objects in the scene, be they cameras, robots,
    or just obstacles.
    """
    scene_loaders = core.ExtensionPoint(ISceneLoader)    
    
    def __init__(self, name, scene_file):
        """
        @type name: string
        @param name: The name of scene
        """
        self.name = name
        self._objects = {}
        
        # Create Ogre SceneManager and OgreNewt World
        self._scene_manager = \
            Ogre.Root.getSingleton().createSceneManager(Ogre.ST_GENERIC,
                                                        self.name)
        self._world = OgreNewt.World()
        
        # Search for a scene loader compatible with file format then load it
        loaders = [loader for loader in self.scene_loaders \
                   if loader.can_load(scene_file) ]
        
        if len(loaders) == 0:
            raise SceneError('No Loader found for "%s".' % scene_file)
        
        loader = loaders[0]()
        loader.load(self, scene_file)
        Ogre.ResourceGroupManager.getSingleton().initialiseResourceGroup(self.name)
        

    def update(self, time_since_last_update):
        _update_physics(time_since_last_update)
        
        # Update all of our objects
        for obj in _objects.itervalues():
            obj.update(time_since_last_update)
    
    @fixed_udpate('_physics_update_interval')
    def _update_physics(self, time_since_last_update):
        """
        Updates the physics of the sim at a rate near the 
        _physics_update_interval attribute of the object.
        """
        self._world.update(time_since_last_update)
	
    def reload(self):
        # TODO: Finish me
        pass
    
	def add_resource_locations(self, location_map):
		"""
		Adds resouce location to the scene internal resource group.  This 
		resources will only be after the scene is loaded initialized.
		
		@type location_map: dict: string to string
		@param location_map: maps resource type, to its location.  Valid types
							 are 'Zip' and 'FileSystem'.
		"""
        rsrc_mrg = Ogre.ResourceGroupManager.getSingleton()
        for resource_type in location_map:
            for location in location_map[resource_type]:
                location = os.path.abspath(location)
                rsrc_mrg.addResourceLocation(location, resource_type, self.name)
	
	def create_object(self, obj_type, *args, **kwargs):
		"""
		Creates an object
		
		@type type: Interface
		@param type: The interface
		"""
		kwargs['scene'] = self
		core.Component.create(obj_type, *args, **kwargs)
	
class ModuleSceneLoader(core.Component):
	"""
	Loads a scene by loading a python module from file and executing the
	create_scene method.
	
	B{Implements:}
	 - ISceneLoader
	"""
	
	core.implements(ISceneLoader)
	
	# ISceneLoader Methods
	@staticmethod
	def can_load(name):
		"""
		Any name ending in ".py" will be accepted.
		"""
		if name.endswith('.py'):
			return True
		return False
		
	def load(self, name, scene):
		"""
		Uses the python imp module to load the module given the path to it.
		"""
		
		# Sanity check to make sure we can load the scene
		if not self.can_load(name):
			raise SceneError("%s cannon load: %s" % (self.__name__, name))
		
		directory, mod_name = os.path.split(name)
		search_path = [directory]
		
		# Strip off extension
		mod_name = mod_name[0:-3]
		
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
					
			scene._bodes.extend(new_scene.create_scene(self, scene))
					
		except ImportError, e:
			raise SimulationError('Could not load scene "%s"\n On path: %s\n Error: %s' % (mod_name, search_path, str(e)))
	
	# End ISceneLoader Methods
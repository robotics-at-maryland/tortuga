import imp
import os

class SimulationError (Exception):
    """ Base class for exceptions in the simulation """
    pass

def load_scene(config, graphics_sys, physics_sys):
    """
    Loads a scene based on the configuration data. This expects a configuration
    of the following format (with the Scenes node being passed in):
    Scenes:  
        current: name_of_module
        path: path_on_which_module_exits
    """
    mod_name = config['current']
    search_path = [os.path.abspath(p) for p in config['path']]
    
    try:
        # Load the modules
        modfile, path, desc = imp.find_module(mod_name, search_path)
        
        scene = None
        try:
            scene = imp.load_module(mod_name, modfile, path, desc)
        finally:
            if modfile:
                modfile.close()
                
        new_scene = scene.Scene()
        new_scene.create_scene(graphics_sys, physics_sys)
        return new_scene
                
    except ImportError, e:
        raise SimulationError('Could not load scene "%s" on path %s' % \
                              (mod_name, search_path))
        
    
        
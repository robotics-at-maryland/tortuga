

# Project Imports
import baseapp
from vehicle.vehicle import Vehicle
from module import ModuleManager
from ext.core import ConfigNode as _ConfigNode
from core import Component
from module import IModule

class OCIApp(baseapp.AppBase):
    def __init__(self, config_path):     
        baseapp.AppBase.__init__(self, config_path)
    
    def _create_module_manager(self):
        # Define custom loader ModuleManager that passes the vehicle to each
        # module
        def load_func(mod_type, mod_config):
            if mod_config['name'] != 'Vehicle':
                vehicle = ModuleManager.get().get_module('Vehicle')
                assert vehicle is not None
                mod_type(vehicle, mod_config)
            else:
                mod_type(mod_config)
        
        ModuleManager(self._config, load_func)
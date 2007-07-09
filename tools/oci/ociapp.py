

# Project Imports
import baseapp
from vehicle.vehicle import Vehicle
from module import ModuleManager
from ext.core import ConfigNode as _ConfigNode
from core import Component
from module import IModule

class OCIApp(baseapp.AppBase):
    def __init__(self, config_path):
        baseapp.AppBase.__init__(self, config_path, False)
        
        # Load up all of our modules
        self._load_modules()
        ModuleManager.get().load_modules(self._config)
        
    def _load_modules(self):
        mod_config = self._config['Modules']
        veh_config = mod_config['Vehicle']
        veh_config['name'] = 'Vehicle'
        print 'Vehicle created'
        self.vehicle = Vehicle(veh_config)
        print 'Vehicle Done'
        
        self.controller = self._load_mod('Controller', mod_config['Controller'])
        
    def _load_mod(self, name, config_node):
        class_name = config_node['type']
        config_node['name'] = name
        print 'Loading',name,' of type ',class_name
        mod_type = Component.get_class(IModule, class_name)
        mod_type(self.vehicle, config_node)
        
    def _to_cpp_config(self, cfg):
        return _ConfigNode.fromString(str(cfg))
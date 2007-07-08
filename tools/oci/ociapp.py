

# Project Imports
import baseapp
import vehicle.vehicle
import vehicle.devices
from vehicle.vehicle import Vehicle
from module import ModuleManager
from ext.core import ConfigNode as _ConfigNode

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
        
    def _to_cpp_config(self, cfg):
        return _ConfigNode.fromString(str(cfg))
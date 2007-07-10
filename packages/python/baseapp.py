# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/baseapp.py


# Library Imports
import yaml

# Project Imports
import vehicle.vehicle
import vehicle.devices
import control.bwpdcontrol
import event
from module import ModuleManager


# TODO: Move me somewhere more global
class AppBase(object):
    """
    General RAM (Find a better name for this system) Application
    """
    def __init__(self, config_file_path, load_modules = True):
        self._modules = []
        
        # Load our config file, in the future this will be custom class that is
        # able to import config files into other config files and still save
        # them back out into the proper files
        self._config = yaml.load(file(config_file_path))
        
        print 'Loading modules'
        if load_modules:
            # Create Singleton Module manager, automatically creates all modules
            # based on the config file
            ModuleManager(self._config )
            
            # Gather up all modules that need updating (ie arn't running there own
            # background threads)
            self._modules = [mod for mod in ModuleManager.get().itermodules()
                             if mod.backgrounded() == False]
        else:
            ModuleManager()
        
        # Now register register for module events, so we can keep that
        # list up to data
        event.register_handlers({'MODULE_SHUTDOWN' : self._remove_module,
                                 'MODULE_CREATED' : self._add_module} )
        
        print 'Loading finished'
        
    def _remove_module(self, mod):
        if self._modules.count(mod) > 0:
            self._modules.remove(mod)
            
    def _add_module(self, mod):
        if mod.backgrounded() == False:
            self._modules.append(mod)
           
    def update(self, time_since_last_update):
        """
        Updates all modules that need to be updated
        """
        for mod in self._modules:
            if not mod.running():
                mod.update(time_since_last_update)
            
    def _load_mod(self, name, config_node):
        class_name = config_node['type']
        config_node['name'] = name
        print 'Loading',name,' of type ',class_name
        mod_type = Component.get_class(IModule, class_name)
        mod_type(self.vehicle, config_node)
            
    def main_loop(self):
        """
        Run the main module specified from the config file
        """
        
        main_name = self._config['main']
        
        # Find main module
        possible = [m for m in self._modules if m.name == main_name]
        if len(possible) > 0:
            raise "Error multiple modules with name: '%s' found" % main_name
        main_mod = possible[0]
        
        # Start Everything but the main module
        mod_config = self._config['Modules']
        
        
        
        # Grab update interval
        update_interval = self._config['Modules'][main_name]['update_interval']
        
        self._last_time = time.time()
        
        while(1):
            current_time = time.time()
            time_since_last_iteration = (current_time - self._last_time);

            # Update main iteration
            main_mod.update(time_since_last_iteration)
            
            # Store old time
            self._last_time = current_time
        
            # Sleep until the next update time
            sleep_time = update_interval - (time.time() - current_time)
            if sleep_time < 0:
                sleep_time = update_interval
                
            time.sleep(sleep_time)
        
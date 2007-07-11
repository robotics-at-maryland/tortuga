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
        
        self._create_module_manager()
        
        # Gather up all modules that need updating (ie arn't running there own
        # background threads)
        self._modules = [mod for mod in ModuleManager.get().itermodules()
                         if mod.backgrounded() == False]
        
        # Now register register for module events, so we can keep that
        # list up to data
        event.register_handlers({'MODULE_SHUTDOWN' : self._remove_module,
                                 'MODULE_CREATED' : self._add_module} )
        
        print 'Loading finished'
        
    def _create_module_manager(self):
        if not ModuleManager.singletonCreated():
            ModuleManager(self._config)
        
    def _remove_module(self, mod):
        if self._modules.count(mod) > 0:
            self._modules.remove(mod)
            
    def _add_module(self, mod):
        if mod.backgrounded() == False:
            self._modules.append(mod)
            
    def main_loop(self):
        """
        Run the main module specified from the config file
        """
        
        # TODO: Error handling
        main_mod = ModuleManager.get().get_module(self._config['main'])
        
        # Start Everything but the main module
        mod_config = self._config['Modules']
        for name in mod_config.keys:
            ModuleManager.get().get_module(name).start()
        
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
        
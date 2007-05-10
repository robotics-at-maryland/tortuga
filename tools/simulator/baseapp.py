# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/baseapp.py


# Library Imports
import yaml

# Project Imports
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
            mod.update(time_since_last_update)
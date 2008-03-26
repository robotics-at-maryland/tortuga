# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/baseapp.py


# Python Imports
import time
import sys

# Library Imports
import yaml

# Project Imports
import vehicle.vehicle
import vehicle.devices
import control.bwpdcontrol
import ram.event as event
from ram.module import ModuleManager
import scheduler


# TODO: Move me somewhere more global
class AppBase(object):
    """
    General RAM (Find a better name for this system) Application
    """
    def __init__(self, config_file_path, load_modules = True):
        self._modules = []
        self._quit = 0
        self.main_mod = None
        
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
        
        #print 'Loading finished'
        
    def _create_module_manager(self):
        
        #if not ModuleManager.singletonCreated():
        # Define custom loader ModuleManager that passes the vehicle to each
        # module
        def load_func(mod_type, mod_config):
            #print 'Creating Module',mod_config['name']
            if mod_config['name'] != 'Vehicle':
                vehicle = ModuleManager.get().get_module('Vehicle')
                assert vehicle is not None
                mod_type(vehicle, mod_config)
            else:
                mod_type(mod_config)
        
        ModuleManager(self._config, load_func)
        #ModuleManager(self._config)
        
    def _remove_module(self, mod):
        if mod == self.main_mod:
            self._quit = True
        
        if self._modules.count(mod) > 0:
            self._modules.remove(mod)
            
    def _add_module(self, mod):
        if mod.backgrounded() == False:
            self._modules.append(mod)
            
    def main_loop(self, singlethreaded = False):
        if singlethreaded:
            self._single_main_loop()
        else:
            self._multi_main_loop()
            
    def _single_main_loop(self):
        
        # Build list of objects for the schedules
        objects = []
        
        mod_config = self._config['Modules']
        for name in mod_config.keys():
            cfg = mod_config[name]
            mod = ModuleManager.get().get_module(name)            
            info = scheduler.updatableInformation(mod, cfg["update_interval"]/1000.0)
            objects.append(info)
            
            if name == 'Vehicle':
                dev_cfg = cfg['Devices']
                starboardThruster = mod.getDevice('StarboardThruster')
                starboardThruster.background(dev_cfg['StarboardThruster']['update_interval'])
                
                imu = mod.getDevice('IMU')
                interval = dev_cfg['IMU']['update_interval'] / 1000.0
                info = scheduler.updatableInformation(imu, interval)
            
        # Grab the vehicle and add the proper devices to the scheduler
        veh = ModuleManager.get().get_module('Vehicle')
        
            
        # Scheduler object
        sched = scheduler.scheduler(objects);
        
        # Start up
        sched.sleepOperate()
        
            
    def _multi_main_loop(self):
        """
        Run the main module specified from the config file
        """
        
        # TODO: Error handling
        main_name = self._config['main_module']
        self.main_mod = ModuleManager.get().get_module(main_name)
        
        # Start Everything but the main module
        mod_config = self._config['Modules']
        for name in mod_config.keys():
            if name != main_name:
                ModuleManager.get().get_module(name).start()
        
        # Grab update interval
        update_interval = 1000.0 / self._config['Modules'][main_name]['update_interval'] / 1000.0
        self._last_time = time.time()
        
        
        #print 'Interval',update_interval
        while(1):
            # Quit if the main module has shutdown
            if self._quit:
                break;
            
            current_time = time.time()
            time_since_last_iteration = (current_time - self._last_time);

            # Update main iteration
            #print 'Running: ',time_since_last_iteration
            self.main_mod.update(time_since_last_iteration)
            
            # Store old time
            self._last_time = current_time
        
            # Sleep until the next update time
            sleep_time = update_interval - (time.time() - current_time)
            if sleep_time < 0:
                sleep_time = update_interval
                
            #print 'Sleeping for',sleep_time
            time.sleep(sleep_time)
        
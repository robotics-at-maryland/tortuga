# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  main.py

"""
    The entry point to the program.  This load the configuration files, 
instantiates the proper vehicle and then jumps into the main loop.
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Stdlib Imports
import os
import sys
import time

# Library Imports
import yaml
import CEGUI

# Project Imports
import logloader
import event

import sim
from vehicle.sim.simvehicle import Vehicle, VehicleFactory

def main_loop(components):
    last_time = time.clock()
    time_since_last_iteration = 0
    run = True
    
    while (run):
        # Loop over all components updating them, if one returns false exit
        for component in components:
            if not component.update(time_since_last_iteration):
                run = False
        
        event.process_events()
        
        current_time = time.clock()
        time_since_last_iteration = current_time - last_time;
        last_time = current_time

def main():
    # Parse Command line options

    # Read in value from config file and create the right vehicle
    config = yaml.load(file(os.path.join('..', 'sim.yml')))
    logloader.load_loggers(config["Logging"])
    
    vehicle_type = config['vehicle']
    # Pass along the subsection of the config corresponding to the vehicle
    vehicle = VehicleFactory.create(vehicle_type,
                                    config['Vehicles'][vehicle_type])
    
    components = [vehicle]
    del vehicle
    
    # Main Loop
    main_loop(components)


    # Still Have some kind of circular vehicle reference, __del__, doesn't get
    # called even when all the refernce from the function are gone
    for component in components:
        component.__del__()
    
    del components

    print 'Shutdown Complete'
    return 0
    
if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)
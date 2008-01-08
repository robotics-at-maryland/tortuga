# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulation/src/main.py

# STD Imports
import os
import sys

# Library Imports
import yaml

# Project Imports
import ram.module as module
import sim.subsystems
import ext.core

def stop(event):
    global APPLICATION
    APPLICATION.stopMainLoop()

def main(args = None):
    if args is None:
        args = sys.argv
        
    # Module Manager needed for Simulation
    moduleManager = module.ModuleManager()
    
    path = os.path.abspath(os.path.join('data', 'config','sim.yml'))
    print 'PATH',path
    app = ext.core.Application(path)
    
    # Hook into stop event to stop main loop
    global APPLICATION
    APPLICATION = app
    simulation = app.getSubsystem("Simulation")
    conn = simulation.subscribe(sim.subsystems.Simulation.SHUTDOWN, stop)
    
    app.mainLoop()
    
    # Remove extra references
    conn.disconnect()
    
    simulation.shutdown()
    APPLICATION = None
    del app
    print 'DONE'

if __name__ == '__main__':
    sys.exit(main())
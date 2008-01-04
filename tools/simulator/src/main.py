# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulation/main.py

# STD Imports
import os
import sys
import time
import warnings

# Library Imports
import yaml

warnings.simplefilter('ignore', RuntimeWarning)
import ogre.renderer.OGRE as ogre
import ogre.physics.OgreNewt as ogrenewt
#import ogre.io.OIS
warnings.simplefilter('default', RuntimeWarning)

# Project Imports
import logloader
import event
import ram.module as module

import ram.sim.simulation
import ram.sim.input

import sim.vehicle
import sim.control

import ext.core
import ext.control

def getTime():
    if os.name == 'posix':
        return time.time()
    else:
        return time.clock()    

def mainLoop(components, window):
    last_time = getTime()
    time_since_last_iteration = 0
    run = True
    
    # We have to pump it once here so the isClosed and isActive return proper
    # values
    ogre.WindowEventUtilities.messagePump()
    while run and not window.isClosed():
        if window.isActive():
            # Loop over all components updating them, if one returns false exit
            for component in components:
                component.update(time_since_last_iteration)
                        
            event.process_events()
            
            sim = components[0]
            ogrenewt.Debugger.getSingleton().showLines(sim.get_scene('Main').world)
            current_time = getTime()
            time_since_last_iteration = current_time - last_time;
            last_time = current_time
        else:
            # we got minimized or de-focused, so slow it down and stop
            # rendering until we get focus back
            time.sleep(0.1)
            ogre.WindowEventUtilities.messagePump()

def setupVehicleController(scene, subSysConfig):
    """
    This function is a stand in for the Application class
    """
    vehicle = sim.vehicle.Vehicle(scene._robots['AUT'])
    
    # List of dependents
    dependents = ext.core.SubsystemList()
    dependents.append(vehicle)
    
    cfg = ext.core.ConfigNode.fromString(str(subSysConfig['Controller']))
    controller = ext.core.SubsystemMaker.newObject(cfg, dependents)
    controller = ext.control.IController.castTo(controller)
    
    return (vehicle, controller)
    

def main(args = None):
    if args is None:
        args = sys.argv
        
    # Module Manager needed for Simulation
    moduleManager = module.ModuleManager()
    
    config = yaml.load(file(os.path.join('data', 'config','sim.yml')))
    subSysCfg = config['Subsystems']
    simulation = ram.sim.simulation.Simulation(subSysCfg['Simulation'])
    
    # Create Window so that we may load resources
    root = ogre.Root.getSingleton()
    window = root.createRenderWindow("Simulator", 800, 600, False)
    
    # Create all the scenes (this loads the resources)
    simulation.create_all_scenes()
    
    # Debugging
    ogrenewt.Debugger.getSingleton().init(simulation.get_scene('Main').scene_mgr)
    
    # Setup viewport
    camera = simulation.get_scene('Main').get_camera('Main').camera    
    camera.setAutoAspectRatio(True)
    viewport = window.addViewport(camera)
    viewport._updateDimensions()
    
    # Setup Input System
    inputForwarder = ram.sim.input.OISInputForwarder({}, simulation.input_system, 
                                                     window)
    
    # Setup the vehicle and controller
    vehicle, controller = setupVehicleController(simulation.get_scene('Main'), 
                                                 subSysCfg)
    
    # Place main loop here
    simulation.start()
    controller.setSpeed(1)
    keyController = sim.control.KeyboardController({}, [controller])
    mainLoop([simulation, controller, keyController], window)
    
    # Now shutdown
    ogrenewt.Debugger.getSingleton().deInit()
    inputForwarder.shutdown()
    simulation.shutdown()
    window.removeAllViewports()
    window.destroy()

if __name__ == '__main__':
    sys.exit(main())
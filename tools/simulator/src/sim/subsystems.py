# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/src/sim/subsystems.py

"""
Provides C++ based subsystems to allow integration of the simulation with the 
the rest of the system
"""

import time

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core as core
import ram.sim.simulation as simulation
import ram.sim.input as input
import event

class WindowListener(ogre.WindowEventListener):
    def __init__(self, closeHandler):
        ogre.WindowEventListener.__init__(self)
        
        self._closeHandler = closeHandler

    # Window Event Listener Methods
    def windowClosed(self, window):
        print 'WINDOW SHUTDOWN'
        self._closeHandler(window)
        
    def windowMoved(self, window):
        pass
    
    def windowResized(self, window):
        pass
     
    def windowFocusChange(self, window):
        pass        
    

class Simulation(core.Subsystem):
    SHUTDOWN = 'SIMULATION_SHUTDOWN'
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'Simulation'))
        self._windowListener = WindowListener(self.windowClosed)
        
        self._simulation = simulation.Simulation(config)
        self._root = ogre.Root.getSingleton()
        self._window = self._root.createRenderWindow("Simulator", 800, 600, 
                                                     False)
        
        # Load Scenes and grab the main one
        self._simulation.create_all_scenes()
        self.scene = self._simulation.get_scene('Main')
        
        # Input System
        self._inputForwarder = input.OISInputForwarder( \
            {}, self._simulation.input_system, self._window)
        
        # Setup viewport
        camera = self.scene.get_camera('Main').camera    
        camera.setAutoAspectRatio(True)
        viewport = self._window.addViewport(camera)
        viewport._updateDimensions()
        
        self._simulation.start()

        time.sleep(1)
        #ogrenewt.Debugger.getSingleton().showLines(sim.get_scene('Main').world)
        
    def backgrounded(self):
        return False
        
    def unbackground(self, join = True):
        pass
        
    def update(self, timeSinceLastUpdate):
        ogre.WindowEventUtilities.messagePump()
        if not self._window.isClosed() and self._window.isActive():
            self._root.renderOneFrame()
            self._simulation.update(timeSinceLastUpdate)
        elif not self._simulation.backgrounded() and self._window.isClosed():
            self.windowClosed(self._window)
        else:
            ogre.WindowEventUtilities.messagePump()
            
        event.process_events()

    # Window Event Listener Methods
    def windowClosed(self, window):
        self.publish(Simulation.SHUTDOWN, core.Event())
        
    def shutdown(self):
        if not self._simulation.backgrounded():
            self._simulation.shutdown()
            self._inputForwarder.shutdown()
            #ogrenewt.Debugger.getSingleton().deInit()
            
            

core.SubsystemMaker.registerSubsystem('Simulation', Simulation)
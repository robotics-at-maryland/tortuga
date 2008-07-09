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
import os
import wx
import yaml
import ogre.renderer.OGRE as ogre
import ogre.physics.OgreNewt as ogrenewt

# Project Imports
import ext.core as core
import ram.sim.simulation as simulation
import ram.sim.input as input
import ram.module
import ram.event as event

# Ensure the module manager is started up (legacy code)
moduleManager = None
if not hasattr(ram.module.ModuleManager, 'get'):
    moduleManager = ram.module.ModuleManager()

# Special Event types
event.add_event_types(['SCREENSHOT'])

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
    SHUTDOWN = core.declareEventType('SIMULATION_SHUTDOWN')
    UPDATE = core.declareEventType('UPDATE')
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'Simulation'))
        self._windowListener = WindowListener(self.windowClosed)
        
        self._simulation = simulation.Simulation(config)
        self._root = ogre.Root.getSingleton()
        
        # Load data file
        self._guiFileName = config.get('guiConfig', 'ramsim.yml')
        guiBasePath = wx.StandardPaths.Get().GetUserConfigDir()
        if wx.Platform == '__WXGTK__':
            self._guiFileName = '.' + self._guiFileName
        self._guiFileName = \
            os.path.abspath(os.path.join(guiBasePath, self._guiFileName))
        guiData = {}
        try:
            stream = file(self._guiFileName, 'r')
            guiData = yaml.load(stream)
            stream.close()
        except (IOError, yaml.YAMLError):
            # File does not exist, ignore
            pass

            
        # Load settings and create screen
        simGUICfg = guiData.get('SIM', {})
        width = simGUICfg.get("windowWidth", 800)
        height = simGUICfg.get("windowHeight", 600)
        top = simGUICfg.get("windowTop", 100)
        left = simGUICfg.get("windowLeft", 100)
        params = ogre.NameValuePairList()
        params['top'] = str(top)
        params['left'] = str(left)
        
        self._window = self._root.createRenderWindow("Simulator", width, height, 
                                                     False, params)
        self._debug = config.get('debug', False)
        self._backgrounded = False
        
        # Load Scenes and grab the main one
        self._simulation.create_all_scenes()
        self.scene = self._simulation.get_scene('Main')
        
        # Input System
        self._inputForwarder = input.OISInputForwarder( \
            {}, self._simulation.input_system, self._window)
        
        event.register_handlers({'SCREENSHOT' : self._onScreenshot})
        
        # Setup viewport
        camera = self.scene.get_camera('Main').camera    
        camera.setAutoAspectRatio(True)
        viewport = self._window.addViewport(camera)
        viewport._updateDimensions()
        
        self._simulation.start()

        time.sleep(1)
        if self._debug:
            ogrenewt.Debugger.getSingleton().init(self.scene.scene_mgr)
        
    def rateLimit(self, rate):
        """
        Turns on a rate limiting mode, which tries to run the simulator at the
        given rate.
        
        @type  rate: int
        @param rate: The number of times it updates per second
        NOT CURRENTLY USED
        """
        self._backgrounded = True
        self._updateInterval = 1 / rate
        self._lastUpdate = None
        self._nextUpdate = ram.timer.time() + self._updateInterval 
        
        # Create a time which triggers the update at the desired rate
        self._timer = ram.timer.Timer(self, Simulation.UPDATE, 
                                      self._updateInterval)
        
    def _rateUpdateHandler(self, event):
        now = ram.timer.time()
        optimalNextUpdate = now + self._updateInterval
        self._timer.stop()
        
        # Run our update
        if self._lastUpdate is not None:
            # Normal update
            self.update(now - self._lastUpdate)
        else:
            # First update
            self.update(self._updateInterval)
        
        # Check for running too slow and start the next timer
        if self._nextUpdate < now:
            # Update rate too fast
            print "UPDATE RATE TOO FAST!!!"
            self._timer = ram.timer.Timer(self, Simulation.UPDATE, 
                                          self._updateInterval)
            self._nextUpdate = now + self._updateInterval
        else:
            # Normal update rate
            updateInterval = (now - self._nextUpdate) + self._updateInterval
            
            self._timer = ram.timer.Timer(self, Simulation.UPDATE, 
                                          updateInterval)
            
            self._nextUpdate += self._updateInterval
        
    def backgrounded(self):
        return self._backgrounded
        
    def unbackground(self, join = True):
        self.shutdown()
        
    def update(self, timeSinceLastUpdate):
        ogre.WindowEventUtilities.messagePump()
        if not self._window.isClosed() and self._window.isActive():
            self._root.renderOneFrame()
            self._simulation.update(timeSinceLastUpdate)
            if self._debug:
                ogrenewt.Debugger.getSingleton().showLines(self.scene.world)
        elif not self._simulation.backgrounded() and self._window.isClosed():
            self.windowClosed(self._window)
        #else:
        #    ogre.WindowEventUtilities.messagePump()
            
        event.process_events()

    # Window Event Listener Methods
    def windowClosed(self, window):
        self.publish(Simulation.SHUTDOWN, core.Event())
        
    def shutdown(self):
        if not self._simulation.backgrounded():
            self._saveWindowSettings()
            self._simulation.shutdown()
            self._inputForwarder.shutdown()
            if self._debug:
                ogrenewt.Debugger.getSingleton().deInit()
               
    def _onScreenshot(self, *args):
        self._window.writeContentsToTimestampedFile('ScreenShot','.png')
                
    def _saveWindowSettings(self):
        guiData = {}
        try:
            stream = file(self._guiFileName, 'r')
            guiData = yaml.load(stream)    
            stream.close()
            print 'SIM READ FILE'
        except (IOError, yaml.YAMLError):
            # File does not exist, ignore
            pass
        
        # Grab window metrics
        width, height, colorDepth, left, top = self._window.getMetrics()
        
        layoutStream = file(self._guiFileName, 'w+')
        simGUICfg = guiData.get("SIM", {})
        simGUICfg["windowWidth"] = int(width)
        simGUICfg["windowHeight"] = int(height)
        if wx.Platform != '__WXGTK__':
            simGUICfg["windowTop"] = int(top)
            simGUICfg["windowLeft"] = int(left)
        guiData["SIM"] = simGUICfg
        yaml.dump(guiData,layoutStream)
        layoutStream.close()
        print 'FILE DONE'

            

core.SubsystemMaker.registerSubsystem('Simulation', Simulation)

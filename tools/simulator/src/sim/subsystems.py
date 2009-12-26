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
from encodings import ascii

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
    SIMULATION = None
    
    def _getGUIFileName(self, config):
        guiBasePath = ''
        if wx.GetApp() is not None:
            guiBasePath = wx.StandardPaths.Get().GetUserConfigDir()
        else:
            homePath = os.path.abspath(os.environ.get('HOME', '~/'))
            if wx.Platform == '__WXMAC__':
                guiBasePath = os.path.join(homePath, 'Library', 'Preferences')
            else:
                guiBasePath = homePath

        guiFileName = config.get('guiConfig', 'ramsim.yml')
        if wx.Platform == '__WXGTK__':
            guiFileName = '.' + guiFileName
        guiFileName = os.path.abspath(os.path.join(guiBasePath, guiFileName))
        return guiFileName
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'Simulation'))
        Simulation.SIMULATION = self
        self._windowListener = WindowListener(self.windowClosed)
        
        self._simulation = simulation.Simulation(config)
        self._root = ogre.Root.getSingleton()
        
        # Load data file
        self._guiFileName = self._getGUIFileName(config)
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

        self._window = self._root.createRenderWindow("Simulator", width,
                                                     height, False, params)
        self._debug = config.get('debug', False)
        self._backgrounded = False
        
        # Load Scenes and grab the main one
        self._simulation.create_all_scenes()
        self.scene = self._simulation.get_scene('Main')
        
        # Input System
        self._inputForwarder = input.OISInputForwarder( \
            {}, self._simulation.input_system, self._window)
        
        event.register_handlers({'SCREENSHOT' : self._onScreenshot})
        
        # Determine camera settings
        margin = 0.005
        width = 0.5 - 1.5 * margin
        height = width * 0.75
        top = 1 - margin - height
        mainHeight = 1 - 2*margin - height
        if not config.get('vehicleView'):
            mainHeight = 1
        
        # Setup viewport
        camera = self.scene.get_camera('Main').camera    
        camera.setAutoAspectRatio(True)
        viewport = self._window.addViewport(camera, height = mainHeight)
        viewport._updateDimensions()
        
        if config.get('vehicleView'):
            robot = self.scene._robots['Tortuga']
            forwardCamera = Simulation._createCamera(
                robot, '_forward', (0.5, 0, 0), (1, 0, 0))
            viewport = self._window.addViewport(forwardCamera, ZOrder = 1, 
                                                top = top, left = margin, 
                                                height = height, width = width)
            viewport._updateDimensions()
        
            downwardCamera = Simulation._createCamera( 
                robot, '_downward', (0.5, 0, -0.1), (0, 0, -1))
            viewport = self._window.addViewport(downwardCamera, ZOrder = 2, 
                                                top = top, left = 0.5 + margin/2, 
                                                height = height, width = width)
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
        width = 0
        height = 0
        colorDepth = 0
        left = 0
        top = 0
        result = self._window.getMetrics(width, height, colorDepth, left, top)
        width, height, colorDepth, left, top = result            
        
        # Record results from file
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

    @staticmethod
    def _createCamera(robot, name, position, direction):
        """Lets hack on a camera (integrate better in the future)"""
        
        sceneMgr = ogre.Root.getSingleton().getSceneManager('Main')
        if sceneMgr.hasCamera(name):
            return sceneMgr.getCamera(name) 
        
        node = robot._main_part._node
        
        # Create camera and attached it to our ourself
        camera = sceneMgr.createCamera(name)

        # Align and Position
        camera.position = position
        camera.lookAt(camera.position + ram.sim.OgreVector3(direction))
        camera.nearClipDistance = 0.05
        node.attachObject(camera)

        # This needs be set from the config file (only VERTICAL FOV)
        camera.FOVy = ogre.Degree(78)

        # NOTE: Fix not needed because camera on the vehicle is offset in the
        #       same way, what an odd coincidence
        # Account for the odd up vector difference between our and Ogre's 
        # default coordinate systems
        camera.roll(ogre.Degree(-90))

        return camera

core.SubsystemMaker.registerSubsystem('Simulation', Simulation)

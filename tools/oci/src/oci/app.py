# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sandbox/gui_example/src/oci/app.py

# Python Imports
import time

# Library Imports
import wx
import yaml

# Project Imports
import ext.core
import oci.frame

import oci.model.subsystem # Import needed for registration of Subsystems
            
class Application(wx.App):
    """
    wx GUI Application which integrates with C++ subsystems and events
    
    @type timer: wx.Timer
    @ivar timer: Triggers the updates of all the subsystems
    
    @type _lastTime: double
    @ivar _lastTime: The time the last wx.Timer event was recieved
    
    @type _updateInterval: double
    @ivar _updateInterval: The time the timer waits inbetween triggering
    
    @type _heartBeat: int
    @ivar _heartBeat: updated every timer update
    """
    
    def __init__(self, configPath = ''):
        """
        @type  configPath: str 
        @param configPath: The path to configuration file containing the 
                           subsystems to start. 
        """
        self._configPath = configPath
        
        # Now call parent class
        wx.App.__init__(self, 0)
    
    
    def OnInit(self):
        # Initialize instance variables
        self.timer = None
        self._lastTime = 0.0
        self._updateInterval = 0.0
        self._heartBeat = 0

        # Create config file
        config = {}
        if len(self._configPath) > 0:
            try:
                config = yaml.load(file(self._configPath))
            except (IOError, yaml.YAMLError):
                pass # No history file availale, continue 
        
        # Create our C++ app
        self._app = ext.core.Application(self._configPath)

        # Build a list of subsystems
        subsystems = []
        names = self._app.getSubsystemNames()
        for i in xrange(0, len(names)):
            subsystems.append(self._app.getSubsystem(names[i]))

        # Create the Main Frame
        guiCfg = config.get('GUI', {})
        frame = oci.frame.MainFrame(guiCfg, subsystems)
                                      
        frame.Show(True)
        frame.Bind(wx.EVT_CLOSE, self._onClose)
        self.SetTopWindow(frame)
        
        # Setup Update timer and start timer loop
        self.timer = wx.Timer()
        self.Bind(wx.EVT_TIMER, self._onTimer, self.timer)
    
        self._updateInterval = 1000.0 / guiCfg.get('updateRate', 10)
        self._lastTime = self._getTime()
        self.timer.Start(self._updateInterval, True)
        
        return True
        
    def OnExit(self):
        # The timer has to be stopped so we don't get events after we shut down
        self.timer.Stop()
        
        # Make sure we don't keep around any references to C++ objects
        # Seems to cause a crash
        del self._app
        
        return 0

    def _onClose(self, event):
        """
        Closes the timer when the main application closes
        """
        self.timer.Stop()
        event.Skip()
        
    def _getTime(self):
        """
        Returns the time since program start
        
        Due to some odd platform differences different time module functions 
        have different accuracies, on different platforms.  The function takes
        that into account.
        
        @rtype:  double
        @return: Seconds since program start
        """
        if wx.Platform == '__WXGTK__':
            return time.time()
        else:
            return time.clock()


    def _onTimer(self, timerEvent):
        """
        Updates all subsystems when the timer event is recieved
        
        This will ensure that if subsystem updates take longer than the 
        _updateInterval, that is just waits another _updateInterval
        """
        currentTime = self._getTime()
        timeSinceLastIteration = (currentTime - self._lastTime)
        
        # Update each subsystem with the time since the last update
        names = self._app.getSubsystemNames()
        subsystemIter = (self._app.getSubsystem(names[i]) for i in 
                         xrange(0, len(names)) )
        for subsystem in subsystemIter:
            if not subsystem.backgrounded():
                subsystem.update(timeSinceLastIteration)
        
        self._lastTime = currentTime
        
        # If we have run over into the next intervale, just wait an entire 
        # interval
        updateTime = self._updateInterval - (self._getTime() - currentTime)
        if updateTime < 0:
            updateTime = self.updateInterval
        self.timer.Start(updateTime, True)
        
        # Update heart beat
        self._heartBeat += 1

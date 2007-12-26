# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sandbox/gui_example/src/oci/app.py

# Python Imports
import time
import sys
import os.path
from optparse import OptionParser

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
    
    def OnInit(self):
        # Initialize instance variables
        self.timer = None
        self._lastTime = 0.0
        self._updateInterval = 0.0
        self._heartBeat = 0
        
        # Parse command line options
        parser = OptionParser()
        parser.add_option("-c", "--config", dest="configPath", default='',
                          help="The path to the config file")
        (options, args) = parser.parse_args()

        # Create config file
        config = {}
        if len(options.configPath) > 0:
            config = yaml.load(file(options.configPath))
        
        # Create our C++ app
        self._app = ext.core.Application(options.configPath)

        # Build a list of subsystems
        subsystems = []
        names = self._app.getSubsystemNames()
        for name in names:
            subsystems.append(self._app.getSubsystem(name))

        # Create the Main Frame
        subCfg = config.get('Subsystems', {})
        frame = oci.frame.MainFrame(subCfg.get('GUI',[]), subsystems)
                                      
        frame.Show(True)
        self.SetTopWindow(frame)
        
        # Setup Update timer and start timer loop
        self.timer = wx.Timer()
        self.Bind(wx.EVT_TIMER, self._onTimer, self.timer)
    
        self._updateInterval = 1000.0 / config.get('updateRate', 10)
        self._lastTime = self._getTime()
        self.timer.Start(self._updateInterval, True)
        
        return True
        
    def OnExit(self):
        # The timer has to be stopped so we don't get events after we shut down
        self.timer.Stop()
        
        # Make sure we don't keep around any references to C++ objects
        del self._app
        
        return 0
        
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
        subsystemIter = (self._app.getSubsystem(name) for name in 
                         self._app.getSubsystemNames())
        for subsystem in subsystemIter:
            try:
                subsystem.update(timeSinceLastIteration)
            except wx.PyDeadObjectError,e: 
                """
                 An exception here means the panel has been destroyed and there is no longer any reason to update this subsystem
                 Perhaps it would be better to have the onclose of the panels remove the subsystem from the subsystemiter
                """
                pass
        
        self._lastTime = currentTime
        
        # If we have run over into the next intervale, just wait an entire 
        # interval
        updateTime = self._updateInterval - (self._getTime() - currentTime)
        if updateTime < 0:
            updateTime = self.updateInterval
        self.timer.Start(updateTime, True)
        
        # Update heart beat
        self._heartBeat += 1
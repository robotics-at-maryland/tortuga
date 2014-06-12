# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/oci/model/subsystem.py

# STD Imports
import math

# Project Imports
import ext.core as core

class DemoPower(core.Subsystem, core.EventPublisher):
    """
    A demo power subsystem with a single value that goes up and down.
    
    @type power: double
    @ivar power: Watts in use
    
    @type _currentTime: double
    @ivar _currentTime: current time accumlated by from update timestep
    
    @type POWER_UPDATE: string
    @ivar POWER_UPDATE: Event type of event through when power updated
    """
    POWER_UPDATE = 'POWER_UPDATE'
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'])
        core.EventPublisher.__init__(self)
        
        self.power = 0
        self._currentTime = 0.0
        
    def update(self, timestep):
        """
        Updates power level based a simple sine wave
        
        @type  timestep: double
        @param timestep: The time since the last update, added to _currentTime
        """
        self._currentTime += timestep
        self.power = 50.0 * math.sin(self._currentTime) + 50.0
        
        event = core.Event()
        event.power = self.power
        self.publish(DemoPower.POWER_UPDATE, event)
        
        
# Register Subsystem so it can be created from a config file
core.SubsystemMaker.registerSubsystem('DemoPower', DemoPower)

        
class DemoSonar(core.Subsystem, core.EventPublisher):
    """
    Sonar system demo, has an x and y positiion of the sonar
    
    @type x: double
    @ivar x: x position of pinger
    
    @type y: double
    @ivar y: y position of pinger
    
    @type _currentTime: double
    @ivar _currentTime: current time accumlated by from update timestep
    """
    
    SONAR_UPDATE = 'SONAR_UPDATE'
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'])
        core.EventPublisher.__init__(self)
        
        self.x = 0
        self.y = 0
        self._currentTime = 0.0
        
    def update(self, timestep):
        """
        Updates x and y position of the pinger, based a simple sine wave
        
        @type  timestep: double
        @param timestep: The time since the last update, added to _currentTime
        """
        
        self._currentTime += timestep
        self.x = 10 * math.sin(self._currentTime)
        self.y = 10 * math.sin(self._currentTime + 3)
        
        event = core.Event()
        event.x = self.x
        event.y = self.y
        self.publish(DemoSonar.SONAR_UPDATE, event)
        
# Register Subsystem so it can be created from a config file
core.SubsystemMaker.registerSubsystem('DemoSonar', DemoSonar)
        
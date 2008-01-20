# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/oci/model/subsystem.py

# STD Imports
import math

# Project Imports
import ext.math
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device as device


class Vehicle(vehicle.IVehicle):
    def __init__(self, config, deps):
        eventHub = core.Subsystem.getSubsystemOfExactType(core.EventHub, deps)
        vehicle.IVehicle.__init__(self, config["name"], eventHub)
        
        self._devices = {}
        self._currentTime = 0.0
        self._depth = 0.0
        self._orientation = ext.math.Quaternion(0, 0, 0, 0)

        self._addThruster(eventHub, 'PortThruster', 1)
        self._addThruster(eventHub, 'StartboardThruster', 2)
        self._addThruster(eventHub, 'AftThruster', 3)
        self._addThruster(eventHub, 'ForeThruster', 4)

    def _addThruster(self, eventHub, name, offset):
        self._devices[name] = Thruster(eventHub, name, offset)

    def backgrounded(self):
        return False

    def getDevice(self, name):
        return self._devices[name]
    
    def getDeviceNames(self):
        return self._devices.keys()

    def update(self, timestep):
        self._currentTime += timestep  
        
        # Depth
        self.depth = 10 * math.sin(self._currentTime) + 10
        event = core.Event()
        event.number = self.depth
        self.publish(vehicle.IVehicle.DEPTH_UPDATE, event)
        
        # Orientation
        x = 1.0 * math.sin(self._currentTime) + 1.0
        y = 1.0 * math.sin(self._currentTime + 5) + 1.0
        z = 1.0 * math.sin(self._currentTime + 10) + 1.0
        vector = ext.math.Vector3(x, y, z)
        vector.normalise()
        
        angle = 2*math.pi * math.sin(self._currentTime + 15) + 1.0
        self._orientation.FromAngleAxis(ext.math.Radian(angle), vector)
        
        self._orientation.normalise()
        event = core.Event()
        event.orientation = self._orientation
        self.publish(vehicle.IVehicle.ORIENTATION_UPDATE, event)
        
        # Update Devices
        for device in self._devices.itervalues():
            device.update(timestep)

    def unbackground(self, join):
        pass

core.SubsystemMaker.registerSubsystem('DemoVehicle', Vehicle)

class Thruster(core.EventPublisher, device.IThruster):    
    def __init__(self, eventHub, name, offset):
        core.EventPublisher.__init__(self, eventHub)
        device.IThruster.__init__(self)
        
        self.force = 0
        self._currentTime = 0.0
        self._offset = offset
        self._name = name
                
    def getName(self):
        return self._name
                
    def update(self, timestep):
        self._currentTime += timestep
        self.force = 100 * math.sin(self._currentTime + self._offset)
        
        event = core.Event()
        event.force = self.force
        self.publish(device.IThruster.FORCE_UPDATE, event)
        
    def getMinForce(self):
        return -100;
    
    def getMaxForce(self):
        return 100;
        
    

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
        

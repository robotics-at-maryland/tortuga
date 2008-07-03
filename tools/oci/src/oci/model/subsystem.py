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
        self._addThruster(eventHub, 'StarboardThruster', 2)
        self._addThruster(eventHub, 'AftThruster', 3)
        self._addThruster(eventHub, 'ForeThruster', 4)

        self._addPowerSource(eventHub, 'Batt 1', 1)
        self._addPowerSource(eventHub, 'Batt 2', 2)
        self._addPowerSource(eventHub, 'Batt 3', 3)
        self._addPowerSource(eventHub, 'Batt 4', 4)
        self._addPowerSource(eventHub, 'Shore', 5)
        
        self._addTempSensor(eventHub, 'Sensor Board', 1)
        self._addTempSensor(eventHub, 'Balancer Board', 2)
        self._addTempSensor(eventHub, 'Distro Board', 3)

    def _addThruster(self, eventHub, name, offset):
        self._devices[name] = Thruster(eventHub, name, offset)
        
    def _addPowerSource(self, eventHub, name, offset):
        self._devices[name] = PowerSource(eventHub, name, offset)
        
    def _addTempSensor(self, eventHub, name, offset):
        self._devices[name] = TempSensor(eventHub, name, offset)

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

class Thruster(device.IThruster):  
    def __init__(self, eventHub, name, offset):
        device.IThruster.__init__(self, eventHub)
        
        self.force = 0
        self._currentTime = 0.0
        self._offset = offset
        self._name = name
        self._current = 0.0
                
    def getName(self):
        return self._name
                
    def update(self, timestep):
        self._currentTime += timestep
        sinVal = math.sin(self._currentTime + self._offset)
        self.force = 100 * sinVal
        self.current = 3 * sinVal + 3
        
        event = ext.math.NumericEvent()
        event.number = self.force
        self.publish(device.IThruster.FORCE_UPDATE, event)
        
        event = ext.math.NumericEvent()
        event.number = self.current
        self.publish(device.ICurrentProvider.UPDATE, event)
    
    def getCurrent(self):
        return self.current
        
    def getMinForce(self):
        return -100;
    
    def getMaxForce(self):
        return 100;
        
class PowerSource(device.IPowerSource):
    def __init__(self, eventHub, name, offset):
        device.IPowerSource.__init__(self, eventHub)
        self._offset = offset
        self._currentTime = 0.0
        self._name = name
    
    def getName(self):
        return self._name
    
    def getVoltage(self):
        return self.voltage
    
    def getCurrent(self):
        return self.current
    
    def isEnabled(self):
        return self.enabled
    
    def inUse(self):
        return self.inUse
    
    def update(self, timestep):
        self._currentTime += timestep
        
        sinVal = math.sin(self._currentTime + self._offset)
        self.voltage = 2.0 * sinVal + 26
        self.current = 5.0 * sinVal + 5
        if sinVal >= 0:
            self.enabled = True
            self.publish(device.IPowerSource.ENABLED, core.Event())
        else:
            self.enabled = False
            self.publish(device.IPowerSource.DISABLED, core.Event())
        
        if math.sin(self._currentTime + self._offset + 1) >= 0:
            self.inUse= True
            self.publish(device.IPowerSource.USING, core.Event())
        else:
            self.inUse = False
            self.publish(device.IPowerSource.NOT_USING, core.Event())
        
        
        event = ext.math.NumericEvent()
        event.number = self.voltage
        self.publish(device.IVoltageProvider.UPDATE, event)
        
        event = ext.math.NumericEvent()
        event.number = self.current
        self.publish(device.ICurrentProvider.UPDATE, event)
        
class TempSensor(device.ITempSensor):
    def __init__(self, eventHub, name, offset):
        device.ITempSensor.__init__(self, eventHub)
        self._offset = offset
        self._currentTime = 0.0
        self._name = name
    
    def getName(self):
        return self._name
    
    def update(self, timestep):
        self._currentTime += timestep
        
        sinVal = math.sin(self._currentTime + self._offset)
        self.temp = 20 * sinVal + 39

        event = ext.math.NumericEvent()
        event.number = self.temp
        self.publish(device.ITempSensor.UPDATE, event)

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
        

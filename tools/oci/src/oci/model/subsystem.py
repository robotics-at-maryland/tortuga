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
import ext.vision as vision
import ext.control as control

import ram.ai.state
from ram.test import Mock

class Vehicle(vehicle.IVehicle):
    def __init__(self, config, deps):
        eventHub = core.Subsystem.getSubsystemOfExactType(core.EventHub, deps)
        vehicle.IVehicle.__init__(self, config["name"], eventHub)
        
        self._devices = {}
        self._currentTime = 0.0
        self._depth = 0.0
        self._orientation = ext.math.Quaternion(0, 0, 0, 0)
        
        self._markerDropper = PayloadSet(eventHub, 'MarkerDropper')
        self._devices[self._markerDropper.getName()] = self._markerDropper
        self._torpedoLauncher = PayloadSet(eventHub, 'TorpedoLauncher', 4)
        self._devices[self._torpedoLauncher.getName()] = self._torpedoLauncher

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

    def dropMarker(self):
        self._markerDropper.releaseObject()
        
    def fireTorpedo(self):
        self._torpedoLauncher.releaseObject()

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
        
        # Position Update
        x = 1.0 * math.sin(self._currentTime) + 1.0
        y = 1.0 * math.sin(self._currentTime + 5) + 1.0
        
        event = ext.math.Vector2Event()
        event.vector2 = ext.math.Vector2(x, y)
        self.publish(vehicle.IVehicle.POSITION_UPDATE, event)
        
        # Velocity Update
        x = 1.0 * math.sin(self._currentTime + 10) + 1.0
        y = 1.0 * math.sin(self._currentTime + 15) + 1.0
        
        event = ext.math.Vector2Event()
        event.vector2 = ext.math.Vector2(x, y)
        self.publish(vehicle.IVehicle.VELOCITY_UPDATE, event)
        
        
        # Update Devices
        for device in self._devices.itervalues():
            device.update(timestep)

    def unbackground(self, join):
        pass

core.SubsystemMaker.registerSubsystem('DemoVehicle', Vehicle)

class A(object):
    pass

class B(object):
    pass

class DemoMachine(ram.ai.state.Machine):
    def __init__(self, cfg = None, deps = None):
        ram.ai.state.Machine.__init__(self, cfg, deps)
        self._currentTime = 0
        
    def update(self, timestep):
        self._currentTime += timestep
        event = core.StringEvent()
        exitEvent = core.StringEvent()
        
        if math.sin(self._currentTime) > 0:
            event.string = A.__name__
            exitEvent.string = B.__name__
        else:
            event.string = B.__name__
            exitEvent.string = A.__name__
            
        self.publish(ram.ai.state.Machine.STATE_EXITED, exitEvent)
        self.publish(ram.ai.state.Machine.STATE_ENTERED, event)
    
    def backgrounded(self):
        return False

    def unbackground(self, join):
        pass

core.SubsystemMaker.registerSubsystem('DemoStateMachine', DemoMachine)

class PayloadSet(device.IPayloadSet):
    def __init__(self, eventHub, name, count = 2):
        device.IPayloadSet.__init__(self, eventHub)
        
        self._name = name
        self._initialCount = count
        self._count = count
        
    def update(self, timeStep):
        pass
        
    def getName(self):
        return self._name
        
    def initialObjectCount(self):
        return self._initialCount
    
    def objectCount(self):
        return self._count
    
    def releaseObject(self):
        if self._count != 0:
            self._count -= 1
            event = core.Event()
            self.publish(device.IPayloadSet.OBJECT_RELEASED, event)

class Thruster(device.IThruster):  
    def __init__(self, eventHub, name, offset):
        device.IThruster.__init__(self, eventHub)
        
        self.force = 0
        self._currentTime = 0.0
        self._offset = offset
        self._name = name
        self._current = 0.0
        self.enabled = False
                
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
        
        
        if sinVal >= 0:
            self.enabled = True
            self.publish(device.IThruster.ENABLED, core.Event())
        else:
            self.enabled = False
            self.publish(device.IThruster.DISABLED, core.Event())
    
    def getCurrent(self):
        return self.current
        
    def getMinForce(self):
        return -100;
    
    def getMaxForce(self):
        return 100;
    
    def isEnabled(self):
        return self.enabled
        
class PowerSource(device.IPowerSource):
    def __init__(self, eventHub, name, offset):
        device.IPowerSource.__init__(self, eventHub)
        self._offset = offset
        self._currentTime = 0.0
        self._name = name
        self.enabled = False
        self.used = False
        self.voltage = 0.0
        self.current = 0.0
    
    def getName(self):
        return self._name
    
    def getVoltage(self):
        return self.voltage
    
    def getCurrent(self):
        return self.current
    
    def isEnabled(self):
        return self.enabled
    
    def inUse(self):
        return self.used
    
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
            self.used= True
            self.publish(device.IPowerSource.USING, core.Event())
        else:
            self.used = False
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

class DemoSonar(core.Subsystem):
    """
    Sonar system demo, has an x and y positiion of the sonar
    
    @type x: double
    @ivar x: x position of pinger
    
    @type y: double
    @ivar y: y position of pinger
    
    @type _currentTime: double
    @ivar _currentTime: current time accumlated by from update timestep
    """
    
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'], deps)
        
        self.x = 0
        self.y = 0
        self.z = 0
        self.x2 = 0
        self.y2 = 0
        self.z2 = 0
        self._currentTime = 0.0
        self._pingCount = 1
        self._mainPing = True
        
    def update(self, timestep):
        """
        Updates x and y position of the pinger, based a simple sine wave
        
        @type  timestep: double
        @param timestep: The time since the last update, added to _currentTime
        """
        
        self._currentTime += timestep
        self.x = 10 * math.sin(self._currentTime)
        self.y = 10 * math.sin(self._currentTime + 3)
        self.z = 10 * math.sin(self._currentTime + 6)
        self.x2 = 10 * math.sin(self._currentTime + 9)
        self.y2 = 10 * math.sin(self._currentTime + 12)
        self.z2 = 10 * math.sin(self._currentTime + 15)
        
        event = ext.vehicle.SonarEvent()
        event.pingCount = self._pingCount        

        if self._mainPing:
            event.direction = ext.math.Vector3(self.x, self.y, self.z)
            event.pingerID = 0
        else:
            event.direction = ext.math.Vector3(self.x2, self.y2, self.z2)
            event.pingerID = 1

        self._mainPing = not self._mainPing
        self._pingCount += 1
        self.publish(device.ISonar.UPDATE, event)

        
# Register Subsystem so it can be created from a config file
core.SubsystemMaker.registerSubsystem('DemoSonar', DemoSonar)
        
class DemoVisionSystem(vision.VisionSystem):
    def __init__(self, config, deps_):
        # Initialize super class with fake cameras
        config = core.ConfigNode.fromString(str(config))
        deps = core.SubsystemList()
        for d in deps_:
            deps.append(d)
        
        cam1 = vision.Camera(640,480)
        cam2 = vision.Camera(640,480)
        vision.VisionSystem.__init__(self, cam1, cam2, config, deps)
        
        self._currentTime = 0;

    def update(self, timestep):
        self._currentTime += timestep
        
        sinVal = math.sin(self._currentTime)

        eventLight = vision.RedLightEvent(sinVal, sinVal)
        eventTarget = vision.TargetEvent(sinVal, sinVal * -1, sinVal * -2, 
                                         sinVal * -3)
        eventBarbedWire = vision.BarbedWireEvent(sinVal, sinVal * -1,
                                                 sinVal * -2, sinVal * -3,
                                                 sinVal * -4, sinVal * -5)

        if sinVal >= 0:
            self.publish(vision.EventType.LIGHT_FOUND, eventLight)
            self.publish(vision.EventType.TARGET_LOST, eventTarget)
            self.publish(vision.EventType.BARBED_WIRE_FOUND, eventBarbedWire)
        else:
            self.publish(vision.EventType.LIGHT_LOST, eventLight)
            self.publish(vision.EventType.TARGET_FOUND, eventTarget)
            self.publish(vision.EventType.BARBED_WIRE_LOST, eventBarbedWire)
    
    def backgrounded(self):
        return False
    
    def background(self, interval):
        pass
    
# Register Subsystem so it can be created from a config file
core.SubsystemMaker.registerSubsystem('DemoVisionSystem', DemoVisionSystem)

class DemoController(control.IController):
    def __init__(self, config, deps):
        eventHub = core.Subsystem.getSubsystemOfExactType(core.EventHub, deps)
        control.IController.__init__(self, config["name"], eventHub)
        self._currentTime = 0;
        self._started = False

    def update(self, timestep):
        if not self._started:
            self._setupDebug()
            self._started = True
        
        self._currentTime += timestep

        event = control.ParamUpdateEvent()
        event.values.append(math.sin(self._currentTime))
        event.values.append(math.sin(self._currentTime + 1))
        event.values.append(math.sin(self._currentTime + 2))
        self.publish(control.IController.PARAM_UPDATE, event)

    def backgrounded(self):
        return False
    
    def unbackground(self, join):
        pass
    
    def _setupDebug(self):
        event = control.ParamSetupEvent()
        event.labels.append("P Gain")
        event.labels.append("D Gain")
        event.labels.append("I Gain")
        self.publish(control.IController.PARAM_SETUP, event)
    
# Register Subsystem so it can be created from a config file
core.SubsystemMaker.registerSubsystem('DemoController', DemoController)

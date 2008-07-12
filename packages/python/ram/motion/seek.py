# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/seek.py

# STD Imports
import math

# Project Imports
import ram.core as core
from ram.motion.basic import Motion
import ext.core

class PointTarget(ext.core.EventPublisher):
    """
    This represents the state of the target 
    """
    UPDATE = ext.core.declareEventType('UPDATE')
    
    def __init__(self, azimuth, elevation, range, x, y):
        ext.core.EventPublisher.__init__(self)
        self.setState(azimuth, elevation, range, x, y, publish = False)

    def setState(self, azimuth, elevation, range, x, y, publish = True):
        self.azimuth = azimuth
        self.elevation = elevation
        self.range = range
        self.x = x
        self.y = y

        # Change them to degrees if they are ext.math.Degree/Radian types
        if hasattr(self.azimuth, 'valueDegrees'):
            self.azimuth = self.azimuth.valueDegrees()
        if hasattr(self.elevation, 'valueDegrees'):
            self.elevation = self.elevation.valueDegrees()

        if publish:
            self.publish(PointTarget.UPDATE, ext.core.Event())
        
    class relativeDepth(core.cls_property):
        """
        Not currently used
        """
        def fget(self):
            elevation = self.elevation * math.pi / 180.0
            return -1 * self.range * math.sin(elevation)
        
class SeekPoint(Motion):
    """
    This points at, and drives toward a target, it controls all types of motion
    normally.  If depthGain is set to zero, it will not control DEPTH, if
    maxSpeed is 0 it will not control IN_PLANE.
    """
    
    def __init__(self, target, maxSpeed = 0.0, depthGain = 1):
        """
        @type  target: ram.motion.seek.PointTarget
        @param target: Target to attempt to reach
        """
        _type = Motion.ORIENTATION
        if 0 != depthGain:
            _type = _type | Motion.DEPTH
        if 0 != maxSpeed:
            _type = _type | Motion.IN_PLANE
        Motion.__init__(self, _type = _type)
        
        self._running = False
        self._target = target
        self._maxSpeed = maxSpeed
        self._depthGain = depthGain
        self._conn = target.subscribe(PointTarget.UPDATE, self._onBouyUpdate)
        
    def _start(self):
        self._running = True
        self._seek()
    
    def _seek(self):
        """
        Commands the controller to seek the current vehicle

        @note: Everything is in DEGREES!
        """
        
        # Determine new Depth
        if 0 != self._depthGain:
            currentDepth = self._vehicle.getDepth()
            newDepth = currentDepth - self._target.y * self._depthGain
            self._controller.setDepth(newDepth)
    
        # Determine how to yaw the vehicle
        vehicleHeading =  self._vehicle.getOrientation().getYaw(True)
        vehicleHeading = vehicleHeading.valueDegrees()
        absoluteTargetHeading = vehicleHeading + self._target.azimuth
        
        desiredHeading = self._controller.getDesiredOrientation().getYaw(True)
        desiredHeading = desiredHeading.valueDegrees()
        
        yawCommand = absoluteTargetHeading - desiredHeading
        self._controller.yawVehicle(yawCommand)

        # Drive toward light
        if self._maxSpeed != 0:
            self._controller.setSpeed(self._speedScale() * self._maxSpeed)

    def _speedScale(self):
        """
        Scales the speed based on far from the center of the field of view
        the light is.  It uses both azimuth & elevation, with a max moving angle
        of 90 degrees
        """
        maxPossible = math.sqrt(45**2 * 2)
        azimuth = self._target.azimuth**2
        elevation = self._target.elevation**2
        
        scale = 1 - (math.sqrt(azimuth + elevation) / maxPossible)

        if scale < 0.0:
            scale = 0.0
        return scale

    def _onBouyUpdate(self, event):
        """
        Called when the bouy's state is updated, seeks on new location
        """
        if self._running:
            # Buoy data now updated, so we just seek again
            self._seek()
        
    def stop(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self._running = False
        self._controller.setSpeed(0)
        self._conn.disconnect()

class SeekPointToRange(SeekPoint):
    """
    Seeks a point, but stops a certain range
    """
    def __init__(self, target, desiredRange, maxSpeed = 0.0, depthGain = 1):
        SeekPoint.__init__(self, target, maxSpeed, depthGain)
        self._desiredRange = desiredRange
    
    def _speedScale(self):
        baseSpeed = SeekPoint._speedScale(self)
        
        
class ObserverControllerSeekPoint(Motion):
    def __init__(self, target, maxSpeed = 0.0):
        """
        @type  target: ram.motion.seek.PointTarget
        @param target: Target to attempt to reach
        """
        Motion.__init__(self)
        
        self._running = False
        self._target = target
        self._maxSpeed = maxSpeed
        self._conn = target.subscribe(PointTarget.UPDATE, self._onBouyUpdate)
        
    def _start(self):
        self._running = True
        self._seek()
    
    def _seek(self):
        """
        Commands the controller to seek the current vehicle

        @note: Everything is in DEGREES!
        """
        
        # Determine new Depth
        absoluteTargetDepth = \
            self._vehicle.getDepth() + self._target.relativeDepth
        self._controller.setDepth(absoluteTargetDepth)    
    
        # Determine how to yaw the vehicle
        vehicleHeading =  self._vehicle.getOrientation().getYaw(True)
        vehicleHeading = vehicleHeading.valueDegrees()
        absoluteTargetHeading = vehicleHeading + self._target.azimuth
        
        desiredHeading = self._controller.getDesiredOrientation().getYaw(True)
        desiredHeading = desiredHeading.valueDegrees()
        
        yawCommand = absoluteTargetHeading - desiredHeading
        self._controller.yawVehicle(yawCommand)

        # Drive toward light
        self._controller.setSpeed(self._speedScale() * self._maxSpeed)

    def _speedScale(self):
        """
        Scales the speed based on far from the center of the field of view
        the light is.  It uses both azimuth & elevation, with a max moving angle
        of 90 degrees
        """
        maxPossible = math.sqrt(45**2 * 2)
        azimuth = self._target.azimuth**2
        elevation = self._target.elevation**2
        
        scale = 1 - (math.sqrt(azimuth + elevation) / maxPossible)

        if scale < 0.0:
            scale = 0.0
        return scale

    def _onBouyUpdate(self, event):
        """
        Called when the bouy's state is updated, seeks on new location
        """
        if self._running:
            # Buoy data now updated, so we just seek again
            self._seek()
        
    def stop(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self._running = False
        self._controller.setSpeed(0)
        self._conn.disconnect()

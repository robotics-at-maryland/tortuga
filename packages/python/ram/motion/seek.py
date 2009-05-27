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
import ram.timer as timer
import ram.motion.common as common
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
    normally.  In the translate mode 
    
    @type  target:  PointTarget
    @param target: The target we are seeking
    
    @type  maxSpeed: float
    @param maxSpeed: Max forward speed, if 0 and no translate is false it will 
                     not control IN_PLANE
    
    @type  depthGain: float
    @param depthGain: Gain for depth control, if 0 will not control DEPTH
    
    @type  translate: bool
    @param translate: If true, side-side translation is used to center the 
                      target instead of yaw control.
                      
    @type  translateGain: float
    @param translateGain: Gain for side-side motion
    """
    
    POINT_ALIGNED = ext.core.declareEventType('POINT_ALIGNED')
    
    def __init__(self, target, maxSpeed = 0.0, depthGain = 1, 
                 translate = False, translateGain = 1, iDepthGain = 0,
                 dDepthGain = 0, iTranslateGain = 0, dTranslateGain = 0,
                 yawGain = 1.0):
        """
        @type  target: ram.motion.seek.PointTarget
        @param target: Target to attempt to reach
        """
        _type = Motion.ORIENTATION
        if 0 != depthGain:
            _type = _type | Motion.DEPTH
        if 0 != maxSpeed or translate:
            _type = _type | Motion.IN_PLANE
        Motion.__init__(self, _type = _type)
        
        self._first = True
        self._running = False
        self._target = target
        self._maxSpeed = maxSpeed
        
        self._yawGain = yawGain 
        
        self._translate = translate
        self._translateGain = translateGain
        self._iTranslateGain = iTranslateGain
        self._dTranslateGain = dTranslateGain
        self._sumX = 0
        self._oldX = 0
        
        self._depthGain = depthGain
        self._iDepthGain = iDepthGain
        self._dDepthGain = dDepthGain
        self._sumDepth = 0;
        self._oldDepth = 0;
        
        self._lastSeekTime = 0.0
        
        self._conn = target.subscribe(PointTarget.UPDATE, self._onBouyUpdate)
        
        
    def _start(self):
        self._running = True
        self._seek()
    
    def _seek(self):
        """
        Commands the controller to seek the current vehicle

        @note: Everything is in DEGREES!
        """
        # Compute time since the last run
        deltaT = 1.0/40.0
        now = timer.time()
        if self._lastSeekTime != 0.0:
            deltaT = now - self._lastSeekTime
        self._lastSeekTime = now
        
        # Determine new Depth
        if 0 != self._depthGain:        
            dtDepth, self._sumDepth, self._oldDepth = common.PIDLoop(
                x = self._target.y,
                xd = 0, 
                dt = deltaT,
                dtTooSmall = 1.0/100.0, 
                dtTooBig = 1.0, 
                kp = self._depthGain, 
                kd = self._dDepthGain, 
                ki = self._iDepthGain,
                sum = self._sumDepth, 
                xOld = self._oldDepth)
            
            currentDepth = self._vehicle.getDepth()
            newDepth = currentDepth + dtDepth
            self._controller.setDepth(newDepth)
    
        # Do pointing control
        if not self._translate:
            # Determine how to yaw the vehicle
            vehicleHeading =  self._vehicle.getOrientation().getYaw(True)
            vehicleHeading = vehicleHeading.valueDegrees()
            absoluteTargetHeading = vehicleHeading + self._target.azimuth
            
            desiredHeading = self._controller.getDesiredOrientation().getYaw(True)
            desiredHeading = desiredHeading.valueDegrees()
            
            yawCommand = absoluteTargetHeading - desiredHeading
            self._controller.yawVehicle(yawCommand * self._yawGain)
        else:
            sidewaysSpeed, self._sumX, self._oldX = common.PIDLoop(
                x = self._target.x,
                xd = 0, 
                dt = deltaT,
                dtTooSmall = 1.0/100.0, 
                dtTooBig = 1.0, 
                kp = self._translateGain, 
                kd = self._dTranslateGain, 
                ki = self._iTranslateGain,
                sum = self._sumX, 
                xOld = self._oldX)

            self._controller.setSidewaysSpeed(-1 * sidewaysSpeed)

        # Drive toward light
        if self._maxSpeed != 0:
            self._controller.setSpeed(self._speedScale() * self._maxSpeed)
        
        if self._alignment() <= 0.05 and not self._first:
            self.publish(SeekPoint.POINT_ALIGNED, ext.core.Event())

        self._first = False
    def _alignment(self):
        vect = ext.math.Vector2(self._target.x, self._target.y)
        return vect.length()

    def _speedScale(self, timeSinceLastRun = 0.0):
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
    def __init__(self, target, desiredRange, maxRangeDiff, rangeGain = 1.0, 
                 maxSpeed = 0.0, depthGain = 1, translate = False, 
                 translateGain = 1, iDepthGain = 0, dDepthGain = 0,
                 iTranslateGain = 0, dTranslateGain = 0, yawGain = 1.0):
        """
        @type desiredRange: float
        @param desiredRange: The range you wish to be at relative to the target
    
        @type maxRangeDiff: float
        @param maxRangeDiff: The range difference you wish your speed to max 
                             out at
        """
        SeekPoint.__init__(self, target, 
                           maxSpeed = maxSpeed, depthGain = depthGain, 
                           translate = translate, translateGain = translateGain, 
                           iDepthGain = iDepthGain, dDepthGain = dDepthGain,
                           iTranslateGain = iTranslateGain, 
                           dTranslateGain = dTranslateGain, yawGain = yawGain)
        
        self._desiredRange = desiredRange
        self._maxRangeDiff = maxRangeDiff
        self._rangeGain = rangeGain
    
    def _speedScale(self):
        baseScale = SeekPoint._speedScale(self)
        
        # Pos if we are to far, neg if we are too close, then scale
        rangeDiff = self._target.range - self._desiredRange
        rangeScale = rangeDiff / self._maxRangeDiff
        
        # Find final scale, then cap
        scale = baseScale * (rangeScale/self._rangeGain)
        
        if (scale < 0) and (scale < -1):
            scale = -1
        elif (scale > 0) and (scale > 1):
            scale = 1
            
        return scale
        
        
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

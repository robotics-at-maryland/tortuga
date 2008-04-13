# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/pipe.py

# STD Imports
import math

# Project Imports
import ram.core as core
from ram.motion.basic import Motion
import ext.core

class Pipe(ext.core.EventPublisher):
    """
    Represents the pipe we are trying to follow
    """
    UPDATE = ext.core.declareEventType('UPDATE')
    
    def __init__(self, x, y, relativeAngle):
        ext.core.EventPublisher.__init__(self)
        self.setState(x, y, relativeAngle, publish = False)

    def setState(self, x, y, relativeAngle, publish = True):
        self.x = x
        self.y = y
        self.relativeAngle = relativeAngle

        # Change them to degrees if they are ext.math.Degree/Radian types
        if hasattr(self.relativeAngle, 'valueDegrees'):
            self.relativeAngle = self.relativeAngle.valueDegrees()

        if publish:
            self.publish(Pipe.UPDATE, ext.core.Event())
            
class Hover(Motion):
    """
    Hovers over and aligns with pipe
    """
    def __init__(self, pipe, maxSpeed = 0.0, maxSidewaysSpeed = 0.0,
                 #speedGain = 1.0, sidewaysSpeedGain = 1.0, 
                 yawGain = 1.0):
        """
        @type  target: ram.motion.pipe.Pipe
        @param target: Target to attempt to reach
        """
        Motion.__init__(self)
        
        self._running = False
        self._pipe = pipe
        self._maxSpeed = maxSpeed
        self._maxSidewaysSpeed = maxSidewaysSpeed
        #self._speedGain = speedGain
        #self._sidewaysSpeedGain = sidewaysSpeedGain
        
        if yawGain > 1:
            raise TypeError("Yaw Gain must be <= 1")
        self._yawGain = yawGain
        
        self._conn = pipe.subscribe(Pipe.UPDATE, self._onPipeUpdate)
        
    def _start(self):
        self._running = True
        self._seek()
        
    def _seek(self):
        
        # Determin forward speed (and bound within limits)
        forwardSpeed = self._pipe.y * self._maxSpeed
#        forwardSpeed = Hover._limit(self._pipe.y * self._speedGain,
#                                    -self._maxSpeed, self._maxSpeed)
        
        # Determine (and bound within limits)
        sidewaysSpeed = self._pipe.x * self._maxSidewaysSpeed
#        sidewaysSpeed = Hover._limit(self._pipe.x * self._sidewaysSpeedGain,
#                                     -self._maxSidewaysSpeed,
#                                     self._maxSidewaysSpeed)
        
        # Determine turn
        vehicleHeading =  self._vehicle.getOrientation().getYaw(True)
        vehicleHeading = vehicleHeading.valueDegrees()
        absoluteTargetHeading = vehicleHeading + self._pipe.relativeAngle
        
        desiredHeading = self._controller.getDesiredOrientation().getYaw(True)
        desiredHeading = desiredHeading.valueDegrees()
        
        yawCommand = (absoluteTargetHeading - desiredHeading) * self._yawGain
        
        # Command the vehicle
        self._controller.setSpeed(forwardSpeed)
        self._controller.setSidewaysSpeed(sidewaysSpeed)
        self._controller.yawVehicle(yawCommand) 
        
    @staticmethod
    def _limit(val, min, max):
        if val < min:
            return min
        elif val > max:
            return max
        return val
        
        
    def _onPipeUpdate(self, event):
        if self._running:
            # Data already updated, so lets just keep seeking
            self._seek()
            
    def stop(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self._running = False
        self._controller.setSpeed(0)
        self._controller.setSidewaysSpeed(0)
        self._conn.disconnect()

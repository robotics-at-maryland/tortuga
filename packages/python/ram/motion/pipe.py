# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/pipe.py

# STD Imports
import math

# Project Imports
import ext.core
import ram.core as core
import ram.motion.common as common
import ram.timer
from ram.motion.basic import Motion

class Pipe(common.Target):
    """
    Represents the pipe we are trying to follow
    """
    
    def __init__(self, x, y, relativeAngle, timeStamp = None,
                 kp = 1.0, kd = 1.0):
        common.Target.__init__(self, x, y, timeStamp, kp, kd)
        self.prevRelativeAngle = None
        self.relativeAngle = relativeAngle

    def setState(self, x, y, relativeAngle, timeStamp, publish = True):
        common.Target.setState(self, x, y, timeStamp, False)
        
        # Store the old values
        self.prevRelativeAngle = self.relativeAngle

        # Store the new values
        self.relativeAngle = relativeAngle

        # Change them to degrees if they are ext.math.Degree/Radian types
        if hasattr(self.relativeAngle, 'valueDegrees'):
            self.relativeAngle = self.relativeAngle.valueDegrees()

        if publish:
            self.publish(Pipe.UPDATE, ext.core.Event())

    def changeOverTime(self):
        diff = common.Target.changeOverTime(self)
        if self.prevTimeStamp is not None:
            diffAngle = self.relativeAngle - self.prevRelativeAngle
            diffTime = self.timeStamp - self.prevTimeStamp

            return diff + ((diffAngle / diffTime),)
        else:
            return diff + (float('inf'),)

    def errorAdj(self):
        adj = common.Target.errorAdj(self)
       
        errorAngle = self._kp * abs(self.relativeAngle) + \
            self._kd * Pipe.changeOverTime(self)[2]

        return adj + (errorAngle,)
    
class Hover(common.Hover):
    """
    A version of ram.motion.common.Hover which aligns to target too
    """
    def __init__(self, pipe, maxSpeed = 0.0, maxSidewaysSpeed = 0.0,
                 speedGain = 1.0, sidewaysSpeedGain = 1.0, 
                 yawGain = 1.0,
                 iSpeedGain = 0.0, dSpeedGain = 0.0, iSidewaysSpeedGain = 0.0,
                 dSidewaysSpeedGain = 0.0):
        """
        @type  pipe: ram.motion.pipe.Pipe
        @param pipe: Target to attempt to reach
        """
        _mtype = Motion.IN_PLANE
        if yawGain > 0:
            _mtype = Motion.IN_PLANE | Motion.ORIENTATION
        
        common.Hover.__init__(self, pipe, maxSpeed, maxSidewaysSpeed,
                              _type = _mtype,
                              speedGain = speedGain, 
                              sidewaysSpeedGain = sidewaysSpeedGain,
                              iSpeedGain = iSpeedGain, dSpeedGain = dSpeedGain, 
                              iSidewaysSpeedGain = iSidewaysSpeedGain,
                              dSidewaysSpeedGain = dSidewaysSpeedGain)
        
        self._pipe = pipe
        
        if yawGain > 1:
            raise TypeError("Yaw Gain must be <= 1")
        self._yawGain = yawGain
        
    def _turn(self):
        """Determine turn"""
        # If yawGain is zero, don't run any of this code
        if self._yawGain != 0.0:
            vehicleHeading =  self._vehicle.getOrientation().getYaw(True)
            vehicleHeading = vehicleHeading.valueDegrees()
            absoluteTargetHeading = vehicleHeading + self._pipe.relativeAngle
        
            desiredHeading = self._controller.getDesiredOrientation().getYaw(True)
            desiredHeading = desiredHeading.valueDegrees()
        
            yawCommand = (absoluteTargetHeading - desiredHeading) * self._yawGain

            # Command the vehicle
            self._controller.yawVehicle(yawCommand) 
        
    def _seek(self):
        common.Hover._seek(self)
        self._turn()

class Follow(Hover):
    def _setForwardSpeed(self):
        """Determine forward speed (and bound within limits)"""
        forwardSpeed = (1 - math.fabs(self._pipe.x)) * self._speedGain
        
        if forwardSpeed > self._maxSpeed:
            forwardSpeed = self._maxSpeed
        elif forwardSpeed < self._minSpeed:
            forwardSpeed = self._minSpeed

        self._controller.setSpeed(forwardSpeed)

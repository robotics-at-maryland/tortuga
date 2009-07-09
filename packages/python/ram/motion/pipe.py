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
from ram.motion.basic import Motion

class Pipe(common.Target):
    """
    Represents the pipe we are trying to follow
    """
    
    def __init__(self, x, y, relativeAngle):
        common.Target.__init__(self, x, y)
        self.setState(x, y, relativeAngle, publish = False)

    def setState(self, x, y, relativeAngle, publish = True):
        common.Target.setState(self, x, y, False)
        self.relativeAngle = relativeAngle

        # Change them to degrees if they are ext.math.Degree/Radian types
        if hasattr(self.relativeAngle, 'valueDegrees'):
            self.relativeAngle = self.relativeAngle.valueDegrees()

        if publish:
            self.publish(Pipe.UPDATE, ext.core.Event())
    
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
        common.Hover.__init__(self, pipe, maxSpeed, maxSidewaysSpeed,
                              _type = Motion.IN_PLANE | Motion.ORIENTATION,
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

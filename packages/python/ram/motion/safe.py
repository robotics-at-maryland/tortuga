# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/motion/safe.py

# STD Imports
import math as pmath

# Project Imports
import ext.core as core

import ram.timer as timer
import ram.motion as motion
import ram.motion.common
import ram.motion.basic


class OffsettingHover(motion.common.Hover):
    """
    A standard hover motion which slowly moves the vehicle offcenter of the 
    target
    """
    NEXT_OFFSET = core.declareEventType('NEXT_OFFSET')
    
    X = 1
    Y = 2
    
    def __init__(self, target, direction, offset, speed, rate = 10,
                 maxSpeed = 0.0, maxSidewaysSpeed = 0.0,
                 speedGain = 1.0, sidewaysSpeedGain = 1.0,
                 _type = motion.basic.Motion.IN_PLANE,
                 iSpeedGain = 0.0, dSpeedGain = 0.0, iSidewaysSpeedGain = 0.0,
                 dSidewaysSpeedGain = 0.0):
        assert (direction == OffsettingHover.X) or (direction == OffsettingHover.Y)
        
        # Call super class
        _type = motion.basic.Motion.IN_PLANE
        motion.common.Hover.__init__(self, target, maxSpeed, maxSidewaysSpeed,
                                     _type = _type,
                                     speedGain = speedGain, 
                                     sidewaysSpeedGain = sidewaysSpeedGain,
                                     iSpeedGain = iSpeedGain, 
                                     dSpeedGain = dSpeedGain, 
                                     iSidewaysSpeedGain = iSidewaysSpeedGain,
                                     dSidewaysSpeedGain = dSidewaysSpeedGain)
        
        
        # Init derived parameters to default values
        self._stepCount = 0
        self._stepSize = 1/10000.0
        self._timerConn = None
        self._timer = None
        self._interval = 1/10.0
        
        # Record given parameters
        self._target = target
        self._offset = offset
        self._direction = direction
        self._speed = float(speed)
        self._rate = float(rate)
        
        # Compute derived parameters
        self._interval = 1 / float(rate)
        
    def _start(self):
        # Start the actual hover motion
        motion.common.Hover._start(self)
        
        # Compute difference between our offset, and where we want to be
        currentVal = self._getCurrentVal()
        difference = self._offset - currentVal
        absDifference = pmath.fabs(difference)
            
        # Determine how many and of what size the our steps will be
        self._stepCount = absDifference / (self._speed / self._rate)
        if self._stepCount != 0:
            self._stepSize = difference / self._stepCount
        self._stepCount = int(self._stepCount)
        
        # Create timer whichs steps us along, and register for its events
        self._timer  = timer.Timer(self, OffsettingHover.NEXT_OFFSET,
                                   self._interval, repeat = True)
        # Register to NEXT_OFFSET events
        self._timerConn = self._eventHub.subscribeToType(
            OffsettingHover.NEXT_OFFSET, self._onTimer)
        self._timer.start()
  
        # Ensure the desired is consistent with the current
        if self._direction == OffsettingHover.X:
            self._xDesired = currentVal
        else:
            self._yDesired = currentVal
  
    def _getCurrentVal(self):
        """Gets the X, or Y value depending on direction"""
        currentVal = 0
        if self._direction == OffsettingHover.X:
            currentVal = self._target.x
        else:
            currentVal = self._target.y
        return currentVal
  
    def _onTimer(self, event):
        absDifference = pmath.fabs(self._getCurrentVal() - self._offset)
        
        if (self._stepCount > 0) and (absDifference > 0.0001):
            if self._direction == OffsettingHover.X:
                self._xDesired += self._stepSize
            else:
                self._yDesired += self._stepSize

            self._stepCount -= 1
        else:
            self._finish()
        
    def _finish(self):
        """
        Finishes off the motion, disconnects events, and putlishes finish event
        """
        self.stop()
        motion.common.Hover._finish(self)

    def stop(self):
        motion.common.Hover.stop(self)
        
        if self._timer is not None:
            self._timer.stop()
        if self._timerConn is not None:
            self._timerConn.disconnect()
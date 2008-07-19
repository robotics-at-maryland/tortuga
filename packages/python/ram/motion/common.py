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

class Target(ext.core.EventPublisher):
    """
    Represents a target with an X, Y position below the vehicle
    """
    UPDATE = ext.core.declareEventType('UPDATE')
    
    def __init__(self, x, y):
        ext.core.EventPublisher.__init__(self)
        Target.setState(self, x, y, publish = False)

    def setState(self, x, y, publish = True):
        self.x = x
        self.y = y

        if publish:
            self.publish(Target.UPDATE, ext.core.Event())
    
class Hover(Motion):
    """
    Base motion it centers the vehicle over the target
    """
    def __init__(self, target, maxSpeed = 0.0, maxSidewaysSpeed = 0.0,
                 speedGain = 1.0, sidewaysSpeedGain = 1.0,
                 _type = Motion.IN_PLANE):
        """
        @type  target: ram.motion.common.Target
        @param target: Target to attempt to reach
        """
        Motion.__init__(self, _type = _type)
        
        self._running = False
        self._target = target

        self._maxSpeed = maxSpeed
        self._minSpeed = -1 * maxSpeed

        self._maxSidewaysSpeed = maxSidewaysSpeed
        self._minSidewaysSpeed = -1 * maxSidewaysSpeed

        self._speedGain = speedGain
        self._sidewaysSpeedGain = sidewaysSpeedGain
        
        self._conn = target.subscribe(Target.UPDATE, self._onTargetUpdate)
        
    def _start(self):
        self._running = True
        self._seek()
        
    def _setForwardSpeed(self):
        """Determin forward speed (and bound within limits)"""
        forwardSpeed = self._target.y * self._speedGain

        # Clamp speed ranges
        if forwardSpeed > self._maxSpeed:
            forwardSpeed = self._maxSpeed
        elif forwardSpeed < self._minSpeed:
            forwardSpeed = self._minSpeed

        self._forwardSpeed = forwardSpeed
        self._controller.setSpeed(forwardSpeed)
        
    def _setSidewaysSpeed(self):
        """Determine sideways speed (and bound within limits)"""
        sidewaysSpeed = self._target.x * self._sidewaysSpeedGain

        # Clamp speed ranges
        if sidewaysSpeed > self._maxSidewaysSpeed:
            sidewaysSpeed = self._maxSidewaysSpeed
        elif sidewaysSpeed < self._minSidewaysSpeed:
            sidewaysSpeed = self._minSidewaysSpeed

        self._sidewaysSpeed = sidewaysSpeed
        self._controller.setSidewaysSpeed(sidewaysSpeed)

    def _seek(self):
        self._setForwardSpeed()
        self._setSidewaysSpeed()

    @staticmethod
    def _limit(val, min, max):
        if val < min:
            return min
        elif val > max:
            return max
        return val

    def _onTargetUpdate(self, event):
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

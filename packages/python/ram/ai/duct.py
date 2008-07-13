# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/duct.py
  
"""
A state machine for finding and hitting the red light
 - Search for duct
 - Seeks till its in range of duct
 - Aligns to the front of the duct while holding range
 - Does a timed run through the duct
 - Halts vehicle
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# STD Imports
import math

# Project Imports
import ext.core as core
import ext.vision as vision

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.motion.duct

THROUGH_DUCT = core.declareEventType('THROUGH_DUCT')

class Searching(state.State):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.DUCT_FOUND : SeekingToRange }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.ductDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 2.5)
        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class SeekingToRange(state.State):
    """
    Heads toward the duct until it reaches the desired range
    """
    
    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions():
        return { vision.EventType.DUCT_FOUND : SeekingToRange,
                 vision.EventType.DUCT_LOST : Searching,
                 SeekingToRange.IN_RANGE : SeekingToAligned }
        
    def DUCT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        azimuth = event.x * -107.0/2.0
        elevation = event.y * -80.0/2.0
        self._duct.setState(azimuth, elevation, event.range, event.x, event.y)
        
        rangeError = math.fabs(event.range - self._desiredRange)
        frontDistance = math.sqrt(event.x * event.x + event.y * event.y)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())
        
    def enter(self):
        # Ensure vision system is on
        self.visionSystem.ductDetectorOn()
        
        # Create tracking object
        self._duct = ram.motion.seek.PointTarget(0, 0, 0, 0, 0)
        
        # Read in configuration settings
        self._rangeThreshold = self._config.get('rangeThreshold', 0.05)
        self._frontThreshold = self._config.get('frontThreshold', 0.15)
        depthGain = self._config.get('depthGain', 1.5)
        self._desiredRange = self._config.get('desiredRange', 0.3)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxSpeed = self._config.get('maxSpeed', 3)
        
        motion = ram.motion.seek.SeekPointToRange(target = self._duct,
            desiredRange = self._desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxSpeed = maxSpeed,
            depthGain = depthGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class DuctAlignState(object):
    def DUCT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        azimuth = event.x * -107.0/2.0
        elevation = event.y * -80.0/2.0
        self._duct.setState(azimuth, elevation, event.range, event.x, 
                            event.y, event.alignment)
        
    def enter(self):
        # Ensure vision system is on
        self.visionSystem.ductDetectorOn()
        
        # Create tracking object
        self._duct = ram.motion.duct.Duct(0, 0, 0, 0, 0, 0)
        
        # Read in configuration settings
        depthGain = self._config.get('depthGain', 1.5)
        desiredRange = self._config.get('desiredRange', 0.3)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxAlignDiff = self._config.get('maxAlignDiff', 45)
        maxSpeed = self._config.get('maxSpeed', 3)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 3)

        motion = ram.motion.duct.DuctSeekAlign(target = self._duct,
            desiredRange = desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxAlignDiff = maxAlignDiff,
            maxSpeed = maxSpeed,
            maxSidewaysSpeed = maxSidewaysSpeed,
            depthGain = depthGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()   
        
class SeekingToAligned(DuctAlignState, state.State):
    ALIGNED = core.declareEventType('ALIGNED')
    
    @staticmethod
    def transitions():
        return { vision.EventType.DUCT_FOUND : SeekingToAligned,
                 vision.EventType.DUCT_LOST : Searching,
                 SeekingToAligned.ALIGNED : Aligning }

    def DUCT_FOUND(self, event):
        DuctAlignState.DUCT_FOUND(self, event)
        
        # Publish aligned event if needed
        if event.aligned:
            self.publish(SeekingToAligned.ALIGNED, core.Event())
    
        
class Aligning(DuctAlignState, state.State):
    SETTLED = core.declareEventType('ALIGNED')
       
    @staticmethod
    def transitions():
        return { vision.EventType.DUCT_FOUND : Aligning,
                 vision.EventType.DUCT_LOST : Searching,
                 Aligning.SETTLED : Through }

    def enter(self):
        DuctAlignState.enter(self)
        
        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 5))
        self.timer.start()
        
    def exit(self):
        DuctAlignState.exit(self)
        self.timer.stop()
        
class Through(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Through.FORWARD_DONE : End}

    def enter(self):
        self.visionSystem.ductDetectorOff()

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        self.timer = self.timerManager.newTimer(
            Through.FORWARD_DONE, self._config.get('forwardTime', 15))
        self.timer.start()
        self.controller.setSpeed(3)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        self.publish(THROUGH_DUCT, core.Event())
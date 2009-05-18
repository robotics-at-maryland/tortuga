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

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.motion.duct

COMPLETE = core.declareEventType('COMPLETE')

class FilteredState(object):
    """
    Provides filter on the inputs from the vision system
    """
    def enter(self):
        # Read in filter size
        filterSize = self._config.get('filterSize', 1)
        
        # Create filters
        self._xFilter = filter.MovingAverageFilter(filterSize)
        self._yFilter = filter.MovingAverageFilter(filterSize)
        self._rangeFilter = filter.MovingAverageFilter(filterSize)
        self._alignFilter = filter.MovingAverageFilter(filterSize)
        
        # Zero values
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()
        
    def _updateFilters(self, event):
        # Add to filters
        self._xFilter.append(event.x)
        self._yFilter.append(event.y)
        self._rangeFilter.append(event.range)
        self._alignFilter.append(event.squareNess - 1)
        
        # Get new result
        self._filterdX = self._xFilter.getAverage()
        self._filterdY = self._yFilter.getAverage()
        self._filterdRange = self._rangeFilter.getAverage()
        self._filterdAlign = self._alignFilter.getAverage()

class RangeXYHold(FilteredState, state.State):
    """
    Base state that holds the target centered in X, Y and depth without yawing.
    It will also throw off an event whenever it is within range and centering
    tolerances.
    """
    
    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod   
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = RangeXYHold
        if trans is None:
            trans = {}
        trans.update({vision.EventType.BARBED_WIRE_FOUND : myState,
                      vision.EventType.BARBED_WIRE_LOST : FindAttempt })
        return trans
        
    def BARBED_WIRE_FOUND(self, event):
        """Update the state of the target, this moves the vehicle"""
        # Todo: consider filter removal
        #self._updateFilters(event)
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also take into account any offset we wish to hold the object at.
        y = 0
        if self._depthGain != 0:
            y = event.topY - self._yZero
        
        # Width == 1 when we are close, so inversion is needed to get range 
        range = 1 - event.topWidth
        
        # Finally set the state (We ignore azimuth and elevation because we 
        # aren't using them)
        self._target.setState(0, 0, range = range, x = event.topX, y = y)
        
        # Only triggered the in range event if we are close and the target is
        # centered in the field of view
        rangeError = math.fabs(range - self._desiredRange)
        #print "RE",rangeError,"AR",rge,"DR",self._desiredRange
        frontDistance = math.sqrt(event.topX ** 2 + y ** 2)
        if (rangeError < self._rangeThreshold) and \
            (frontDistance < self._frontThreshold):
            self.publish(SeekingToRange.IN_RANGE, core.Event())
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.barbedWireDetectorOn()
        
        # Create tracking object
        self._target = ram.motion.seek.PointTarget(0, 0, 0, 0, 0)
        
        # Read in configuration settings
        self._yZero = self._config.get('yZero', 0.5)
        self._rangeThreshold = self._config.get('rangeThreshold', 0.05)
        self._frontThreshold = self._config.get('frontThreshold', 0.15)
        
        self._depthGain = self._config.get('depthGain', 1.5)
        iDepthGain = self._config.get('iDepthGain', 0.3)
        dDepthGain = self._config.get('dDepthGain', 0.15)
        
        self._desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        
        translateGain = self._config.get('translateGain', 1)
        iTranslateGain = self._config.get('iTranslateGain', 0.25)
        dTranslateGain = self._config.get('dTranslateGain', 0.125)
        
        motion = ram.motion.seek.SeekPointToRange(target = self._target,
            desiredRange = self._desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxSpeed = maxSpeed,
            depthGain = self._depthGain,
            iDepthGain = iDepthGain,
            dDepthGain = dDepthGain,
            translate = True,
            translateGain = translateGain,
            iTranslateGain = iTranslateGain,
            dTranslateGain = dTranslateGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Start(state.State):
    """
    Gets the vehicle to the proper depth, so it can acquire the target.
    """
    
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : FindAttempt }
    
    def enter(self):
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('diveDepth', 12),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.State):
    """
    Attempt to find the target, before starting search pattern.  This will
    catch the object if its front of the vehicle.
    """
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def transitions():
        return { FindAttempt.TIMEOUT : Searching,
                 vision.EventType.BARBED_WIRE_FOUND : SeekingToRange }
    
    def enter(self):
        # Turn all off all motions, hold the current heading
        self.motionManager.stopCurrentMotion()
        self.controller.holdCurrentHeading()
        # TODO: Hold deth        
        
        # Turn on the detector
        self.visionSystem.barbedWireDetectorOn()

        # Start up the timer 
        delay = self._config.get('delay', 2)
        self.timer = self.timerManager.newTimer(FindAttempt.TIMEOUT, delay)
        self.timer.start()

    def exit(self):
        self.timer.stop()

class Searching(state.State):
    """
    Runs a zig-zag search pattern until it finds the duct
    """
    
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : SeekingToRange }

    def enter(self):
        # Make sure the detector is on the vision system
        self.visionSystem.barbedWireDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 2.5)
        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class SeekingToRange(RangeXYHold):
    """
    Heads toward the target until it reaches the desired range
    """
    
    @staticmethod
    def transitions():
        return RangeXYHold.transitions(SeekingToRange, {
            RangeXYHold.IN_RANGE : SeekingToAligned })
        
class TargetAlignState(FilteredState):
    def BARBED_WIRE_FOUND(self, event):
        # Todo remove filters
        #self._updateFilters(event)
        
        """Update the state of the light, this moves the vehicle"""
        azimuth = event.topX * -107.0/2.0
        elevation = event.topY * -80.0/2.0
        
        # Determine y value based on whether we are ignoring depth or not, 
        # also take into account any offset we wish to hold the object at.
        y = 0
        if self._depthGain != 0:
            y = event.topY - self._yZero
        
        range = 1 - event.topWidth
        alignment = event.bottomX - event.topX
        self._target.setState(azimuth, elevation, range = range,
                              x = event.topX, y = y, 
                              alignment = alignment)
        
    def enter(self):
        FilteredState.enter(self)
        
        # Ensure vision system is on
        self.visionSystem.barbedWireDetectorOn()
        
        # Create tracking object
        self._target = ram.motion.duct.Duct(0, 0, 0, 0, 0, 0)
        
        # Read in configuration settings
        self._yZero = self._config.get('yZero', 0.5)
        self._depthGain = self._config.get('depthGain', 1)
        desiredRange = self._config.get('desiredRange', 0.5)
        maxRangeDiff = self._config.get('maxRangeDiff', 0.2)
        maxAlignDiff = self._config.get('maxAlignDiff', 0.5)
        alignGain = self._config.get('alignGain', 1.0)
        maxSpeed = self._config.get('maxSpeed', 0.75)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 1)

        motion = ram.motion.duct.DuctSeekAlign(target = self._target,
            desiredRange = desiredRange,
            maxRangeDiff = maxRangeDiff,
            maxAlignDiff = maxAlignDiff, 
            maxSpeed = maxSpeed,
            maxSidewaysSpeed = maxSidewaysSpeed,
            alignGain = alignGain,
            depthGain = self._depthGain)
        
        self.motionManager.setMotion(motion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()   
        
class SeekingToAligned(TargetAlignState, state.State):
    """
    Holds the target at range and in the center of the field of view while 
    rotating around it.  If its rotating the wrong direction is will reverse
    that direction and keep going until its close enough to aligned.
    """
    ALIGNED = core.declareEventType('ALIGNED')
    CHECK_DIRECTION = core.declareEventType('CHECK_DIRECTION_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : SeekingToAligned,
                 vision.EventType.BARBED_WIRE_LOST : FindAttempt,
                 SeekingToAligned.CHECK_DIRECTION : SeekingToAligned,
                 SeekingToAligned.ALIGNED : Aligning }

    def BARBED_WIRE_FOUND(self, event):
        # Publish aligned event if needed
        alignment = math.fabs(event.topX - event.bottomX)
        if alignment < self._minAlignment:
            self.publish(SeekingToAligned.ALIGNED, core.Event())
            
        # Update motion
        TargetAlignState.BARBED_WIRE_FOUND(self, event)
        
    def enter(self):
        TargetAlignState.enter(self)
        
        # Record threshold
        self._minAlignment = self._config.get('minAlignment', 0.1)
        
    
# The next two states are extra states, not sure if they are needed    
class Aligning(TargetAlignState, state.State):
    SETTLED = core.declareEventType('ALIGNED')
       
    @staticmethod
    def transitions():
        return { vision.EventType.BARBED_WIRE_FOUND : Aligning,
                 vision.EventType.BARBED_WIRE_LOST : FindAttempt,
                 Aligning.SETTLED : Through }

    def enter(self):
        TargetAlignState.enter(self)
        
        self.timer = self.timerManager.newTimer(
            Aligning.SETTLED, self._config.get('settleTime', 15))
        self.timer.start()
        
    def exit(self):
        TargetAlignState.exit(self)
        self.timer.stop()
        
class Through(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Through.FORWARD_DONE : End}

    def enter(self):
        self.visionSystem.barbedWireDetectorOff()

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        self.timer = self.timerManager.newTimer(
            Through.FORWARD_DONE, self._config.get('forwardTime', 15))
        self.timer.start()
        self.controller.setSidewaysSpeed(0)
        self.controller.setSpeed(3)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
